#include "ui_gameframe.h"
#include "gameframe.h"

GameFrame::GameFrame()
:QMainWindow()
{
	ui = new Ui::GameFrame();
	ui->setupUi(this);
    ui->centralwidget->setLayout(ui->verticalLayout);

    ui->infoView->setGame(ui->gameView->game());

    connect(ui->actionSimple, SIGNAL(triggered()), ui->gameView, SLOT(onActionSimple()) );
    connect(ui->actionNormal, SIGNAL(triggered()), ui->gameView, SLOT(onActionNormal()) );
    connect(ui->actionHard, SIGNAL(triggered()), ui->gameView, SLOT(onActionHard()) );
}

GameFrame::~GameFrame()
{

}