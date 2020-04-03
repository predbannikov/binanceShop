#ifndef LISTPAIRS_H
#define LISTPAIRS_H

#include <QObject>
#include <QBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QTableView>
#include <QCheckBox>
#include <QHeaderView>
#include <QPushButton>
#include <QCheckBox>
#include "pairs.h"

class ListPairs : public QWidget
{
    Q_OBJECT
public:
    explicit ListPairs(QWidget *parent = nullptr);
    ~ListPairs();
    void setModel(ModelPairs *_modelPairs);
    void setListFavorite(QStringList list);
    QStringList getListFavoritCoin();
    void setStatisticsPairs(QMap<QString, Statistic> stats);
    void setDataInfo(AccountInfo *_accountInfo, ExchangeInfo *_exchInfo);
private:
    ModelPairs *modelPairs;
    MySortFilterProxyModel *proxyModel;
    AccountInfo *accountInfo;
    ExchangeInfo *exchangeInfo;
    QTableView *tablePairs;
    QLabel *lblProf;

    QCheckBox *checkBTC;
    QCheckBox *checkETH;
    QCheckBox *checkBNB;
    QCheckBox *checkUSDT;
    void change();
    QStringList filterQuote;
    QStringList favoritCoins;
    QString patternFilter;
private slots:
    void getSymbol(QModelIndex index);
    void getCurStatProf(QMap <QString, QString> map);


signals:
    void getPair(QJsonObject);
public slots:
    void selectedRow(QModelIndex index);
    void textFilterChanged(QString regexp);
//    void setStateFavorite(QModelIndex index);
};

#endif // LISTPAIRS_H
