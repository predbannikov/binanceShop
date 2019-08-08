#ifndef RESTAPI_H
#define RESTAPI_H

#include <QObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QRegExp>
#include <QQueue>
#include <QEventLoop>
#include "global.h"
#include "binacpp.h"

class RestApi : public QObject
{
    Q_OBJECT
public:
    RestApi(QObject *parent = nullptr);
    ~RestApi();
    void setKeys(QString sk, QString ak);

private:
    Binacpp *binacpp;

public slots:
    void getCandLestick(QByteArray symbol, QByteArray interval);
    void getExchangeInfo();
    void getListenKey();
    void getAccountInfo();
    void sendKeepAlive(QString keepAlive);
    void tradeList(QJsonArray jarray);
    void getTradesOfPair(qint64 dtStart, QString symbol);
    void doWork();
signals:
    void sendCandLestick(QJsonArray);
    void sendExchangeInfo(QJsonObject);
    void sendListenKey(QString);
    void sendAccountInfo(QJsonObject);
    void sendReceivKeepAlive(QJsonObject);
    void sendReceivTradeList(QJsonArray);
    void sendReceivTradesOfPair(QJsonArray, QString);
};

#endif // RESTAPI_H
