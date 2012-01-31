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


#include "HTTPRspCodeClassString.h"

HTTPRspCodeClassString::HTTPRspCodeClassString(void)
{
	m_CodeClassStrings[ 1 ] = "Informational - Request received, continuing process";
	m_CodeClassStrings[ 2 ] = "Success - The action was successfully received understood, and accepted";
	m_CodeClassStrings[ 3 ] = "Redirection - Further action must be taken in order to complete the request";
	m_CodeClassStrings[ 4 ] = "Client Error - The request contains bad syntax or cannot be fulfilled";
	m_CodeClassStrings[ 5 ] = "Server Error - The server failed to fulfill an apparently valid request";
}

std::string HTTPRspCodeClassString::getInfo( unsigned int p_Code ) {
	unsigned int codeClass = p_Code / 100;
	std::map<unsigned int,std::string>::iterator codeClassItr = m_CodeClassStrings.find( codeClass );
	if ( codeClassItr != m_CodeClassStrings.end() ) {
		return codeClassItr->second;
	}
	return "UNKNOWN CODE CLASS";
}
