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

// This dll handles the communication to Collective2 from MT4
//
//





#define CURL_STATICLIB

#define WIN32_LEAN_AND_MEAN

#include "stringtok.h"

#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>
#include "convert.h"
#include "CURLRetriever.h"
#include "HTTPRspCodeString.h"
#include "HTTPRspCodeClassString.h"
#include "HTTPRspCodeDescription.h"
#include <windows.h>
#include "xmlParser.h"
#include <list>


#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#define MT4_EXPFUNC __declspec(dllexport)

#define export extern "C" __declspec( dllexport )

#ifdef __cplusplus
extern "C"
{
#endif

    struct MqlStr
    {
        int               len;
        char             *string;
    };

    enum TradeOperation
    {
        OP_BUY = 0,
        OP_SELL = 1,
        OP_BUYLIMIT = 2,
        OP_SELLLIMIT = 3,
        OP_BUYSTOP = 4,
        OP_SELLSTOP = 5
    };

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
    MT4_EXPFUNC void  __stdcall Collective2OrderSend(char *symbol, TradeOperation cmd, const double volume,
            const double stoploss, const double price, const double takeprofit,
            const int orderid, char *systemid, char *password, const int debugFlag, int signalID[],
            int statusCode[], char* statusText)
    {

        CURLRetriever::InitializeCURL();

        std::string aOpenRequest = "";


        //+------------------------------------------------------------------+
        //|                                                                  |
        //+------------------------------------------------------------------+

        aOpenRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signal&systemid=";
        aOpenRequest += systemid;
        aOpenRequest += "&pw=";
        aOpenRequest += password;
        aOpenRequest += "&instrument=forex&action=";

        if (cmd == OP_BUY || cmd == OP_BUYLIMIT || cmd == OP_BUYSTOP)
        {
            aOpenRequest += "BTO&quant=";
        }
        else
        {
            aOpenRequest += "STO&quant=";
        }
        aOpenRequest += stringify(volume).c_str();
        aOpenRequest += "&symbol=";
        aOpenRequest += symbol;

        if (cmd == OP_BUYLIMIT || cmd == OP_SELLLIMIT)
        {
            aOpenRequest += "&limit=";
            aOpenRequest += stringify(price).c_str();
        }

        if (cmd == OP_BUYSTOP || cmd == OP_SELLSTOP)
        {
            aOpenRequest += "&stop=";
            aOpenRequest += stringify(price).c_str();
        }

        if (stoploss > 0)
        {
            aOpenRequest += "&stoploss=";
            aOpenRequest += stringify(stoploss).c_str();
        }

        if (takeprofit > 0)
        {
            aOpenRequest += "&profittarget=";
            aOpenRequest += stringify(takeprofit).c_str();
        }


        aOpenRequest += "&duration=GTC";

        aOpenRequest += "&signalid=";

        aOpenRequest += stringify(orderid).c_str();

        aOpenRequest += "&sourcecode=MT&sourceversion=1.0";

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),aOpenRequest.c_str(),"Request",MB_OK);

        }

        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(aOpenRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent = "";
        std::string sRetrievingErrorDescription = "";
        unsigned int codeClass = 0;
        int retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }

        urlRetriever.Uninitialize();
        CURLRetriever::UninitializeCURL();


        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }

        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");


        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }

        XMLNode xSignalIDNode=xMainNode.getChildNode("signalid");

        if (xSignalIDNode.getText() != NULL)
        {
            signalID[0] = convertToInt(xSignalIDNode.getText());
        }
        else
        {
            statusCode[0] = 99;
            signalID[0] = 0;
        }



    }

    MT4_EXPFUNC void __stdcall Collective2OrderClose(char* symbol, const TradeOperation cmd, const double volume,
            char* systemid, char* password, const int orderid, const char* email,
            const int debugFlag, int signalID[], int statusCode[], char* statusText)
    {

        CURLRetriever::InitializeCURL();

//        //First lets check to see if order was ever sent
//
//        std::string aCheckRequest = "";
//
//        aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signalstatus&signalid=";
//        aCheckRequest += stringify(orderid) ;
//        aCheckRequest += "&pw=";
//        aCheckRequest += password;
//        aCheckRequest += "&c2email=";
//        aCheckRequest += email;
//        aCheckRequest += "&showrelated=children&showdetails=1";
//        aCheckRequest += "&sourcecode=MT&sourceversion=1.0";
//
//        if (debugFlag == 1)
//        {
//            MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);
//
//        }
//
//
//        CURLRetriever urlRetriever;
//
//        HRESULT hResult = urlRetriever.Initialize(aCheckRequest.c_str());
//
//        LONG lResponseCode = 0;
//        std::string sRetrievedContent = "";
//        std::string sRetrievingErrorDescription = "";
//        unsigned int codeClass;
//        int retries = 0;
//        do
//        {
//
//            lResponseCode = 0;
//            sRetrievedContent = "";
//            sRetrievingErrorDescription = "";
//            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
//            codeClass = lResponseCode / 100;
//            retries++;
//        }
//        while (codeClass == 5 && retries < 10);
//
//        if (codeClass > 2)
//        {
//            HTTPRspCodeDescription codeDescr;
//            std::string codeStringText = "Communications problem with server.  Description: ";
//            codeStringText += codeDescr.getInfo( lResponseCode );
//            strcpy(statusText,codeStringText.c_str());
//            statusCode[0] = 99;
//            CURLRetriever::UninitializeCURL();
//            return;
//        }
//
//
//        if (hResult != S_OK)
//        {
//            std::string codeStringText = "Communications problem with server.  Description: ";
//            codeStringText += sRetrievingErrorDescription.c_str();
//            strcpy(statusText,codeStringText.c_str());
//            statusCode[0] = 99;
//            CURLRetriever::UninitializeCURL();
//            return;
//        }
//
//        urlRetriever.Uninitialize();
//
//
//        if (debugFlag == 1)
//        {
//            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
//
//        }
//
//        XMLNode xMainStatusNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");
//
//
//        XMLNode xSCStatusNode=xMainStatusNode.getChildNode("status");
//
//        if (xSCStatusNode.getText() != NULL)
//        {
//            if (strcmp(xSCStatusNode.getText(), "error") == 0)
//            {
//                XMLNode xSCStatusTextNode=xMainStatusNode.getChildNode("errortype");
//
//                if (xSCStatusTextNode.getText() != NULL)
//                {
//                    if (strncasecmp(xSCStatusTextNode.getText(), "No valid signal", 14) == 0)
//                    {
//                        statusCode[0] = 0;
//                        return;
//                    }
//                }
//            }
//
//        }

        //Now send close

        std::string openRequest = "";

        openRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signal&systemid=";
        openRequest += systemid;
        openRequest += "&pw=";
        openRequest += password;
        openRequest += "&instrument=forex&action=";

        if (cmd == OP_BUY || cmd == OP_BUYLIMIT || cmd == OP_BUYSTOP)
        {
            openRequest += "STC&quant=";
        }
        else
        {
            openRequest += "BTC&quant=";
        }


        openRequest += stringify(volume).c_str();
        openRequest += "&symbol=";
        openRequest += symbol;
        openRequest += "&duration=GTC";

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),openRequest.c_str(),"Request",MB_OK);

        }

        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(openRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent = "";
        std::string sRetrievingErrorDescription = "";
        unsigned int codeClass;
        int retries = 0;

        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }

        urlRetriever.Uninitialize();
        CURLRetriever::UninitializeCURL();

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }

        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");

        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }


        XMLNode xSignalIDNode=xMainNode.getChildNode("signalid");

        if (xSignalIDNode.getText() != NULL)
        {
            signalID[0] = convertToInt(xSignalIDNode.getText());
        }
        else
        {
            statusCode[0] = 99;
            signalID[0] = 0;
        }



    }

    MT4_EXPFUNC void __stdcall Collective2OrderDelete(int orderid, const char* systemid, const char* password, int debugFlag, int signalID[],
            int statusCode[], char* statusText)
    {

        std::string openRequest;

        CURLRetriever::InitializeCURL();


        openRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=cancel&systemid=";
        openRequest += systemid;
        openRequest += "&pw=";
        openRequest += password;
        openRequest += "&signalid=";

        openRequest += stringify(orderid).c_str();

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),openRequest.c_str(),"Request",MB_OK);

        }

        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(openRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent;
        std::string sRetrievingErrorDescription;
        unsigned int codeClass;
        int retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }

        urlRetriever.Uninitialize();
        CURLRetriever::UninitializeCURL();

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }

        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");

        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }


    }
#ifndef ORDERMODIFYEXCLUDE
    //+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
    MT4_EXPFUNC void  __stdcall Collective2OrderModify(const int orderid, const double price,
            const double stoploss, const double takeprofit,
            const char* systemid, const char* password, const char* email, const int debugFlag,
            int signalID[], int statusCode[], char* statusText)
    {

        CURLRetriever::InitializeCURL();

        std::string aCheckRequest = "";
        std::string requestResult = "";
        std::string parentAction = "";
        std::string orderSymbol = "";
        std::string orderQuant = "";
        std::string orderOCAGroupID = "";
        std::string stopAction = "";
        std::string limitAction= "";

        int limitOrderID = 0;
        int stopOrderID = 0;
        double stopValue = 0.0;
        double limitValue = 0.0;

        std::string S_res = "0";
        //PCHAR retValue = "0";


        //+------------------------------------------------------------------+
        //|     First need to check for any OCO orders                       |
        //+------------------------------------------------------------------+

        /*

        signal.mpl?cmd=signalstatus&signalid=26618011&pw=myc2password&c2email=matthew@collective2.com
        &showrelated=children&showdetails=1
        */
        aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signalstatus&signalid=";
        aCheckRequest += stringify(orderid) ;
        aCheckRequest += "&pw=";
        aCheckRequest += password;
        aCheckRequest += "&c2email=";
        aCheckRequest += email;
        aCheckRequest += "&showrelated=children&showdetails=1";
        aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

        }


        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(aCheckRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent = "";
        std::string sRetrievingErrorDescription = "";
        unsigned int codeClass;
        int retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
        }

        urlRetriever.Uninitialize();


        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }


        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");


        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }

        XMLNode xParentSignalNode=xMainNode.getChildNode("signal");
        XMLNode xParentAction=xParentSignalNode.getChildNode("action");

        if (xParentAction.getText() != NULL)
        {
            parentAction=std::string(xParentAction.getText());
        }

        XMLNode xOrderQuant=xParentSignalNode.getChildNode("quant");

        if (xOrderQuant.getText() != NULL)
        {
            orderQuant=std::string(xOrderQuant.getText());
        }
        XMLNode xOrderSymbol=xParentSignalNode.getChildNode("symbol");

        if (xOrderSymbol.getText() != NULL)
        {
            orderSymbol=std::string(xOrderSymbol.getText());
        }

        int i = 0;

        //Find the Collective2 signalid for the stoploss and takeprofit order

        //XMLNode nodeChild=xParentSignalNode.getChildNode("child");

        while (i<xParentSignalNode.nChildNode("child"))
        {

            XMLNode nodeChild=xParentSignalNode.getChildNode("child", i);

            XMLNode nodeChildSignal=nodeChild.getChildNode("signal");



            XMLNode childSignalIDNode=nodeChildSignal.getChildNode("signalid");

            XMLNode childLimit=nodeChildSignal.getChildNode("limit");

            if (childLimit.getText() != NULL)
            {
                if (convertToDouble(childLimit.getText()) > 0) // We have a limit order
                {
                    limitOrderID = convertToInt(childSignalIDNode.getText());


                    XMLNode StopLimitNode = nodeChildSignal.getChildNode("action");

                    if (StopLimitNode.getText() != NULL)
                    {
                        limitAction=std::string(StopLimitNode.getText());
                    }

                    XMLNode xOrderOCAGroup=nodeChildSignal.getChildNode("ocagroupid");

                    if (xOrderOCAGroup.getText() != NULL)
                    {
                        orderOCAGroupID=std::string(xOrderOCAGroup.getText());
                    }

                    XMLNode xLimitValue=nodeChildSignal.getChildNode("limit");

                    if (xLimitValue.getText() != NULL)
                    {
                        limitValue= convertToDouble(xLimitValue.getText());
                    }
                }
            }



            XMLNode childStop=nodeChildSignal.getChildNode("stop");

            if (childStop.getText() != NULL)
            {
                if (convertToDouble(childStop.getText()) > 0) // We have a stop action
                {
                    stopOrderID = convertToInt(childSignalIDNode.getText());


                    XMLNode StopActioNode = nodeChildSignal.getChildNode("action");

                    if (StopActioNode.getText() != NULL)
                    {
                        stopAction=std::string(StopActioNode.getText());
                    }

                    XMLNode xOrderOCAGroup=nodeChildSignal.getChildNode("ocagroupid");

                    if (xOrderOCAGroup.getText() != NULL)
                    {
                        orderOCAGroupID=std::string(xOrderOCAGroup.getText());
                    }

                    XMLNode xStopValue=nodeChildSignal.getChildNode("stop");

                    if (xStopValue.getText() != NULL)
                    {
                        stopValue= convertToDouble(xStopValue.getText());
                    }
                }
            }
            i++;
        }

        if (stopOrderID == 0 && stoploss > 0)
        {
            if (parentAction == "STO")
            {
                stopAction = "BTC";
            }

            if (parentAction == "BTO")
            {
                stopAction = "STC";
            }
        }

        if (limitOrderID == 0 && takeprofit > 0)
        {
            if (parentAction == "STO")
            {
                limitAction = "BTC";
            }

            if (parentAction == "BTO")
            {
                limitAction = "STC";
            }
        }

        if (orderOCAGroupID == "")
            orderOCAGroupID = stringify(orderid) ;


        //Need to add parent to OCAGroup
        /*std::string addOCARequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=addtoocagroup&signalid=";
        addOCARequest += stringify(orderid) ;
        addOCARequest += "&pw=";
        addOCARequest += password;
        addOCARequest += "&c2email=";
        addOCARequest += email;
        addOCARequest += "&systemid=";
        addOCARequest += systemid;


        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),addOCARequest.c_str(),"Request",MB_OK);
        }

        hResult = urlRetriever.Initialize(addOCARequest.c_str());

        lResponseCode = 0;
        sRetrievedContent = "";
        sRetrievingErrorDescription = "";
        codeClass = 0;
        retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
            //return with bad error
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
            //return with bad error
        }

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
        }
        urlRetriever.Uninitialize();
        */


        /*        if ((stopOrderID == 0) && (limitOrderID == 0))
                {
                    std::string innerComments = "";
                    //See if the comment field has the SL and TP orderid
                    XMLNode xParentComment=xParentSignalNode.getChildNode("comment");

                    if (xParentComment.getText() != NULL)
                        innerComments=stringDup(xParentAction.getText());

                    std::list<string> ls;
                    int countIter = 0;
                    stringtok (ls, innerComments);
                    for (std::list<string>::const_iterator iter = ls.begin();
                            iter != ls.end(); ++ iter)
                    {
                        if (countIter = 0)
                        {
                            stopOrderID = convertToInt(*iter);
                        }
                        else
                        {
                            limitOrderID = convertToInt(*iter);
                        }

                        countIter++;
                    }

                    if (stopOrderID != 0)
                    {
                        aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signalstatus&signalid=";
                        aCheckRequest += stringify(stopOrderID) ;
                        aCheckRequest += "&pw=";
                        aCheckRequest += password;
                        aCheckRequest += "&c2email=";
                        aCheckRequest += email;
                        aCheckRequest += "&showdetails=1";
                        aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

                        if (debugFlag == 1)
                        {
                            MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

                        }

                        requestResult.clear();
                        requestResult = " ";

                        requestResult = ezcurl.GetStringURL(aCheckRequest);
                        requestResult += " ";

                        XMLNode xMainStopNode=XMLNode::parseString(requestResult.c_str(),"collective2");

                        XMLNode xStopNode=xMainStopNode.getChildNode("signal");

                        XMLNode xStopAction=xStopNode.getChildNode("action");

                        if (xStopAction.getText() != NULL)
                        {
                            stopAction=stringDup(xStopAction.getText());
                        }

                        XMLNode xOrderOCAGroup=xStopNode.getChildNode("ocagroupid");

                        if (xOrderOCAGroup.getText() != NULL)
                        {
                            orderOCAGroupID=stringDup(xOrderOCAGroup.getText());
                        }
                    }

                    if (limitOrderID != 0)
                    {
                        aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signalstatus&signalid=";
                        aCheckRequest += stringify(limitOrderID) ;
                        aCheckRequest += "&pw=";
                        aCheckRequest += password;
                        aCheckRequest += "&c2email=";
                        aCheckRequest += email;
                        aCheckRequest += "&showdetails=1";
                        aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

                        if (debugFlag == 1)
                        {
                            MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

                        }

                        requestResult.clear();

                        requestResult = ezcurl.GetStringURL(aCheckRequest);
                        requestResult += " ";

                        XMLNode xMainLimitNode=XMLNode::parseString(requestResult.c_str(),"collective2");

                        XMLNode xLimitNode=xMainLimitNode.getChildNode("signal");

                        XMLNode xLimitAction=xLimitNode.getChildNode("action");

                        if (xLimitAction.getText() != NULL)
                        {
                            limitAction=stringDup(xLimitAction.getText());
                        }

                        XMLNode xOrderOCAGroup=xLimitNode.getChildNode("ocagroupid");

                        if (xOrderOCAGroup.getText() != NULL)
                        {
                            orderOCAGroupID=stringDup(xOrderOCAGroup.getText());
                        }
                    }
                }
        */
        //Cancel both orders first
        /* if (stopOrderID > 0)
         {
             std::string cancelStopRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=cancel&signalid=";
             cancelStopRequest += stringify(stopOrderID) ;
             cancelStopRequest += "&pw=";
             cancelStopRequest += password;
             cancelStopRequest += "&systemid=";
             cancelStopRequest += systemid;


             if (debugFlag == 1)
             {
                 MessageBox(GetActiveWindow(),cancelStopRequest.c_str(),"Request",MB_OK);
             }

             hResult = urlRetriever.Initialize(cancelStopRequest.c_str());

             lResponseCode = 0;
             sRetrievedContent = "";
             sRetrievingErrorDescription = "";
             codeClass = 0;
             retries = 0;
             do
             {

                 lResponseCode = 0;
                 sRetrievedContent = "";
                 sRetrievingErrorDescription = "";
                 hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
                 codeClass = lResponseCode / 100;
                 retries++;
             }
             while (codeClass == 5 && retries < 10);

             if (codeClass > 2)
             {
                 HTTPRspCodeDescription codeDescr;
                 std::string codeStringText = "Communications problem with server.  Description: ";
                 codeStringText += codeDescr.getInfo( lResponseCode );
                 strcpy(statusText,codeStringText.c_str());
                 statusCode[0] = 99;
                 CURLRetriever::UninitializeCURL();
                 return;
                 //return with bad error
             }


             if (hResult != S_OK)
             {
                 std::string codeStringText = "Communications problem with server.  Description: ";
                 codeStringText += sRetrievingErrorDescription.c_str();
                 strcpy(statusText,codeStringText.c_str());
                 statusCode[0] = 99;
                 CURLRetriever::UninitializeCURL();
                 return;
                 //return with bad error
             }

             if (debugFlag == 1)
             {
                 MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
             }
             urlRetriever.Uninitialize();


         }

         if (limitOrderID > 0)
         {
             std::string cancelStopRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=cancel&signalid=";
             cancelStopRequest += stringify(limitOrderID) ;
             cancelStopRequest += "&pw=";
             cancelStopRequest += password;
             cancelStopRequest += "&systemid=";
             cancelStopRequest += systemid;


             if (debugFlag == 1)
             {
                 MessageBox(GetActiveWindow(),cancelStopRequest.c_str(),"Request",MB_OK);
             }

             hResult = urlRetriever.Initialize(cancelStopRequest.c_str());

             lResponseCode = 0;
             sRetrievedContent = "";
             sRetrievingErrorDescription = "";
             codeClass = 0;
             retries = 0;
             do
             {

                 lResponseCode = 0;
                 sRetrievedContent = "";
                 sRetrievingErrorDescription = "";
                 hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
                 codeClass = lResponseCode / 100;
                 retries++;
             }
             while (codeClass == 5 && retries < 10);

             if (codeClass > 2)
             {
                 HTTPRspCodeDescription codeDescr;
                 std::string codeStringText = "Communications problem with server.  Description: ";
                 codeStringText += codeDescr.getInfo( lResponseCode );
                 strcpy(statusText,codeStringText.c_str());
                 statusCode[0] = 99;
                 CURLRetriever::UninitializeCURL();
                 return;
                 //return with bad error
             }


             if (hResult != S_OK)
             {
                 std::string codeStringText = "Communications problem with server.  Description: ";
                 codeStringText += sRetrievingErrorDescription.c_str();
                 strcpy(statusText,codeStringText.c_str());
                 statusCode[0] = 99;
                 CURLRetriever::UninitializeCURL();
                 return;
                 //return with bad error
             }
             if (debugFlag == 1)
             {
                 MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
             }
             urlRetriever.Uninitialize();

         }
        */
        // Send New stoploss
        if (stoploss > 0 && (stopValue != stoploss))
        {
            aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signal&systemid=";
            aCheckRequest += systemid;
            aCheckRequest += "&pw=";
            aCheckRequest += password;
            aCheckRequest += "&instrument=forex&action=";
            aCheckRequest += stopAction;
            aCheckRequest += "&quant=";
            aCheckRequest += orderQuant;
            aCheckRequest += "&stop=";
            aCheckRequest += stringify(stoploss);
            aCheckRequest += "&symbol=";
            aCheckRequest += orderSymbol;
            aCheckRequest += "&ocaid=";
            aCheckRequest += orderOCAGroupID;
            if (stopOrderID > 0)
            {
                aCheckRequest += "&xreplace=";
                aCheckRequest += stringify(stopOrderID);
            }

            aCheckRequest += "&duration=GTC";
            aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

            if (debugFlag == 1)
            {
                MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

            }

            hResult = urlRetriever.Initialize(aCheckRequest.c_str());

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            codeClass = 0;
            retries = 0;
            do
            {

                lResponseCode = 0;
                sRetrievedContent = "";
                sRetrievingErrorDescription = "";
                hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
                codeClass = lResponseCode / 100;
                retries++;
            }
            while (codeClass == 5 && retries < 10);

            if (codeClass > 2)
            {
                HTTPRspCodeDescription codeDescr;
                std::string codeStringText = "Communications problem with server.  Description: ";
                codeStringText += codeDescr.getInfo( lResponseCode );
                strcpy(statusText,codeStringText.c_str());
                statusCode[0] = 99;
                CURLRetriever::UninitializeCURL();
                return;
                //return with bad error
            }


            if (hResult != S_OK)
            {
                std::string codeStringText = "Communications problem with server.  Description: ";
                codeStringText += sRetrievingErrorDescription.c_str();
                strcpy(statusText,codeStringText.c_str());
                statusCode[0] = 99;
                CURLRetriever::UninitializeCURL();
                return;
                //return with bad error
            }

            if (debugFlag == 1)
            {
                MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
            }

            urlRetriever.Uninitialize();

            XMLNode xMainNode2=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");

            XMLNode xStatusCodeNode=xMainNode2.getChildNode("signalid");

            if (xStatusCodeNode.getText() != NULL)
            {
                stopOrderID = convertToInt(xStatusCodeNode.getText());
            }
        }

        // Change limit

        if (takeprofit > 0 && (limitValue != takeprofit))
        {
            aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signal&systemid=";
            aCheckRequest += systemid;
            aCheckRequest += "&pw=";
            aCheckRequest += password;
            aCheckRequest += "&instrument=forex&action=";
            aCheckRequest += limitAction;
            aCheckRequest += "&quant=";
            aCheckRequest += orderQuant;
            aCheckRequest += "&limit=";
            aCheckRequest += stringify(takeprofit);
            aCheckRequest += "&symbol=";
            aCheckRequest += orderSymbol;
            aCheckRequest += "&ocaid=";
            aCheckRequest += orderOCAGroupID;

            if (limitOrderID > 0)
            {
                aCheckRequest += "&xreplace=";
                aCheckRequest += stringify(limitOrderID);
            }

            aCheckRequest += "&duration=GTC";
            aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

            if (debugFlag == 1)
            {
                MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

            }

            hResult = urlRetriever.Initialize(aCheckRequest.c_str());

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            codeClass = 0;
            retries = 0;
            do
            {

                lResponseCode = 0;
                sRetrievedContent = "";
                sRetrievingErrorDescription = "";
                hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
                codeClass = lResponseCode / 100;
                retries++;
            }
            while (codeClass == 5 && retries < 10);

            if (codeClass > 2)
            {
                HTTPRspCodeDescription codeDescr;
                std::string codeStringText = "Communications problem with server.  Description: ";
                codeStringText += codeDescr.getInfo( lResponseCode );
                strcpy(statusText,codeStringText.c_str());
                statusCode[0] = 99;
                CURLRetriever::UninitializeCURL();
                return;
                //return with bad error
            }


            if (hResult != S_OK)
            {
                std::string codeStringText = "Communications problem with server.  Description: ";
                codeStringText += sRetrievingErrorDescription.c_str();
                strcpy(statusText,codeStringText.c_str());
                statusCode[0] = 99;
                CURLRetriever::UninitializeCURL();
                return;
                //return with bad error
            }

            if (debugFlag == 1)
            {
                MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);
            }

            urlRetriever.Uninitialize();

            XMLNode xMainNode3=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");


            XMLNode xStatusCodeNode3=xMainNode3.getChildNode("signalid");

            if (xStatusCodeNode3.getText() != NULL)
            {
                limitOrderID = convertToInt(xStatusCodeNode3.getText());
            }
        }
        //Update comment field with new stoploss, takeprofit.
        /*
                aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=newcomment&systemid=";
                aCheckRequest += systemid;
                aCheckRequest += "&pw=";
                aCheckRequest += password;
                aCheckRequest += "&signalid=";
                aCheckRequest += stringify(orderid);
                aCheckRequest += "&commentary=";
                aCheckRequest += stringify(stopOrderID);
                aCheckRequest += ",";
                aCheckRequest += stringify(limitOrderID);
                aCheckRequest += "&duration=GTC";
                aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

                if (debugFlag == 1)
                {
                    MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

                }

                requestResult.clear();

                requestResult = ezcurl.GetStringURL(aCheckRequest);
                requestResult += " ";

                if (debugFlag == 1)
                {
                    MessageBox(GetActiveWindow(),requestResult.c_str(),"Result",MB_OK);

                }
        */
        CURLRetriever::UninitializeCURL();

        return;

    }

#endif

  MT4_EXPFUNC void  __stdcall Collective2CheckLimits(const int orderid,
            const char* systemid, const char* password, const char* email, const int debugFlag,
            char* stoplossTradeEntered, double stoplossTradedPrice[],
            char* takeprofitTradeEntered, double takeprofitTradedPrice[],
            int statusCode[], char* statusText)
    {

        CURLRetriever::InitializeCURL();

        std::string aCheckRequest = "";



        int limitOrderID = 0;
        int stopOrderID = 0;
        double stopValue = 0.0;
        double limitValue = 0.0;

        std::string S_res = "0";

        aCheckRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=signalstatus&signalid=";
        aCheckRequest += stringify(orderid) ;
        aCheckRequest += "&pw=";
        aCheckRequest += password;
        aCheckRequest += "&c2email=";
        aCheckRequest += email;
        aCheckRequest += "&showrelated=children&showdetails=1";
        aCheckRequest += "&sourcecode=MT&sourceversion=1.0";

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),aCheckRequest.c_str(),"Request",MB_OK);

        }


        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(aCheckRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent = "";
        std::string sRetrievingErrorDescription = "";
        unsigned int codeClass;
        int retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
            CURLRetriever::UninitializeCURL();
            return;
        }

        urlRetriever.Uninitialize();


        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }


        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");


        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }

        XMLNode xParentSignalNode=xMainNode.getChildNode("signal");


        int i = 0;

        //Find the Collective2 signalid for the stoploss and takeprofit order


        while (i<xParentSignalNode.nChildNode("child"))
        {

            XMLNode nodeChild=xParentSignalNode.getChildNode("child", i);

            XMLNode nodeChildSignal=nodeChild.getChildNode("signal");

            XMLNode childSignalIDNode=nodeChildSignal.getChildNode("signalid");

            XMLNode childLimit=nodeChildSignal.getChildNode("limit");

            if (childLimit.getText() != NULL)
            {
                if (convertToDouble(childLimit.getText()) > 0) // We have a limit order
                {

                    XMLNode LimitTradedWhen = nodeChildSignal.getChildNode("tradedwhen");

                    if (LimitTradedWhen.getText() != NULL)
                    {
                        strcpy(takeprofitTradeEntered,LimitTradedWhen.getText());
                    }

                    XMLNode LimitTradedPrice = nodeChildSignal.getChildNode("tradeprice");

                    if (LimitTradedPrice.getText() != NULL)
                    {
                        takeprofitTradedPrice[0] = convertToDouble(LimitTradedPrice.getText());
                    }

                }
            }



            XMLNode childStop=nodeChildSignal.getChildNode("stop");

            if (childStop.getText() != NULL)
            {
                if (convertToDouble(childStop.getText()) > 0) // We have a stop action
                {
                    XMLNode StopTradedWhen = nodeChildSignal.getChildNode("tradedwhen");

                    if (StopTradedWhen.getText() != NULL)
                    {
                        strcpy(stoplossTradeEntered,StopTradedWhen.getText());
                    }

                    XMLNode StopTradedPrice = nodeChildSignal.getChildNode("tradeprice");

                    if (StopTradedPrice.getText() != NULL)
                    {
                        stoplossTradedPrice[0] = convertToDouble(StopTradedPrice.getText());
                    }
                }
            }
            i++;
        }

        CURLRetriever::UninitializeCURL();

        return;

    }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
    MT4_EXPFUNC double  __stdcall Collective2CheckBuyingPower(char *systemid, char *password, const int debugFlag,
            int statusCode[], char* statusText)
    {

        CURLRetriever::InitializeCURL();

        double lBuyingPower = 0.0;

        std::string aOpenRequest = "";


        //+------------------------------------------------------------------+
        //|                                                                  |
        //+------------------------------------------------------------------+

        aOpenRequest = "http://www.collective2.com/cgi-perl/signal.mpl?cmd=getbuypower&systemid=";
        aOpenRequest += systemid;
        aOpenRequest += "&pw=";
        aOpenRequest += password;
         aOpenRequest += "&sourcecode=MT&sourceversion=1.0";

        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),aOpenRequest.c_str(),"Request",MB_OK);

        }

        CURLRetriever urlRetriever;

        HRESULT hResult = urlRetriever.Initialize(aOpenRequest.c_str());

        LONG lResponseCode = 0;
        std::string sRetrievedContent = "";
        std::string sRetrievingErrorDescription = "";
        unsigned int codeClass = 0;
        int retries = 0;
        do
        {

            lResponseCode = 0;
            sRetrievedContent = "";
            sRetrievingErrorDescription = "";
            hResult = urlRetriever.Retrieve(lResponseCode, sRetrievedContent, sRetrievingErrorDescription);
            codeClass = lResponseCode / 100;
            retries++;
        }
        while (codeClass == 5 && retries < 10);

        if (codeClass > 2)
        {
            HTTPRspCodeDescription codeDescr;
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += codeDescr.getInfo( lResponseCode );
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }


        if (hResult != S_OK)
        {
            std::string codeStringText = "Communications problem with server.  Description: ";
            codeStringText += sRetrievingErrorDescription.c_str();
            strcpy(statusText,codeStringText.c_str());
            statusCode[0] = 99;
        }

        urlRetriever.Uninitialize();
        CURLRetriever::UninitializeCURL();


        if (debugFlag == 1)
        {
            MessageBox(GetActiveWindow(),sRetrievedContent.c_str(),"Result",MB_OK);

        }

        XMLNode xMainNode=XMLNode::parseString(sRetrievedContent.c_str(),"collective2");


        XMLNode xStatusCodeNode=xMainNode.getChildNode("status");

        if (xStatusCodeNode.getText() != NULL)
        {
            if (strcmp(xStatusCodeNode.getText(), "error") == 0)
            {
                statusCode[0] = 99;
                XMLNode xStatusTextNode=xMainNode.getChildNode("errortype");

                if (xStatusTextNode.getText() != NULL)
                {
                    strcpy(statusText,xStatusTextNode.getText());
                }
            }
            else
            {
                statusCode[0] = 0;
            }
        }
        else
        {
            statusCode[0] = 0;
        }

        XMLNode xBuyPowerNode=xMainNode.getChildNode("buypower");

        if (xBuyPowerNode.getText() != NULL)
        {
            lBuyingPower = convertToDouble(xBuyPowerNode.getText());
        }
        else
        {
            statusCode[0] = 99;
            lBuyingPower = 0.0;
        }

        return (lBuyingPower);

    }


    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
        switch (fdwReason)
        {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
        }
        return TRUE; // succesful
    }
#ifdef __cplusplus
}
#endif
