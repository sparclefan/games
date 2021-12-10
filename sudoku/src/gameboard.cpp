#include "gameboard.h"
#include <QPainter>
#include <QResizeEvent>
#include "sudoku.h"
#include "ui_successpane.h"

#define GRID_LINE 15

const QColor conColorBg[] = {
    QColor(0xbb, 0xad, 0xa0),
    QColor(0xb0, 0xad, 0xa0)
};


SuccessPane::SuccessPane(QWidget *parent)
:QWidget(parent)
{
    ui = new Ui::SuccessPane();
    ui->setupUi(this);
}

SuccessPane::~SuccessPane()
{

}


GameBoard::GameBoard(QWidget *parent)
:QFrame(parent), m_blockWidth(0), m_blockHeight(0), m_selCol(-1), m_selRow(-1), 
m_undo(), m_try(false), m_successPane(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
    {
        m_numblocks[col][row] = new NumBlock(this, 0);
        int blockId = Sudoku::blockIndex(row, col);
        m_area[blockId].push_back(m_numblocks[col][row]);
    }

}

GameBoard::~GameBoard()
{

}

QRect GameBoard::blockRect(int col, int row)
{
    int x = col*(m_blockWidth+GRID_LINE) + GRID_LINE;
    int y = row*(m_blockHeight+GRID_LINE) + GRID_LINE;
    return QRect(x, y, m_blockWidth, m_blockHeight);
}

void GameBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect = contentsRect();
	int colWidth = rect.width()/3;
	int rowHeight = rect.height()/3;

    QPen gridPen(QBrush(QColor(0x4c,0x3c,0x3c, 128)), GRID_LINE);
    painter.setPen(gridPen);
	for(int row=0; row<3; row++)
	for(int col=0; col<3; col++)
	{
        QRect rgnRect(col*colWidth, row*rowHeight, colWidth, rowHeight);
		painter.fillRect(rgnRect, conColorBg[(row+col)%2]);
        painter.drawRect(rgnRect);
	}

    // painter.fillRect(tpRect, conColorBg[0]);

    QColor color(0xfa, 0xf8, 0xef);
    painter.setPen(color);

    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
    {
        if( (row==m_selRow)&&(col==m_selCol) )
            color.setAlphaF(1);
        else if( isTipsCell(col, row) )
            color.setAlphaF(0.2);
        else
            color.setAlphaF(0.5);

        painter.setBrush(QBrush(color));
        painter.drawRoundedRect(blockRect(col, row), 20.0, 20.0, Qt::RelativeSize);
    }

    QFrame::paintEvent(event);

}

void GameBoard::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();

    m_blockWidth = (size.width() - GRID_LINE*10)/9;
    m_blockHeight = (size.height() - GRID_LINE*10)/9;

    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
    {
            QRect rect = blockRect(col,row);
            m_numblocks[col][row]->setGeometry(rect);
    }
}

void GameBoard::selectNum(int col, int row)
{
    for(int i=0; i<9; i++)
    for(int j=0; j<9; j++)
    {
        if(  (!m_numblocks[col][row]->empty()) && ( m_numblocks[col][row]->equals(m_numblocks[i][j])) )
        {
            m_numblocks[i][j]->select(true);
        }else if(!m_numblocks[i][j]->empty()){
            m_numblocks[i][j]->select(false);
        }
    }
}

bool GameBoard::isComplete()
{
    for(int col=0; col<9; col++)
    for(int row=0; row<9; row++)
    {
        if( m_numblocks[col][row]->empty() )
            return false;

        if( m_numblocks[col][row]->isConflict() )
            return false;

        if( m_numblocks[col][row]->isTry())
            return false;
    }
    return true;
}

void GameBoard::mousePressEvent(QMouseEvent *event)
{
    int col = event->x()/(m_blockWidth+GRID_LINE);
    int row = event->y()/(m_blockHeight+GRID_LINE);

    if( col>=0 && col<9 && row>=0 && row<9 )
    {
        m_selCol = col;
        m_selRow = row;

        selectNum(col, row);
        candidate(col, row);
        repaint();
    }
}

void GameBoard::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_F1:
        // onTryToggle();
        emit tryStateChanged(!m_try);
        break;
    case Qt::Key_F2:
        newGame(Medium);
        break;
    case Qt::Key_F3:
        newGame(Hard);
        break;
    case Qt::Key_F5:
        undo();
        break;
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        setNum(key-Qt::Key_0);
        break;
    default:
        break;            
    }
}

void GameBoard::reSetNums()
{
    for(int i=0; i<9; i++)
    for(int j=0; j<9; j++)
    {
        m_numblocks[i][j]->setNumber(0);
        m_numblocks[i][j]->setFlag(NumBlock::BlockFlag::Empty);
        m_numblocks[i][j]->select(false);
    }

    m_selCol = -1;
    m_selRow = -1;
    m_try = 0;

    m_toolPane->resetAll();

    stack<Operator> empty;
    m_undo.swap(empty);

    repaint();
}

void GameBoard::newGame(GameLevel level)
{
    int empty = level * 20;

    Sudoku sudoku;
    vector<SudokuUnit> puzzle = sudoku.generatorPuzzle(empty);

    reSetNums();
    for(auto unit : puzzle){
        NumBlock *block = m_numblocks[unit.col][unit.row];
        block->setFlag(NumBlock::BlockFlag::Origin);
        block->setNumber(unit.num);
        m_toolPane->setOn(unit.num);
    }

    m_infoPane->startTimer();

    repaint();
}


bool GameBoard::tryCellReset(int num)
{
    NumBlock *block = m_numblocks[m_selCol][m_selRow];

    Operator op;
    op.row = m_selRow;
    op.col = m_selCol;
    op.oldNumber = block->number();
    op.newNumber = num;
    op.op = SetOnNumber;
    if(block->isTry())
        op.oldFlag = NumBlock::BlockFlag::Try;
    else
        op.oldFlag = NumBlock::BlockFlag::SetOn;

    if(!block->isTry()){
        m_toolPane->restoreNumber(op.oldNumber);
    }

    block->setNumber(num);

    m_undo.push(op);

    return true;

}

void GameBoard::onTryToggle(int state)
{
    m_try = (state==Qt::Checked);
    // setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

bool GameBoard::setNum(int num)
{
    if( m_selRow<0 || m_selCol<0)
        return false;

    NumBlock *block = m_numblocks[m_selCol][m_selRow];
    if(block->isOrigin()) return false;

    if( (!m_try) && (!m_toolPane->canSet(num)) ) 
        return false;

    if( !block->empty() )
    {
        if ( (!block->isTry()) && (!block->isConflict()) ) {
            return false;
        }

        if( !block->isTry()){
            m_toolPane->restoreNumber(block->number());
        }
    }

    Operator op;
    op.row = m_selRow;
    op.col = m_selCol;
    op.oldNumber = block->number();
    op.newNumber = num;
    op.op = SetOnNumber;
    if(block->isTry())
        op.oldFlag = NumBlock::BlockFlag::Try;
    else
        op.oldFlag = NumBlock::BlockFlag::SetOn;

    if( m_try ){
        block->setFlag(NumBlock::BlockFlag::Try);
        op.op = TrySetNumber;
    }
    else{
        block->setFlag(NumBlock::BlockFlag::SetOn);
        m_toolPane->setOn(num);
    }

    block->setNumber(num);
    if(block->isConflict()) checkConflict();
    QRect rect = blockRect(m_selCol, m_selRow);
    QRect rect0 = QRect(rect.center(), QSize(0,0));  //block->geometry();
    MoveAnimation *ani = new MoveAnimation(block, rect0, rect);
    connect(ani,  SIGNAL(finished()), this, SLOT(onSetNumFinished()) );

    m_undo.push(op);
    ani->start();
}

void GameBoard::onSetNumFinished()
{
    selectNum(m_selCol, m_selRow);
    conflict(m_selCol, m_selRow);
    m_toolPane->recommend(-1);

    if( isComplete() )
    {
        doComplete();
    }else{
        repaint();
    }
}

void GameBoard::doComplete()
{
    m_infoPane->stopTimer();

    if(m_successPane){
        delete(m_successPane);
        m_successPane = nullptr;
    }
    m_successPane = new SuccessPane(this);
    connect(m_successPane->ui->pb_newpuzzle, SIGNAL(clicked()), this, SLOT(onNewPuzzle()));
    m_successPane->show();
    QPropertyAnimation *ani = new QPropertyAnimation(m_successPane, "geometry");
    QRect rect(0, 0, width(), height());
    ani->setDuration(500);
    ani->setStartValue(QRect(rect.center(), QSize(0,0)));
    ani->setEndValue(rect);
    ani->start();
}

void GameBoard::onNewPuzzle()
{
    if(m_successPane){
        delete(m_successPane);
        m_successPane = nullptr;
    } 

    newGame(Hard);
}

bool GameBoard::conflict(int col, int row)
{
    bool bConflict = false;

    NumBlock *block = m_numblocks[col][row];
    int num = block->number();
    int blockId = Sudoku::blockIndex(row, col);
    for(int i=0; i<9; i++)
    {
        if( (i!=col) && m_numblocks[i][row]->number()==num )
        {
            bConflict = true;
            m_numblocks[i][row]->conflict(true);
            m_numblocks[i][row]->update();
        }
        if( (i!=row) && m_numblocks[col][i]->number()==num )
        {
            bConflict = true;
            m_numblocks[col][i]->conflict(true);
            m_numblocks[col][i]->update();
        }
        if( (block != m_area[blockId][i]) && m_area[blockId][i]->number()==num )
        {
            bConflict = true;
            m_area[blockId][i]->conflict(true);
            m_area[blockId][i]->update();
        }
    }

    if(bConflict) block->conflict(true);

    return bConflict;
}

bool GameBoard::isConflict(int col, int row, int num)
{
    int blockId = Sudoku::blockIndex(row, col);
    NumBlock *block = m_numblocks[col][row];
    if(num==0) num = block->number();
    for(int i=0; i<9; i++)
    {
        if( (i!=col) && m_numblocks[i][row]->number()==num )
            return true;
        if( (i!=row) && m_numblocks[col][i]->number()==num )
            return true;
        if( (block != m_area[blockId][i]) && m_area[blockId][i]->number()==num )
            return true;
    }

    return false;
}

void GameBoard::checkConflict()
{
    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
    {
        NumBlock *block = m_numblocks[col][row];
        if( block->empty()) continue;
        if( !block->isConflict()) continue;
        if( !isConflict(col, row, 0) ){
            block->conflict(false);
            block->update();
        }
    }
}


void GameBoard::undo()
{
    if(m_undo.empty())
        return;

    Operator op = m_undo.top();
    m_undo.pop();
    NumBlock *block = m_numblocks[op.col][op.row];

    if( !block->isTry() )
        m_toolPane->restoreNumber(op.newNumber);

    block->select(false);
    block->setNumber(op.oldNumber);
    block->setFlag(op.oldFlag);

    if(block->isConflict())
        checkConflict();

    conflict(op.col, op.row);

    block->update();

    if( !block->isTry())
        m_toolPane->setOn(op.oldNumber);

    setFocus();
}

bool GameBoard::isTipsCell(int col, int row)
{
    if(m_selRow<0 || m_selCol<0 )
        return false;

    if( m_numblocks[m_selCol][m_selRow]->empty() )
        return (col!=m_selCol && row!=m_selRow);

    int num = m_numblocks[m_selCol][m_selRow]->number();
    if( isConflict(col, row, num) )
        return true;

    return (!m_numblocks[col][row]->empty());
}

void GameBoard::candidate(int col, int row)
{
    m_toolPane->recommend(-1);

    if( !m_numblocks[col][row]->empty() ){
        return;
    }

    int candNum = 0;
    for(int num=1; num<=9; num++){
        if( !isConflict(col, row, num) ){
            if(candNum) return;
            candNum = num;
        }
    }

    if( !candNum ) return;
    m_toolPane->recommend(candNum);

}