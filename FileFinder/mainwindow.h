#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MyModel.h"

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

private:
    Ui::MainWindow *ui;
    MyModel *model;

private slots:
    void on_insertButton_released();
    void on_toolButton_released();

    void ShowContextMenu(const QPoint& pos);
    void menu1ClickedSlot();
    void menu2ClickedSlot();
    void menu3ClickedSlot();

};

#endif // MAINWINDOW_H
