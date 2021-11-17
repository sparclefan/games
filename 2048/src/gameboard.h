#ifndef __gameboard_h_Sparcle_2021_11_11
#define __gameboard_h_Sparcle_2021_11_11

#include <QFrame>
#include <QPainter>
#include <QKeyEvent>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include "numblock.h"

class Loc
{
public:
    Loc(int c, int r):col(c),row(r){};

    int col;
    int row;
};

enum Direction{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace Ui{
	class InfoBoard;
}

class GameInfoBoard : public QWidget
{
    Q_OBJECT
public:
    GameInfoBoard(QWidget *parent);
    ~GameInfoBoard();

public:
    Ui::InfoBoard *ui_info;
};


class GameBoard : public QFrame
{
    Q_OBJECT
public:
    GameBoard(QWidget *parent);
    ~GameBoard();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void resizeEvent(QResizeEvent *event);

signals:
    void sigScoreChanged(int);

public slots:
    void finishMoveAnimate();
    void onRestartGame();

private:
    void moveAction(Direction dir);
    void lineDrop(Loc loc0, Loc loc1, Loc loc2, Loc loc3, QAnimationGroup *group);
    void lineMerge(Loc loc0, Loc loc1, Loc loc2, Loc loc3, QAnimationGroup *group);

    bool newNum();

    void animateGameOver();
    void animateNewBlocks();
    void moveBlock(Loc src, Loc dst, QAnimationGroup *group);
    void mergeBlock(Loc src, Loc dst, QAnimationGroup *group);

    inline NumBlock *getBlock(Loc loc) const{return m_blocks[loc.col][loc.row]; };
    inline void setBlock(Loc loc, NumBlock *block) { m_blocks[loc.col][loc.row] = block;  };

    QRect blockRect(int col, int row);
    int emptyCount();

    bool isGameOver();

    NumBlock *m_blocks[4][4];
    int m_blockWidth;
    int m_blockHeight;
    int m_score;

    QSequentialAnimationGroup m_moveAniGroup;

    GameInfoBoard *m_info;
};

#endif //__gameboard_h_Sparcle_2021_11_11