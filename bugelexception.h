#ifndef BUGELEXCEPTION
#define BUGELEXCEPTION

#include <QString>

#include <string>
#include <sstream>
#include <exception>

// shitty exception class
// why doesn't qt have something like this default
// god damn it shit fuck shit fuck fuck shit

class BugelException : public std::exception
{
public:
    std::string msg;
    const char* what() const throw() { return msg.c_str(); }
    template<typename T>
    friend BugelException& operator<<(BugelException& e, T msg);
};

template<typename T>
BugelException& operator<<(BugelException& e, T appendMsg)
{
    std::stringstream ss;
    ss << e.msg << appendMsg;
    e.msg = ss.str().c_str();
    return e;
}

std::ostream& operator<<(std::ostream& stream, const QString& str);

// sub-exceptions
class EventException : public BugelException {};
class ScriptException : public BugelException {};

#endif // BUGELEXCEPTION

