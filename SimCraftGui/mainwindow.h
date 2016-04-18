#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "arcaneMage.h"
#include "combatlog.h"

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
    void displayArcaneMage(arcaneMage& kettle);

private:
    Ui::MainWindow *ui;
    arcaneMage kettle;
    combatlog log;

private slots:
    void on_pushButton_released();

    void on_pushButton_pressed();
    void on_openButton_clicked();
    void on_stepButton_clicked();
    void on_forwardButton_clicked();
};

#endif // MAINWINDOW_H
