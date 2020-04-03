#include "pairs.h"
#include <cmath>

/***********************************************************************************
 ***                                 TableModel                                  **/

ModelPairs::ModelPairs(QObject *parent): QAbstractTableModel(parent)                // Торговые пары
{

    chkfiltr = false;
//    QUOTE_STATE = QUOTE_NON;
}

int ModelPairs::rowCount(const QModelIndex &) const {return currencies.count();}

int ModelPairs::columnCount(const QModelIndex &) const {return COLUMN;}

QVariant ModelPairs::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();
    const Currency &rec = currencies.at(index.row());
//    const Currency &rec = currencies.at(index.row());
    switch(role){
    case Qt::CheckStateRole:
        if (index.column()==filter)
            //                return currencies.at(index.row()).filter;
        {
            bool check = rec.favorit;
            if(check)
            {
                return Qt::Checked;
            } else return Qt::Unchecked;

        }
        break;
    case Qt::BackgroundRole: {
        int key = index.column();
        switch (key) {
        case PriceChangePercent:
            if(rec.priceChangePercent.toDouble() > 0)
                return QColor(0, 255, 0, 50);
            else
                return QColor(255, 0, 0, 50);

        }
    }
    case Qt::DisplayRole:{
        int key = index.column();
        switch( key) {
        case Label:
            return rec.symbol;
        case PriceChangePercent:
            return rec.priceChangePercent.toDouble();
        case LastPrice:
            return rec.lastPrice.toDouble();
        case SPREAD:
            return rec.spread.toDouble();
        case QuoteVolume:
            return rec.quoteVolume.toDouble();
        case TotalSpend:
            return rec.totalSpend.toDouble();
        case BALANCE:
            return rec.balance.toDouble();
        case BalanceInQuote:
            return rec.balanceInQuote.toDouble();
        case Profit:
            if(favoriteCoin.contains(rec.symbol)) {
                if(statistics[rec.symbol].totalSpend != "unknown") {
                    return statistics[rec.symbol].totalSpend.toDouble() + rec.balanceInQuote.toDouble();
//                    if(statistics[rec.symbol].totalSpend.toDouble() > 0) {
//                    } else {
//                        return (rec.balanceInQuote.toDouble() / statistics[rec.symbol].totalSpend.toDouble() - 1) * 100.;
//                    }
                }
            }
            return "unknown";
        }
    }
    default:
        return QVariant();
    }
//     Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    return result;
}

QVariant ModelPairs::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Для любой роли, кроме запроса на отображение, прекращаем обработку
    if (role != Qt::DisplayRole)
        return QVariant();
    // формируем заголовки по номуру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case filter:
            return tr("X");
        case Label:
            return tr("Symbol");
        case PriceChangePercent:
            return tr("ChangePerc");
        case LastPrice:
            return tr("Last Price");
        case SPREAD:
            return tr("SPREAD");
        case QuoteVolume:
            return tr("QuoteVolume");
        case TotalSpend:
            return tr("TotalSpend");
        case BALANCE:
            return tr("Balance");
        case BalanceInQuote:
            return tr("BalInQuote");
        case Profit:
            return tr("curTotal");

        }
    }
    return QVariant();
}

Qt::ItemFlags ModelPairs::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;

}

bool ModelPairs::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
//    for (int row = 0; row < rows; ++row) {
//        currencies.removeAt(position);
//    }
    endRemoveRows();
    return true;
}

bool ModelPairs::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    endInsertRows();
    return true;
}

void ModelPairs::clearCurrencies()
{
    if(currencies.empty())
        return;
    removeRows(0,currencies.count(),QModelIndex());
    currencies.clear();
}

bool ModelPairs::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::CheckStateRole){
        checkFavorit(value.toBool(), index);
        mapCurrencies[data(index.siblingAtColumn(1), Qt::DisplayRole).toString()].favorit = value.toBool();
        qDebug() << " list =" << favoriteCoin;
        return true;

    }
    return false;
}

void ModelPairs::parsing(QJsonArray *array)
{    
    clearCurrencies();

    for(int i=0; i<array->size(); i++)
    {
        QJsonObject object = array->at(i).toObject();
        Currency curr;
        curr.symbol = object["s"].toString();
        curr.quoteVolume = object["q"].toString();
        curr.priceChange = object["p"].toString();
        curr.priceChangePercent = object["P"].toString();
        curr.weightAvePrice = object["w"].toString();
        curr.lastPrice = object["c"].toString();
        curr.lastQuantity = object["Q"].toString();
        curr.bid = object["b"].toString();
        curr.bidQuantity = object["B"].toString();
        curr.ask = object["a"].toString();
        curr.askQuantity = object["A"].toString();
        curr.openPrice = object["o"].toString();
        curr.hightPrice = object["h"].toString();
        curr.lowPrice = object["l"].toString();
        curr.baseVolume = object["v"].toString();
        curr.quoteVolume = object["q"].toString();
//        Balances bal;
//        bal.f = "0.0";
        Balances balance = accountInfo->B.value(exchangeInfo->symbols[curr.symbol].baseAsset, Balances());
        curr.balance = QString::number(balance.f.toDouble() + balance.l.toDouble(), 'f', 8);
        if(curr.balance.toDouble() > 0.00000001)
            curr.balanceInQuote = QString::number(curr.balance.toDouble() * curr.bid.toDouble(), 'f', 8);
        else
            curr.balanceInQuote = "0.0";

        curr.spread = QString::number(100.0 - (curr.bid.toDouble() / curr.ask.toDouble()  * 100.0), 'f', 3);
//        curr.spread = QString::number((curr.ask.toDouble() / curr.bid.toDouble() - 1) * 100.0, 'f', 3);

        if(favoriteCoin.contains(curr.symbol))
        {
            curr.favorit = true;
        }
        else
            curr.favorit = false;
        Statistic stat;
        stat.totalSpend = "unknown";
        curr.totalSpend = statistics.value(curr.symbol, stat).totalSpend;

        mapCurrencies.insert(curr.symbol, curr);
    }
    double comProf = 0;
    QMap <QString, QString> map;
    for(QString str: favoriteCoin) {
        if(map.contains(exchangeInfo->symbols[str].quoteAsset))
            comProf = map[exchangeInfo->symbols[str].quoteAsset].toDouble();
        comProf += statistics[str].totalSpend.toDouble() + mapCurrencies[str].balanceInQuote.toDouble();

        map.insert(exchangeInfo->symbols[str].quoteAsset, QString::number(comProf, 'f', 8));
        comProf = 0;
    }
    emit sendCurProf(map);
    currencies = mapCurrencies.values();
    setNewPairs();
}

void ModelPairs::applyChanges()
{
    int size = currencies.size();
    if(size == 0)
    {
        qDebug() << "Нет элементов в QList<Currencies>";
        return;
    }
    insertRows(0, size, QModelIndex());
}

void ModelPairs::setNewPairs()
{
    applyChanges();
//    clearCurrencies();
}

void ModelPairs::findProfit()
{
    if(chkFindProfit){      // Ищем профит если фильтр включен

    }
}

QStringList ModelPairs::getFavoritCoin()
{
    return favoriteCoin;
}

void ModelPairs::setFavoritCoin(QStringList list)
{
    favoriteCoin = list;
}

void ModelPairs::setStatisticsPair(QMap<QString, Statistic> stats)
{
    statistics = stats;
}

void ModelPairs::setDataInfo(AccountInfo *accInfo, ExchangeInfo *_exchInfo)
{
    accountInfo = accInfo;
    exchangeInfo = _exchInfo;
}

//void ModelPairs::addFavorite(QModelIndex index)
//{
////    currencies[index.row()].favorit = true;
//    QString symbol = data(index.siblingAtColumn(1), Qt::DisplayRole).toString();
//    favoriteCoin.append(symbol);
//}

//void ModelPairs::dellFavorite(QModelIndex index)
//{
////    currencies[index.row()].favorit = false;
//    QString symbol = data(index.siblingAtColumn(1), Qt::DisplayRole).toString();
//    favoriteCoin.removeOne(symbol);
//}

void ModelPairs::checkFavorit(bool check, QModelIndex index)
{
    QModelIndex sindex = index.siblingAtColumn(1);
    QString symbol = data(sindex, Qt::DisplayRole).toString();
    if(check)
        favoriteCoin.append(symbol);
    else
        favoriteCoin.removeOne(symbol);
}


//************************************************************************************************************
//*                                        MySortFilterProxyModel                                            *
//************************************************************************************************************

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      checkFavorite(false)
{

}

bool MySortFilterProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::CheckStateRole){

        QModelIndex sindex = mapToSource(index);
        sourceModel()->setData(sindex, value, Qt::CheckStateRole);
        emit QSortFilterProxyModel::dataChanged(index, index);
        return true;
    }
    return false;
}

void MySortFilterProxyModel::setCheckFavorite(bool check)
{
    checkFavorite = check;
}

void MySortFilterProxyModel::setFavoritCoin(QStringList list)
{
    favoritCoin = list;
}

bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 1, sourceParent);
    if(checkFavorite) {

        return sourceModel()->data(index0).toString().contains(filterRegExp()) && favoritCoin.contains(sourceModel()->data(index0).toString());
    }

    return sourceModel()->data(index0).toString().contains(filterRegExp());
}

bool MySortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
     if(leftData.type() == QVariant::Double)
    {
        return leftData.toDouble() < rightData.toDouble();
    } else
    {
        QRegExp *emailPattern = new QRegExp("([\\w\\.]*@[\\w\\.]*)");

        QString leftString = leftData.toString();
        if(left.column() == 1 && emailPattern->indexIn(leftString) != -1)
             leftString = emailPattern->cap(1);

        QString rightString = rightData.toString();
        if(right.column() == 1 && emailPattern->indexIn(rightString) != -1)
             rightString = emailPattern->cap(1);

        return QString::localeAwareCompare(leftString, rightString) < 0;
     }
}

//************************************************************************************************************
//*                                                  ДЕЛЕГАТЫ                                                *
//************************************************************************************************************

void StarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        starRating.paint(painter, option.rect, option.palette,
                         StarRating::ReadOnly);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QString StarDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return locale.toString(value.toDouble(), 'f', 8);
}

QSize StarDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

QWidget *StarDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
    if (index.data().canConvert<StarRating>()) {
        StarEditor *editor = new StarEditor(parent);
        connect(editor, &StarEditor::editingFinished,
                this, &StarDelegate::commitAndCloseEditor);
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void StarDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());
        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
        starEditor->setStarRating(starRating);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void StarDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
        model->setData(index, QVariant::fromValue(starEditor->starRating()));
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void StarDelegate::commitAndCloseEditor()
{
    StarEditor *editor = qobject_cast<StarEditor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}


StarEditor::StarEditor(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(true);
}

QSize StarEditor::sizeHint() const
{
    return myStarRating.sizeHint();
}

void StarEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    myStarRating.paint(&painter, rect(), this->palette(),
                       StarRating::Editable);
}

void StarEditor::mouseMoveEvent(QMouseEvent *event)
{
    int star = starAtPosition(event->x());

    if (star != myStarRating.starCount() && star != -1) {
        myStarRating.setStarCount(star);
        update();
    }
}

void StarEditor::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit editingFinished();
}

int StarEditor::starAtPosition(int x)
{
    int star = (x / (myStarRating.sizeHint().width()
                     / myStarRating.maxStarCount())) + 1;
    if (star <= 0 || star > myStarRating.maxStarCount())
        return -1;

    return star;
}

const int PaintingScaleFactor = 20;

StarRating::StarRating(int starCount, int maxStarCount)
{
    myStarCount = starCount;
    myMaxStarCount = maxStarCount;

    starPolygon << QPointF(1.0, 0.5);
    for (int i = 1; i < 5; ++i)
        starPolygon << QPointF(0.5 + 0.5 * std::cos(0.8 * i * 3.14),
                               0.5 + 0.5 * std::sin(0.8 * i * 3.14));

    diamondPolygon << QPointF(0.4, 0.5) << QPointF(0.5, 0.4)
                   << QPointF(0.6, 0.5) << QPointF(0.5, 0.6)
                   << QPointF(0.4, 0.5);
}

QSize StarRating::sizeHint() const
{
    return PaintingScaleFactor * QSize(myMaxStarCount, 1);
}

void StarRating::paint(QPainter *painter, const QRect &rect,
                       const QPalette &palette, EditMode mode) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (mode == Editable) {
        painter->setBrush(palette.highlight());
    } else {
        painter->setBrush(palette.foreground());
    }

    int yOffset = (rect.height() - PaintingScaleFactor) / 2;
    painter->translate(rect.x(), rect.y() + yOffset);
    painter->scale(PaintingScaleFactor, PaintingScaleFactor);

    for (int i = 0; i < myMaxStarCount; ++i) {
        if (i < myStarCount) {
            painter->drawPolygon(starPolygon, Qt::WindingFill);
        } else if (mode == Editable) {
            painter->drawPolygon(diamondPolygon, Qt::WindingFill);
        }
        painter->translate(1.0, 0.0);
    }

    painter->restore();
}
