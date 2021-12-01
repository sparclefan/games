#include <QApplication>
#include "mainwindow.h"
// #include "dlx.h"
// #include "sudoku.h"

int main(int argc, char *argv[])
{
	// return unit_test_Sudoku();
	// return unit_test_DancingLinksX();
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}