#ifndef SCORE_H
#define SCORE_H

#include <QGraphicsTextItem>
#include <QGraphicsItem>


class Score : public QGraphicsTextItem
{
public:
    Score(QGraphicsItem* parent = 0);
    ~Score();
    void increase() {++score; display();}
    int getScore() const {return score;}
    void display();
private:
    int score;
};

#endif // SCORE_H
