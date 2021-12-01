#ifndef __SUDOKU_MAINWINDOW_H_SPARCLE_2021_11_22
#define __SUDOKU_MAINWINDOW_H_SPARCLE_2021_11_22

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

#endif //__SUDOKU_MAINWINDOW_H_SPARCLE_2021_11_22