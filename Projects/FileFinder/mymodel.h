#ifndef MYMODEL_H
#define MYMODEL_H

#include "filesystems.h"
#include <QAbstractTableModel>
#include <QTimer>

const int ROWS= 5;

struct columnTypes
{
    QString col0, col1, col2, col3;
    QString& operator[](int i);
    const QString& operator[](int i) const;
};


class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QTimer *timer;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
    void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;
    void insertNewRow();
    bool removeRows(int position, int rows, const QModelIndex &parent) Q_DECL_OVERRIDE;
    bool update(const char* cpath, matcher& m,  bool includeSubFolders, bool skipSystemFolders);

//    sortByColumn
    FindFiles ff;
    files fi;

private:
    //int selectedCell;
    int nRows;
    std::vector< columnTypes > m_gridData;  //holds text entered into QTableView
private slots:
    void timerHit();
signals:
    void editCompleted(const QString &);
};

#endif // MYMODEL_H


/* Reading material:
    http://woboq.com/blog/new-signals-slots-syntax-in-qt5.html
    http://doc.qt.io/qt-4.8/model-view-programming.html - selections, insert/delete rows
    http://doc.qt.io/qt-5/modelview.html - delegates
    http://doc.qt.io/qt-5/model-view-programming.html - sorting
    https://forum.qt.io/topic/31233/how-to-create-a-custom-context-menu-for-qtableview/3 - context menus on table require subclassing QTableView
    http://stackoverflow.com/questions/18299077/dragging-a-qwidget-in-qt-5 Drag and drop example
*/
