#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow()
:QMainWindow()
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

	ui->gmboard->setInfoPane(ui->infoPane);
	ui->gmboard->setToolPane(ui->toolpane);
	connect(ui->toolpane, SIGNAL(numberClicked(int)), ui->gmboard, SLOT(setNum(int)));
	connect(ui->cb_try, SIGNAL(stateChanged(int)), ui->gmboard, SLOT(onTryToggle(int)));
	connect(ui->gmboard, SIGNAL(tryStateChanged(bool)), this, SLOT(setTryButtonState(bool)));
	connect(ui->pb_undo, SIGNAL(clicked()), ui->gmboard, SLOT(undo()));

	ui->gmboard->setFocus();
	// QScreen *scn = QGuiApplication::primaryScreen();
	// QRect sc = scn->geometry();
	// int w = sc.width()/2;
	// int h = w; //sc.height()/2;
	// int x = (sc.width()-w)/2;
	// int y = (sc.height()-h)/2;
	// this->setGeometry(x,y,w,h);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setTryButtonState(bool state)
{
	ui->cb_try->setChecked(state);
}