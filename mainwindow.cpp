#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    launch(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MessageProcess msgProc;

#if defined(Q_OS_WIN)
    if(!QFile::exists(".\\config")) {
        QDir dir;
        dir.mkdir(".\\config");
    }
    if(!QFile::exists(".\\config\\trades")) {
        QDir dir;
        dir.mkdir(".\\config\\trades");
    }
    if(!QFile::exists(".\\config\\history")) {
        QDir dir;
        dir.mkdir((".\\config\\history"));
    }
#elif defined(Q_OS_LINUX)
    if(!QFile::exists("./config")) {
        QDir dir;
        dir.mkdir("./config");
    }
    if(!QFile::exists("./config/trades")) {
        QDir dir;
        dir.mkdir("./config/trades");
    }
    if(!QFile::exists("./config/history")) {
        QDir dir;
        dir.mkdir(("./config/history"));
    }
#endif



    if(!loadConfig(msgProc)){
        qDebug() << msgProc.msg;
        ui->statusBar->showMessage(msgProc.msg);
    } else
    {
        QString msg = "config load success";
        qDebug() << msg;
    }


    accountInfo = new AccountInfo;
    eventReport = new EventReport;
    exchangeInfo = new ExchangeInfo;


    thread = new QThread;
    th_restApi = new RestApi;
    th_restApi->setKeys(userData.secret_key, userData.api_key);
    th_restApi->moveToThread(thread);
    connect(this, &MainWindow::getExchangeInfo, th_restApi, &RestApi::getExchangeInfo);
    connect(th_restApi, &RestApi::sendExchangeInfo, this, &MainWindow::receivExchangeInfo);
    connect(this, &MainWindow::tradeList, th_restApi, &RestApi::tradeList);
    connect(th_restApi, &RestApi::sendReceivTradeList, this, &MainWindow::receivTradeList);
    connect(this, &MainWindow::getAllTradesOfPair, th_restApi, &RestApi::getTradesOfPair);
    connect(th_restApi, &RestApi::sendReceivTradesOfPair, this, &MainWindow::receivAllTradesOfPair);

    connect(thread, &QThread::finished, th_restApi, &QObject::deleteLater);
    connect(thread, &QThread::started, th_restApi, &RestApi::doWork);
    thread->start();

    emit getExchangeInfo();

    modelPairs = new ModelPairs(this);
    listPairs = new ListPairs;
    ui->centralWidget->layout()->addWidget(listPairs);
    listPairs->setModel(modelPairs);


    connect(ui->regExpFilter, &QLineEdit::textChanged, listPairs, &ListPairs::textFilterChanged);

    flowlayout = new FlowLayout(ui->chooseLayout);

//    ui->cont = new QWidget(this);
//    ui->contain->setHorizontalSpacing(0);

    ticketArr = new TicketArr(this);
    connect(ticketArr, &TicketArr::sendMsg, modelPairs, &ModelPairs::parsing);
    connect(ticketArr, &TicketArr::finishConnect, this, &MainWindow::startProc);
    connect(listPairs, &ListPairs::getPair, this, &MainWindow::choosePair);



    dataUserStream = new DataUserStream(th_restApi, eventReport, accountInfo, this);
    connect(dataUserStream, &DataUserStream::sendAccountInfo, this, &MainWindow::updateBalance);
    connect(dataUserStream, &DataUserStream::sendEventReport, this, &MainWindow::eventReportUpdate);


//    QVBoxLayout *vbLayoutBalances;

    for(QString str: quotes) {
        QLabel *label = new QLabel(this);
        label->setObjectName(str);
        label->setText(str);
        ui->hlQuoteBalances->addWidget(label);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receivExchangeInfo(QJsonObject obj)
{
    exchangeInfo->timezone = obj["timezone"].toString();
    exchangeInfo->serverTime = static_cast<quint64>(obj["serverTime"].toDouble());
    QJsonArray array1 = obj["rateLimits"].toArray();
    for(int i = 0; i < array1.size(); i++)
    {
        RateLimits rate;
        rate.rateLimitType =  array1[i].toObject().value("rateLimitType").toString();
        rate.interval = array1[i].toObject().value("interval").toString();
        rate.limit = array1[i].toObject().value("limit").toInt();
        exchangeInfo->rateLimits.append(rate);
    }
    QJsonArray array2 = obj["symbols"].toArray();
    for(int i = 0; i < array2.size(); i++)
    {
        Symbols symbols;
        symbols.symbol =  array2[i].toObject().value("symbol").toString();
        symbols.status =  array2[i].toObject().value("status").toString();
        symbols.baseAsset =  array2[i].toObject().value("baseAsset").toString();
        symbols.baseAssetPrecision =  array2[i].toObject().value("baseAssetPrecision").toInt();
        symbols.quoteAsset =  array2[i].toObject().value("quoteAsset").toString();
        symbols.quotePrecision =  array2[i].toObject().value("quotePrecision").toInt();

        QJsonArray arr = array2[i].toObject().value("orderTypes").toArray();
        for(int j = 0; j < arr.size(); j++)
            symbols.orderTypes.append(arr[j].toString());
        symbols.icebergAllowed =  array2[i].toObject().value("icebergAllowed").toBool();
        QJsonArray arr2 = array2[i].toObject().value("filters").toArray();
        for(int j = 0; j < arr2.size(); j++)
        {
            QJsonObject fil = arr2[j].toObject();
            if(fil["filterType"].toString() == "PRICE_FILTER")
            {
                symbols.filters.priceFilter.minPrice = fil["minPrice"].toString();
                symbols.filters.priceFilter.maxPrice = fil["maxPrice"].toString();
                symbols.filters.priceFilter.tickSize = fil["tickSize"].toString();
            } else if (fil["filterType"] == "LOT_SIZE")
            {
                symbols.filters.lotSize.minQty = fil["minQty"].toString();
                symbols.filters.lotSize.maxQty = fil["maxQty"].toString();
                symbols.filters.lotSize.stepSize = fil["stepSize"].toString();

            } else if (fil["filterType"] == "MIN_NOTIONAL")
            {
                symbols.filters.minNotional.minNotional = fil["minNotional"].toString();
            }
        }
        exchangeInfo->symbols[symbols.symbol] = symbols;
    }

}

void MainWindow::updateBalance()
{
    for(QString str: quotes) {
        QLabel *label =  ui->widget->findChild<QLabel *>(str, Qt::FindDirectChildrenOnly);
        label->setStyleSheet("QLabel { background-color : black; color : green; }");
        label->setText(QString("%1%2").arg(str, -7).arg(QString::number(accountInfo->B[str].f.toDouble() + accountInfo->B[str].l.toDouble()), 12));
    }
}

void MainWindow::choosePair(QJsonObject jobj)
{
    QString symbol = jobj["symbol"].toString();
    QString lastPrice = jobj["lastPrice"].toString();
    QList<QLayoutItem *> listItem = flowlayout->getItems();
    QStringList listSymbol;
    for(int i = 0; i < listItem.size(); i++) {
        ChoosePair *chooser = qobject_cast<ChoosePair *>(listItem[i]->widget());
        listSymbol << chooser->getName();
    }
    if(listSymbol.contains(symbol))
        return;


    ChoosePair *chPair = new ChoosePair(symbol, ui->comboBox->currentText(), QString::number(ui->spinAmount->value()), jobj);
    flowlayout->addWidget(chPair);
}

void MainWindow::eventReportUpdate(EventReport *a_eventReport)
{

}

bool MainWindow::loadConfig(MessageProcess &msgProc)
{
    QString pathConfig;
#if defined(Q_OS_WIN)
    pathConfig = ".\\config\\config.json";
#elif defined(Q_OS_LINUX)
    pathConfig = "./config/config.json";
#endif
    QFile fileConfig(pathConfig);
    if(!fileConfig.open(QFile::ReadOnly | QFile::Text))
    {
        msgProc.msg = "Not open file config for loading";
        msgProc.result = false;
        return false;
    }
    QJsonDocument jdoc = QJsonDocument::fromJson(fileConfig.readAll());
    fileConfig.close();
    QJsonObject obj = jdoc.object();
    userData.api_key = obj["api_key"].toString();
    userData.secret_key = obj["secret_key"].toString();

    ui->leApiKey->setText(userData.api_key);
    ui->leSecretKey->setText(userData.secret_key);
    return true;
}

QStringList MainWindow::loadFavorits()
{
    QString pathConfig;
#if defined(Q_OS_WIN)
    pathConfig = ".\\config\\favorits";
#elif defined(Q_OS_LINUX)
    pathConfig = "./config/favorits";
#endif
    QStringList list;
    QFile fileConfig(pathConfig);
    if(!fileConfig.open(QFile::ReadOnly | QFile::Text))
    {
        QString msg = "Not open file favorits for read";
        qDebug() << msg;
        ui->statusBar->showMessage(msg);
        return list;
    }
    QJsonDocument jdoc = QJsonDocument::fromJson(fileConfig.readAll());
    fileConfig.close();
    QJsonArray jarr = jdoc.array();
    for(int i = 0; i < jarr.size(); i++) {
        list.append(jarr[i].toString());
    }
    return list;

}

void MainWindow::saveConfig()
{
    QString pathConfig;
#if defined(Q_OS_WIN)
    pathConfig = ".\\config\\config.json";
#elif defined(Q_OS_LINUX)
    pathConfig = "./config/config.json";
#endif
    QFile fileConfig(pathConfig);
    if(!fileConfig.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Not open file config for loading";
        return;
    }
    QJsonObject jobj;
    jobj["api_key"] = ui->leApiKey->text();
    jobj["secret_key"] = ui->leSecretKey->text();
    QJsonDocument jdoc(jobj);
    fileConfig.write(jdoc.toJson());
    fileConfig.close();
}

void MainWindow::receivTradeList(QJsonArray jarray)
{
    QString message;
    for(int i = 0; i < jarray.size(); i++) {
        QJsonObject jobj = jarray[i].toObject();
#if defined(Q_OS_WIN)
        QString pathFile = ".\\config\\trades\\";
#elif defined(Q_OS_LINUX)
        QString pathFile = "./config/trades/";
#endif
        pathFile.append(jobj["symbol"].toString());
        QFile file(pathFile);
        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            QString msg = QString("Not open file %1 for writing").arg(pathFile);
            qDebug() << msg;
            message.append(msg + "\t");
            return;
        }
        file.write(jobj["data"].toString().toUtf8());
        file.close();
        if(jobj.contains("msg"))
            message.append(jobj["msg"].toString() + jobj["symbol"].toString() + "\t");
    }
    ui->statusBar->showMessage(message);
}

void MainWindow::calcQuantity(QString &vol, QString symbol)
{
    double value = vol.toDouble();
    double step = exchangeInfo->symbols[symbol].filters.lotSize.stepSize.toDouble();
    double res = static_cast<quint64>(value/step)*step;
    vol = QString::number(res, 'f', 8);
}

//void MainWindow::calcAllTradesOfPair()
//{
//    QString quote = "0.0";
//    QString base = "0.0";
//    QString commission = "0.0";
//    QString volume = "0.0";

//    for(int i = 0; i < filledTrades.size(); i++)
//    {
//        if(filledTrades[i].side == "BUY") {
//            base = QString::number(base.toDouble() + filledTrades[i].origQty.toDouble());
//            quote = QString::number(quote.toDouble() - filledTrades[i].cummulativeQuoteQty.toDouble());
//            volume = QString::number(volume.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());

//        }
//        else {
//            base = QString::number(base.toDouble() - filledTrades[i].origQty.toDouble());
//            quote = QString::number(quote.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());
//            volume = QString::number(volume.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());
//        }
////        qDebug() << "QUOTE=" << quote;
//    }
//    commission = QString::number(volume.toDouble() / 100.0 * 0.075);
//    quote = QString::number(quote.toDouble() - commission.toDouble(), 'f', 8);
//    QString totalSpent;
//    QString needPrice;
//    QString resultStr;
//    totalSpent = quote;
//    if(quote.toDouble() < 0)
//    {
//        needPrice = QString::number((quote.toDouble() * -1.0) / base.toDouble(), 'f', 8);
//        resultStr = QString("totalSpent = %1 needPrice = %2").arg(totalSpent).arg(needPrice);
//    }
//    else
//    {
//        resultStr = QString("%1 %2").arg("totalSpent = " + totalSpent, -25).arg("Profit = " + totalSpent, 25);
//    }

//    qDebug() << resultStr;
//    QString symbol;
//    if(!filledTrades.isEmpty())
//    {
//        symbol = filledTrades.first().symbol;
//        Statistic stat;
//        stat.totalSpend = totalSpent;
//        statistics.insert(symbol, stat);
//    }
//}
void MainWindow::calcAllTradesOfPair(QJsonObject &jobj, QJsonArray jarrOld, QJsonArray jarray)      // Не разрешать пустой jarray
{
    QString quote = "0.0";
    QString base = "0.0";
    QString commission = "0.0";
    QString volume = "0.0";
    qint64 time = 2554472357344;
    int count = 0;
    for(int i = 0; i < jarray.size(); i++)
    {
//        QJsonObject jobj = jarray[i].toObject();
//        if(static_cast<qint64> (jarray[i].toObject()["updateTime"].toDouble()) > time)
//            time = static_cast<qint64> (jarray[i].toObject()["updateTime"].toDouble());
        if(jarray[i].toObject()["status"] == "FILLED" || jarray[i].toObject()["status"] == "PARTIALLY_FILLED")
        {
            bool check = false;
            for(int k = 0; k < jarrOld.size(); k++)
                if(jarray[i].toObject()["clientOrderId"].toString() == jarrOld[k].toObject()["clientOrderId"].toString() && jarrOld[k].toObject()["status"].toString() == "FILLED") {
//                    qDebug() << "has already ";
                    check = true;
                    break;
                }
            if(check)
                continue;

            if(jarray[i].toObject()["side"] == "BUY") {
                base = QString::number(base.toDouble() + jarray[i].toObject()["origQty"].toString().toDouble(), 'f', 8);
                quote = QString::number(quote.toDouble() - jarray[i].toObject()["cummulativeQuoteQty"].toString().toDouble(), 'f', 8);
                volume = QString::number(volume.toDouble() + jarray[i].toObject()["cummulativeQuoteQty"].toString().toDouble(), 'f', 8);
                count++;
            } else if(jarray[i].toObject()["side"] == "SELL") {
                base = QString::number(base.toDouble() - jarray[i].toObject()["origQty"].toString().toDouble(), 'f', 8);
                quote = QString::number(quote.toDouble() + jarray[i].toObject()["cummulativeQuoteQty"].toString().toDouble(), 'f', 8);
                volume = QString::number(volume.toDouble() + jarray[i].toObject()["cummulativeQuoteQty"].toString().toDouble(), 'f', 8);
                count++;
            } else
                qDebug() << "Error: not defined state";
        } else if(jarray[i].toObject()["status"] == "NEW") {
            if(static_cast<qint64> (jarray[i].toObject()["time"].toDouble()) < time)
                time = static_cast<qint64> (jarray[i].toObject()["time"].toDouble());
        } else if(jarray[i].toObject()["status"] == "CANCELED") {

        }
    }
    qDebug() << "calc base =" << base << " count=" << count << "quote=" << quote;
    jobj["lastTime"] = QString::number(time);
    commission = QString::number(volume.toDouble() / 100.0 * 0.075, 'f', 8);
    jobj["totalSpend"] = QString::number(quote.toDouble() - commission.toDouble(), 'f', 8);
    jobj["totalVolume"] = volume;
}

//QList<TradesPair> MainWindow::parsAllMyTrades(QJsonArray jarray)
//{
//    tradesList.clear();
//    filledTrades.clear();
//    for(int i = 0; i < jarray.size(); i++) {
//        QJsonObject jobj = jarray[i].toObject();
//        TradesPair trPair;
//        trPair.symbol = jobj["symbol"].toString();
//        trPair.origQty = jobj["origQty"].toString();
//        trPair.executedQty = jobj["executedQty"].toString();
//        trPair.cummulativeQuoteQty = jobj["cummulativeQuoteQty"].toString();
//        trPair.price = jobj["price"].toString();
//        if(qFuzzyIsNull(trPair.price.toDouble()))
//            trPair.price = QString::number(trPair.cummulativeQuoteQty.toDouble() / trPair.origQty.toDouble(), 'f', 8);
//        trPair.status = jobj["status"].toString();
//        trPair.side = jobj["side"].toString();
////        trPair.amount = QString::number(trPair.price.toDouble() * trPair.origQty.toDouble(), 'f', 8);
//        trPair.time = static_cast <qint64> (jobj["time"].toDouble());
//        trPair.isWorking = jobj["isWorking"].toBool();

//        tradesList.append(trPair);
//    }
//    int count = 0;
//    for(int i = 0; i < tradesList.size(); i++)
//    {
//        QString type;
//        if(tradesList[i].status == "FILLED" || tradesList[i].status == "PARTIALLY_FILLED") {
//            count++;
//            filledTrades.prepend(tradesList[i]);

//        } /*else if(tradesList[i].status != "CANCELED"){
//            qDebug() << tradesList[i].status;
//        }*/
//    }
//    return tradesList;
//}

void MainWindow::receivAllTradesOfPair(QJsonArray jarray, QString symbol)
{
    emit tradeHistoryPairComplate();
    if(jarray.isEmpty()) {
        QString msg =  "receivAllTradesOfPair " + symbol + " isEmpty";
//        ui->statusBar->showMessage(msg);
        qDebug() << msg;
        return;
    }
    qDebug() << "receivAllTradesOfPair " << symbol << jarray.size();
    QJsonObject jobj = readHistory(symbol);
//    QJsonArray jarr = jobj["data"].toArray();
//    QString totalSpend;
//    QString totalVolume;
//    QString lastTime;
    QJsonObject _jobj;
    QJsonArray jarr = jobj["data"].toArray();
    if(jobj.isEmpty()) {
        calcAllTradesOfPair(jobj, jarr, jarray);
        jobj["totalSpend"] = _jobj["totalSpend"].toString();
        jobj["totalVolume"] = _jobj["totalVolume"].toString();
        jobj["lastTime"] = _jobj["lastTime"].toString();
        jobj["balance"] = QString::number(accountInfo->B[exchangeInfo->symbols[symbol].baseAsset].f.toDouble() + accountInfo->B[exchangeInfo->symbols[symbol].baseAsset].l.toDouble(), 'f', 8);
        jobj["data"] = jarray;
    } else {
        calcAllTradesOfPair(_jobj, jarr, jarray);
        jobj["totalSpend"] = QString::number(jobj["totalSpend"].toString().toDouble() + _jobj["totalSpend"].toString().toDouble(), 'f', 8);
        jobj["totalVolume"] = QString::number(jobj["totalVolume"].toString().toDouble() + _jobj["totalVolume"].toString().toDouble(), 'f', 8);
        jobj["lastTime"] = _jobj["lastTime"].toString();
        jobj["balance"] = QString::number(accountInfo->B[exchangeInfo->symbols[symbol].baseAsset].f.toDouble() + accountInfo->B[exchangeInfo->symbols[symbol].baseAsset].l.toDouble(), 'f', 8);
        for(int i = 0; i < jarray.size(); i++) {
            jarr.append(jarray[i]);
        }
        jobj["data"] = jarr;
    }
    writeHistory(jobj, symbol);         // Затестить записывалку
}

QJsonObject MainWindow::readHistory(QString symbol)                                                         // READ
{
//    QStringList listFiles = QDir(path).entryList(QDir::Files, QDir::NoSort);
#if defined(Q_OS_WIN)
    QFile file(".\\config\\history\\" + symbol);
#elif defined(Q_OS_LINUX)
    QFile file("./config/history/" + symbol);
#endif


    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString msg = QString("Not open file %1 for read").arg(symbol);
        qDebug() << msg;
        ui->statusBar->showMessage(msg);
        return QJsonObject();
    }
    QByteArray array = file.readAll();
    file.close();

    return QJsonDocument::fromJson(array).object();
}

void MainWindow::writeHistory(QJsonObject jobj, QString symbol)                                             // WRITE
{
#if defined(Q_OS_WIN)
    QFile file(".\\config\\history\\" + symbol);
#elif defined(Q_OS_LINUX)
    QFile file("./config/history/" + symbol);
#endif
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QString msg = QString("Not open file %1 for read").arg(jobj["data"].toArray().last()["symbol"].toString());
        qDebug() << msg;
        ui->statusBar->showMessage(msg);
        return ;
    }
    file.write(QJsonDocument(jobj).toJson());
    file.close();
}

void MainWindow::checkHistory(QString symbol, QString &bal, QString &lastTime)
{
    QJsonObject jobj = readHistory(symbol);
    bal = jobj["balance"].toString();
    lastTime = jobj["lastTime"].toString();
}

void MainWindow::startProc()
{
    if(!launch)
    {
        launch = true;
        connect(&timerUpdateHisory, &QTimer::timeout, this, &MainWindow::updateHistory);
        timerUpdateHisory.start(60000 * 1);

        dataUserStream->checkBalances();
        favorits = loadFavorits();
        listPairs->setListFavorite(favorits);
        listPairs->setDataInfo(accountInfo, exchangeInfo);

        for(int i = 0; i < favorits.size(); i++) {
            prepStatistic(favorits[i]);
        }
        listPairs->setStatisticsPairs(statistics);
    }
}

void MainWindow::updateHistory()
{
    favorits = listPairs->getListFavoritCoin();
    for(int i = 0; i < favorits.size(); i++) {
        QString lastTimeStr;
        qint64 lastTime = 0;
#if defined(Q_OS_WIN)
        QString path = ".\\config\\history\\";
#elif defined(Q_OS_LINUX)
        QString path = "./config/history/";
#endif
        path.append(favorits[i]);
        qDebug() << "updateHistory " << favorits[i];
        if(!QFile::exists(path)) {
            lastTime = QDateTime::fromString("2018-09-01 00:00:00", "yyyy-MM-dd hh:mm:ss").toMSecsSinceEpoch();
            emit getAllTradesOfPair(lastTime, favorits[i]);
            qDebug() << "updateHistory if empty" << favorits[i] << QDateTime::fromMSecsSinceEpoch(lastTime).toString("yyyy-MM-dd hh:mm:ss");
            QEventLoop loop;
            connect(this, &MainWindow::tradeHistoryPairComplate, &loop, &QEventLoop::quit);
            loop.exec();
        } else {
            QString bal;
            checkHistory(favorits[i], bal, lastTimeStr);
            lastTime = lastTimeStr.toLongLong();
            if(/*(QDateTime::currentMSecsSinceEpoch() - lastTime) > 60000 * 60 ||*/ !qFuzzyCompare(bal.toDouble(), accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].f.toDouble() + accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].l.toDouble())) {
                emit getAllTradesOfPair(lastTime, favorits[i]);
                qDebug() << " bal =" << bal.toDouble() << " accBal =" << accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].f.toDouble() + accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].l.toDouble();
                qDebug() << "updateHistory check" << favorits[i] << QDateTime::fromMSecsSinceEpoch(lastTime).toString("yyyy-MM-dd hh:mm:ss") << " balCheck=" << qFuzzyCompare(bal.toDouble(), accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].f.toDouble() + accountInfo->B[exchangeInfo->symbols[favorits[i]].baseAsset].l.toDouble());
                QEventLoop loop;
                connect(this, &MainWindow::tradeHistoryPairComplate, &loop, &QEventLoop::quit);
                loop.exec();
            }
        }


        prepStatistic(favorits[i]);
    }
    qDebug() << "updateHisotry setStatistics";
    listPairs->setStatisticsPairs(statistics);
}

void MainWindow::prepStatistic(QString symbol)
{
    QJsonObject jobj = readHistory(symbol);
    QString totalSpend = jobj["totalSpend"].toString();

    Statistic stat;
    stat.totalSpend = totalSpend;
    statistics.insert(symbol, stat);
}

void MainWindow::on_pushButton_clicked()
{
    saveConfig();
}

ChoosePair::ChoosePair(QString symbol, QString _quote, QString qAmount, QJsonObject jobj)
{
    QString lastPrice = jobj["lastPrice"].toString();
    jobject = jobj;
    quote = _quote;
    QHBoxLayout *hbLayout = new QHBoxLayout;
    QPushButton *pbutton = new QPushButton;
    QSizePolicy sizePolicy = pbutton->sizePolicy();
    sizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
    pbutton->setSizePolicy(sizePolicy);
    pbutton->setText("delete");
    label = new QLabel(this);
    label->setText(symbol);
    hbLayout->addWidget(label);
    hbLayout->addWidget(pbutton);
    spBox = new QDoubleSpinBox(this);
    connect(pbutton, &QPushButton::clicked, this, &ChoosePair::deleteLater);
    spBox->setValue(qAmount.toDouble());
    spBox->setDecimals(8);
    spBox->setMaximum(1000000.0);
    spBox->setMinimum(0.00000001);
    spBox->setSingleStep(0.001);
    QVBoxLayout *vbLayout = new QVBoxLayout(this);
    vbLayout->addLayout(hbLayout);
    vbLayout->addWidget(spBox);
    setLayout(vbLayout);

}

ChoosePair::~ChoosePair()
{
//    QStringList list = this->findChildren();
}

QString ChoosePair::getName()
{
    return label->text();
}

QString ChoosePair::getQuote()
{
    return quote;
}

QString ChoosePair::getQuantity()
{
    double amount = spBox->value() / jobject["lastPrice"].toString().toDouble();
    return QString::number(amount, 'f', 8);
}

QJsonObject ChoosePair::getJObject()
{
    return jobject;
}

void MainWindow::on_pbBuy_clicked()
{
    QList<QLayoutItem *> listItem = flowlayout->getItems();
    QJsonArray jarray;
    for(int i = 0; i < listItem.size(); i++) {
        ChoosePair *chooser = qobject_cast<ChoosePair *>(listItem[i]->widget());
        QJsonObject jobj;
        jobj["symbol"] = chooser->getName();
        QString quantity = chooser->getQuantity();
        QJsonObject jobject = chooser->getJObject();

        calcQuantity(quantity, chooser->getName());

        while((quantity.toDouble() * jobject["lastPrice"].toString().toDouble()) < exchangeInfo->symbols[chooser->getName()].filters.minNotional.minNotional.toDouble())
            quantity = QString::number(quantity.toDouble() + exchangeInfo->symbols[chooser->getName()].filters.lotSize.stepSize.toDouble());


        jobj["quantity"] = quantity;
        jobj["side"] = "BUY";
        jarray.append(jobj);
    }
    emit tradeList(jarray);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->statusBar->showMessage("not working this button");
    updateHistory();
}
