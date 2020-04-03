#ifndef DATAUSERSTREAM_H
#define DATAUSERSTREAM_H

#include <QObject>
#include <QWebSocket>
#include <QMutex>
#include <QTimer>
#include "global.h"
#include "binacpp.h"
#include "restapi.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QWebSocketServer)


class DataUserStream : public QObject
{
    Q_OBJECT
public:
    explicit DataUserStream(RestApi *net, EventReport *repInfo, AccountInfo *acInfo, QObject *parent = nullptr);
    ~DataUserStream();
    void checkBalances();
    void startConnected();

private:
    int sizeBuffSave;
    QString keepAlive;
//    Binacpp *binacpp;
    QWebSocket socket;
    QTimer *timerPing;
    QTimer *timerKeepAlive;
    QMutex	m_mutex;
    EventReport *eventReport;
    AccountInfo  *accountInfo;

    RestApi *th_restApi;
    QThread *thread;

    enum STATUS_DATA_STREAM {DATASTREAM_OK, DATASTREAM_SEND, DATASTREAM_CONNECT} STATUS;

    void printStatusConnect(QAbstractSocket::SocketState state);
    void sockConnected();
    void sockClosed();
    void sockMsgReceived(QString message);
    void parsAccountInfo(QJsonObject obj);
    QString parsListenKey(QByteArray array);




signals:
    void sendEventReport(EventReport *a_eventReport);
    void sendAccountInfo();
    void finishConnectDataStream();
    void getListenKey();
    void getAccountInfo();
    void sendKeepAlive(QString);

private slots:
    void receivePong(quint64 elapsTime, QByteArray array);
    void pingServer();
    void procKeepAlive();
    void receivListenKey(QString listenKey);
    void receivKeepAlive(QJsonObject jobj);
};

#endif // DATAUSERSTREAM_H
