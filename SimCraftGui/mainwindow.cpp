#include <windows.h>
#include <QFileDialog>
#include <QFrame>
#include <QDebug>
#include <QProgressBar>
#include <QLabel>
#include "combatlog.h"
#include "arcaneMage.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>



void Plate::init(QFrame* _base, QProgressBar* _durationBar, QProgressBar* _cdBar, QLabel* _pic, const char* path, std::pair<float, status>* _attribute)
{
    base =_base;
    durationBar = _durationBar;
    cdBar = _cdBar;
    pic =_pic;
    attribute = _attribute;
    QPixmap pixy(path);
    pic->setPixmap(pixy);
    durationBar->setMaximum(attribute->second.duration);
    durationBar->setValue(0);
    if (cdBar!=NULL)
    {
        cdBar->setMaximum(attribute->second.cd);
        cdBar->setValue(attribute->second.cd);
    }
}



void Plate::display(float time)
{
   // qDebug()<<time<<"  "<<attribute->first<<"  "<<attribute->second.cd<<"\n";
    pic->setEnabled( (time - attribute->first > attribute->second.cd));
    durationBar->setVisible(attribute->second.charges>0);
    durationBar->setValue(time - attribute->first);
    if (cdBar!=NULL)
    {
        cdBar->setValue(time - attribute->first);
    }

    if (time - attribute->first < attribute->second.duration)
    {
        //base->setP TODO
    }
}

MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    std::pair<float, status> , temporal_power
//    std::pair<float, status> , weapon_enchant,

    prismaticCrystal.init(ui->frame_pc, ui->pc_progressBar1, ui->pc_progressBar2, ui->pic_prismaticcrystal, "L:\\Projects\\SimCraftGui\\prismaticcrystal.jpg", &kettle.presence_of_mind);
    nithramus.init(ui->frame_nith, ui->nith_progressBar1, ui->nith_progressBar2, ui->pic_nithramus, "L:\\Projects\\SimCraftGui\\nithramus.jpg",&kettle.nithramus);
    hero.init(ui->frame_hero, ui->hero_progressBar1, ui->hero_progressBar2, ui->pic_hero, "L:\\Projects\\SimCraftGui\\timeWrap.jpg",&kettle.hero);
    intellectPotion.init(ui->frame_pot, ui->pot_progressBar1, ui->pot_progressBar2, ui->pic_intellectpotion, "L:\\Projects\\SimCraftGui\\intellectPotion.jpg", &kettle.draenic_intellect_potion);
    presenceofmind.init(ui->frame_pm, ui->pm_progressBar1, ui->pm_progressBar2, ui->pic_presenceofmind, "L:\\Projects\\SimCraftGui\\presenceofmind.jpg", &kettle.presence_of_mind);
    arcanepower.init(ui->frame_ap, ui->ap_progressBar1, ui->ap_progressBar2, ui->pic_arcanepower, "L:\\Projects\\SimCraftGui\\arcanepower.jpg",&kettle.arcane_power);
    doomNova.init(ui->frame_doom, ui->doom_progressBar1, NULL, ui->pic_doom, "L:\\Projects\\SimCraftGui\\doomNova.jpg",&kettle.doom_nova);
    weaponEnchant.init(ui->frame_ench, ui->ench_progressBar1, NULL, ui->pic_enchant, "L:\\Projects\\SimCraftGui\\enchant.jpg",&kettle.weapon_enchant);

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

    QPixmap incantersflow("L:\\Projects\\SimCraftGui\\incantersflow.jpg");
    ui->incantersflow->setPixmap(incantersflow);

    displayArcaneMage(kettle, log.getTime());
} // MainWindow::init()

QString reformat(float f)
{
   if (100*(f-int(f)>0))
        return QString::number( f, 'f', 2);
   return QString::number( f, 'f', 0);
}

void MainWindow::displayArcaneMage(arcaneMage& kettle, float time)
{
// Arcane charges
    ui->arcanecharge1->setEnabled(kettle.arcane_charge.second.charges>0);
    ui->arcanecharge2->setEnabled(kettle.arcane_charge.second.charges>1);
    ui->arcanecharge3->setEnabled(kettle.arcane_charge.second.charges>2);
    ui->arcanecharge4->setEnabled(kettle.arcane_charge.second.charges>3);    
    // show remaining time
    ui->ac_progressBar->setVisible(kettle.arcane_charge.second.charges>0);
    ui->ac_progressBar->setValue( kettle.arcane_charge.second.duration -(time-kettle.arcane_charge.first) );

//Arcane Missiles
    ui->arcanemissile1->setEnabled(kettle.arcane_missiles.second.charges>0);
    ui->arcanemissile2->setEnabled(kettle.arcane_missiles.second.charges>1);
    ui->arcanemissile3->setEnabled(kettle.arcane_missiles.second.charges>2);
    // show remaining time
    ui->am_progressBar->setVisible(kettle.arcane_missiles.second.charges>0);
    ui->am_progressBar->setValue( kettle.arcane_missiles.second.duration -(time-kettle.arcane_missiles.first) );


// Incanter's Flow
    ui->IFText->setText(QString("%1").arg(kettle.incanters_flow));

    QPalette sample_palette;
//    QColor c = (25, 25, 25, 255);
    sample_palette.setColor(QPalette::WindowText, Qt::white);
//    sample_palette.setColor(QPalette::WindowText, c);
    ui->IFText->setPalette(sample_palette);

// Mana
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

    if (kettle.intellectBuff>0) ui->Intellect->setPalette(green); else ui->Intellect->setPalette(black);
    ui->Intellect->setText(QString("Intellect:%1 (%2%)").arg(kettle.stat_value(INTELLECT)).arg(reformat(kettle.stat_percentage(INTELLECT))));

    if (kettle.spellpowerBuff>0) ui->SpellPower->setPalette(green); else ui->SpellPower->setPalette(black);
    ui->SpellPower->setText(QString("SpellPower:%1").arg(kettle.stat_value(SPELLPOWER)));

    if (kettle.masteryBuff>0) ui->Mastery->setPalette(green); else ui->Mastery->setPalette(black);
    ui->Mastery->setText(QString("Mastery:%1 (%2%)").arg(kettle.stat_value(MASTERY)).arg(reformat(kettle.stat_percentage(MASTERY))));

    if (kettle.hasteBuff>0) ui->Haste->setPalette(green); else ui->Haste->setPalette(black);
    ui->Haste->setText(QString("Haste:%1 (%2%)").arg(kettle.stat_value(HASTE)).arg(reformat(kettle.stat_percentage(HASTE))));

    if (kettle.multistrikeBuff>0) ui->Multistrike->setPalette(green); else ui->Multistrike->setPalette(black);
    ui->Multistrike->setText(QString("Multistrike:%1 (%2%)").arg(kettle.stat_value(MULTISTRIKE)).arg(reformat(kettle.stat_percentage(MULTISTRIKE))));

    if (kettle.versatilityBuff>0) ui->Versatility->setPalette(green); else ui->Versatility->setPalette(black);
    ui->Versatility->setText(QString("Versatility:%1 (%2%)").arg(kettle.stat_value(VERSATILITY)).arg(reformat(kettle.stat_percentage(VERSATILITY))));

    if (kettle.critBuff>0) ui->Crit->setPalette(green); else ui->Crit->setPalette(black);
    ui->Crit->setText(QString("Crit:%1 (%2%)").arg(kettle.stat_value(CRIT)).arg(reformat(kettle.stat_percentage(CRIT))));

// Qprogressbar with real values: http://www.qtcentre.org/archive/index.php/t-6217.html

//    Availabale abilities
    prismaticCrystal.display(time);
    nithramus.display(time);
    hero.display(time);
    intellectPotion.display(time);
    presenceofmind.display(time);
    arcanepower.display(time);
    doomNova.display(time);
    weaponEnchant.display(time);
    //supernova

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
    displayArcaneMage(kettle, log.getTime());
}

void MainWindow::on_forwardButton_clicked()
{
    doomNova.base->move(600,190);

//    log.parseCombatLog(10);
//    displayArcaneMage(kettle, log.getTime());
}
