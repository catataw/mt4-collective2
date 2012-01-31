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

#include "HTTPRspCodeString.h"

HTTPRspCodeString::HTTPRspCodeString(void)
{
	m_StatusCodeStrings[ 100 ] = "Continue";
	m_StatusCodeStrings[ 101 ] = "Switching Protocols";
	m_StatusCodeStrings[ 200 ] = "OK";
	m_StatusCodeStrings[ 201 ] = "Created";
	m_StatusCodeStrings[ 202 ] = "Accepted";
	m_StatusCodeStrings[ 203 ] = "Non-Authoritative Information";
	m_StatusCodeStrings[ 204 ] = "No Content";
	m_StatusCodeStrings[ 205 ] = "Reset Content";
	m_StatusCodeStrings[ 206 ] = "Partial Content";
	m_StatusCodeStrings[ 300 ] = "Multiple Choices";
	m_StatusCodeStrings[ 301 ] = "Moved Permanently";
	m_StatusCodeStrings[ 302 ] = "Found";
	m_StatusCodeStrings[ 303 ] = "See Other";
	m_StatusCodeStrings[ 304 ] = "Not Modified";
	m_StatusCodeStrings[ 305 ] = "Use Proxy";
	m_StatusCodeStrings[ 306 ] = "Reserved (Unused)";
	m_StatusCodeStrings[ 307 ] = "Temporary Redirect";
	m_StatusCodeStrings[ 400 ] = "Bad Request";
	m_StatusCodeStrings[ 401 ] = "Unauthorized";
	m_StatusCodeStrings[ 402 ] = "Payment Required";
	m_StatusCodeStrings[ 403 ] = "Forbidden";
	m_StatusCodeStrings[ 404 ] = "Not Found";
	m_StatusCodeStrings[ 405 ] = "Method Not Allowed";
	m_StatusCodeStrings[ 406 ] = "Not Acceptable";
	m_StatusCodeStrings[ 407 ] = "Proxy Authentication Required";
	m_StatusCodeStrings[ 408 ] = "Request Time-out";
	m_StatusCodeStrings[ 409 ] = "Conflict";
	m_StatusCodeStrings[ 410 ] = "Gone";
	m_StatusCodeStrings[ 411 ] = "Length Required";
	m_StatusCodeStrings[ 412 ] = "Precondition Failed";
	m_StatusCodeStrings[ 413 ] = "Request Entity Too Large";
	m_StatusCodeStrings[ 414 ] = "Request-URI Too Large";
	m_StatusCodeStrings[ 415 ] = "Unsupported Media Type";
	m_StatusCodeStrings[ 416 ] = "Requested range not satisfiable";
	m_StatusCodeStrings[ 417 ] = "Expectation Failed";
	m_StatusCodeStrings[ 500 ] = "Internal Server Error";
	m_StatusCodeStrings[ 501 ] = "Not Implemented";
	m_StatusCodeStrings[ 502 ] = "Bad Gateway";
	m_StatusCodeStrings[ 503 ] = "Service Unavailable";
	m_StatusCodeStrings[ 504 ] = "Gateway Time-out";
	m_StatusCodeStrings[ 505 ] = "HTTP Version not supported";
}

std::string HTTPRspCodeString::getInfo( unsigned int p_StatusCode ) {
	std::map<unsigned int, std::string>::iterator codeIter = m_StatusCodeStrings.find( p_StatusCode );
	if ( codeIter != m_StatusCodeStrings.end() ) {
		return codeIter->second;
	}
	return "UNKNOWN RESPONSE CODE";
}
