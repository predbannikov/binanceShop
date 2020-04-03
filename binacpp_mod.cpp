#include "binacpp_mod.h"


#define     SERVERHOST      "https://api.binance.com"
#define     ACCOUNTINFO     "/api/v3/account"
#define     USERDATASTREAM  "/api/v1/userDataStream"
#define     OPENORDER       "/api/v3/openOrders"



Binacpp_mod::Binacpp_mod(QObject *parent): QObject(parent)
{
    netMan = new QNetworkAccessManager(this);
    connect(netMan, &QNetworkAccessManager::finished, this, &Binacpp_mod::sendFinished);
    connect(netMan, &QNetworkAccessManager::finished, this, &Binacpp_mod::readReply);
}

Binacpp_mod::~Binacpp_mod()
{
    delete netMan;
}

void Binacpp_mod::setKeys(QString sk, QString ak)
{

    api_secret = sk.toUtf8();
    api_key = ak.toUtf8();

    qDebug() << "...set api_key success";
}
/************************************************************
 *                     GET EXCHANGE INFO                    *
 * **********************************************************/

//QByteArray Binacpp_mod::getExchangeInfo()
//{
//    QUrl urlreq("https://www.binance.com/api/v1/exchangeInfo");
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

/************************************************************
 *                       GET ACOUNT INFO                    *
 * **********************************************************/
//QByteArray Binacpp_mod::getAccountInfo()
//{
//    QByteArray par(getTimeStamp());
//    par.append("&recvWindow=5000");
//    QByteArray data;
//    if(DEBUG_TEST)
//    {
//        data.append(HOST_MACHINES);
//        data.append("/account");
//    } else {
//        data.append(SERVERHOST);
//        data.append(ACCOUNTINFO);
//    }
//    data.append("?");
//    data.append(par);
//    data.append("&");
//    data.append("signature=");
//    QByteArray decodeSign = Sha256Hmac(par);
//    data.append(decodeSign);
//    QUrl urlreq(data);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
//    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
////    QThread::msleep(150);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

/************************************************************
 *                      GET LISTEN KEY                      *
 * **********************************************************/
//QByteArray Binacpp_mod::getListenKey()
//{
//    QByteArray data;
//    data.append(SERVERHOST);
//    data.append(USERDATASTREAM);
//    QUrl urlreq(data);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
//    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
//    return getServerData(req, POST);
//}
/************************************************************
 *                         KEEPALIVE                        *
 * **********************************************************/
//QByteArray Binacpp_mod::keepAlive(QByteArray key)
//{
//    QByteArray data;
//    data.append(SERVERHOST);
//    data.append(USERDATASTREAM);
//    data.append("?listenKey=");
//    data.append(key);
//    QUrl urlreq(data);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
//    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
//    return getServerData(req, PUT);
//}
/************************************************************
 *                        DELETE STREAM                     *
 * **********************************************************/
//QByteArray Binacpp_mod::deleteStream(QByteArray key)
//{
//    QByteArray data;
//    data.append(SERVERHOST);
//    data.append(USERDATASTREAM);
//    data.append("?listenKey=");
//    data.append(key);
//    QUrl urlreq(data);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
//    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
//    return getServerData(req, DELETE);
//}
/************************************************************
 *                          TRADE                           *
 * **********************************************************/
void Binacpp_mod::trade(QString price, QString amount, QString symbol, QString side, QString type, QString newClientOrderId, QString window)
{
    QString head = QString("symbol=%1&side=%2&type=%3&timeInForce=GTC&quantity=%4&price=%5&newClientOrderId=%6&recvWindow=%7&")
            .arg(symbol)
            .arg(side)
            .arg(type)
            .arg(amount)
            .arg(price)
            .arg(newClientOrderId)
            .arg(window);
    QByteArray data(head.toUtf8());
    data.append(getTimeStamp());
    QByteArray signatur = Sha256Hmac(data);
    data.append("&signature=");
    data.append(signatur);
    if(DEBUG_TEST)
        data.prepend(HOST_MACHINES "/order?");
    else
        data.prepend("https://api.binance.com/api/v3/order?");
    QUrl urlreq(data);
    QNetworkRequest req;
    req.setUrl(urlreq);
    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
    getServerData(req, POST);
}

/************************************************************
 *                        DELET ORDER                       *
 * **********************************************************/
void Binacpp_mod::deleteOrder(QString symbol, QString origClientOrderId, QString newClientOrderId, QString window)
{
    QString head = QString("symbol=%1&origClientOrderId=%2&recvWindow=%3&newClientOrderId=%4&")
            .arg(symbol)
            .arg(origClientOrderId)
            .arg(newClientOrderId)
            .arg(window);
    QByteArray data(head.toUtf8());
    data.append(getTimeStamp());
    QByteArray signatur = Sha256Hmac(data);
    data.append("&signature=");
    data.append(signatur);
    if(DEBUG_TEST)
        data.prepend(HOST_MACHINES "/order?");
    else
        data.prepend("https://api.binance.com/api/v3/order?");
    QUrl urlreq(data);
    QNetworkRequest req;
    req.setUrl(urlreq);
    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
    getServerData(req, DELETE);
}

void Binacpp_mod::deleteOrder(QString symbol, QString origClientOrderId, QString window)
{
    QString head = QString("symbol=%1&origClientOrderId=%2&recvWindow=%3&")
            .arg(symbol)
            .arg(origClientOrderId)
            .arg(window);
    QByteArray data(head.toUtf8());
    data.append(getTimeStamp());
    QByteArray signatur = Sha256Hmac(data);
    data.append("&signature=");
    data.append(signatur);
    if(DEBUG_TEST)
        data.prepend(HOST_MACHINES "/order?");
    else
        data.prepend("https://api.binance.com/api/v3/order?");
    QUrl urlreq(data);
    QNetworkRequest req;
    req.setUrl(urlreq);
    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);
    getServerData(req, DELETE);
}

/************************************************************
 *                        ORDER BOOK                        *
 * **********************************************************/
//QByteArray Binacpp_mod::getDepth(QByteArray symbol, QByteArray limit)
//{
//    QString url = "https://api.binance.com/api/v1/depth";
//    url.append("?symbol=");
//    url.append(symbol);
//    url.append("&limit=");
//    url.append(limit);
//    QUrl urlreq(url);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

/************************************************************
 *                Kline/Candlestick data                    *
 * **********************************************************/
//QByteArray Binacpp_mod::getCandLestick(QByteArray symbol, QByteArray interval, QByteArray limit)
//{
//    QString url = "https://www.binance.com/api/v1/klines?symbol=";
//    url.append(symbol);
//    url.append("&interval=");
//    url.append(interval);
//    url.append("&limit=");
//    url.append(limit);
//    QUrl urlreq(url);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

//QByteArray Binacpp_mod::getServerTime()
//{
//    QUrl urlreq("https://www.binance.com/api/v1/time");
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

//QByteArray Binacpp_mod::getTicker24()
//{
//    QUrl urlreq("https://api.binance.com/api/v1/ticker/24hr");
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

//QByteArray Binacpp_mod::getBookTicker(QByteArray symbol)
//{
//    QString url;
//    if(DEBUG_TEST)
//        url.append(HOST_MACHINES "/bookTicker");
//    else
//        url.append("https://api.binance.com/api/v3/ticker/bookTicker");
//    url.append("?symbol=");
//    url.append(symbol);
//    QUrl urlreq(url);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    QByteArray array = getServerData(req, GET);
//    return array;
//}

/************************************************************
 *                       OPEN ORDERS                        *
 * **********************************************************/
//QByteArray Binacpp_mod::getOpenOrders(QString symbol)
//{
//    QString url;
//    if(DEBUG_TEST)
//        url.append(HOST_MACHINES "/openOrders?");
//    else
//        url.append("https://api.binance.com/api/v3/openOrders?");

//    QByteArray par;
//    if(!symbol.isEmpty())
//    {
//        par.append("symbol=");
//        par.append(symbol);
//        par.append("&");
//    }
//    par.append("recvWindow=5000&");
//    par.append(getTimeStamp());
//    par.append("&");
//    url.append(par);
//    url.append("&");
//    url.append("signature=");
//    QByteArray decodeSign = Sha256Hmac(par);
//    url.append(decodeSign);

//    QUrl urlreq(url);
//    QNetworkRequest req;
//    req.setUrl(urlreq);
//    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
//    req.setRawHeader(QByteArray("X-MBX-APIKEY"),   api_key);

//    QByteArray array = getServerData(req, GET);
//    return array;
//}

QByteArray Binacpp_mod::Sha256Hmac(QByteArray req)
{
    QMessageAuthenticationCode code(QCryptographicHash::Sha256);
    code.setKey(api_secret);
    code.addData(req);
    return code.result().toHex();
}

QByteArray Binacpp_mod::Sha256Hmac(QByteArray req, QByteArray api_sec)
{
    QMessageAuthenticationCode code(QCryptographicHash::Sha256);
    code.setKey(api_sec);
    code.addData(req);
    return code.result().toHex();
}

void Binacpp_mod::parsError(QByteArray array)
{
    QJsonParseError parsError;
    QJsonDocument document = QJsonDocument::fromJson(array, &parsError);
    if(parsError.error != QJsonParseError::NoError)
    {
        qDebug() << "Binacpp_mod::parsError = " << parsError.errorString();
        qDebug() << array;
        return ;
    }
    QJsonObject obj = document.object();
    int error = obj["code"].toInt();
    emit sendError(error);

}

QByteArray Binacpp_mod::getTimeStamp()
{
    QByteArray request("timestamp=");
    qint64 unixtimestamp = QDateTime::currentMSecsSinceEpoch();
    request.append(QString::number(unixtimestamp));
    return request;
}

void Binacpp_mod::getServerData(QNetworkRequest req, TYPE t)
{
    if(api_key.isEmpty() || api_secret.isEmpty())
    {
        QString strError = "Error: keys binacpp is empty";
        emit sendMsgToServ(strError);
        return ;
    }
//    QEventLoop loop;
    QNetworkReply *reply;
    switch(t)
    {
    case GET:
        reply = netMan->get(req);
        break;
    case POST:
        reply = netMan->post(req, "");
        break;
    case DELETE:
        reply = netMan->deleteResource(req);
        break;
    case PUT:
        reply = netMan->put(req, "");
        break;
    }
    if (reply == nullptr) {
        qWarning("Reply is NULL");
        delete reply;
        return ;
    }
//    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
//    loop.exec();
//    if (reply->error() != QNetworkReply::NoError) {
//        qCritical("%s", qPrintable(QString("[%1] %2").arg(reply->error()).arg(reply->errorString())));
//        QByteArray data = reply->readAll();
//        qDebug() << data;
//        parsError(data);
//        delete reply;
//        return data;
//    }

//    QByteArray data = reply->readAll();
//    reply->deleteLater();
//    delete reply;
////    qDebug() << data;

    //    return data;
}

void Binacpp_mod::readReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qCritical("%s", qPrintable(QString("[%1] %2").arg(reply->error()).arg(reply->errorString())));
        QByteArray data = reply->readAll();
        qDebug() << data;
        delete reply;
    }
}
