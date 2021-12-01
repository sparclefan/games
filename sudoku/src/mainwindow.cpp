#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow()
:QMainWindow()
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

	ui->gmboard->setInfoPane(ui->infoPane);
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

