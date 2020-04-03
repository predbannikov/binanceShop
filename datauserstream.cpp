#include "datauserstream.h"

#define TIME_PING 7000
#define TIME_SEND_KEEPALIVE 1800000


DataUserStream::DataUserStream(RestApi *net, EventReport *repInfo, AccountInfo *acInfo, QObject *parent) : QObject(parent)
{
    thread = new QThread;
    th_restApi = net;
//    th_restApi->moveToThread(thread);
    connect(th_restApi, &RestApi::sendListenKey, this, &DataUserStream::receivListenKey);
    connect(this, &DataUserStream::getListenKey, th_restApi, &RestApi::getListenKey);
    connect(this, &DataUserStream::getAccountInfo, th_restApi, &RestApi::getAccountInfo);
    connect(th_restApi, &RestApi::sendAccountInfo, this, &DataUserStream::parsAccountInfo);
    connect(this, &DataUserStream::sendKeepAlive, th_restApi, &RestApi::sendKeepAlive);
    connect(th_restApi, &RestApi::sendReceivKeepAlive, this, &DataUserStream::receivKeepAlive);
//    thread->start();
//    binacpp = net;
    eventReport = repInfo;
    accountInfo = acInfo;
//    keepAlive = parsListenKey(binacpp->getListenKey());
//    qDebug() << keepAlive;
//    QString openStream = QString("wss://stream.binance.com:9443/ws/%1").arg(keepAlive);
    QObject::connect(&socket, &QWebSocket::connected, this, &DataUserStream::sockConnected);
    QObject::connect(&socket, &QWebSocket::disconnected, this, &DataUserStream::sockClosed);
    QObject::connect(&socket, &QWebSocket::pong, this, &DataUserStream::receivePong);
    QObject::connect(&socket, &QWebSocket::stateChanged, this, &DataUserStream::printStatusConnect);


    connect(&socket, &QWebSocket::textMessageReceived,this, &DataUserStream::sockMsgReceived);
//    connect(&socket, &QWebSocket::binaryMessageReceived,this, &DataUserStream::sockMsgReceived);

//    socket.open(QUrl(openStream));

//    startConnected();
    emit getListenKey();
    timerPing = new QTimer(this);
    QObject::connect(timerPing, &QTimer::timeout, this, &DataUserStream::pingServer);
    timerKeepAlive = new QTimer(this);
    QObject::connect(timerKeepAlive, &QTimer::timeout, this, &DataUserStream::procKeepAlive);
    //    checkBalances();
}

DataUserStream::~DataUserStream()
{
    deleteLater();
}

void DataUserStream::checkBalances()
{
    emit getAccountInfo();
//    parsAccountInfo(binacpp->getAccountInfo());
}

void DataUserStream::startConnected()
{
    qDebug() << "startConnected";
    QString openStream;
    if(DEBUG_TEST)
        openStream.append(HOST_MACHINES_WS);
    else {
        qDebug() << keepAlive;
        openStream.append(QString("wss://stream.binance.com:9443/ws/%1").arg(keepAlive));

    }
    qDebug() << "socket.open:" << openStream;
    socket.open(QUrl(openStream));

}

void DataUserStream::sockConnected()
{
    qDebug() << ">>> WebSocket DataUserStream connected ";
//    connect(&socket, &QWebSocket::textMessageReceived,this, &DataUserStream::sockMsgReceived);

    emit finishConnectDataStream();

    STATUS = DATASTREAM_OK;
    timerPing->setInterval(TIME_PING);
    timerPing->start();
    timerKeepAlive->setInterval(TIME_SEND_KEEPALIVE);
    timerKeepAlive->start();
}

void DataUserStream::sockClosed()
{
    qDebug() << "Close socket user data stream";
//    timerPing->stop();
    timerPing->start();
//    if(STATUS != DATASTREAM_OK)
//    {
////        QObject::disconnect(&socket, &QWebSocket::textMessageReceived, this, &DataUserStream::sockMsgReceived);
//    } else
//    {
//        qDebug() << "socket.deleteLater();";
//        socket.deleteLater();
//    }
}

void DataUserStream::sockMsgReceived(QString message)
{
    QJsonParseError parsError;

    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8(), &parsError);
    if(parsError.error != QJsonParseError::NoError)
    {
        if(parsError.error != QJsonParseError::GarbageAtEnd)
        {
            qDebug() << "DataUserStream::sockMsgReceived::parsError = " << parsError.errorString();
            qDebug() << message;
            qDebug() << "size =" << message.size() << "buffer.size =" << socket.readBufferSize();
            return ;
        } else {
            message.remove(0, sizeBuffSave - 1);
            document = QJsonDocument::fromJson(message.toUtf8(), &parsError);
            if(parsError.error != QJsonParseError::NoError)
            {
                    qDebug() << "2DataUserStream::sockMsgReceived::parsError = " << parsError.errorString();
                    qDebug() << message;
                    qDebug() << "size =" << message.size() << "buffer.size =" << socket.readBufferSize();
                    return ;
            }
        }
    }
    sizeBuffSave = message.size();
    QJsonObject obj = document.object();

    if(obj["e"].toString() == "executionReport")
    {
        m_mutex.lock();
        eventReport->e = obj["e"].toString();
        eventReport->E.setMSecsSinceEpoch(static_cast<qint64>(obj["E"].toDouble()));
        eventReport->s = obj["s"].toString();
        eventReport->c = obj["c"].toString();
        eventReport->S = obj["S"].toString();
        eventReport->o = obj["o"].toString();
        eventReport->f = obj["f"].toString();
        eventReport->q = obj["q"].toString();
        eventReport->p = obj["p"].toString();
        eventReport->P = obj["P"].toString();
        eventReport->F = obj["F"].toString();
        eventReport->g = obj["g"].toInt();
        eventReport->C = obj["C"].toString();
        eventReport->x = obj["x"].toString();
        eventReport->X = obj["X"].toString();
        eventReport->r = obj["r"].toString();
        eventReport->i = obj["i"].toInt();
        eventReport->l = obj["l"].toString();
        eventReport->z = obj["z"].toString();
        eventReport->L = obj["L"].toString();
        eventReport->n = obj["n"].toString();
        eventReport->N = obj["N"].toString();
        eventReport->T.setMSecsSinceEpoch(static_cast<qint64>(obj["T"].toDouble()));
        eventReport->t = obj["t"].toInt();
        eventReport->I = obj["I"].toInt();
        eventReport->w = obj["w"].toBool();
        eventReport->m = obj["m"].toInt();
        eventReport->M = obj["M"].toString();
//        qDebug() << " > > REPORT ORDER NOTIFY: " << eventReport->s;
//        qDebug() << "Event time: " << eventReport->E.toString("yyyy.MM.dd hh:mm:ss");
//        qDebug() << "orderId = " << eventReport->i;
//        qDebug() << "clientOrderId = " << eventReport->c;
    //    qDebug() << "Order type = " << eventReport->o;
    //    qDebug() << "Time in force = " << eventReport->f;
//        qDebug() << "Order quantity = " << eventReport->q;
//        qDebug() << "Order price = " << eventReport->p;
    //    qDebug() << "Stop price = " << eventReport->P;
    //    qDebug() << "Iceberg quantity = " << eventReport->F;
//        qDebug() << "Original client order ID; This is the ID of the order being canceled = " << eventReport->C;
//        qDebug() << "Side = " << eventReport->S;
//        qDebug() << "Current execution type = " << eventReport->x;
//        qDebug() << "Current order status = " << eventReport->X;
//        qDebug() << "Order reject reason; will be an error code. = " << eventReport->r;
    //    qDebug() << "Last executed quantity = " << eventReport->l;
    //    qDebug() << "Cumulative filled quantity = " << eventReport->z;
    //    qDebug() << "Last executed price = " << eventReport->L;
    //    qDebug() << "Commission amount = " << eventReport->n;
    //    qDebug() << "Commission asset = " << eventReport->N;
    //    qDebug() << "Transaction time = " << eventReport->T.toString("yyyy.MM.dd hh:mm:ss");
//        qDebug() << "Transaction time = " << eventReport->T.toMSecsSinceEpoch();
//        qDebug() << "Trade ID = " << eventReport->t;
//        qDebug() << "Ignore = " << eventReport->I;
    //    qDebug() << "Is the order working? Stops will have = " << eventReport->w;
    //    qDebug() << "Is this trade the maker side? = " << eventReport->m;
//        qDebug() << "Ignore = " << eventReport->M;
//        qDebug() << " -> end report";
        m_mutex.unlock();
        emit sendEventReport(eventReport);

    } else if(obj["e"].toString() == "outboundAccountInfo")
    {
        m_mutex.lock();
//        qDebug() << " --> outboundAccountInfo";
        accountInfo->e = obj["e"].toString();
        accountInfo->E.setMSecsSinceEpoch(static_cast<qint64>(obj["E"].toDouble()));
        accountInfo->m = obj["m"].toDouble();
        accountInfo->t = obj["t"].toDouble();
        accountInfo->b = obj["b"].toDouble();
        accountInfo->s = obj["s"].toDouble();
        accountInfo->T = obj["T"].toBool();
        accountInfo->W = obj["W"].toBool();
        accountInfo->D = obj["D"].toBool();
        accountInfo->u.setMSecsSinceEpoch(static_cast<qint64>(obj["u"].toDouble()));
        accountInfo->B.clear();
        QJsonArray jarray = obj["B"].toArray();
        for(int i = 0; i < jarray.size(); i++)
        {
            Balances bal;
            QJsonObject jbal = jarray[i].toObject();
            bal.a = jbal["a"].toString();
            bal.f = jbal["f"].toString();
            bal.l = jbal["l"].toString();
            accountInfo->B.insert(jbal["a"].toString(), bal);
        }
        m_mutex.unlock();

    //    QString str = QString("Account info:\n%1 %2\n%3 %4\n%5 %6")
    //            .arg("Can trade?", -15)
    //            .arg(accountInfo->T?"true":"false", -16)
    //            .arg("Can withdraw?", -15)
    //            .arg(accountInfo->W?"true":"false", -16)
    //            .arg("Can deposit?", -15)
    //            .arg(accountInfo->D?"true":"false", -16);
    //    ui->teOutput->append(str);

        qDebug() << "--------BALANCE----------";
        foreach (const Balances bal, accountInfo->B.values()) {
            if(bal.f.toDouble() > 0 )
            {
                QString str = QString("%1%2")
                        .arg(bal.a, -10)
                        .arg(bal.f, 16);
//                qDebug() << str;
            }
        }

        emit sendAccountInfo();
    }

}

void DataUserStream::parsAccountInfo(QJsonObject obj)
{
//    QJsonParseError parsError;
//    QJsonDocument document = QJsonDocument::fromJson(array, &parsError);
//    if(parsError.error != QJsonParseError::NoError)
//    {
//        qDebug() << "DataUserStream::sockMsgReceived::parsError = " << parsError.errorString();
//        qDebug() << array;
//        qDebug() << "size =" << array.size() << "buffer.size =" << socket.readBufferSize();
//        return ;
//    }
//    QJsonObject obj = document.object();

    m_mutex.lock();
    accountInfo->e = "Main single request";
    accountInfo->E = QDateTime::currentDateTime();
    accountInfo->m = obj["makerCommission"].toDouble();
    accountInfo->t = obj["takerCommission"].toDouble();
    accountInfo->b = obj["buyerCommission"].toDouble();
    accountInfo->s = obj["sellerCommission"].toDouble();
    accountInfo->T = obj["canTrade"].toBool();
    accountInfo->W = obj["canWithdraw"].toBool();
    accountInfo->D = obj["canDeposit"].toBool();
    accountInfo->u.setMSecsSinceEpoch(static_cast<qint64>(obj["updateTime"].toDouble()));
    accountInfo->B.clear();
    QJsonArray jarray = obj["balances"].toArray();
    for(int i = 0; i < jarray.size(); i++)
    {
        Balances bal;
        QJsonObject jbal = jarray[i].toObject();
        bal.a = jbal["asset"].toString();
        bal.f = jbal["free"].toString();
        bal.l = jbal["locked"].toString();
        accountInfo->B.insert(jbal["asset"].toString(), bal);
    }
//    QString str = QString("Account info:\n%1 %2\n%3 %4\n%5 %6")
//            .arg("Can trade?", -15)
//            .arg(accountInfo.T?"true":"false", -16)
//            .arg("Can withdraw?", -15)
//            .arg(accountInfo.W?"true":"false", -16)
//            .arg("Can deposit?", -15)
//            .arg(accountInfo.D?"true":"false", -16);
//    ui->teOutput->append(str);

    m_mutex.unlock();
    foreach (const Balances bal, accountInfo->B.values()) {
        if(bal.f.toDouble() > 0 )
        {
            QString str = QString("%1%2")
                    .arg(bal.a, -10)
                    .arg(bal.f, 16);
            qDebug() << str;
        }
    }
    emit sendAccountInfo();
}

QString DataUserStream::parsListenKey(QByteArray array)
{
    QJsonParseError parsError;
    QJsonDocument document = QJsonDocument::fromJson(array, &parsError);
    if(parsError.error != QJsonParseError::NoError)
    {
        qDebug() << "listenKey::parsError = " << parsError.errorString();
        qDebug() << array;
        return QString();
    }
    if(document.isEmpty())
    {
        qDebug() << document;
        return QString();
    }
    QJsonObject jobj = document.object();
    return jobj["listenKey"].toString();
}

void DataUserStream::receivePong(quint64 elapsTime, QByteArray array)
{
    Q_UNUSED(elapsTime);
    Q_UNUSED(array);
    STATUS = DATASTREAM_OK;
}

void DataUserStream::pingServer()
{
//    qDebug() << "DataUserStream::pingServer()" << STATUS;
//    qDebug() << "this->thread" << this->thread()->currentThreadId();
//    qDebug() << "QThread" << QThread::currentThreadId();

    if(STATUS == DATASTREAM_OK)
    {
        STATUS = DATASTREAM_SEND;
        socket.ping("OK");
//        qDebug() << "DataUserStream::pingServer::if(STATUS == OK)";
    } else if(STATUS == DATASTREAM_CONNECT){
        timerPing->stop();
        QNetworkRequest req = socket.request();
        socket.open(req);
    }else if(STATUS == DATASTREAM_SEND){
//        socket.close();
        socket.abort();
        STATUS = DATASTREAM_CONNECT;
    }

}

void DataUserStream::procKeepAlive()
{
    if(DEBUG_TEST)
        return;
    qDebug() << " > > send keepalive " << keepAlive;
    emit sendKeepAlive(keepAlive);
//    QByteArray array = binacpp->keepAlive(keepAlive.toUtf8());
//    QJsonParseError parsError;
//    QJsonDocument document = QJsonDocument::fromJson(array, &parsError);
//    QJsonObject object = document.object();
//    if(object.isEmpty())
//        qDebug() << "keepalive success";
//    else
//    {
//        qDebug() << "keepalive not success";
//    }
}

void DataUserStream::receivListenKey(QString listenKey)
{
    keepAlive = listenKey;
    startConnected();
}

void DataUserStream::receivKeepAlive(QJsonObject jobj)
{
    if(jobj.isEmpty())
        qDebug() << "keepalive success";
    else
    {
        qDebug() << "keepalive not success";
    }
}

void DataUserStream::printStatusConnect(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
//    qDebug() << "SocketState:" << state;
}
