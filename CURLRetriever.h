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
#include <string>
#define CURL_STATICLIB
#include <curl/curl.h>

//////////////////////////////////////////////////////////////////////////
#define UR_DOWNLOAD_OPERATION_TIMEOUT (30000) // (in milliseconds)
//////////////////////////////////////////////////////////////////////////

typedef struct  {
    char server_type[10];
    char server[30];
} proxy_server_list;

class CURLRetriever
{
public:
	static VOID InitializeCURL();
	static VOID UninitializeCURL();

	CURLRetriever();
	virtual ~CURLRetriever();

protected:
	VOID Clear();
	HRESULT GetProxyInfo();

public:
	HRESULT Initialize(IN LPCSTR lpszURL);
	HRESULT Uninitialize();

	HRESULT Retrieve(OUT LONG &lResponseCode, OUT std::string &sRetrievedContent, OUT std::string &sRetrievingErrorDescription);

protected:
	static size_t OnDataDownloaded(LPVOID pData, size_t nDataBlockCount, size_t nDataBlocSize, LPVOID pStream);

protected:
	std::string m_sURL;

	CURL* m_hCURL;
	proxy_server_list ProxyServerList[4];
	int ProxyServerListCount ;
};
