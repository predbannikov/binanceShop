#include "modelpairs.h"

ModelPairs::ModelPairs(QObject *parent) : QAbstractTableModel(parent)
{

}

int ModelPairs::rowCount(const QModelIndex &) const {return currencies.count();}

int ModelPairs::columnCount(const QModelIndex &) const {return COLUMN;}

QVariant ModelPairs::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
        return QVariant();
    const Currency &rec = currencies.at(index.row());
    switch(role){
        case Qt::CheckStateRole:
            if (index.column()==filter)
//                return currencies.at(index.row()).filter;
            {
                QString label = currencies.at(index.row()).symbol;
                if(favoriteCoin.contains(label))
                    return true;
                else
                    return false;
            }
            break;
        case Qt::DisplayRole:{
            int key = index.column();
            switch( key) {
//                case TradePairId:
//                    return rec.Id;
                case Label:
                    return rec.symbol;
                case LastPrice:
                    return rec.lastPrice;
            }
        }
        default:
            return QVariant();
    }
//     Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    return result;
}

bool ModelPairs::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::SizeHintRole){

    }
}

QVariant ModelPairs::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Для любой роли, кроме запроса на отображение, прекращаем обработку
    if (role != Qt::DisplayRole)
    return QVariant();
    // формируем заголовки по номуру столбца
    if (orientation == Qt::Horizontal) {
        switch (section) {
//            case TradePairId:
//            return tr("Id");
            case Label:
            return tr("Label");
            case LastPrice:
            return tr("Price");
            case filter:
            return tr("X");
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
    for (int row = 0; row < rows; ++row) {
        currencies.removeAt(position);
    }
    endRemoveRows();
    return true;
}

bool ModelPairs::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    endInsertRows();
    return true;
}

void ModelPairs::setCheckFilter(bool state)
{
    chkfiltr = state;
}

void ModelPairs::appendCurrency(QJsonArray array)
{

}

void ModelPairs::clearCurrencies()
{
    if(currencies.empty())
        return;
    removeRows(0,currencies.count(),QModelIndex());
    currencies.clear();
}

void ModelPairs::parsing(QJsonArray array)
{
//    switch(exchange)
//    {
//    case CRYPTOPIA:
//        for(int i=0; i<array.size(); i++)
//        {
//            QJsonObject jdata = array[i].toObject();
//            Currency curr;
//            if(jdata.contains("Label") && jdata["Label"].isString())
//            {
//                curr.label = jdata["Label"].toString();
//            }
//            if(jdata.contains("TradePairId") && jdata["TradePairId"].isDouble())
//            {
//                curr.Id = jdata["TradePairId"].toInt();
//            }
//            if(jdata.contains("LastPrice") && jdata["LastPrice"].isDouble())
//            {
//                curr.lastPrice = jdata["LastPrice"].toDouble();
//            }
//            sourceCurrencies.append(curr);
//        }
//        break;
//    case BINANCE:
//        for(int i=0; i<array.size(); i++)
//        {
//            QJsonObject jdata = array[i].toObject();
//            Currency curr;
//            if(jdata.contains("symbol") && jdata["symbol"].isString())
//            {
//                curr.label = jdata["symbol"].toString();
//            }
//            if(jdata.contains("price") && jdata["price"].isString())
//            {
//                curr.lastPrice = jdata["price"].toString().toDouble();
//            }
//            sourceCurrencies.append(curr);
//        }
//        break;
//    default:
//        return;
//    }

}

void ModelPairs::checkFilter()
{
    favoriteCheck();
    findProfit();
    applyChanges();
}

void ModelPairs::favoriteCheck()
{
    clearCurrencies();
//    qDebug() << "chkfiltr = " << chkfiltr;
    if(chkfiltr)
    {
        for(int i = 0; i < sourceCurrencies.size(); i++)
        {
            if(favoriteCoin.contains(sourceCurrencies[i].symbol))
            {
                currencies.append(sourceCurrencies[i]);
            }
        }

    } else {
        currencies = sourceCurrencies;
    }
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

//void ModelPairs::setExchange(Exchange ex)
//{
//    exchange = ex;
//}

void ModelPairs::setNewPairs(QJsonArray array)
{
    sourceCurrencies.clear();
    parsing(array);
    favoriteCheck();
    applyChanges();
}

void ModelPairs::findProfit()
{
    if(chkFindProfit){      // Ищем профит если фильтр включен

    }
}

void ModelPairs::selectedRow(QModelIndex index)
{
    if(index.column()==0){                  // Разбираемся с чекбоксами фаворитных монет
        QString symbol;
        symbol = currencies[index.row()].symbol;
        if(!favoriteCoin.contains(symbol))      // Если в фаворитных монетках нет,
            favoriteCoin.append(symbol);        // то добавляем
        else                                // иначе
            favoriteCoin.removeOne(symbol);     // удаляем из фаворитного списка

        emit dataChanged( index, index );   // Изменяем вид текущего индекса
    } else {
//        currentCurrency = currencies.at(index.row());
        emit getMarket(this->currencies.at(index.row()).symbol);
    }
}
