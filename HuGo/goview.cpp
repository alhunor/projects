#include "goview.h"
#include "mainwindow.h"
#include <QDebug>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>


bool goView::init(MainWindow* _mw)
{
  mw = _mw;
  setBackgroundBrush(QBrush(QImage(":/images/texture.jpg")));
//    ui->graphicsView->setBackgroundBrush(QBrush(QImage(":/images/Blank_Go_board.png")));

  int h = geometry().height();
  int w = geometry().width();
  scene = new QGraphicsScene();
  scene->setSceneRect(0,0,w,h);

  bs = QPixmap(":/images/black.png").scaled(QSize(30,30));
  ws = QPixmap(":/images/white.png").scaled(QSize(30,30));

  board.resize(19);
  for (int i = 0; i<19; ++i)
  {
      board[i].resize(19);
      for (int j = 0; j<19; ++j)
      {
          board[i][j].icon=NULL;
          board[i][j].item=NoStone;
      }
  }

  setScene(scene);
  QGraphicsLineItem * lines[19+19]; // horizontal and vertical lines
  ch = h / 19.5;
  y0 = (h - ch*18) /2 -1; // -1 is needed because every line is 3 pixels wide
  cw = w / 19.5;
  x0 = (w - cw*18) /2 -1; // -1 is needed because every line is 3 pixels wide
  QPen pen(Qt::black, 3, Qt::SolidLine);
  int i,j;
  for (i = 0; i<19; ++i)
  {
      // horizontal lines
      lines[i] = new QGraphicsLineItem(NULL);
      lines[i]->setLine(x0, y0+i*ch, x0+18*cw, y0+i*ch);
      lines[i]->setPen(pen);
      scene->addItem(lines[i]);

      // vertical lines
      lines[19+i] = new QGraphicsLineItem(NULL);
      lines[19+i]->setLine(x0+i*cw, y0, x0+i*cw, y0+18*ch);
      lines[19+i]->setPen(pen);
      scene->addItem(lines[19+i]);
  }
  // add tengen and star points
  int size = 5;
  int positions[3] = {3,9,15};
  QBrush brush(Qt::SolidPattern);
  for (i = 0; i<3; ++i)
  {
    for (j = 0; j<3; ++j)
    {
        scene->addEllipse(x0+positions[i]*cw-size, y0+positions[j]*ch-size, 2*size, 2*size, pen, brush);
    }
  }
  return true;
} // goView::init()

goView::~goView()
{
// TODO delete scene

  // delete goboard
  for (int i = 0; i<19; ++i)
  {
      for (int j = 0; j<19; ++j)
      {
          if (board[i][j].icon!=NULL) delete board[i][j].icon;
          if (board[i][j].label!=NULL) delete board[i][j].label;
      }
  }
}

void goView::mouseReleaseEvent (QMouseEvent * event)
{
  if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
  {
      int x = event->x();
      x = x - x0;
      int y = event->y();
      y = y - y0;
      if (y<0 || x<0) return;
      float i = y / (float)ch;
      float frac = i - floor(i);
      if (frac>0.33 && frac<0.66) return; /// click is not precise
      i = floor(i);
      if (frac>0.66) ++i;
      float j = x / (float)cw;
      frac = j - floor(j);
      if (frac>0.33 && frac<0.66) return; /// click is not precise
      j = floor(j);
      if (frac>0.66) ++j;

      //qDebug()<<"GoView (i,j): ("<<i<<","<<j<<")";
      // player: 1 black, 2 white
      int action = 0; // no action
      if (event->button() == Qt::LeftButton)
      {
          action = +1; // increment
      } else
      {
          action = -1; // decrement
      }

      stoneT newStone = stoneT((board[i][j].item + action) % (WhiteStone+1)); // cycles through nostone, blackstone and whitestone.
      if (newStone<NoStone) newStone = WhiteStone;

      h.add(i,j, board[i][j].item, newStone);

      updateBoard(i,j, newStone);
      mw->setButtons();
  }
} // void goView::mouseReleaseEvent (QMouseEvent * event)


bool goView::updateBoard(int i, int j, stoneT newStone)
{
  // positions are numbered 0..18 x 0..18

  if (board[i][j].item==newStone) return false; // this should never happen

  // If board already has a stone at (i,j) remove it
  if (board[i][j].item!=NoStone)
  {
      // delete current item
      scene->removeItem(board[i][j].icon);
  }

  board[i][j].item = newStone;
  if (newStone == NoStone) return true;


  // Draw the stone on the board
  QPixmap* p;
  if (newStone==BlackStone)
    {
      p = &bs;
    }
    if (newStone==WhiteStone)
      {
        p = &ws;
      }
  board[i][j].icon = scene->addPixmap(*p);
  board[i][j].icon->setPos(x0+j*cw-15,y0+i*ch-15);
  board[i][j].icon->setZValue(1);

  return true;
} // bool goView::addStone(int i, int j, bool leftclick)


bool goView::save(const QString& filename)
{
  qDebug()<<"Saving "<<filename;
  QString extension = filename.right(3);
  if (extension=="hug")
  {
      qDebug()<<"Hunor go";
  } else qDebug()<<extension;

  return true;
}


bool goView::load(const QString& filename)
{
  qDebug()<<"Loading "<<filename;
  return true;
} // bool goView::load(char* filename)


config* history :: undo()
{
  // poistion always points to the next emplacement where a new stone would be added

  if (position>0)
  {
      qDebug()<<"Undo position "<<position;
    --position;
    return &data[position];
   }
  return NULL;
}

config* history :: redo()
{
  if (position<maxSize)
  {
    config * c = &data[position];
    ++position;
    return c;
   }
  return NULL;
}


void history :: add(int i, int j, stoneT oldStone, stoneT newStone)
{
//  qDebug()<<"Adding ("<< i<<","<<j<<","<<oldStone<<","<< newStone <<")";
  if (position>0 && i == data[position-1].i && j == data[position-1].j)
  {
  //  qDebug()<<"Changed position "<< position-1;
    data[position-1].newStone = newStone;
    return;
  }
  struct config c(i, j, oldStone, newStone);
  if (position==maxSize) ++maxSize;
  if (position==data.size()) data.push_back(c); else data[position] = c;
  ++position;
  // reset undo if new action done before all undo completed
  if (position<maxSize) maxSize = position;
//  qDebug()<<"Added to position "<< position-1;
}
