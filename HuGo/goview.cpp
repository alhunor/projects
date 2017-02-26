#include "goview.h"
#include "mainwindow.h"
#include <QDebug>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <fstream>

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

      int nr = h.add(i,j, board[i][j].item, newStone);

      updateBoard(i,j, newStone, nr);
      mw->setButtons();
  }
} // void goView::mouseReleaseEvent (QMouseEvent * event)




bool goView::updateBoard(int i, int j, stoneT newStone, int nr)
{
  // positions are numbered 0..18 x 0..18

  if (board[i][j].item==newStone) return false; // this should never happen

  // If board already has a stone at (i,j) remove it
  if (board[i][j].item!=NoStone)
  {
      // delete current item
      scene->removeItem(board[i][j].icon);
      scene->removeItem(board[i][j].label);
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

  // add label
  board[i][j].nr = nr + h.getOffset();
  QGraphicsTextItem* ql = scene->addText(QString::number(board[i][j].nr));
  ql->setDefaultTextColor(board[i][j].item==WhiteStone?Qt::black:Qt::white);
  ql->setFont(QFont("times", 12));
  board[i][j].label = ql;
  int dx = ql->boundingRect().width()/2;

  ql->setPos(x0+j*cw-dx,y0+i*ch-15);
  board[i][j].label->setZValue(2);

  return true;
} // bool goView::addStone(int i, int j, bool leftclick)


bool goView::save(const QString& filename)
{
  qDebug()<<"Saving "<<filename;
  QString extension = filename.right(3);
  if (extension=="hug")
  {
      std::ofstream out;
      out.open(filename.toStdString().c_str(), std::ofstream::out | std::ofstream::trunc);
      if (! out.is_open()) return false;

      int i,j, b, code, sumcode[4] ;
      sumcode[0]= 0;
      sumcode[1]= 0;
      sumcode[2]= 0;
      sumcode[3]= 0;
      const int n = 18;

      for (i =0; i<=n; ++i )
      {
        for (j =0; j<=n; ++j )
        {
            b = board[i][j].item;
            if (b!=NoStone)
            {
               code = b + 3*(j+i*(n+1));
               sumcode[0]+=code;
               code = b + 3*(n-i+j*(n+1));
               sumcode[1]+=code;
               code = b + 3*(n-j+(n-i)*(n+1));
               sumcode[2]+=code;
               code = b + 3*(i+(n-j) *(n+1));
               sumcode[3]+=code;
            }
        } // for (j =0; j<19; ++j )
      } // for (i =0; i<19; ++i )
      // now find cannonical rotation
      int canonical = 0;
      for (i =0; i<4; ++i )
      {
          qDebug() <<sumcode[i];
          if (sumcode[i]<sumcode[canonical]) canonical = i;
      }
      out<<canonical<<std::endl;
      qDebug() <<"canonical"<<canonical;
      // iterate again and output cannonical codes
      for (i =0; i<=n; ++i )
      {
        for (j =0; j<=n; ++j )
        {
            b = board[i][j].item;
            if (b!=NoStone)
            {
                if (canonical==0) code = b + 3*(j+i*(n+1));
                if (canonical==1) code = b + 3*(n-i+j*(n+1));
                if (canonical==2) code = b + 3*(n-j+(n-i)*(n+1));
                if (canonical==3) code = b + 3*(i+(n-j)*(n+1));
                out<<code<<std::endl;
            }
        } // for (j =0; j<=n; ++j )
      } // for (i =0; i<=n; ++i )
      out.close();
  } else
  {
      qDebug()<<"Unsupported"<<extension;

  }
  return true;
}

void goView::clear()
{
  h.clear();
  const int n = 18;
  int i,j;

  for (i =0; i<=n; ++i )
  {
    for (j =0; j<=n; ++j )
    {
        if (board[i][j].item!=NoStone)
        {
            // remove current contents from the scene
            scene->removeItem(board[i][j].icon);
            scene->removeItem(board[i][j].label);
            // todo : check if they need to be deleted - probably no for icon and yes for label
        }
        board[i][j].item = NoStone;
        board[i][j].nr =0;
    }
} // void goView::clear()
}

bool goView::open(const QString& filename)
{
  std::ifstream in;
  in.open(filename.toStdString().c_str());
  if (! in.is_open()) return false;

  clear();

  // TODO determine extension

  // suppose file is in hug format

  // read cannonical
  int canonical;
  in>>canonical;
  // read stone codes
  int aux, b, i, j;
  const int n = 18;
  int nr = 0;
  while (!in.eof())
  {
      in>>aux;
      i = aux / (3*(n+1));
      aux %= (3*(n+1));
      j = aux / 3;
      b = aux % 3;
      if (canonical == 0) updateBoard(i, j, (stoneT)b, nr);
      if (canonical == 1) updateBoard(j, n-i, (stoneT)b, nr);
      if (canonical == 2) updateBoard(n-i, n-j, (stoneT)b, nr);
      if (canonical == 3) updateBoard(n-j, i, (stoneT)b, nr);
      ++nr;
  }
  h.setOffset(nr);
/*
  qDebug()<<"Loading "<<filename;
  // TODO determine extension

  // suppose file is in hug format

  // read cannonical
  int cannonical = 3;
  // read stone code
  int stones[4] = {64, 71, 133, 137};
  int nbStones = 4;
  int aux, b, i, j;
  const int n = 18;
  for (int k =0; k<nbStones; ++k )
  {
      aux = stones[k];
      i = aux / (3*(n+1));
      aux %= (3*(n+1));
      j = aux / 3;
      b = aux % 3;
      qDebug()<<i<<" "<<j<<" "<<b;
      if (cannonical == 0) updateBoard(i, j, (stoneT)b, k);
      if (cannonical == 1) updateBoard(j, n-i, (stoneT)b, k);
      if (cannonical == 2) updateBoard(n-i, n-j, (stoneT)b, k);
      if (cannonical == 3) updateBoard(n-j, i, (stoneT)b, k);
  }
*/
  return true;
} // bool goView::load(char* filename)


config* history :: undo()
{
  // poistion always points to the next emplacement where a new stone would be added

  if (pos>0)
  {
//      qDebug()<<"Undo position "<<position;
    --pos;
    return &data[pos];
   }
  return NULL;
}

config* history :: redo()
{
  if (pos<maxPos)
  {
    config * c = &data[pos];
    ++pos;
    return c;
   }
  return NULL;
}


int history :: add(int i, int j, stoneT oldStone, stoneT newStone)
{
//  qDebug()<<"Adding ("<< i<<","<<j<<","<<oldStone<<","<< newStone <<")";
  if (pos>0 && i == data[pos-1].i && j == data[pos-1].j)
  {
  //  qDebug()<<"Changed position "<< position-1;
    data[pos-1].newStone = newStone;
    return pos-1;
  }
  struct config c(i, j, oldStone, newStone, pos-1);
  if (pos==maxPos) ++maxPos;
  if (pos==data.size()) data.push_back(c); else data[pos] = c;
  ++pos;
  // reset undo if new action done before all undo completed
  if (pos<maxPos) maxPos = pos;
  return pos-1;
//  qDebug()<<"Added to position "<< position-1;
}
