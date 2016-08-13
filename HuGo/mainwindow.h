#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool init();
    void setButtons();
    QGraphicsScene* scene;
    void resizeEvent(QResizeEvent* event);
    //Click event = MousePress->MouseRelease
    void mouseReleaseEvent ( QMouseEvent * event);


private slots:
    void on_pushButton_clicked();
    void newFile();
    void openFile();
    void saveFile();
    void closeFile();
    void resizeDone();
    void refreshBoard();
    void openRecentFile() {}

    void on_undoButton_clicked();

    void on_redoButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* m_resizeTimer;
    int MaxRecentFiles;
    QAction** recentFileActions;
    QMenu* fileMenu;

};

#endif // MAINWINDOW_H
