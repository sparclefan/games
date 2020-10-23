#ifndef __TERRIS_TERRISGAME_H_SPARCLE_2020_10_22
#define __TERRIS_TERRISGAME_H_SPARCLE_2020_10_22

#include "block.h"
#include <QObject>
#include <QThread>

#define MAXLINES 20
#define TOTALWIDTH 10

class TerrisGame : public QObject
{
	Q_OBJECT
public:
	TerrisGame();
	~TerrisGame();

    void draw(QPainter &painter, int totalWidth, int totalHeight);

    void Left();
    void Right();
    void Rotate();
    void Down();

public slots:
    void newGame();

signals:
    void sigUpdate();

private:
    void gameThread();

    void newBlock();
    BOOL checkCur();
    BOOL canDown();
    BOOL canLeft();
    BOOL canRight();
    void Drop();
    void killLine(int line);
    void checkFillLine();

    void drawGameBoard(QPainter &painter, int blockWidth);
    void drawScoreLevel(QPainter &painter, int blockWidth);

    BYTE m_blocks[MAXLINES][TOTALWIDTH];
    Blocks *m_pCur;
    Blocks *m_pNext;
    BOOL m_bGameOver;

    int m_score;
    int m_level;

    QThread* m_pThread;
};


#endif //__TERRIS_TERRISGAME_H_SPARCLE_2020_10_22