#include <windows.h>
#include <QFileDialog>
#include "combatlog.h"
#include "arcaneMage.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

void MainWindow::init()
{
    kettle.init();
    QPixmap ac("L:\\Projects\\SimCraftGui\\arcaneCharge.png");
    ui->arcanecharge1->setPixmap(ac);
    ui->arcanecharge2->setPixmap(ac);
    ui->arcanecharge3->setPixmap(ac);
    ui->arcanecharge4->setPixmap(ac);


    QPixmap am("L:\\Projects\\SimCraftGui\\arcaneMissile.png");
    ui->arcanemissile1->setPixmap(am);
    ui->arcanemissile2->setPixmap(am);
    ui->arcanemissile3->setPixmap(am);

    QPixmap acryst("L:\\Projects\\SimCraftGui\\arcanecrystal.jpg");
//    QPixmap acryst(":/res/arcanecrystal.jpg");
    ui->arcanecrystal->setPixmap(acryst);

    QPixmap nith("L:\\Projects\\SimCraftGui\\nithramus.jpg");
    ui->nithramus->setPixmap(nith);

    QPixmap hero("L:\\Projects\\SimCraftGui\\timeWrap.jpg");
    ui->hero->setPixmap(hero);

    QPixmap intellectPotion("L:\\Projects\\SimCraftGui\\intellectPotion.jpg");
    ui->intellectPotion->setPixmap(intellectPotion);

    QPixmap presenceofmind("L:\\Projects\\SimCraftGui\\presenceofmind.jpg");
    ui->presenceofmind->setPixmap(presenceofmind);

    QPixmap apower("L:\\Projects\\SimCraftGui\\arcanepower.jpg");
    ui->arcanepower->setPixmap(apower);

    QPixmap incantersflow("L:\\Projects\\SimCraftGui\\incantersflow.jpg");
    ui->incantersflow->setPixmap(incantersflow);


    QPixmap doomNova("L:\\Projects\\SimCraftGui\\doomNova.jpg");
    ui->doomNova->setPixmap(doomNova);

    displayArcaneMage(kettle);
} // MainWindow::init()


void MainWindow::displayArcaneMage(arcaneMage& kettle)
{
    ui->arcanecharge1->setEnabled(kettle.arcane_charge.second>0);
    ui->arcanecharge2->setEnabled(kettle.arcane_charge.second>1);
    ui->arcanecharge3->setEnabled(kettle.arcane_charge.second>2);
    ui->arcanecharge4->setEnabled(kettle.arcane_charge.second>3);

    ui->arcanemissile1->setEnabled(kettle.arcane_missiles.second>0);
    ui->arcanemissile2->setEnabled(kettle.arcane_missiles.second>1);
    ui->arcanemissile3->setEnabled(kettle.arcane_missiles.second>2);

    ui->IFText->setText(QString("%1").arg(kettle.incanters_flow+5));
    QPalette sample_palette;
//    QColor c = (25, 25, 25, 255);
    sample_palette.setColor(QPalette::WindowText, Qt::white);
//    sample_palette.setColor(QPalette::WindowText, c);
    ui->IFText->setPalette(sample_palette);

    int mana_pct = int(kettle.mana *100 / kettle.maxMana);
    ui->mana->setText(QString("%1").arg(mana_pct));
    QColor c = Qt::green;
    if (mana_pct<93)
    {
        c = Qt::yellow;
    } else if (mana_pct<50)
    {
        c = Qt::red;
    };
    QPalette palette;
    palette.setColor(QPalette::WindowText, c);
    ui->mana->setPalette(palette);


// Attributes
    QPalette green;
    green.setColor(QPalette::WindowText, Qt::green);
    QPalette black;
    black.setColor(QPalette::WindowText, Qt::black);

    ui->Intellect->setText(QString("Intellect:%1 (%2%)").arg(kettle.intellect+kettle.intellectBuff).arg(11));
    ui->SpellPower->setText(QString("SpellPower:%1").arg(kettle.spellpower+kettle.spellpowerBuff));

    if (kettle.masteryBuff>0) ui->Mastery->setPalette(green); else ui->Mastery->setPalette(black);
    ui->Mastery->setText(QString("Mastery:%1 (%2%)").arg(kettle.mastery+kettle.masteryBuff).arg(11));

    if (kettle.hasteBuff>0) ui->Haste->setPalette(green); else ui->Haste->setPalette(black);
    ui->Haste->setText(QString("Haste:%1 (%2%)").arg(kettle.haste+kettle.hasteBuff).arg(11));

    if (kettle.multistrikeBuff>0) ui->Multistrike->setPalette(green); else ui->Multistrike->setPalette(black);
    ui->Multistrike->setText(QString("Multistrike:%1 (%2%)").arg(kettle.multistrike+kettle.multistrikeBuff).arg(66.98));

    if (kettle.versatilityBuff>0) ui->Versatility->setPalette(green); else ui->Versatility->setPalette(black);
    ui->Versatility->setText(QString("Versatility:%1 (%2%)").arg(kettle.versatility+kettle.versatilityBuff).arg(11));

    if (kettle.critBuff>0) ui->Crit->setPalette(green); else ui->Crit->setPalette(black);
    ui->Crit->setText(QString("Crit:%1 (%2%)").arg(kettle.crit+kettle.critBuff).arg(11));

} // void MainWindow::displayArcaneMage(arcaneMage& kettle)


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_released()
{
//    MessageBoxA(NULL, "on_pushbuton_clicked", "on_pushbuton_clicked", MB_OK);

 /*   QString st = QString (
                "QProgressBar::chunk {"
                "background-color: #ff0000;"
                 "}");

    st.append("QProgressBar {"
              "border: 1px solid grey;"
              "border-radius: 2px;"
              "text-align: center;"
              "background: #eeeeee;"
              "}");

    ui->progressBar->setStyleSheet(st);*/
}

void MainWindow::on_pushButton_pressed()
{

}


void MainWindow::on_openButton_clicked()
{
//    QString dir = QDir::currentPath();
    QString dir = "L:\\Projects\\SimCraftGui\\";

    if (dir=="") dir = "c:\\";


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Combat log"), dir, tr("Log Files (*.txt)"));
    bool b = false;
    if (fileName !="")
    {
        b = log.init(fileName.toStdString().c_str(), kettle, ui->actionList);

/*      QPalette sample_palette;
        sample_palette.setColor(QPalette::Text, Qt::red);
        ui->actionList->setPalette(sample_palette);*/
        ui->actionList->insertPlainText(fileName);
    }
    ui->stepButton->setEnabled(b);
}

void MainWindow::on_stepButton_clicked()
{
    log.step();
    ui->forwardButton->setEnabled(true);
    displayArcaneMage(kettle);
}

void MainWindow::on_forwardButton_clicked()
{
    log.parseCombatLog(10);
    displayArcaneMage(kettle);
}
