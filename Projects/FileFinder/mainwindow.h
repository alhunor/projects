#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
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
    QLabel* statusLabel2;


private slots:
    // If you have a QPushButton named btn. And you created a slot named on_btn_clicked(). It will automatically get connected.
    // A call to connect(btn, SIGNAL(released()), this, SLOT(on_btn_clicked())) will connect the slot another time and you will get two events
    void on_insertButton_released();
    void on_searchButton_released();
    void on_pathButton_released();

    void ShowContextMenu(const QPoint& pos);
    void menu1ClickedSlot();
    void menu2ClickedSlot();
    void menu3ClickedSlot();

    void on_pathButton_clicked();
};

#endif // MAINWINDOW_H


