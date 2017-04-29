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
  void clear();
  int nr;
};

struct config
{
  config( int _i, int _j, stoneT _oldStone, stoneT _newStone, int _nr) : oldStone(_oldStone), newStone(_newStone), i(_i), j(_j), nr(_nr) {}
  stoneT oldStone, newStone; // 0 - nostone, 1 - black, 2 - white
  int i,j; // position starting from (0,0) to (18,18)
  int nr;
};


class history
{
public:
  history(std::vector<std::vector<BoardPosition> >& _board) : pos(0), maxPos(0), board(&_board), offset(0) {}
  struct config* undo();
  struct config* redo();
  bool hasUndo() {return pos>0;}
  bool hasRedo() {return pos<maxPos;}
  void clear() {pos =0; maxPos=0; offset=0;}

  // return the the serial number of the item added, used for numbering stones
  int add(int i, int j, stoneT oldStone, stoneT newStone);
  void setOffset( int _offset) {offset = _offset;}
  int getOffset() {return offset;}

private:
  QList<config> data;
  int pos, maxPos, offset;
  std::vector<std::vector<BoardPosition> >* board;
};

class goView : public QGraphicsView
{
    Q_OBJECT
public:
    goView(QFrame* qf) : QGraphicsView(qf), h(board), mw(NULL) {}
    bool init(MainWindow* _mw);
    ~goView();
    void mouseReleaseEvent ( QMouseEvent * event);
    bool updateBoard (int i, int j, stoneT newStone, int nr);
    bool save(const QString& filename);
    bool open(const QString& filename);
    void clear();

    history h;
    std::vector<std::vector<BoardPosition> > board;
private:
    MainWindow* mw;
    QGraphicsScene* scene;
    int x0, y0, ch, cw;
    QPixmap bs, ws; // black and white stones
};

#endif // GOVIEW_H
