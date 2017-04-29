#include "score.h"

#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QFont>

Score::Score(QGraphicsItem* parent) : QGraphicsTextItem(parent), score(0)
{
// draw the text
    display();
}

Score::~Score()
{

}

void Score::display()
{
    setPlainText(QString("Score: ") + QString::number(score));
    setDefaultTextColor(Qt::blue);
    setFont(QFont("times", 16));
}
