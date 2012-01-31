/*=============================================================================
	This file is a part of the MetaTrader connector for Collective2 (mt4-collective2).

    mt4-collective2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

     mt4-collective2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mt4-collective2.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (c) 2010 by OpenThinking Systems, LLC.
=============================================================================*/

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
