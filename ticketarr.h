#ifndef TICKETARR_H
#define TICKETARR_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QWebSocketServer)


class TicketArr : public QObject
{
    Q_OBJECT
public:
    explicit TicketArr(QObject *parent = nullptr);
    ~TicketArr();

    QWebSocket socket;
    QTimer *timerPing;
    enum STATUS_DATA_STREAM {DATASTREAM_OK, DATASTREAM_SEND, DATASTREAM_CONNECT} STATUS;


    void startConnected();

    void printStatusConnect(QAbstractSocket::SocketState state);
    void sockConnected();
    void sockClosed();
    void sockMsgReceived(QString message);


signals:
    void finishConnect();
    void sendMsg(QJsonArray*);
private slots:
    void receivePong(quint64 elapsTime, QByteArray array);
    void pingServer();
};

#endif // TICKETARR_H
