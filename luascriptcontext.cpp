#include "luascriptcontext.h"

#include "timeline.h"
#include "timelineevent.h"
#include "bugelexception.h"
#include "project.h"

#include <QDebug>
#include <QMap>

LuaScriptContext::LuaScriptContext(const QString& script)
{
    try {
        mState.doFile(script.toStdString());
    } catch(lua::LoadError& e) {
        throw BugelException() << "Script file \"" << script <<
                                  "\" not found!\n" << e.what();
    } catch(lua::RuntimeError& e) {
        throw BugelException() << "Error in Lua script!\n" <<
                                  e.what();
    }
}

void luaPushVariant(lua_State* state, const QVariant& val)
{
    if (val.isNull()) {
        lua_pushnil(state);
        return;
    }

    switch(val.type()) {
    case QMetaType::Bool:
        lua_pushboolean(state, val.value<bool>());
        break;

    case QMetaType::Char:
    case QMetaType::Short:
    case QMetaType::Int:
    case QMetaType::Long:
    case QMetaType::LongLong:
        lua_pushinteger(state, val.value<lua_Integer>());
        break;

    case QMetaType::Float:
    case QMetaType::Double:
        lua_pushnumber(state, val.value<lua_Number>());
        break;

    case QMetaType::QString:
    default:
        lua_pushstring(state, val.toString().toUtf8().constData());
        break;
    }
}

#define SAFE_LUA_GET(check, get, stack_idx) \
    if (!check(state, stack_idx)) \
        luaL_error(state, "Invalid value \"%s\" for %s[\"%s\"]! (Expected type %s.)", \
                   lua_tostring(state, stack_idx), typeName, \
                   key, QMetaType::typeName(propType->type)); \
    value = get(state, stack_idx);

int addEventHelper(lua_State* state)
{
    TimelineLayer* layer = (TimelineLayer*)lua_touserdata(state, lua_upvalueindex(1));

    luaL_checktype(state, 1, LUA_TTABLE);

    // get typename
    lua_getfield(state, 1, "typeName"); // type at -1
    if (!lua_isstring(state, -1))
        return luaL_error(state, "typeName is missing or not a string!");
    const QString typeName = lua_tostring(state, -1);

    // get event time
    lua_getfield(state, 1, "time"); // time at -1, type at -2
    if (!lua_isnumber(state, -1))
        return luaL_error(state, "time is missing or not a number!");
    const lua_Number time = lua_tonumber(state, -1);

    lua_pop(state, 2);

    // find the event type
    const auto& eventTypes = Project::get()->eventTypes();
    const auto evType = std::find_if(eventTypes.begin(), eventTypes.end(),
        [&](const std::shared_ptr<EventType>& ev) {
        return (ev->name.c_str() == typeName);
    });

    if (evType == eventTypes.end())
        return luaL_error(state, "Unknown event type \"%s\"!", typeName.toUtf8().constData());

    std::shared_ptr<TimelineEvent> ev =
            std::make_shared<TimelineEvent>(*evType, time);

    // read arguments from the table
    const auto& props = evType->get()->properties;

    lua_pushnil(state); // lua_next expects a key value on the stack
    while (lua_next(state, 1) != 0) {
        // key is at stack index -2
        // value is at stack index -1

        // copy the key in case checkstring needs to do conversion to make it a string
        // we can't convert it directly because that will mess up lua_next,
        // which expects exactly the previous key (changing types changes the value)
        lua_pushvalue(state, -2); // copy -2 and push it onto the stack
        const char* key = lua_tostring(state, -1);

        // real key for lua_next is at -3
        // value is at -2
        // string key is at -1

        // ignore "typeName" and "time", we already got those
        if (!(strcmp(key, "typeName") == 0 || strcmp(key, "time") == 0)) {
            // find the property
            const auto propType = props.find(key);
            if (propType == props.end())
                return luaL_error(state, "Unknown property \"%s\" (for event type %s)", key, typeName);

            QVariant value;
            switch (propType->type) {
            case QMetaType::Bool:
                SAFE_LUA_GET(lua_isboolean, lua_toboolean, -2)
                break;
            case QMetaType::Int:
                SAFE_LUA_GET(lua_isnumber, lua_tointeger, -2);
                break;
            case QMetaType::Double:
                SAFE_LUA_GET(lua_isnumber, lua_tonumber, -2);
                break;
            case QMetaType::QString:
                SAFE_LUA_GET(lua_isstring, lua_tostring, -2);
                break;
            default:
                return luaL_error(state, "Can't read Lua type for QMetaType::Type = %d", propType->type);
            }

            ev->setProperty(key, value);
        }

        // remove string key and value, keep original key for next iteration
        lua_pop(state, 2);
        // real key is at -1
    }

    layer->events().addEvent(ev);
    return 0;
}

void LuaScriptContext::process(const TimelineLayer* sourceLayer, TimelineLayer* outputLayer)
{
    lua_State* state = mState.getState();

    lua_pushlightuserdata(state, outputLayer);
    lua_pushcclosure(state, addEventHelper, 1);
    lua_setglobal(state, "addEvent");

    // name of function to call
    lua_getglobal(state, "process");

    // could possibly improve this to preallocate memory with lua_createtable
    // if the Lua documentation told me what the fuck "sequential elements"
    // vs "other elements" meant
    lua_newtable(state); // event_table

    const auto& sourceEvents = sourceLayer->events();
    int idx = 0;
    for (auto it = sourceEvents.begin(); it != sourceEvents.end(); it++, idx++) {

        // event_table[idx] = { ... }
        lua_pushinteger(state, idx);
        lua_newtable(state);

        // event["typeName"] = ev->typeName()
        lua_pushstring(state, it->get()->typeName());
        lua_setfield(state, -2, "typeName");

        // event["time"] = ev->time();
        lua_pushnumber(state, it->get()->time());
        lua_setfield(state, -2, "time");

        // copy properties (TODO)
        const auto& properties = it->get()->properties();
        for (auto propIt = properties.begin(); propIt != properties.end(); propIt++) {
            luaPushVariant(state, propIt.value());
            lua_setfield(state, -2, propIt.key().toUtf8().constData());
        }

        // actually put event into event_table
        lua_settable(state, -3);
    }

    // call the function
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
        const QString error = lua_tostring(state, -1);
        lua_pop(state, 1);
        throw ScriptException() << "Error processing timeline:\n"
                                << error;
    }
}
