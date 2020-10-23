#ifndef __TERRIS_MAINWINDOW_H_SPARCLE_2020_10_22
#define __TERRIS_MAINWINDOW_H_SPARCLE_2020_10_22

#include <QMainWindow>

namespace Ui{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private:
     Ui::MainWindow *ui;

};

#endif //__TERRIS_MAINWINDOW_H_SPARCLE_2020_10_22