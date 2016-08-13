#ifndef GOVIEW_H
#define GOVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFrame>
#include <QList>
#include <QMouseEvent>
#include "mainwindow.h"

typedef enum {NoStone=0, BlackStone, WhiteStone} stoneT;


struct BoardPosition
{
  QGraphicsPixmapItem* icon;
  stoneT item; // 0 - empty, 1 - black, 2 - white
  QGraphicsTextItem* label;
  int serialNo;
};


struct config
{
  config( int _i, int _j, stoneT _oldStone, stoneT _newStone) : oldStone(_oldStone), newStone(_newStone), i(_i), j(_j) {}
  stoneT oldStone, newStone; // 0 - nostone, 1 - black, 2 - white
  int i,j; // position starting from (0,0) to (18,18)
};


class history
{
public:
  history(std::vector<std::vector<BoardPosition> >& _board) : position(0), maxSize(0), board(&_board) {}
  struct config* undo();
  struct config* redo();
  bool hasUndo() {return position>0;}
  bool hasRedo() {return position<maxSize;}

  void add(int i, int j, stoneT oldStone, stoneT newStone);
private:
  QList<config> data;
  int position, maxSize;
  std::vector<std::vector<BoardPosition> >* board;
  stoneT lastStone;
};

class goView : public QGraphicsView
{
    Q_OBJECT
public:
    goView(QFrame* qf) : QGraphicsView(qf), h(board), mw(NULL) {}
    bool init(MainWindow* _mw);
    ~goView();
    void mouseReleaseEvent ( QMouseEvent * event);
    bool updateBoard (int i, int j, stoneT newStone);
    bool save(const QString& filename);
    bool load(const QString& filename);
    history h;
    std::vector<std::vector<BoardPosition> > board;
private:
    MainWindow* mw;
    QGraphicsScene* scene;
    int x0, y0, ch, cw;
    QPixmap bs, ws; // black and white stones
};

#endif // GOVIEW_H
