#ifndef LUASCRIPTCONTEXT_H
#define LUASCRIPTCONTEXT_H

#include <QString>
#include <QMetaType>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning( disable : 4100 )
#endif

#include <LuaState.h>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

class TimelineLayer;

class LuaScriptContext
{
public:
    LuaScriptContext(const QString& script);

    void process(const TimelineLayer* sourceLayer, TimelineLayer* outputLayer);

private:
    lua::State mState;
};

#endif // LUASCRIPTCONTEXT_H
