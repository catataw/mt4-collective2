//+------------------------------------------------------------------+
//| Copyright 2010 OpenThinking Systems, LLC
//|
//|  Licensed under the Apache License, Version 2.0 (the "License");
//|  you may not use this file except in compliance with the License.
//|  You may obtain a copy of the License at
//|
//|               http://www.apache.org/licenses/LICENSE-2.0
//|
//|  Unless required by applicable law or agreed to in writing,
//|  software distributed under the License is distributed on an
//|  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//|  KIND, either express or implied. See the License for the
//|  specific language governing permissions and limitations under
//|  the License.
//+------------------------------------------------------------------+
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
