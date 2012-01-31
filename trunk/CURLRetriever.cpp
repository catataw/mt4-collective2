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
#include "CURLRetriever.h"
#include <wininet.h>
#include <iostream>
#include <sstream>

using namespace std;

VOID CURLRetriever::InitializeCURL()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

VOID CURLRetriever::UninitializeCURL()
{
	curl_global_cleanup();
}

CURLRetriever::CURLRetriever()
{
	Clear();
}

CURLRetriever::~CURLRetriever()
{
	Clear();
}

VOID CURLRetriever::Clear()
{
	m_sURL = "";
	m_hCURL = NULL;
}

HRESULT CURLRetriever::GetProxyInfo()
{
    unsigned long        nSize = 4096;
    char                 szBuf[4096] = { 0 };
    HRESULT error_status = S_OK;
    int tokens = 0;
    ProxyServerListCount = 0;

    INTERNET_PROXY_INFO* pInfo = (INTERNET_PROXY_INFO*)szBuf;

    if(!InternetQueryOption(NULL, INTERNET_OPTION_PROXY, pInfo, &nSize))
    {
        //printf("InternetQueryOption failed! (%d)\n", GetLastError());
        error_status = E_FAIL;
    }
    const char *proxy  = pInfo->lpszProxy;
    const char *bypass = pInfo->lpszProxyBypass;

    //proxy will be of the form https=microchip.blr.secure:20 http=microchip.blr.org:80 ftp=microchip.blr.ftp:32
    //so parsing it to obtain the right value
    if (proxy != NULL)
    {
        char *ptr;
        char scanlist[100];
        strcpy(scanlist, proxy);
        ptr = strtok (scanlist," =");
        while (ptr != NULL)
        {
            //printf ("%s\n",ptr);
            if(tokens%2 == 0)
            {
                strcpy(ProxyServerList[ProxyServerListCount].server_type, ptr);
            }
            else {
                strcpy(ProxyServerList[ProxyServerListCount].server, ptr);
                ProxyServerListCount++;
            }
            ptr = strtok (NULL, " =");
            tokens++;
        }
        // however if a single server is set for all protocols then the output would be microchip.blr.org;
        // the server type would be missing as it is knownly implicitly to be HTTP
        if(tokens == 1)
        {
            strcpy(ProxyServerList[0].server_type, "http");
            strcpy(ProxyServerList[0].server, proxy);
            ProxyServerListCount = 1;
        }
    }
    else
    {
      ProxyServerListCount = -1;
    }
    return error_status;
}

HRESULT CURLRetriever::Initialize(IN LPCSTR lpszURL)
{
	HRESULT hResult = E_FAIL;
	int count;

	Uninitialize();

	hResult = E_INVALIDARG;
	if (lpszURL != NULL)
	{
		m_sURL = lpszURL;

		hResult = E_FAIL;
		{
			m_hCURL = curl_easy_init();
			if (m_hCURL != NULL)
			{
				//curl_easy_setopt(m_hCURL, CURLOPT_CONNECTTIMEOUT_MS, (LONG)UR_DOWNLOAD_OPERATION_TIMEOUT);
				curl_easy_setopt(m_hCURL, CURLOPT_FOLLOWLOCATION, TRUE);
				curl_easy_setopt(m_hCURL, CURLOPT_FAILONERROR, TRUE);
				curl_easy_setopt(m_hCURL, CURLOPT_WRITEFUNCTION, &CURLRetriever::OnDataDownloaded);
				curl_easy_setopt(m_hCURL, CURLOPT_SSL_VERIFYPEER, FALSE);
				curl_easy_setopt(m_hCURL, CURLOPT_CONNECTTIMEOUT, 0);
				curl_easy_setopt(m_hCURL, CURLOPT_TIMEOUT, 0);
                if(GetProxyInfo() == S_OK && ProxyServerListCount > 0)
                {
                    for(count = 0; count < ProxyServerListCount; count++)
                    {
                        if(strcmp(ProxyServerList[count].server_type, "http") == 0)
                        {
                            curl_easy_setopt(m_hCURL, CURLOPT_PROXY, ProxyServerList[count].server);
                            curl_easy_setopt(m_hCURL, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
                        }
                    }
                }
				hResult = S_OK;
			}
		}
	}

	if (hResult != S_OK)
	{
		Uninitialize();
	}

	return hResult;
}

HRESULT CURLRetriever::Uninitialize()
{
	HRESULT hResult = E_FAIL;

	m_sURL = "";
	curl_easy_cleanup(m_hCURL);
	m_hCURL = NULL;

	Clear();

	hResult = S_OK;

	return hResult;
}

HRESULT CURLRetriever::Retrieve(OUT LONG &lResponseCode, OUT std::string &sRetrievedContent, OUT std::string &sRetrievingErrorDescription)
{
	HRESULT hResult = E_FAIL;

	lResponseCode = 0;
	sRetrievedContent = "";
	sRetrievingErrorDescription = "";

	if (m_hCURL != NULL)
	{
		curl_easy_setopt(m_hCURL, CURLOPT_WRITEDATA, &sRetrievedContent);
		curl_easy_setopt(m_hCURL, CURLOPT_URL, m_sURL.c_str());

		CURLcode result = curl_easy_perform(m_hCURL);

		curl_easy_getinfo(m_hCURL, CURLINFO_RESPONSE_CODE, &lResponseCode);

		if (result == CURLE_OK)
		{
			hResult = S_OK;
		}
		else
		{
			sRetrievingErrorDescription = curl_easy_strerror(result);
		}
	}
	else
	{
		sRetrievingErrorDescription = "CURLRetriever::Initialize() should be called first.";
	}

	return hResult;
}

size_t CURLRetriever::OnDataDownloaded(LPVOID pData, size_t nDataBlockCount, size_t nDataBlocSize, LPVOID pStream)
{
	size_t nResult = 0;

	try
	{
		if (pData != NULL && pStream != NULL)
		{
			string *pRetrievedContent = (string*)pStream;
			if (pRetrievedContent != NULL)
			{
				size_t nDataSize = (DWORD)nDataBlockCount * nDataBlocSize;
				if (nDataSize > 0)
				{
					pRetrievedContent->append((LPCSTR)pData, nDataSize);
					nResult = nDataSize;
				}
			}
		}
	}
	catch (...)
	{
		nResult = 0;
	}

	return nResult;
}

