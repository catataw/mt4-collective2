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

#pragma once

#include "AbstractCodeInfo.h"
#include <string>
#include <map>

class HTTPRspCodeClassString: public AbstractCodeInfo
{
protected:
	std::map<unsigned int,std::string> m_CodeClassStrings;

public:
	HTTPRspCodeClassString(void);
	virtual std::string getInfo( unsigned int p_Code );
};
