//+------------------------------------------------------------------+
//|                                                 TradeMonitor.mq4 |
//|                     Copyright © 2010, OpenThinking Software, LLC |
//|                              http://www.openthinkingsoftware.com |
//+------------------------------------------------------------------+
#property copyright "Copyright © 2010, OpenThinking Software, LLC"
#property link      "http://www.openthinkingsystems.com"

#import "MT4C2SignalEntry.dll"
void Collective2OrderSend(string symbol, int cmd, double volume, double stoploss, double price, 
                          double takeprofit, int orderid, string systemid, string password, int debugFlag, 
                          int signalID[], int statusCode[], string statusText);
void Collective2OrderClose(string symbol, int cmd, double volume, string systemid, string password, 
                           int debugFlag, int signalID[], int statusCode[], string statusText);
void Collective2OrderDelete(int orderid, string systemid, string password, int debugFlag, int signalID[],
                              int statusCode[], string statusText);
void  Collective2OrderModify(int orderid, double price, double stoploss, double takeprofit,
            string systemid, string password, string email, int debugFlag,
            int signalID[], int statusCode[], string statusText);
#import

extern string C2SystemID = "";
extern string C2Password = "";
extern string C2Email = "";
extern double C2LotMultiplier = 0;
extern bool Ignore_MT4Stoploss = false;
extern bool Ignore_MT4TakeProfit = false;
extern double SecondsBetweenPolling = 4;
extern bool SendOnlyMarketOrders = false;
extern bool TurnOnDebug = false;

int    g_StoredOrderTicket[];             //    OrderTicket()
string g_StoredOrderSymbol[];          //OrderSymbol()
int    g_StoredOrderType[];             //      OrderType()
double g_StoredOrderOpenPrice[];             //     OrderOpenPrice()
double g_StoredOrderStopLoss[];             //     OrderStopLoss()
double g_StoredOrdeTakeProfit[];             //     OrderTakeProfit()
double g_StoredOrderLots[];             //     OrderLots()
datetime g_StoredOrderOpenTime[];
bool g_StoredHedgeTrade[];

int debugFlag = 0;
bool badStart = false;

//bool   g_bFirstRun=True;     //


//+----------------------------------------------------------------------------+
//|  Custom indicator initialization function                                  |
//+----------------------------------------------------------------------------+
void init()
{
    
    badStart = false;
    //TODO - add init checks on variables
    if (C2SystemID == "")
    {
        Alert("Collective2 System ID not set.  This external needs to be set to enable communication with Collective2.");
        badStart = true;
    }

    if (C2Password == "")
    {
        Alert ("Collective2 Password not set.  This external needs to be set to enable communication with Collective2.");
        badStart = true;
    }

    if (C2Email== "")
    {
        Alert ("Collective2 Email not set.  This external needs to be set to enable communication with Collective2.");
        badStart = true;
    }
    if (C2LotMultiplier== 0)
    {
        Alert ("Collective2 Lot Multipler not set.  This variable needs to be set to enable communication with Collective2.");
        badStart = true;
    }
    
    if (!IsDllsAllowed())
    {
        Alert ("Please enable DLLs before using TradeMonitorBasic. Go to Tools, Options, Expert Advisors and check Allow DLLs. Then press F7 and do the same on the Common tab. ");
        badStart = true;
    }

    if (GlobalVariableCheck("TradeMonitorRunning"))
    {
        Alert ("TradeMonitor is already running.  Only one should be attached to your account at any time");
        badStart = true;
    }
    else
        GlobalVariableSet("TradeMonitorRunning", CurTime());

    ResetOrderArray();
    if (TurnOnDebug == true)
        debugFlag = 1;
}

//+----------------------------------------------------------------------------+
//|  Custom indicator deinitialization function                                |
//+----------------------------------------------------------------------------+
void deinit()
{
    GlobalVariableDel("TradeMonitorRunning");
}

//+----------------------------------------------------------------------------+
//|  Custom indicator iteration function                                       |
//+----------------------------------------------------------------------------+
void start()
{

    if (badStart == true)
    {
        return(0);
    }
    
    
    bool LoopFlag = true;
    int loopPoll = SecondsBetweenPolling * 1000;

    while (LoopFlag == true)
    {
        processTrades();
        //if (orderCount >= 5) LoopFlag = false;
        Sleep(loopPoll);
    }

    return(0);
}

void processTrades()
{
    double p;
    int    d;
    int    i;
    int    in;
    int    k=OrdersTotal();
    int    ot;
    int    origk=k;

    int    StoredOrderTicket[];             //    OrderTicket()
    string StoredOrderSymbol[];
    int    StoredOrderType[];             //      OrderType()
    double StoredOrderOpenPrice[];             //     OrderOpenPrice()
    double StoredOrderStopLoss[];             //     OrderStopLoss()
    double StoredOrderTakeProfit[];             //     OrderTakeProfit()
    double StoredOrderLots[];
    datetime StoredOrderOpenTime[];
    bool   StoredHedgeTrade[];
   
    // Store current orders to compare against old data.

    ArrayResize(StoredOrderTicket, k);
    ArrayResize(StoredOrderSymbol, k);
    ArrayResize(StoredOrderType, k);
    ArrayResize(StoredOrderOpenPrice, k);
    ArrayResize(StoredOrderStopLoss, k);
    ArrayResize(StoredOrderTakeProfit, k);
    ArrayResize(StoredOrderLots, k);
    ArrayResize(StoredOrderOpenTime, k);
    ArrayResize(StoredHedgeTrade, k);

    for (i=0; i<k; i++)
    {
        if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
        {
            StoredOrderTicket[i]=OrderTicket();
            StoredOrderSymbol[i]=OrderSymbol();
            StoredOrderType[i]=OrderType();
            StoredOrderOpenPrice[i]=OrderOpenPrice();
            StoredOrderTakeProfit[i]=OrderTakeProfit();
            StoredOrderStopLoss[i]=OrderStopLoss();
            StoredOrderLots[i]=OrderLots();
            StoredOrderOpenTime[i]=OrderOpenTime();
            if (SendOnlyMarketOrders == true)
               StoredHedgeTrade[i]=CheckIfHedgeTradeMarketOnly(OrderSymbol(), OrderType(), OrderOpenTime());  
            else
               StoredHedgeTrade[i]=CheckIfHedgeTrade(OrderSymbol(), OrderType(), OrderOpenTime());
         }
    }
    
    //  Array is now populated.
    // Check to see if order was closed or deleted
    // Search for old OrderTicket in new list.

    k=ArraySize(g_StoredOrderTicket);
    
    for (i=0; i<k; i++)
    {
        if (ArraySearchInt(StoredOrderTicket, g_StoredOrderTicket[i])<0)
        {
            ot=g_StoredOrderType[i];

            //Send closed / delete

            //Send close if market else send delete
            if ((ot == OP_BUY) || (ot == OP_SELL))
                if (g_StoredHedgeTrade[i] == false)
                  SendCloseOrder(g_StoredOrderTicket[i], g_StoredOrderLots[i], g_StoredOrderSymbol[i], g_StoredOrderType[i]);
                else
                  Print("Hedging is not allowed at Collective2 therefore close for ", g_StoredOrderSymbol[i], " is ignored");
            else
                if (g_StoredHedgeTrade[i] == false)
                  SendDeleteOrder(g_StoredOrderTicket[i]);
                else
                  Print("Hedging is not allowed at Collective2 therefore delete for ", g_StoredOrderSymbol[i], " is ignored");

        }
    }

    k=origk;
    
    for (i=0; i<k; i++)
    {
            d=MarketInfo(StoredOrderSymbol[i], MODE_DIGITS);
            p=MarketInfo(StoredOrderSymbol[i], MODE_POINT);
         
            // Search for OrderTicket in old array
            in=ArraySearchInt(g_StoredOrderTicket, StoredOrderTicket[i]);

            // If not found trigger signal.

            if (in<0)
            {

                if (((ot==OP_BUY || ot==OP_SELL) && (SendOnlyMarketOrders == true)) ||
                    (SendOnlyMarketOrders == false))
                if (StoredHedgeTrade[i] == false)
                  SendNewOrder(StoredOrderSymbol[i], StoredOrderType[i], StoredOrderLots[i], StoredOrderStopLoss[i], StoredOrderOpenPrice[i],
                             StoredOrderTakeProfit[i], StoredOrderTicket[i]);
                else
                  Print("Hedging is not allowed at Collective2 therefore trade for ", StoredOrderSymbol[i], " is ignored");


            }
            else
            {
                //If found and ordertype has changed (limit turned to market)
                ot=StoredOrderType[i];
                if (ot!=g_StoredOrderType[in] && (ot==OP_BUY || ot==OP_SELL))
                {

                   if (SendOnlyMarketOrders == true)
                    //Send changed order type
                     if (StoredHedgeTrade[i] == false)
                        SendNewOrder(StoredOrderSymbol[i], StoredOrderType[i], StoredOrderLots[i], StoredOrderStopLoss[i], StoredOrderOpenPrice[i],
                             StoredOrderTakeProfit[i], StoredOrderTicket[i]);
                    //We currently don't do anything since client should have picked up converted order.
                }
                
                //Check to see if changed to nonhedge
                if (((ot==OP_BUY || ot==OP_SELL) && (SendOnlyMarketOrders == true)) ||
                    (SendOnlyMarketOrders == false))
                if ((StoredHedgeTrade[i] == false) && (g_StoredHedgeTrade[in] == true))
                {
                  //Send as new since no longer hedge trade
                  SendNewOrder(StoredOrderSymbol[i], StoredOrderType[i], StoredOrderLots[i], StoredOrderStopLoss[i], StoredOrderOpenPrice[i],
                             StoredOrderTakeProfit[i], StoredOrderTicket[i]);
                
                }
                // Check to see if altered order
                if ((MathAbs(StoredOrderOpenPrice[i]-g_StoredOrderOpenPrice[in])>=p)
                        ||  (MathAbs(StoredOrderStopLoss[i]-g_StoredOrderStopLoss[in])>=p)
                        ||  (MathAbs(StoredOrderTakeProfit[i]-g_StoredOrdeTakeProfit[in])>=p))
                {

                    //Send modified order

                    if (StoredHedgeTrade[i] == false)
                        SendOrderModify(StoredOrderTicket[i], StoredOrderOpenPrice[i], StoredOrderStopLoss[i], StoredOrderTakeProfit[i], g_StoredOrderStopLoss[in], g_StoredOrdeTakeProfit[in], g_StoredOrderOpenPrice[in]);
                    else
                        Print("Hedging is not allowed at Collective2 therefore modify for ", StoredOrderSymbol[i], " is ignored");

                }
            }
        
    }

    
    // Repopulate Array for next tick

    k = ArraySize(StoredOrderTicket);

    // Copy saved array for next loop

    ArrayResize(g_StoredOrderTicket, k);

    ArrayResize(g_StoredOrderSymbol, k);

    ArrayResize(g_StoredOrderType, k);

    ArrayResize(g_StoredOrderOpenPrice, k);

    ArrayResize(g_StoredOrderStopLoss, k);

    ArrayResize(g_StoredOrdeTakeProfit, k);

    ArrayResize(g_StoredOrderLots, k);

    ArrayResize(g_StoredOrderOpenTime, k);


    if (k > 0)

    {

        ArrayCopy(g_StoredOrderTicket, StoredOrderTicket);

        ArrayCopy(g_StoredOrderSymbol, StoredOrderSymbol);

        ArrayCopy(g_StoredOrderType, StoredOrderType);

        ArrayCopy(g_StoredOrderOpenPrice, StoredOrderOpenPrice);

        ArrayCopy(g_StoredOrderStopLoss, StoredOrderStopLoss);

        ArrayCopy(g_StoredOrdeTakeProfit, StoredOrderTakeProfit);

        ArrayCopy(g_StoredOrderLots, StoredOrderLots);
        
        ArrayCopy(g_StoredOrderOpenTime, StoredOrderOpenTime);
        
        ArrayCopy(g_StoredHedgeTrade, StoredHedgeTrade);

    }

}


int ArraySearchInt(int& m[], int e)
{
    for (int i=0; i<ArraySize(m); i++)
    {
        if (m[i]==e) return(i);
    }
    return(-1);
}

void SendNewOrder(string symbol, int cmd, double volume, double stoploss, double price,
                  double takeprofit, int orderid)
{
    double C2LotSize = volume * C2LotMultiplier;
    string c2sigid = "0";
    double stoplossValue = 0.0;
    double takeprofitValue = 0.0;
    string edittedSymbol = StringSubstr(symbol,0,3) + "/" + StringSubstr(symbol,3,3);

    if (MathFloor(C2LotSize) != C2LotSize)
    {
        Print ("Lot size calculation is incorrect.");
        Print ("MetaTrader lot size multiplied by C2LotMultiplier");
        Print ("must be a whole number and greater than zero.");
        Print ("lot size of order: ", volume);
        Print ("C2LotMultiplier: ", C2LotMultiplier);
        Print ("lot size * C2LotMultipler = ", C2LotSize);
        return(0);
    }

    if (Ignore_MT4Stoploss == true)
        stoplossValue = 0.0;
    else
        stoplossValue = stoploss;


    if (Ignore_MT4TakeProfit == true)
        takeprofitValue = 0.0;
    else
        takeprofitValue = takeprofit;

    int tries = 0;

    while (tries < 5)
    {
        Print("Sending Open Signal for ", edittedSymbol, " at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");
        
        int signalID[1] = {0};
        int statusCode[1] = {0};
        string statusText =   "88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888";
        Collective2OrderSend(StringSubstr(symbol, 0, 6), cmd, C2LotSize, stoplossValue,
                             price, takeprofitValue, orderid, C2SystemID, C2Password, debugFlag,
                             signalID, statusCode, statusText);
        if (statusCode[0] != 0)
        {
            Print("Error received from Collective2:");
            Print(statusText);
            Print("Resending Collective2 Open Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");
            tries++;
        }
        else
        {
            tries = 10;
        }
        Sleep(10000);
    }

    if (statusCode[0] != 0)
        Alert ("Order did not get sent to Collective2.  The following error was return - ", statusText);
    else
        Print("Collective2 Open Signal Sent. Signal ID is ", signalID[0]);

}

void SendCloseOrder(int ticket, double lots, string symbol, int cmd)
{

    string edittedSymbol = StringSubstr(symbol,0,3) + "/" + StringSubstr(symbol,3,3);

    Print("Sending Collective2 Close Signal for ", edittedSymbol, " at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");

    double lotsToClose = lots;

    double C2LotSize = lotsToClose * C2LotMultiplier;
    int tries = 0;

    while (tries < 5)
    {
        int signalID[1] = {0};
        int statusCode[1] = {0};
        string statusText =   "88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888";
        Collective2OrderClose(StringSubstr(symbol, 0, 6), cmd, C2LotSize, C2SystemID, C2Password, debugFlag,
                              signalID, statusCode, statusText);
        if (statusCode[0] != 0)
        {
            Print("Error received from Collective2:");
            Print(statusText);
            Print("Resending Collective2 Close Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");
            tries++;
        }
        else
        {
            tries = 10;
        }
        Sleep(10000);
    }

    if (statusCode[0] != 0)
        Alert ("Order did not get closed at Collective2.  The following error was return - ", statusText);
    else
        Print("Collective2 Close Signal Sent. Signal ID is ", signalID[0]);
}

void SendDeleteOrder(int ticket)
{
    Print("Sending Collective2 Delete Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");

    int tries = 0;

    while (tries < 5)
    {
        int signalID[1] = {0};
        int statusCode[1] = {0};
        string statusText =   "88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888";
        Collective2OrderDelete(ticket, C2SystemID, C2Password, debugFlag, signalID, statusCode, statusText);
        
        if (statusCode[0] != 0)
        {
            Print("Error received from Collective2:");
            Print(statusText);
            Print("Resending Collective2 Delete Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");
            tries++;
        }
        else
        {
            tries = 10;
        }
        Sleep(10000);
    }

    if (statusCode[0] != 0)
        Alert ("Order did not get deleted at Collective2.  The following error was return - ", statusText);
    else
        Print("Collective2 Delete Signal Sent. Signal ID is ", signalID[0]);
}

void SendOrderModify(int orderid, double price, double stoploss, double takeprofit, double origstoploss, double origtakeprofit, double origprice)
{

    double calcedSL = 0.0;
    double calcedTP = 0.0;

    if (Ignore_MT4Stoploss == true)
        calcedSL = 0.0;
    else
        calcedSL = stoploss;

    if (Ignore_MT4TakeProfit == true)
        calcedTP = 0.0;
    else
        calcedTP = takeprofit;

    if ((Ignore_MT4Stoploss == false && origstoploss != stoploss) ||
            (Ignore_MT4TakeProfit == false && origtakeprofit != takeprofit) ||
            (Ignore_MT4TakeProfit == true && Ignore_MT4Stoploss == true && origprice != price ))
    {

        Print("Sending Modify Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES));

            int tries = 0;

    while (tries < 5)
    {
        int signalID[1] = {0};
        int statusCode[1] = {0};
        string statusText =   "88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888";
        Collective2OrderModify(orderid, price, calcedSL, calcedTP, C2SystemID, C2Password, C2Email, debugFlag,
                              signalID, statusCode, statusText);
        
        if (statusCode[0] != 0)
        {
            Print("Error received from Collective2:");
            Print(statusText);
            Print("Resending Collective2 Modify Signal at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), ".");
            tries++;
        }
        else
        {
            tries = 10;
        }
        Sleep(10000);
    }

    if (statusCode[0] != 0)
        Alert ("Order did not get modified at Collective2.  The following error was return - ", statusText);
    else
        Print("Collective2 Modify Signal Sent at ", TimeToStr(CurTime(),TIME_DATE|TIME_MINUTES), "." );
        

    }
}

bool CheckIfHedgeTrade(string sym, int cmd, datetime ordertime)
{

    int    k=OrdersTotal();
    bool   retVal = false;

    for (int i=0; i<k; i++)
    {
        if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
        {
            if (sym == OrderSymbol())
            {
               if (cmd != OrderType())
               {
                  if (((cmd == OP_BUY || cmd == OP_BUYLIMIT || cmd == OP_BUYSTOP) &&
                       (OrderType() == OP_SELL || OrderType() == OP_SELLLIMIT || OrderType() == OP_SELLSTOP)) ||
                      ((OrderType() == OP_BUY || OrderType() == OP_BUYLIMIT || OrderType() == OP_BUYSTOP) &&
                       (cmd == OP_SELL || cmd == OP_SELLLIMIT || cmd == OP_SELLSTOP)))
                       if (ordertime > OrderOpenTime())
                        retVal = true;
               }
       
            }
            
         }
    }
    return(retVal);
}

bool CheckIfHedgeTradeMarketOnly(string sym, int cmd, datetime ordertime)
{

    int    k=OrdersTotal();
    bool   retVal = false;

    for (int i=0; i<k; i++)
    {
        if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
        {
            if (sym == OrderSymbol())
            {
               if (cmd != OrderType())
               {
                  if (((cmd == OP_BUY ) &&
                       (OrderType() == OP_SELL)) ||
                      ((OrderType() == OP_BUY) &&
                       (cmd == OP_SELL)))
                       if (ordertime > OrderOpenTime())
                        retVal = true;
               }
       
            }
            
         }
    }
    return(retVal);
}
//+----------------------------------------------------------------------------+
//|  Clearing up the memory for orders                                         |
//+----------------------------------------------------------------------------+
void ResetOrderArray()
{
    int i, k=OrdersTotal();

    ArrayResize(g_StoredOrderTicket, k);
    ArrayResize(g_StoredOrderSymbol, k);
    ArrayResize(g_StoredOrderType, k);
    ArrayResize(g_StoredOrderOpenPrice, k);
    ArrayResize(g_StoredOrderStopLoss, k);
    ArrayResize(g_StoredOrdeTakeProfit, k);
    ArrayResize(g_StoredOrderLots, k);
    ArrayResize(g_StoredOrderOpenTime, k);
    ArrayResize(g_StoredHedgeTrade, k);
    
    for (i=0; i<k; i++)
    {
        if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
        {
            g_StoredOrderTicket[i]=OrderTicket();
            g_StoredOrderSymbol[i]=OrderSymbol();
            g_StoredOrderType[i]=OrderType();
            g_StoredOrderOpenPrice[i]=OrderOpenPrice();
            g_StoredOrderStopLoss[i]=OrderStopLoss();
            g_StoredOrdeTakeProfit[i]=OrderTakeProfit();
            g_StoredOrderLots[i]=OrderLots();
            g_StoredOrderOpenTime[i]=OrderOpenTime();
            g_StoredHedgeTrade[i]=CheckIfHedgeTrade(OrderSymbol(), OrderType(), OrderOpenTime());
        }
    }
}

