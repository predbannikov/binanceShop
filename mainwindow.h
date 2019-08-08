#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDoubleSpinBox>
#include <QLayoutItem>
#include <QLabel>
#include <QDir>
#include "ticketarr.h"
#include "pairs.h"
#include "listpairs.h"
#include "datauserstream.h"
#include "restapi.h"
#include "flowlayout.h"

struct UserData {
    QString api_key;
    QString secret_key;
    QString password;
    QString port;
    QString folder;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void receivExchangeInfo(QJsonObject array);
    void updateBalance();
    void choosePair(QJsonObject jobj);
    void eventReportUpdate(EventReport *a_eventReport);
//    QList<TradesPair> parsAllMyTrades(QJsonArray jarray);
    void receivAllTradesOfPair(QJsonArray jarray, QString symbol);
    void startProc();
    void updateHistory();

    void on_pushButton_clicked();

    void on_pbBuy_clicked();

    void on_pushButton_2_clicked();

private:
    AccountInfo *accountInfo;
    EventReport *eventReport;
    ExchangeInfo *exchangeInfo;
    TicketArr *ticketArr;
    ModelPairs *modelPairs;
    ListPairs *listPairs;
    UserData userData;
    DataUserStream *dataUserStream;
    RestApi *th_restApi;
    QThread *thread;
    FlowLayout *flowlayout;
    QTimer timerUpdateHisory;

    bool launch;


    QList<TradesPair> tradesList;
    QList<TradesPair> filledTrades;
    QMap<QString, Statistic> statistics;
    QStringList favorits;

    QStringList quotes = {"BTC", "ETH", "BNB", "USDT"};

    bool loadConfig(MessageProcess &msgProc);
    QStringList loadFavorits();
    void saveConfig();
    void receivTradeList(QJsonArray jarray);
    void calcQuantity(QString &vol, QString symbol);
    
//    void calcAllTradesOfPair();
    void calcAllTradesOfPair(QJsonObject &jobj, QJsonArray jarrOld, QJsonArray jarray);
//    void calcAllTradesOfPair(QString &totalSpend, QString &totalVolum, QJsonArray jarray, QString &lastTime);
    void prepStatistic(QString symbol);
//    void getAllTradesOfPair();
    QJsonObject readHistory(QString symbol);
    void writeHistory(QJsonObject jobj, QString symbol);
    void checkHistory(QString symbol, QString &bal, QString &lastTime);
private:
    Ui::MainWindow *ui;

signals:
    void getExchangeInfo();
    void getAllTradesOfPair(qint64, QString);
    void tradeList(QJsonArray);
    void tradeHistoryPairComplate();
};


class ChoosePair : public QWidget
{
    Q_OBJECT
public:
    ChoosePair(QString symbol, QString _quote, QString qAmount, QJsonObject jobj);
    ~ChoosePair();
    QString getName();
    QString getQuote();
    QString getQuantity();
    QJsonObject getJObject();
private:
    QJsonObject jobject;
    QLabel *label;
    QString quote;
    QDoubleSpinBox *spBox;
};


#endif // MAINWINDOW_H
