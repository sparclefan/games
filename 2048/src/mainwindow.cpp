#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow()
:QMainWindow()
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

	connect(ui->gameBoard, SIGNAL(sigScoreChanged(int)), this, SLOT(onScoreChanged(int)));
    // connect(ui->actionStart, SIGNAL(triggered()), ui->centralwidget->game(), SLOT(newGame()) );
}

MainWindow::~MainWindow()
{

}

void MainWindow::onScoreChanged(int score)
{
	ui->lb_score->setText(QString("%1").arg(score));
}