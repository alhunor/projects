#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <string>
#include "arcaneMage.h"
#include "combatlog.h"
#include "qcustomplot.h"

class Plate
{
public:
    Plate() {}

    void init(QFrame* _base, QProgressBar* _durationBar, QLabel* _cdLabel, QLabel* _pic_cd, QLabel* _pic_duration, const char* path, std::pair<float, status>* _attribute);
    void display(float time);
    QFrame* base;
    void reset(QFrame* _base, QProgressBar* _durationBar, QLabel* _pic_duration);
protected:
    QProgressBar* durationBar;
    QLabel* pic_cd, *pic_duration, *cdLabel;
    int duration_pos, cd_pos;
    std::pair<float, status>* attribute;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    void displayArcaneMage(arcaneMage& kettle, float time);

private:
    Ui::MainWindow *ui;
    arcaneMage kettle;
    combatlog log;
    Plate prismaticCrystal, nithramus, hero, intellectPotion, presenceofmind, arcanepower, doomNova, weaponEnchant, evocation;
//    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void on_openButton_clicked();
    void on_stepButton_clicked();
    void on_forwardButton_clicked();
};

#endif // MAINWINDOW_H
