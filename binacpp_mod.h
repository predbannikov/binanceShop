#ifndef BINACPP_MOD_H
#define BINACPP_MOD_H

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

class Binacpp_mod : public QObject
{
    Q_OBJECT

public:
    Binacpp_mod(QObject *parent = nullptr);
    ~Binacpp_mod();
    void setKeys(QString sk, QString ak);

//    QByteArray keepAlive(QByteArray key);
//    QByteArray deleteStream(QByteArray key);
    void trade(QString price, QString amount, QString symbol, QString side, QString type, QString newClientOrderId, QString window);
    void deleteOrder(QString symbol, QString origClientOrderId, QString newClientOrderId, QString window);
    void deleteOrder(QString symbol, QString origClientOrderId, QString window);

    QNetworkAccessManager *netMan;
//    QByteArray getAccountInfo();
//    QByteArray getExchangeInfo();
//    QByteArray getListenKey();
    QByteArray Sha256Hmac(QByteArray req);
//    QByteArray getServerTime();
//    QByteArray getTicker24();
//    QByteArray getBookTicker(QByteArray symbol);
//    QByteArray getOpenOrders(QString symbol);
//    QByteArray getDepth(QByteArray symbol, QByteArray limit);
//    QByteArray getCandLestick(QByteArray symbol, QByteArray interval, QByteArray limit);

private:

    QByteArray Sha256Hmac(QByteArray req, QByteArray api_sec);
    void parsError(QByteArray array);


    enum TYPE {GET, POST, DELETE, PUT};

    QByteArray api_key;
    QByteArray api_secret;
    QByteArray getTimeStamp();
    void getServerData(QNetworkRequest req, TYPE t);

private slots:
    void readReply(QNetworkReply *reply);

signals:
    void sendMsgToServ(QString msg);
    void sendError(int error);
    void sendFinished();
};

#endif // BINACPP_MOD_H
