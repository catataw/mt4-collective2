#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

class BadConversion : public std::runtime_error
{
public:
    BadConversion(const std::string& s)
            : std::runtime_error(s)
    { }
};

inline std::string stringify(double x)
{
    std::ostringstream o;
    if (!(o << x))
        throw BadConversion("stringify(double)");
    return o.str();
}

inline std::string stringify(int x)
{
    std::ostringstream o;
    if (!(o << x))
        throw BadConversion("stringify(int)");
    return o.str();
}
inline double convertToDouble(const std::string& s)
{
    std::istringstream i(s);
    double x;
    if (!(i >> x))
        x = 0.0;
    return x;
}

inline double convertToDouble(const char* s)
{
    std::istringstream i(s);
    double x;
    if (!(i >> x))
        x = 0.0;
    return x;
}
inline int convertToInt(const std::string& s)
{
    std::istringstream i(s);
    int x;
    if (!(i >> x))
        x = 0;
    return x;
}

inline int convertToInt(const char* s)
{
    std::istringstream i(s);
    int x;
    if (!(i >> x))
        x = 0;
    return x;
}
