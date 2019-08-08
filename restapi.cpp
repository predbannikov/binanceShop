#include "restapi.h"

RestApi::RestApi(QObject *parent) : QObject (parent)
{
    binacpp = new Binacpp(this);
}

RestApi::~RestApi()
{

}

void RestApi::setKeys(QString sk, QString ak)
{
    binacpp->setKeys(sk, ak);
}

void RestApi::getCandLestick(QByteArray symbol, QByteArray interval)
{
    qDebug() << "start getCandlestick " << symbol << interval;
    QByteArray array = binacpp->getCandLestick(symbol, interval, "100");
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "RestApi::getCandLestick::parsError = " << parseError.errorString();
        qDebug() << array;
        return;
    }
    qDebug() << "end getCandlestick " << symbol << interval;
    emit sendCandLestick(jdoc.array());
}

void RestApi::getExchangeInfo()
{
    QByteArray array = binacpp->getExchangeInfo();
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "RestApi::getExchangeInfo::parsError = " << parseError.errorString();
        qDebug() << array;
        return;
    }
    emit sendExchangeInfo(jdoc.object());
}

void RestApi::getListenKey()
{
    QByteArray array = binacpp->getListenKey();
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "RestApi::getListenKey::parsError = " << parseError.errorString();
        qDebug() << array;
        return;
    }
    emit sendListenKey(jdoc.object()["listenKey"].toString());
}

void RestApi::getAccountInfo()
{
    QByteArray array = binacpp->getAccountInfo();
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "RestApi::getAccountInfo::parsError = " << parseError.errorString();
        qDebug() << array;
        return;
    }
    emit sendAccountInfo(jdoc.object());
}

void RestApi::sendKeepAlive(QString keepAlive)
{
    QByteArray array = binacpp->keepAlive(keepAlive.toUtf8());
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "RestApi::getAccountInfo::parsError = " << parseError.errorString();
        qDebug() << array;
        return;
    }
    emit sendAccountInfo(jdoc.object());
}

void RestApi::tradeList(QJsonArray jarray)
{
//    QMap <QString, QByteArray> mapResponse;
    QJsonArray recjarr;
    for(int i = 0; i < jarray.size(); i++)
    {
        QJsonObject jobj = jarray.at(i).toObject();
        QString quantity = jobj["quantity"].toString();
        QString side = jobj["side"].toString();
        QString symbol = jobj["symbol"].toString();
//        mapResponse.insert(symbol, binacpp->tradeMarket(quantity, symbol, side, "MARKET", "5000"));
        QJsonObject _jobj;
        _jobj["symbol"] = symbol;
//        QByteArray arr = "********" + quantity.toUtf8() + symbol.toUtf8();
        QByteArray arr = binacpp->tradeMarket(quantity, symbol, side, "MARKET", "5000");
        _jobj["data"] = QString(arr);
        recjarr.append(_jobj);
        QThread::msleep(100);
    }
    emit sendReceivTradeList(recjarr);
}

void RestApi::getTradesOfPair(qint64 dtStart, QString symbol)
{
    qint64 sTime = dtStart;
    QJsonParseError parsError;
    QJsonArray jarray;
    do {
        QJsonDocument jdoc = QJsonDocument::fromJson(binacpp->getMyAllTrades(symbol, "1000", QString::number(sTime)), &parsError);
        if(parsError.error != QJsonParseError::NoError)
            break;
        else if(jdoc.array().isEmpty())
            break;
        if(sTime > static_cast <qint64> (jdoc.array()[0].toObject()["time"].toDouble())) {
            qDebug() << "time prep > next";
            return ;
        }
        for(int i = 0; i < jdoc.array().size(); i++)
            jarray.append(jdoc.array()[i].toObject());

        sTime = static_cast <qint64> (jdoc.array()[jdoc.array().size()-1].toObject()["time"].toDouble()) + 1;
        qDebug() << "next stage";

    } while(sTime < QDateTime::currentMSecsSinceEpoch());
    emit sendReceivTradesOfPair(jarray, symbol);
    qDebug() << "stop";
}

void RestApi::doWork()
{
}
