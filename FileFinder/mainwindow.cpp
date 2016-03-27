#include <QFileDialog>
#include <QtGui>
#include "windows.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the data model
  QStringList m_TableHeader;

  m_TableHeader<<"N°"<<"Name"<<"Folder"<<"Size"<<"Date modified";

  ui->tableWidget->setRowCount(10);
  ui->tableWidget->setColumnCount(5);
  ui->tableWidget->setHorizontalHeaderLabels(m_TableHeader);
  ui->tableWidget->verticalHeader()->setVisible(false);

  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
  ui->tableWidget->setShowGrid(false);

  ui->tableWidget->resizeColumnsToContents();
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_released()
{     
    // delete current selected item
//    int idx = (int)ui->tableWidget->currentRow();
//    ui->tableWidget->removeRow(idx);


/*
   // delete many selected items. does not work
    QList<QTableWidgetItem*> selectionRangeList = ui->tableWidget->selectedItems();
    int rowIndex;
    QListIterator<QTableWidgetItem*> selectionRangeListIter(selectionRangeList);

    while(selectionRangeListIter.hasNext())
    {
        rowIndex = ((int)((QTableWidgetItem*)selectionRangeListIter.next())->row());
        ui->tableWidget->removeRow(rowIndex);
    }
*/


    // Deselects all selected items
    ui->tableWidget->clearSelection();

    // Disconnect all signals from table widget ! important !
    ui->tableWidget->disconnect();

    // Remove all items
    ui->tableWidget->clearContents();

    // Set row count to 0 (remove rows)
    ui->tableWidget->setRowCount(0);
}

void MainWindow::on_toolButton_released()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "c:\\",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    ui->LineEditLocation->setText(dir);
}

void MainWindow::on_pushButton_2_released()
{
    ui->tableWidget->setRowCount(25);
    //insert data
    for (int i=0; i<25; ++i)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Hello"));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem("te lo "));
    }
}
