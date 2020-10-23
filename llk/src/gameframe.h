#ifndef __LLK_GAMEFRAME_H_SPARCLE_2020_09_29
#define __LLK_GAMEFRAME_H_SPARCLE_2020_09_29

#include <QMainWindow>
#include "gameview.h"

namespace Ui{
	class GameFrame;
}

class GameFrame : public QMainWindow
{
	Q_OBJECT
public:
	GameFrame();
	~GameFrame();

private:
    Ui::GameFrame *ui;
};


#endif //__LLK_GAMEFRAME_H_SPARCLE_2020_09_29