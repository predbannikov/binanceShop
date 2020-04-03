#ifndef GLOBAL_H
#define GLOBAL_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QDateTime>
#include <QDebug>
//#include "watch.h"

//#define     SMA1                7    //10-15m          4-5m
//#define     SMA2                10    //200-15m        70-5m
//#define     SMA3                7    //80-15m          20-5m

#define     TIME_FRAME          "1m"
#define     PERCENT             0.8
#define     BET                 10.01
#define     KOMMISION           0.075


#define     DEBUG_TEST      0
#define     HOST_MACHINES   "http://192.168.0.105:33333"
#define     HOST_MACHINES_WS   "ws://192.168.0.105:1234"

//#define     GET_COUNT_CANDLE    "100"

//#define     DIFF1               0.00001
//#define     DIFF2               0.00017
//#define     DIFF3               0.00001

//#define     DIFF_1_CHANGES24    0.00001
//#define     DIFF_2_CHANGES24    0.00001
//#define     TFRAME_1M_CHANGES24  "1m"
//#define     TFRAME_15M_CHANGES24  "15m"
//#define     TFRAME_30M_CHANGES24  "30m"
//#define     TFRAME_1H_CHANGES24  "1h"
//#define     TFRAME_2H_CHANGES24  "2h"
//#define     TFRAME_5M_CHANGES24  "5m"
//#define     SMA1_CHANGES24      4
//#define     SMA2_CHANGES24      7
//#define     POLLING_CHANGE24    300000


//#define MIN_USDT    1000000000
//#define MIN_BNB     50000000
//#define MIN_ETH     30000000
//#define MIN_BTC     2000

//#define     COUNT_PLUS      17


enum STATEWATCH { STATEBUY, STATESELL, STATEWAIT, STATELOCK};
enum STATE_TRADE {IN_TRADE_SELL, IN_TRADE_BUY, IN_CANCELED_BUY, IN_CANCELED_SELL, IN_BUY, IN_SELL};
enum STATE_QUOTE_TRADE {TRADE_REPLACE, TRADE_BREAK_BET, TRADE_WAIT};


//******************************
//*
struct RateLimits{
    QString rateLimitType;
    QString interval;
    int limit;
};


struct PriceFilter {
    QString minPrice;
    QString maxPrice;
    QString tickSize;
};

struct LotSize {
    QString minQty;
    QString maxQty;
    QString stepSize;
};

struct MinNotional {
    QString minNotional;
};

struct Filters {
    PriceFilter priceFilter;
    LotSize lotSize;
    MinNotional minNotional;
};


struct Symbols{
      QString symbol;           //  "ETHBTC"
      QString status;           //  "TRADING"
      QString baseAsset;        //  "ETH"
      int baseAssetPrecision;
      QString quoteAsset;       //  "BTC"
      int quotePrecision;
      QList<QString> orderTypes;//  ["LIMIT", "MARKET"]
      bool icebergAllowed;
      Filters filters;
};

struct ExchangeInfo {
    QString timezone;
    quint64 serverTime;
    QList<RateLimits> rateLimits;
    QMap<QString, Symbols> symbols;
};
//*
//*******************************************

//*******************************************
//*
struct Kline {
    qint64     t;      // Kline start time
    qint64     T;      // Kline close time
    QString     s;      // Symbol
    QString     i;      // Interval
    int         f;      // First trade ID
    int         L;      // Last trade ID
    double      o;      // Open price
    double      c;      // Close price
    double      h;      // High price
    double      l;      // Low price
    double      v;      // Base asset volume
    int         n;      // Number of trades
    bool        x;      // Is this kline closed?
    double      q;      // Quote asset volume
    double      V;      // Taker buy base asset volume
    double      Q;      // Taker buy quote asset volume
    QString     B;      // Ignore
};

struct CandLestick {
    QString     e;      // Event type
    QDateTime   E;      // Event time
    QString     s;      // Symbol
    Kline       k;
    double      sma1;
    double      sma2;
    double      sma3;
    double      macd;
    double      historgram;
    double      signal;
//    double      countPlus;
};
//*
//*******************************************

//*******************************************
//*
struct Order{
    QString symbol;
    int orderId;
    QString clientOrderId;
    quint64 transactTime;
    QString price;
    QString origQty;
    QString executedQty;
    QString status;
    QString timeInForce;
    QString type;
    QString side;
};
//*
//*******************************************

//*******************************************
//*
struct Balances {
    QString a;  // for example LTC"
    QString  f;  // free amount
    QString  l;  // locked amount
};

struct AccountInfo {
    QString         e;
    QDateTime       E;  // 1499405658849,           // Event time
    double          m;  // 0,                       // Maker commission rate (bips)
    double          t;  // 0,                       // Taker commission rate (bips)
    double          b;  // 0,                       // Buyer commission rate (bips)
    double          s;  // 0,                       // Seller commission rate (bips)
    bool            T;  // true,                    // Can trade?
    bool            W;  // true,                    // Can withdraw?
    bool            D;  // true,                    // Can deposit?
    QDateTime       u;  // 1499405658848,           // Time of last account update
    QHash<QString, Balances> B;                      //// Balances array
//                      [
//                          {
//                              "a": "LTC",               // Asset
//                              "f": "17366.18538083",    // Free amount
//                              "l": "0.00000000"         // Locked amount
//                          },
//                      ]
};
//*
//*******************************************

//*******************************************
//*
struct EventReport {
    QString         e;      // Event type
    QDateTime       E;      // Event time
    QString         s;      // Symbol
    QString         c;      // Client order ID
    QString         S;      // Side
    QString         o;      // Order type
    QString         f;      // Time in force
    QString         q;      // Order quantity
    QString         p;      // Order price
    QString         P;      // Stop price
    QString         F;      // Iceberg quantity
    int             g;      // Ignore
    QString         C;      // Original client order ID; This is the ID of the order being canceled
    QString         x;      // Current execution type
    QString         X;      // Current order status
    QString         r;      // Order reject reason; will be an error code.
    int             i;      // Order ID
    QString         l;      // Last executed quantity
    QString         z;      // Cumulative filled quantity
    QString         L;      // Last executed price
    QString         n;      // Commission amount
    QString         N;      // Commission asset
    QDateTime       T;      // Transaction time
    int             t;      // Trade ID
    int             I;      // Ignore
    bool            w;      // Is the order working? Stops will have
    bool            m;      // Is this trade the maker side?
    QString         M;      // Ignore
};

//*
//*******************************************

//*******************************************
//*

struct Ticker {
    QString e;      // "e": "24hrTicker",  // Event type
    quint64 E;      // "E": 123456789,     // Event time
    QString s;      // "s": "BNBBTC",      // Symbol
    double  p;      // "p": "0.0015",      // Price change
    double  P;      // "P": "250.00",      // Price change percent
    double  w;      // "w": "0.0018",      // Weighted average price
    double  x;      // "x": "0.0009",      // Previous day's close price
    double  c;      // "c": "0.0025",      // Current day's close price
    double  Q;      // "Q": "10",          // Close trade's quantity
    double  b;      // "b": "0.0024",      // Best bid price
    double  B;      // "B": "10",          // Best bid quantity
    double  a;      // "a": "0.0026",      // Best ask price
    double  A;      // "A": "100",         // Best ask quantity
    double  o;      // "o": "0.0010",      // Open price
    double  h;      // "h": "0.0025",      // High price
    double  l;      // "l": "0.0010",      // Low price
    int     v;      // "v": "10000",       // Total traded base asset volume
    int     q;      // "q": "18",          // Total traded quote asset volume
    quint64 O;      // "O": 0,             // Statistics open time
    quint64 C;      // "C": 86400000,      // Statistics close time
    quint64 F;      // "F": 0,             // First trade ID
    int     L;      // "L": 18150,         // Last trade Id
    int     n;      // "n": 18151          // Total number of trades
};

//*
//*******************************************

//*******************************************
//*

struct Ticker24 {
    QString symbol;
    QString priceChange;
    QString priceChangePercent;
    QString weightedAvgPrice;
    QString prevClosePrice;
    QString lastPrice;
    QString lastQty;
    QString bidPrice;
    QString askPrice;
    QString openPrice;
    QString highPrice;
    QString lowPrice;
    QString volume;
    QString quoteVolume;
    QDateTime openTime;
    QDateTime closeTime;
    quint64 fristId;
    quint64 lastId;
    quint64 count;
};

//*
//*******************************************

//*******************************************
//*

struct TradesPair {
    QString symbol;
    QString price;
    QString origQty;
    QString executedQty;
    QString cummulativeQuoteQty;
    QString status;
    QString side;
    QString commission;
    QString commissionAsset;
    bool isWorking;
    qint64 time;
};
//*
//*******************************************

//*******************************************
//*

struct WatchProc {
//    Watch *watch;
    double p1;
    double p2;
    double p3;
    double diffP2P3;
    double price;
    double sma1;
    double sma2;
    double sma3;
    double percent;
    double minPrice;
    double directHistogram;
    QString pair;
    QString orderId;
    bool liquidity;
    double histogram;
    int countPlusHisrogram;
    bool liqDiffTFrame;
};

struct Depth{
    double price;
    double qty;
};

struct TopCoin {
    enum STATE_FOR_DEL_ORDER {STATE_FOR_DEL_NON, STATE_FOR_DEL_SELL, STATE_FOR_DEL_BUY} stateForDel;
    double askPrice;
    double askQty;
    double bidPrice;
    double bidQty;
//    double priceBuy;
    double maxPrice;
    double minPrice;
    double bet;
    double percent;
    double price;
    double priceUp;
    double priceDown;
    QString pair;
    QString baseAsset;
    QString quoteAsset;
    QString idSell;
    QString idBuy;
    QString strAmountSell;
    QString strAmountBuy;
    QString strPriceSell;
    QString strPriceBuy;
    QString strPrice;
    int count;
    EventReport *reportBuy;
    EventReport *reportSell;
    QString stopLimit;
};


//*******************************************
//*

struct Statistic {
    QString totalSpend;
};

struct Currency {
    bool favorit;
    QString symbol;
    QString priceChange;
    QString priceChangePercent;
    QString weightAvePrice;
    QString lastPrice;
    QString lastQuantity;
    QString bid;
    QString bidQuantity;
    QString ask;
    QString askQuantity;
    QString openPrice;
    QString hightPrice;
    QString lowPrice;
    QString baseVolume;
    QString quoteVolume;
    QString totalSpend;
    QString balance;
    QString balanceInQuote;
    QString spread;
};


struct PairTrade {
    QString symbol;
    QString priceMin;
    QString priceMax;
    QString perc;
};

struct MessageProcess {
    QString msg;
    QString returnStr;
    int error = 0;
    bool result = true;
};



#endif // GLOBAL_H
