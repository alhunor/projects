#include "mainwindow.h"
#include <qDebug>
#include "ui_mainwindow.h"
#include "goview.h"
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QFileDialog>

//void mousePressEvent(QGraphicsSceneMouseEvent *event);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), scene(NULL), MaxRecentFiles(6)
{
    ui->setupUi(this);

//    QPixmap pixy("L:\\Projects\\HuGo\\texture.jpg");
//    QPixmap pixy("L:\\Projects\\HuGo\\stones white.png");
//    QPixmap pixy(":hugores/images/images/texture.jpg");
//    ui->label->setPixmap(pixy);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    fileMenu = ui->menuBar->addMenu(tr("&File"));

    QAction* newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon((":/images/new.png")));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    QAction* openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));


    QAction* closeAction = new QAction(tr("&Close"), this);
    closeAction->setIcon(QIcon(":/images/close.png"));
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

    QAction* saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save to a file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));


    ui->menuBar->addAction("Open");
    ui->menuBar->addAction("Game");
    ui->menuBar->addAction("Acount");
    ui->menuBar->addAction("About");
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();

    typedef QAction* QActionP;

    recentFileActions = new QActionP[6];
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
          recentFileActions[i] = new QAction(tr("tata"), this);
          fileMenu->addAction(recentFileActions[i]);
         // recentFileActions[i]->setVisible(false);
          connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
      }
    fileMenu->addSeparator();

    fileMenu->addAction(closeAction);

// Set imer for resize
    m_resizeTimer = new QTimer();
    m_resizeTimer->setSingleShot(true);
    connect(m_resizeTimer, SIGNAL(timeout()), SLOT(resizeDone()));

} // MainWindow::MainWindow(QWidget *parent)



void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   //qDebug() << Q_FUNC_INFO;
   m_resizeTimer->start(500);
}

void MainWindow::resizeDone()
{
    m_resizeTimer->stop();
   qDebug() << Q_FUNC_INFO;
   refreshBoard();
}


void MainWindow::setButtons()
{
  ui->undoButton->setEnabled(ui->graphicsView->h.hasUndo());
  ui->redoButton->setEnabled(ui->graphicsView->h.hasRedo());
}


bool MainWindow::init()
{
    ui->graphicsView->init(this);
    setButtons();
    return true;
} // bool MainWindow::init()


MainWindow::~MainWindow()
{
    delete ui;
    if (scene!=NULL) delete scene;
    if (m_resizeTimer!=NULL) delete m_resizeTimer;
    qDebug() << MaxRecentFiles;
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
      //  fileMenu->removeAction(recentFileActions[i]);
        if (recentFileActions[i]==NULL)
            qDebug() << "recentFileActions[i]";
        delete recentFileActions[i];
    }

    delete[] recentFileActions;
}


void MainWindow::refreshBoard()
{
    //qDebug()<<"Refresh";
}

void MainWindow::on_pushButton_clicked()
{

    /*    QPoint topleft=ui->label->geometry().topLeft();
    QPoint bottomright=ui->label->geometry().bottomRight();
    int width = bottomright.x()-topleft.x();
    int height = bottomright.y()-topleft.y();
    qDebug()<<width<<"  "<<height<<endl;*/
}


void MainWindow::newFile()
{
    qDebug()<<"newFile";
} // void MainWindow::void newFile()


void MainWindow::openFile()
{
  QString filters("Proprietary format (*.hug);;Smart Go Files (*.sgf)");
  QString defaultFilter("Proprietary format (*.hug)");

  QFileDialog dialog(this,  "Open file", QDir::currentPath(), filters);
  dialog.setDefaultSuffix(defaultFilter);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  if (dialog.exec() == QDialog::Accepted)
    {
      QString selectedFilter = dialog.selectedNameFilter();
      //qDebug()<<selectedFilter ;
      QString fileName = dialog.selectedFiles()[0];
      ui->graphicsView->open(fileName);
    }
  setButtons();
} // void MainWindow::void openFile()

void MainWindow::closeFile()
{
    qDebug()<<"closeFile";
} // void MainWindow::void newFile()

void MainWindow::saveFile()
{
    QString filters("Proprietary format (*.hug);;Smart Go Files (*.sgf)");
    QString defaultFilter("Proprietary format (*.hug)");

    QFileDialog dialog(this,  "Save file", QDir::currentPath(), filters);
    dialog.setDefaultSuffix(defaultFilter);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() == QDialog::Accepted)
      {
        QString selectedFilter = dialog.selectedNameFilter();
        //qDebug()<<selectedFilter ;
        QString fileName = dialog.selectedFiles()[0];
        ui->graphicsView->save(fileName);
      }
    //    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), filters, &defaultFilter);
} // void MainWindow::void saveFile()


void MainWindow::mouseReleaseEvent ( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton)
    {
        qDebug()<<event->x()<<"  "<<event->y();
    }
}

void MainWindow::on_undoButton_clicked()
{
  struct config* c = ui->graphicsView->h.undo();
  if (c==NULL) return;
  //update board
/*  if (ui->graphicsView->board[m->i][m->j].item == NoStone)
  {
      ui->graphicsView->board[m->i][m->j].item = m->stone;
  } else
  {
      ui->graphicsView->board[m->i][m->j].item = NoStone;
  }
  ui->graphicsView->drawStone(m->i, m->j);
  */
  // draw stone for the updated board
//  qDebug()<<"Undo:"<< c->i<<","<<c->j<<","<<c->oldStone<<","<<c->newStone;

  ui->graphicsView->updateBoard(c->i, c->j, c->oldStone, c->nr);
  setButtons();
}

void MainWindow::on_redoButton_clicked()
{
  struct config* c = ui->graphicsView->h.redo();
  if (c==NULL) return;
  //update board
 // ui->graphicsView->board[m->i][m->j].item = m->stone;
  // draw stone for the updated board
  ui->graphicsView->updateBoard(c->i, c->j, c->newStone, c->nr+1);
  //c->nr+1 is needed as redo revover configuration post move while unde is pre move
  setButtons();
}

