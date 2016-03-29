#include <QFileDialog>
#include <QtGui>
#include "windows.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyModel.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}



MainWindow::~MainWindow()
{
    delete ui;
}

/*  -- clear
void MainWindow::on_pushButton_released()
{     
    // delete current selected item
//    int idx = (int)ui->tableWidget->currentRow();
//    ui->tableWidget->removeRow(idx);




    // Deselects all selected items
//    ui->tableWiew->clearSelection();

    // Disconnect all signals from table widget ! important !
//    ui->tableWidget->disconnect();

    // Remove all items
//    ui->tableWidget->clearContents();

    // Set row count to 0 (remove rows)
//    ui->tableWidget->setRowCount(0);
}*/

void MainWindow::on_toolButton_released()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "c:\\",
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->LineEditLocation->setText(dir);
}


void MainWindow::init()
{
    model = new MyModel (0);

    ui->tableView->setModel(model);
    ui->tableView->setSortingEnabled( true );
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setSortIndicatorShown(true);

    ui->tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->tableView->setShowGrid(false);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(ShowContextMenu(const QPoint&)));

  //  ui->tableView->horizontalHeader()->setSortIndicator(0,A);

   connect(ui->insertButton, SIGNAL(clicked()), this, SLOT(on_insertButton_released()));
}


void MainWindow::ShowContextMenu(const QPoint& pos)
{
    //ui->tableView->viewport()->

    QModelIndex index = ui->tableView->indexAt(pos);
    //qDebug() << QString("IndexAt %1 %2.").arg(index.row()).arg(index.column());

    if (index.row()<0) return;

    QMenu *menu=new QMenu(this);


    QAction* action1 = new QAction("Action 1",this);
    QAction* action2 = new QAction("Action 2",this);
    QAction* action3 = new QAction("Action 3",this);

    connect(action1, SIGNAL(triggered()), this, SLOT(menu1ClickedSlot()));
    connect(action2, SIGNAL(triggered()), this, SLOT(menu2ClickedSlot()));
    connect(action3, SIGNAL(triggered()), this, SLOT(menu3ClickedSlot()));


    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
//    menu->exec(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::menu1ClickedSlot()
{
    qDebug() << "Clicked 1.";
}
void MainWindow::menu2ClickedSlot()
{
    qDebug() << "Clicked 2.";
}
void MainWindow::menu3ClickedSlot()
{
    qDebug() << "Clicked 3.";
}


void MainWindow::on_insertButton_released()
{
    qDebug() << "Button Clicked.";
    model->insertNewRow();
}

