#include <QApplication>
#include "gamewin.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GameWin w;
	w.showMaximized();

	return a.exec();
}