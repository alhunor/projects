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



void Plate::init(QFrame* _base, QProgressBar* _durationBar, QLabel* _cdLabel, QLabel* _pic_cd, QLabel* _pic_duration, const char* path, std::pair<float, status>* _attribute)
{
    base =_base;
    durationBar = _durationBar;
    pic_duration =_pic_duration;
    pic_cd =_pic_cd;
    attribute = _attribute;
    cdLabel = _cdLabel;

    if (base) base->setVisible(false);

    QPixmap pixy(path);
    if (pic_duration)  pic_duration->setPixmap(pixy);
    if (pic_cd) pic_cd->setPixmap(pixy);

    if (cdLabel) cdLabel->setText("");

    if (durationBar!=NULL)
    {
        durationBar->setMaximum(attribute->second.duration);
        durationBar->setValue(attribute->second.duration);
    }
}


void Plate::reset(QFrame* _base, QProgressBar* _durationBar, QLabel* _pic_duration)
{
    base = _base;
    durationBar = _durationBar;
    pic_duration = _pic_duration;
}

// http://www.qcustomplot.com/

void Plate::display(float time)
{   // qDebug()<<time<<"  "<<attribute->first<<"  "<<attribute->second.cd<<"\n";

    if (base)
    {
        base->setVisible(attribute->second.charges>0);
    }

    if (durationBar)
    {
        //durationBar->setVisible(attribute->second.charges>0);
        durationBar->setValue(time - attribute->first);
    }
    int time_till_cd_expires =  attribute->second.cd - (time - attribute->first);
    if (time_till_cd_expires>0)
    {
        if (pic_cd) pic_cd->setEnabled(false);
        if (cdLabel) cdLabel->setText(QString("%1 s").arg(time_till_cd_expires));
    } else
    {
        if (pic_cd) pic_cd->setEnabled(true);
        if (cdLabel) cdLabel->setText("");
    }
} // void Plate::display(float time)



MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->customPlot->addGraph();
    ui->customPlot->xAxis->setLabel("time");
    ui->customPlot->yAxis->setLabel("mana");
    ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    ui->customPlot->rescaleAxes();
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    // set title of plot:
    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, "Mana Consumption Profile"));

//    std::pair<float, status> , temporal_power
//    std::pair<float, status> , weapon_enchant,

    prismaticCrystal.init(ui->frame_pc, ui->pc_progressBar1, ui->pc_cd, ui->pic_prismaticcrystal_cd, ui->pic_prismaticcrystal, "L:\\Projects\\SimCraftGui\\prismaticcrystal.jpg", &kettle.prismatic_crystal);
    nithramus.init(ui->frame_nith, ui->nith_progressBar1, ui->nith_cd, ui->pic_nithramus_cd, ui->pic_nithramus, "L:\\Projects\\SimCraftGui\\nithramus.jpg",&kettle.nithramus);
    hero.init(ui->frame_hero, ui->hero_progressBar1, ui->hero_cd, ui->pic_hero_cd, ui->pic_hero, "L:\\Projects\\SimCraftGui\\timeWrap.jpg",&kettle.hero);
    intellectPotion.init(ui->frame_pot, ui->pot_progressBar1, ui->pot_cd, ui->pic_potion_cd, ui->pic_potion, "L:\\Projects\\SimCraftGui\\intellectPotion.jpg", &kettle.draenic_intellect_potion);
    presenceofmind.init(ui->frame_pm, ui->pm_progressBar1, ui->pm_cd, ui->pic_presenceofmind_cd, ui->pic_presenceofmind, "L:\\Projects\\SimCraftGui\\presenceofmind.jpg", &kettle.presence_of_mind);
    arcanepower.init(ui->frame_ap, ui->ap_progressBar1, ui->ap_cd, ui->pic_arcanepower_cd, ui->pic_arcanepower, "L:\\Projects\\SimCraftGui\\arcanepower.jpg",&kettle.arcane_power);
    doomNova.init(ui->frame_doom, ui->doom_progressBar1, NULL, NULL, ui->pic_doom, "L:\\Projects\\SimCraftGui\\doomNova.jpg",&kettle.doom_nova);
    weaponEnchant.init(ui->frame_ench, ui->ench_progressBar1, NULL, NULL, ui->pic_enchant, "L:\\Projects\\SimCraftGui\\enchant.jpg",&kettle.weapon_enchant);
    evocation.init(NULL, NULL, ui->evo_cd, ui->pic_evocation_cd, NULL, "L:\\Projects\\SimCraftGui\\evocation.jpg",&kettle.evocation);

    ui->frame_ao_2->setVisible(false);
    ui->frame_sn_2->setVisible(false);


    ui->openButton->installEventFilter(this);
    ui->stepButton->installEventFilter(this);
    ui->forwardButton->installEventFilter(this);
    ui->actionList->installEventFilter(this);

    init();


}


bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
 // http://www.informit.com/articles/article.aspx?p=1405544 <- capture specific events
// http://www.informit.com/articles/article.aspx?p=1405544&seqNum=2 <- Redefine event filter on main window
// http://www.informit.com/articles/article.aspx?p=1405544&seqNum=3 <- remain responsive during long processing tasks

    //    if (target == ui->stepButton || target == ui->openButton || ...)
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Space)
            {
                if (ui->stepButton->isEnabled()) on_stepButton_clicked();
                return true;
            }
        }
    return QMainWindow::eventFilter(target, event);
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

    ui->customPlot->graph(0)->addData(time, mana_pct);
    // set data of dots:
    // rescale value (vertical) axis to fit the current data:
    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(0)->rescaleKeyAxis();

    ui->customPlot->replot();


// Attributes
    QPalette green;
    green.setColor(QPalette::WindowText, Qt::green);
    QPalette black;
    black.setColor(QPalette::WindowText, Qt::black);

    if (kettle.intellectBuff>0) ui->Intellect->setPalette(green); else ui->Intellect->setPalette(black);
    ui->Intellect->setText(QString("Intellect:%1 (%2%)").arg(kettle.stat_value(INTELLECT)).arg(reformat(kettle.stat_percentage(INTELLECT))));

    if (kettle.spellpowerBuff>0 || kettle.intellectBuff) ui->SpellPower->setPalette(green); else ui->SpellPower->setPalette(black);
    ui->SpellPower->setText(QString("SpellPower:%1").arg(kettle.stat_value(SPELLPOWER)+kettle.intellectBuff));

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

    if (kettle.prismatic_crystal.second.charges>0)
    {
        ui->frame_pc_target->setVisible(true);
    } else
    {
        ui->frame_pc_target->setVisible(false);
    }



    if (kettle.doom_nova.second.charges>0)
    {
        if (kettle.doomTarget=="Fluffy_Pillow")
        {
            ui->frame_doom_2->setVisible(false);
            doomNova.reset(ui->frame_doom, ui->doom_progressBar1, ui->pic_doom);
        } else if (kettle.doomTarget=="Kettle_Active_prismatic_crystal")
        {
            ui->frame_doom->setVisible(false);
            doomNova.reset(ui->frame_doom_2, ui->doom_progressBar1_2, ui->pic_doom_2);
        }
        ui->doomTarget->setText(QString("DoomTarget = ")+QString::fromStdString(kettle.doomTarget));
        // TODO repoint doomtargettext to Prismatic Crystal or dragon
    }

    //    Availabale abilities
    prismaticCrystal.display(time);
    nithramus.display(time);
    hero.display(time);
    intellectPotion.display(time);
    presenceofmind.display(time);
    arcanepower.display(time);
    doomNova.display(time);
    weaponEnchant.display(time);
    evocation.display(time);


    //supernova

} // void MainWindow::displayArcaneMage(arcaneMage& kettle)


MainWindow::~MainWindow()
{
    delete ui;
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



/*void MainWindow::keyPressEvent(QKeyEvent *event)
{
   // if (event->key() == Qt::Key_Space)
    {
        Beep(1000,500);
    }
    QMainWindow::keyPressEvent(event);
}*/

