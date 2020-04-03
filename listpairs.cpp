#include "listpairs.h"

ListPairs::ListPairs(QWidget *parent) : QWidget(parent)
{
//    QWidget *widget = new QWidget;
//    this->layout()->addWidget(widget);
//    this->setMaximumWidth(400);
//    this->setMinimumWidth(390);

    QVBoxLayout *vblayout = new QVBoxLayout;
    QHBoxLayout *hblayout = new QHBoxLayout;
    QHBoxLayout *hblayoutStat = new QHBoxLayout;

    tablePairs = new QTableView(this);
    checkBTC = new QCheckBox("BTC", this);
    checkETH = new QCheckBox("ETH", this);
    checkBNB = new QCheckBox("BNB", this);
    checkUSDT = new QCheckBox("USDT", this);
    QCheckBox   *chkbx = new QCheckBox("Favorit", this);
    connect(chkbx, &QCheckBox::clicked, [=](bool check) {
       proxyModel->setCheckFavorite(check);
    });
    hblayoutStat->addWidget(chkbx);

    lblProf = new QLabel("***");
    hblayoutStat->addWidget(lblProf);

    hblayout->setSpacing(0);
    hblayout->setMargin(0);
    hblayout->addWidget(checkBTC);
    hblayout->addWidget(checkETH);
    hblayout->addWidget(checkBNB);
    hblayout->addWidget(checkUSDT);

    vblayout->addLayout(hblayoutStat);
    vblayout->addLayout(hblayout);
    vblayout->addWidget(tablePairs);
    this->setLayout(vblayout);

}

ListPairs::~ListPairs()
{
    qDebug() << "destructor list pair";
    QFile file("./config/favorits");
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to write favorits";
        return;
    }
    QJsonArray jarray;
    for(int i = 0; i < favoritCoins.size(); i++) {
        jarray.append(favoritCoins[i]);
    }
    file.write(QJsonDocument(jarray).toJson());
    file.close();
}

void ListPairs::setModel(ModelPairs *_modelPairs)
{
    modelPairs = _modelPairs;
    proxyModel = new MySortFilterProxyModel(this);

    proxyModel->setSourceModel(modelPairs);
    tablePairs->setModel(proxyModel);


    tablePairs->setItemDelegateForColumn(2, new StarDelegate);
    tablePairs->setItemDelegateForColumn(3, new StarDelegate);
    tablePairs->setItemDelegateForColumn(4, new StarDelegate);
    tablePairs->setItemDelegateForColumn(5, new StarDelegate);
    tablePairs->setItemDelegateForColumn(6, new StarDelegate);
    tablePairs->setItemDelegateForColumn(7, new StarDelegate);
    tablePairs->setItemDelegateForColumn(8, new StarDelegate);
    tablePairs->setItemDelegateForColumn(9, new StarDelegate);
    tablePairs->setItemDelegateForColumn(10, new StarDelegate);
    tablePairs->setItemDelegateForColumn(11, new StarDelegate);
    tablePairs->setItemDelegateForColumn(12, new StarDelegate);
    tablePairs->setItemDelegateForColumn(13, new StarDelegate);
    tablePairs->setItemDelegateForColumn(14, new StarDelegate);
    tablePairs->setItemDelegateForColumn(15, new StarDelegate);
    tablePairs->setItemDelegateForColumn(16, new StarDelegate);
    tablePairs->setEditTriggers(QAbstractItemView::DoubleClicked
                                | QAbstractItemView::SelectedClicked);
    tablePairs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablePairs->setSortingEnabled(true);

    tablePairs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablePairs->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    tablePairs->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
//    tablePairs->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tablePairs->verticalHeader()->setDefaultSectionSize(18);


    QObject::connect(tablePairs, SIGNAL(clicked(QModelIndex)), this, SLOT(selectedRow(QModelIndex)) );
//    QObject::connect(modelPairs, SIGNAL(sendFavorite(QModelIndex)), this, SLOT(setStateFavorite(QModelIndex)) );


    QObject::connect(checkBTC, &QCheckBox::clicked, [=](bool checked) {
        if(checked)
            filterQuote.append("BTC");
        else
            filterQuote.removeOne("BTC");
        change();
    });
    QObject::connect(checkETH, &QCheckBox::clicked, [=](bool checked) {
        if(checked)
            filterQuote.append("ETH");
        else
            filterQuote.removeOne("ETH");
        change();
    });
    QObject::connect(checkBNB, &QCheckBox::clicked, [=](bool checked) {
        if(checked)
            filterQuote.append("BNB");
        else
            filterQuote.removeOne("BNB");
        change();
});
    QObject::connect(checkUSDT, &QCheckBox::clicked, [=](bool checked) {
        if(checked)
            filterQuote.append("USDT");
        else
            filterQuote.removeOne("USDT");
        change();
    });

    QObject::connect(modelPairs, &ModelPairs::getMarket, this, &ListPairs::getSymbol);
    QObject::connect(modelPairs, &ModelPairs::sendCurProf, this, &ListPairs::getCurStatProf);


}

void ListPairs::setListFavorite(QStringList list)
{
    favoritCoins = list;
    proxyModel->setFavoritCoin(list);
    modelPairs->setFavoritCoin(list);
}

QStringList ListPairs::getListFavoritCoin()
{
    return favoritCoins;
}

void ListPairs::setStatisticsPairs(QMap<QString, Statistic> stats)
{
    modelPairs->setStatisticsPair(stats);
}

void ListPairs::setDataInfo(AccountInfo *_accountInfo, ExchangeInfo *_exchInfo)
{
    accountInfo = _accountInfo;
    exchangeInfo = _exchInfo;
    modelPairs->setDataInfo(_accountInfo, _exchInfo);
}

void ListPairs::change()
{
    if(filterQuote.size() == 0)
        patternFilter.clear();
    else if(filterQuote.size() == 1)
        patternFilter = QString("%1$").arg(filterQuote.first());
    else {
        patternFilter = QString("(");
        for(int i = 0; i < filterQuote.size() - 1; i++)
            patternFilter.append(filterQuote[i] + "|");
        patternFilter.append(filterQuote.last() + ")$");
    }
    qDebug() << patternFilter;
    textFilterChanged(QString());
}

//void ListPairs::setStateFavorite(QModelIndex index)
//{
////    QModelIndex sindex = modelPairs->index(proxyModel->mapToSource(index).row(), 0, QModelIndex());
////    bool state = modelPairs->data(sindex, Qt::DisplayRole).toBool();
////    if(state)
////        state = false;
////    else
////        state = true;
//////    state = (state) ? false : true;
////    qDebug() << sindex << "sindex state =" << state;
////    modelPairs->setData(sindex, state, Qt::EditRole);
//}

void ListPairs::getSymbol(QModelIndex index)
{

    QModelIndex sindex = modelPairs->index(proxyModel->mapToSource(index).row(), 1, QModelIndex());             // Получаем индекс
    QModelIndex sindexLastPrice = modelPairs->index(proxyModel->mapToSource(index).row(), 5, QModelIndex());

    QString lastPrice = modelPairs->data(sindexLastPrice, Qt::DisplayRole).toString();      // Получаем данные из модели по индексу
    QString symbol = modelPairs->data(sindex, Qt::DisplayRole).toString();
    QJsonObject jobj;
    jobj["lastPrice"] = lastPrice;
    jobj["symbol"] = symbol;
    qDebug() << symbol;
    emit getPair(jobj);
}

void ListPairs::getCurStatProf(QMap <QString, QString> map)
{
    QString result;
    QMapIterator <QString, QString> it(map);
    while(it.hasNext()) {
        it.next();
        result.append(QString("%1 %2").arg(it.key()).arg(it.value(), -15));
    }
    lblProf->setStyleSheet("QLabel { background-color : black; color : green; }");
    lblProf->setText(result);
}

void ListPairs::selectedRow(QModelIndex index)
{
    if(index.column()==0){
        bool state = proxyModel->data(index, Qt::CheckStateRole).toBool();
        if(state)
            state = false;
        else
            state = true;
        proxyModel->setData(index, state, Qt::CheckStateRole);
        favoritCoins = modelPairs->getFavoritCoin();
        proxyModel->setFavoritCoin(favoritCoins);

    } else {
        getSymbol(index);
    }
}

void ListPairs::textFilterChanged(QString regexp)
{
//    regexp.append("(ETH|BTC)$");
    regexp.append(patternFilter);
    QRegExp regExp(regexp, Qt::CaseInsensitive);
    proxyModel->setFilterRegExp(regExp);
}
