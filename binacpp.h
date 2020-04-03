#ifndef BINACPP_H
#define BINACPP_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageAuthenticationCode>
#include <QNetworkReply>
#include <QStringList>
#include <QEventLoop>
#include "global.h"

class Binacpp : public QObject
{
    Q_OBJECT

public:
    Binacpp(QObject *parent = nullptr);
    ~Binacpp();
    void setKeys(QString sk, QString ak);

    QByteArray keepAlive(QByteArray key);
    QByteArray deleteStream(QByteArray key);
    QByteArray trade(QString price, QString amount, QString symbol, QString side, QString type, QString newClientOrderId, QString window);
    QByteArray tradeMarket(QString quantity, QString symbol, QString side, QString type, QString window);
    QByteArray deleteOrder(QString symbol, QString origClientOrderId, QString window);

    QNetworkAccessManager *netMan;
    QByteArray getAccountInfo();
    QByteArray getExchangeInfo();
    QByteArray getListenKey();
    QByteArray Sha256Hmac(QByteArray req);
    QByteArray getServerTime();
    QByteArray getTicker24();
    QByteArray getBookTicker(QByteArray symbol);
    QByteArray getOpenOrders(QString symbol);
    QByteArray getDepth(QByteArray symbol, QByteArray limit);
    QByteArray getCandLestick(QByteArray symbol, QByteArray interval, QByteArray limit);
    QByteArray getMyAllTrades(QString symbol, QString limit, QString startTime);


private:

    QByteArray Sha256Hmac(QByteArray req, QByteArray api_sec);
    void parsError(QByteArray array);


    enum TYPE {GET, POST, DELETE, PUT};

    QByteArray api_key;
    QByteArray api_secret;
    QByteArray getTimeStamp();
    QByteArray getServerData(QNetworkRequest req, TYPE t);

signals:
    void sendMsgToServ(QString msg);
    void sendError(int error);

};

#endif // BINACPP_H
