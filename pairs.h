#ifndef PAIRS_H
#define PAIRS_H

#include <QObject>
#include <QAbstractTableModel>
#include <QtWebSockets/QtWebSockets>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QMetaType>
#include <QPointF>
#include <QVector>
#include <QMouseEvent>
#include <QTimer>
#include "global.h"
//#include "tickerarr.h"




//*********************    TableModel    ************************

class ModelPairs: public QAbstractTableModel
{
    Q_OBJECT
public:


    ModelPairs(QObject* parent = 0);
    const int COLUMN = 10;
    int rowCount( const QModelIndex& ) const override;
    int columnCount( const QModelIndex&  ) const override;
    QVariant data( const QModelIndex& index, int role ) const;
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    void clearCurrencies();
    void applyChanges();
    void setNewPairs();
    void findProfit();
    enum KeyMarkets{
        filter=0 , Label, PriceChangePercent, LastPrice, SPREAD,
        QuoteVolume, BALANCE, TotalSpend, BalanceInQuote, Profit
    };
    bool chkfiltr;
    bool chkFindProfit = 0;
    QStringList favoriteCoin;
    QList<Currency> currencies;
    QMap<QString, Currency> mapCurrencies;
    Currency currentCurrency;
    QMap<QString, Statistic> statistics;
    AccountInfo *accountInfo;
    ExchangeInfo *exchangeInfo;

    QStringList getFavoritCoin();
    void setFavoritCoin(QStringList list);
    void setStatisticsPair(QMap<QString, Statistic> stats);
    void setDataInfo(AccountInfo *accInfo, ExchangeInfo *_exchInfo);
    void checkFavorit(bool check, QModelIndex index);

public slots:
    void parsing(QJsonArray *array);

public slots:

//    void replaceCheck(QModelIndex index);
//    void selectedRow(QModelIndex index);


signals:
    void getMarket(QModelIndex);
    void sendFavorite(QModelIndex);
    void sendCurProf(QMap <QString, QString> map);
};

class MySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MySortFilterProxyModel(QObject *parent = 0);
    bool setData(const QModelIndex &index,const QVariant& value, int role ) override;

//    QDate filterMinimumDate() const { return minDate; }
//    void setFilterMinimumDate(const QDate &date);

//    QDate filterMaximumDate() const { return maxDate; }
//    void setFilterMaximumDate(const QDate &date);
    void setCheckFavorite(bool check);
    void setFavoritCoin(QStringList list);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
//    bool dateInRange(const QDate &date) const;
    bool checkFavorite;
    QStringList favoritCoin;

};


class StarRating
{
public:
    enum EditMode { Editable, ReadOnly };

    explicit StarRating(int starCount = 1, int maxStarCount = 5);

    void paint(QPainter *painter, const QRect &rect,
               const QPalette &palette, EditMode mode) const;
    QSize sizeHint() const;
    int starCount() const { return myStarCount; }
    int maxStarCount() const { return myMaxStarCount; }
    void setStarCount(int starCount) { myStarCount = starCount; }
    void setMaxStarCount(int maxStarCount) { myMaxStarCount = maxStarCount; }

private:
    QPolygonF starPolygon;
    QPolygonF diamondPolygon;
    int myStarCount;
    int myMaxStarCount;
};

class StarDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    StarDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QString displayText( const QVariant & value, const QLocale & locale ) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private slots:
    void commitAndCloseEditor();
};

class StarEditor : public QWidget
{
    Q_OBJECT

public:
    StarEditor(QWidget *parent = 0);

    QSize sizeHint() const override;
    void setStarRating(const StarRating &starRating) {
        myStarRating = starRating;
    }
    StarRating starRating() { return myStarRating; }

signals:
    void editingFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int starAtPosition(int x);

    StarRating myStarRating;
};



Q_DECLARE_METATYPE(StarRating)


#endif // PAIRS_H
