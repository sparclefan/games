#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow()
:QMainWindow()
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

    connect(ui->actionStart, SIGNAL(triggered()), ui->centralwidget->game(), SLOT(newGame()) );
}

MainWindow::~MainWindow()
{

}