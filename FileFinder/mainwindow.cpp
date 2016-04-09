#include <QtGui>
#include <windows.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyModel.h"
#include "filesystems.h"

#include <QFileDialog>

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


void MainWindow::init()
{
    model = new MyModel (0);
    QTableView * tableView = ui->tableView;

    tableView->setModel(model);
    tableView->setSortingEnabled( true );
    tableView->verticalHeader()->hide();
    tableView->horizontalHeader()->setSectionsClickable(true);
    tableView->horizontalHeader()->setSortIndicatorShown(true);

    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->setShowGrid(false);

    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(12);



  //  ui->tableView->horizontalHeader()->setSortIndicator(0,A);


    statusLabel2 = new QLabel(this);
    statusLabel2->setText("Status Label2");

    // add the two controls to the status bar
  //  ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->addPermanentWidget(statusLabel2);


   connect(tableView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
 //  connect(ui->insertButton, SIGNAL(released()), this, SLOT(on_insertButton_released()));
 //  connect(ui->searchButton, SIGNAL(released()), this, SLOT(on_searchButton_released()));
 //  connect(ui->pathButton, SIGNAL(released()), this, SLOT(on_pathButton_released()));
}


void MainWindow::ShowContextMenu(const QPoint& pos)
{

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
    // The void QStatusBar::showMessage(const QString & message, int timeout = 0) hides the normal status indications and displays the given message for
    // the specified number of milli-seconds (timeout). If timeout is 0 (default), the message remains displayed until the clearMessage() slot is called
    // or until the showMessage() slot is called again to change the message.
    ui->statusBar->showMessage("Clicked 3.", 3000);
}


void MainWindow::on_insertButton_released()
{
    qDebug() << "Insert button Clicked.";
    model->insertNewRow();
}


void MainWindow::on_pathButton_released()
{
    QString dir;
    dir = ui->LineEditLocation->text();

    if (dir=="") dir = "c:\\";

    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), dir,
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->LineEditLocation->setText(dir);
}


void MainWindow::on_searchButton_released()
{
    bool includeSubFolders = ui->checkBox_Subfolders->isChecked();
    bool skipSystemFolders = ui->checkBox_Skip->isChecked();

    // https://msdn.microsoft.com/en-gb/library/windows/desktop/gg258117(v=vs.85).aspx
    DWORD attributes = 0;
    if (ui->checkBox_AttrNormal->isChecked())
        attributes |= FILE_ATTRIBUTE_NORMAL;
    if (ui->checkBox_AttrReadOnly->isChecked())
        attributes |= FILE_ATTRIBUTE_READONLY;
    if (ui->checkBox_AttrSystem->isChecked())
        attributes |= FILE_ATTRIBUTE_SYSTEM;
    if (ui->checkBox_AttrFolder->isChecked())
        attributes |= FILE_ATTRIBUTE_DIRECTORY;
    if (ui->checkBox_AttrHidden->isChecked())
        attributes |= FILE_ATTRIBUTE_HIDDEN;
    if (ui->checkBox_AttrArchive->isChecked())
        attributes |= FILE_ATTRIBUTE_ARCHIVE;


    if (ui->lineEditFilter->text()=="")
    {
        MessageBoxA(NULL, "Please enter a non-empty search pattern", "Missig search pattern", MB_OK | MB_ICONERROR);
        return;
    }

    std::string path = ui->LineEditLocation->text().toStdString();
    if (path=="")
    {
        MessageBoxA(NULL, "Please enter search path", "Missig path", MB_OK | MB_ICONERROR);
        return;
    }
    char* cpath = new char[path.length()+2];
    int i;
    for (i=0; i<path.length(); ++i)
    {
        if (path[i]=='/') path[i]='\\';
        cpath[i]=path[i];
    }
    if (cpath[i-1]!='\\')
    {
            cpath[i]='\\';
            ++i;
    }
    cpath[i]=0;


    MyModel *mo = (MyModel*)ui->tableView->model();
    matcher m;
    m.init(ui->lineEditFilter->text().toStdString().c_str(), attributes, 0, _UI64_MAX);

    mo->update(cpath,  m, includeSubFolders, skipSystemFolders);
    statusLabel2->setText(QString("Found %1 files.").arg(mo->rowCount()));
    ui->tableView->resizeColumnsToContents();

}


