#ifndef __GAMEBOARD_H_Sparcle_2021_11_22
#define __GAMEBOARD_H_Sparcle_2021_11_22

#include <QFrame>
#include "numblock.h"
#include "infopane.h"
#include "toolpane.h"
#include <map>
#include <vector>
#include <stack>

using namespace std;

enum GameLevel{
    Easy=1,
    Medium,
    Hard
};

namespace Ui{
	class SuccessPane;
}

class SuccessPane : public QWidget
{
    Q_OBJECT
public:
    SuccessPane(QWidget *parent);
    ~SuccessPane();

public:
    Ui::SuccessPane *ui;
};


enum GameOperator{
    SetOnNumber,
    TrySetNumber,
    TryToSetOn
};

typedef struct{
    int row;
    int col;
    int oldNumber;
    int newNumber;
    GameOperator op;
    NumBlock::BlockFlag oldFlag;
} Operator;

class GameBoard : public QFrame
{
    Q_OBJECT
public:
    GameBoard(QWidget *parent);
    ~GameBoard();

    inline void setInfoPane(InfoPane *pane){ m_infoPane = pane;};
    inline void setToolPane(ToolPane *pane){ m_toolPane = pane;};

public slots:
    void onSetNumFinished();
    void onNewPuzzle();
    bool setNum(int num);
    void undo();
    void onTryToggle(int state);

signals:
    void tryStateChanged(bool state);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QRect blockRect(int col, int row);
    bool conflict(int col, int row);
    bool isConflict(int col, int row, int num);
    void checkConflict();
    bool isTipsCell(int col, int row);
    void reSetNums();
    bool tryCellReset(int num);
    bool isComplete();
    void doComplete();
	
    void newGame(GameLevel level);
    void selectNum(int col, int row);
    void candidate(int col, int row);

    int m_blockWidth;
    int m_blockHeight;    
    int m_selCol;
    int m_selRow;
    bool m_try;

    NumBlock *m_numblocks[9][9];
    vector<NumBlock *> m_area[9];
    stack<Operator> m_undo;

    SuccessPane *m_successPane;
    InfoPane *m_infoPane;
    ToolPane *m_toolPane;
};

#endif //__GAMEBOARD_H_Sparcle_2021_11_22