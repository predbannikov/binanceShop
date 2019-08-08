#include "ticketarr.h"

#define     TIME_PING  10000

TicketArr::TicketArr(QObject *parent) : QObject(parent)
{
    QObject::connect(&socket, &QWebSocket::connected, this, &TicketArr::sockConnected);
    QObject::connect(&socket, &QWebSocket::disconnected, this, &TicketArr::sockClosed);
    QObject::connect(&socket, &QWebSocket::pong, this, &TicketArr::receivePong);
    QObject::connect(&socket, &QWebSocket::stateChanged, this, &TicketArr::printStatusConnect);


    connect(&socket, &QWebSocket::textMessageReceived,this, &TicketArr::sockMsgReceived);


    startConnected();

    timerPing = new QTimer(this);
    QObject::connect(timerPing, &QTimer::timeout, this, &TicketArr::pingServer);
}
TicketArr::~TicketArr()
{
    deleteLater();
}

void TicketArr::startConnected()
{
    QString openStream;
    openStream.append(QString("wss://stream.binance.com:9443/stream?streams=!ticker@arr"));
    qDebug() << "socket.open:" << openStream;
    socket.open(QUrl(openStream));

}

void TicketArr::sockConnected()
{
    qDebug() << ">>> WebSocket TicketArr connected ";
    emit finishConnect();

    STATUS = DATASTREAM_OK;
    timerPing->setInterval(TIME_PING);
    timerPing->start();
}

void TicketArr::sockClosed()
{
    qDebug() << "Close socket TicketArr stream";
    timerPing->start();

}

void TicketArr::sockMsgReceived(QString message)
{
    QJsonArray jarray = QJsonDocument::fromJson(message.toUtf8()).object()["data"].toArray();
    emit sendMsg(&jarray);
}

void TicketArr::receivePong(quint64 elapsTime, QByteArray array)
{
    Q_UNUSED(elapsTime);
    Q_UNUSED(array);
    STATUS = DATASTREAM_OK;
}

void TicketArr::pingServer()
{

    if(STATUS == DATASTREAM_OK)
    {
        STATUS = DATASTREAM_SEND;
        socket.ping("OK");
    } else if(STATUS == DATASTREAM_CONNECT){
        timerPing->stop();
        QNetworkRequest req = socket.request();
        socket.open(req);
        qDebug() << "TicketArr::pingServer::if(STATUS == DATASTREAM_CONNECT)";
    }else if(STATUS == DATASTREAM_SEND){
        socket.abort();
        qDebug() << "TicketArr::pingServer::if(STATUS == DATASTREAM_SEND)";
        STATUS = DATASTREAM_CONNECT;
    }

}

void TicketArr::printStatusConnect(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}
