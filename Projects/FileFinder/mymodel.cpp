//http://doc.qt.io/qt-5/modelview.html

#include "mymodel.h"
#include "filesystems.h"

#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QTime>

QString& columnTypes::operator[](int i)
{
    switch(i)
    {
    case 0: return col0;
    case 1: return col1;
    case 2: return col2;
    case 3: return col3;
    default:
        throw ("bad index.");
    }
}

const QString& columnTypes::operator[](int i) const
{
    switch(i)
    {
    case 0: return col0;
    case 1: return col1;
    case 2: return col2;
    case 3: return col3;
    default:
        throw ("bad index.");
    }
}

MyModel::MyModel(QObject *parent) :QAbstractTableModel(parent)
{
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()) , this, SLOT(timerHit()));   
    timer->start();
    nRows = ROWS;
    m_gridData.resize(nRows);
}

void MyModel::timerHit()
{
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}


int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
    return fi.nbFiles();
    //return nRows;
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 4;
}

void MyModel::sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
{
    // generate a log message when this method gets called
    qDebug() << QString("Column %1, Order %2").arg(column).arg(order);
}


// mymodel.cpp
QVariant MyModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    // generate a log message when this method gets called
    // qDebug() << QString("row %1, col%2, role %3").arg(row).arg(col).arg(role);

    switch(role)
    {
    case Qt::DisplayRole:
        //return m_gridData[row][col];
        if (row<fi.nbFiles())
        switch (col)
        {
        case 0:
            return fi[row].path.get();
        case 1:
            return fi[row].cFileName.get();
        case 2:
            return fi[row].fileSize;
        case 3:
            return fi[row].creationTime;
        } // switch(role)
        break;
    case Qt::FontRole:
        if (col == 1)
        {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        break;
    case Qt::BackgroundRole:
        break;
    case Qt::TextAlignmentRole:
            return Qt::AlignRight + Qt::AlignVCenter;
        break;
   } // switch(role)
   return QVariant();
} // QVariant MyModel::data(const QModelIndex &index, int role) const


QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
// http://doc.qt.io/qt-4.8/qt.html -- All enums
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return QString("Folder");
            case 1:
                return QString("Name");
            case 2:
                return QString("Size");
            case 3:
                return QString("Date modified");
            }
        }
    }
    if (role == Qt::InitialSortOrderRole && orientation == Qt::Horizontal)
    {
        // tells the view in what order has this column been sorted before it was clicked.
        qDebug() << QString("InitialSortOrderRole for section %1").arg(section);
    }
    return QVariant();
}


bool MyModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = value.toString();
        //for presentation purposes only: build and emit a joined string
        QString result;
        for (int row= 0; row < nRows; row++)
        {
            for(int col= 0; col < 4; col++)
            {
                result += m_gridData[row][col] + ' ';
            }
        }
        emit editCompleted( result );
    }
    return true;
}

Qt::ItemFlags MyModel::flags(const QModelIndex &index) const
{
    if (index.column()==1)
    {
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    } else
    {
        return QAbstractTableModel::flags(index);
    }
}

void MyModel::insertNewRow()
{
    //http://doc.qt.io/qt-5/model-view-programming.html

    beginInsertRows(QModelIndex(), nRows, nRows);
    nRows++;
    columnTypes ct;
    ct.col0 = "mama";
    m_gridData.push_back(ct);
    endInsertRows();
}


bool MyModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    qDebug() << QString("Remove row %1, to %2").arg(position).arg(position+rows);

    endRemoveRows();
    return true;
}

bool MyModel::update(const char* cpath, matcher& m,  bool includeSubFolders, bool skipSystemFolders)
{
   beginResetModel();
   ff.SetRoot(cpath);
   fi.clear();
   ff.findAll(&fi, m, includeSubFolders, skipSystemFolders);


   fi.getMutex();
   for (int i = 0; i<fi.nbFiles(); ++i)
   {
       qDebug() << fi[i].path.get() << fi[i].cFileName.get() << "  "<< fi[i].fileSize << "  " << fi[i].creationTime <<endl;
   }
   fi.releaseMutex();
   endResetModel();
   return true;
}
