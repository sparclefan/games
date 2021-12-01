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
m_undo(), m_candidateNum(nullptr), m_try(false), m_successPane(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    for(int row=0; row<9; row++)
    for(int col=0; col<9; col++)
        m_nums[col][row] = nullptr;

    for(int num=0; num<9; num++)
    {
        for(int i=0; i<9; i++)
        {
            NumBlock *block = new NumBlock(this, num+1);
            m_blocks[num].push_back(block);
        }
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

QRect GameBoard::toolPaneRect()
{
    QRect rect = contentsRect();
    int toolPaneHeight = m_blockHeight*2+GRID_LINE;
    return QRect(0, rect.height()-toolPaneHeight, rect.width(), toolPaneHeight);
}

void GameBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect tpRect = toolPaneRect();
    QRect rect = contentsRect();
	int colWidth = rect.width()/3;
	int rowHeight = (rect.height()-tpRect.height())/3;

    QPen gridPen(QBrush(QColor(0x4c,0x3c,0x3c, 128)), GRID_LINE);
    painter.setPen(gridPen);
	for(int row=0; row<3; row++)
	for(int col=0; col<3; col++)
	{
        QRect rgnRect(col*colWidth, row*rowHeight, colWidth, rowHeight);
		painter.fillRect(rgnRect, conColorBg[(row+col)%2]);
        painter.drawRect(rgnRect);
	}

    painter.fillRect(tpRect, conColorBg[0]);

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
    m_blockHeight = (size.height() - GRID_LINE*11)/11;

    QRect tpRect = toolPaneRect();
    for(int i=0; i<9; i++)
    {
        QRect blockRect( i*(m_blockWidth+GRID_LINE), tpRect.y()+m_blockHeight/2, m_blockWidth, m_blockHeight);
        for(auto block : m_blocks[i]){
            block->setGeometry(blockRect);
        }

        for(int j=0; j<9; j++){
            if( m_nums[i][j]){
                QRect rect = this->blockRect(i,j);
                m_nums[i][j]->setGeometry(rect);
            }
        }
    }
}

void GameBoard::selectNum(int col, int row)
{
    for(int i=0; i<9; i++)
    for(int j=0; j<9; j++)
    {
        if(  m_nums[col][row] && m_nums[col][row]->equals( m_nums[i][j]) )
        {
            m_nums[i][j]->select(true);
        }else if(m_nums[i][j]){
            m_nums[i][j]->select(false);
        }
    }
}

bool GameBoard::isComplete()
{
    for(int col=0; col<9; col++)
    for(int row=0; row<9; row++)
    {
        if( !m_nums[col][row] )
            return false;

        if( m_nums[col][row]->isConflict() )
            return false;

        if( m_nums[col][row]->isTry())
            return false;
    }
    return true;
}

void GameBoard::mousePressEvent(QMouseEvent *event)
{
    // Loc loc = hitTest(event->x(), event->y());
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
    // else{
    //     m_selCol = -1;
    //     m_selRow = -1;
    // }

}

void GameBoard::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_F1:
        m_try = (!m_try);
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
    QRect tpRect = toolPaneRect();

    for(int i=0; i<9; i++)
    for(int j=0; j<9; j++)
    {
        if( !m_nums[i][j] ) continue;

        NumBlock *block = m_nums[i][j];
        m_nums[i][j] = nullptr;
        int num = block->number();

        QRect blockRect( (num-1)*(m_blockWidth+GRID_LINE), tpRect.y()+m_blockHeight/2, m_blockWidth, m_blockHeight);
        block->setGeometry(blockRect);
        block->setFlag(NumBlock::BlockFlag::Standby);
        block->select(false);
        m_blocks[num-1].push_back(block);
    }

    m_selCol = -1;
    m_selRow = -1;
    m_try = 0;

    if( m_candidateNum ){
        m_candidateNum->candidate(false);
        m_candidateNum = nullptr;
    }
    stack<NumLocation> empty;
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
        // m_nums[unit.col][unit.row] = new NumBlock(this, unit.num);
        // QRect rect = blockRect(unit.col, unit.row);
        // m_nums[unit.col][unit.row]->setGeometry(rect);
        // m_nums[unit.col][unit.row]->show();
        int num = unit.num;
        NumBlock *block = m_blocks[num-1].back();
        m_blocks[num-1].pop_back();
        block->setFlag(NumBlock::BlockFlag::Origin);
        m_nums[unit.col][unit.row] = block;
        QRect rect = blockRect(unit.col, unit.row);
        block->setGeometry(rect);
    }

    m_infoPane->startTimer();

    repaint();
}


bool GameBoard::tryCellReset(int num)
{
    NumBlock *block = m_nums[m_selCol][m_selRow];
    if( block->number() == num ){
        if( (!m_try) && (block->isTry()) ){
            block->setFlag(NumBlock::BlockFlag::SetOn);
        }
        return true;
    }

    if( m_blocks[num-1].empty())
        return false;

    int oldNum = block->number();
    block->setFlag(NumBlock::BlockFlag::Standby);
    block->select(false);
    QRect tpRect = toolPaneRect();
    QRect rectBlock( (num-1)*(m_blockWidth+GRID_LINE), tpRect.y()+m_blockHeight/2, m_blockWidth, m_blockHeight);
    block->setGeometry(rectBlock);
    m_blocks[oldNum-1].push_back(block);
    
    NumBlock *newBlock = m_blocks[num-1].back();
    m_blocks[num-1].pop_back();

    m_nums[m_selCol][m_selRow] = nullptr;
    if( m_try )
        newBlock->setFlag(NumBlock::BlockFlag::Try);
    else if( conflict(m_selCol, m_selRow, num) )
        newBlock->setFlag(NumBlock::BlockFlag::Conflict);
    else
        newBlock->setFlag(NumBlock::BlockFlag::SetOn);

    QRect rect = blockRect(m_selCol, m_selRow);
    newBlock->setGeometry(rect);
    m_nums[m_selCol][m_selRow] = newBlock;

    return true;

}


bool GameBoard::setNum(int num)
{
    if( m_selRow<0 || m_selCol<0)
        return false;

    if( m_nums[m_selCol][m_selRow] )
    {
       NumBlock *block =m_nums[m_selCol][m_selRow];
        if ( block->isTry() || block->isConflict() ) {
            tryCellReset(num);
            if( isComplete() ) doComplete();
            return true;
        }
        else
            return false;
    }

    if( m_blocks[num-1].empty())
        return false;

    NumBlock *block = m_blocks[num-1].back();
    m_blocks[num-1].pop_back();

    if( m_try )
        block->setFlag(NumBlock::BlockFlag::Try);
    else if( conflict(m_selCol, m_selRow, num) )
        block->setFlag(NumBlock::BlockFlag::Conflict);
    else
        block->setFlag(NumBlock::BlockFlag::SetOn);

    m_nums[m_selCol][m_selRow] = block;
    QRect rect = blockRect(m_selCol, m_selRow);
    QRect rect0 = QRect(rect.center(), QSize(0,0));  //block->geometry();
    MoveAnimation *ani = new MoveAnimation(m_nums[m_selCol][m_selRow], rect0, rect);
    connect(ani,  SIGNAL(finished()), this, SLOT(onSetNumFinished()) );
    NumLocation loc = {m_selCol, m_selRow};
    m_undo.push(loc);
    ani->start();
}

void GameBoard::onSetNumFinished()
{
    selectNum(m_selCol, m_selRow);
    if( m_candidateNum ){
        m_candidateNum->candidate(false);
        m_candidateNum = nullptr;
    }

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

bool GameBoard::conflict(int col, int row, int num)
{
    for(int i=0; i<9; i++)
    {
        if( m_nums[i][row] && m_nums[i][row]->number()==num )
            return true;
        if( m_nums[col][i] && m_nums[col][i]->number()==num )
            return true;
    }

    int blockId = Sudoku::blockIndex(row, col);
    for(int i=0; i<9; i++)
    for(int j=0; j<9; j++)
    {
        int bid = Sudoku::blockIndex(j, i);
        if( bid != blockId ) continue;
        if( m_nums[i][j] && m_nums[i][j]->number() == num)
            return true;
    }

    return false;
}

void GameBoard::undo()
{
    if(m_undo.empty())
        return;

    NumLocation loc = m_undo.top();
    m_undo.pop();
    NumBlock *block = m_nums[loc.col][loc.row];

    int num = block->number();
    block->select(false);
    block->setFlag(NumBlock::BlockFlag::Standby);
    
    QRect tpRect = toolPaneRect();
    QRect blockRect( (num-1)*(m_blockWidth+GRID_LINE), tpRect.y()+m_blockHeight/2, m_blockWidth, m_blockHeight);
    block->setGeometry(blockRect);

    m_nums[loc.col][loc.row] = nullptr;
    m_blocks[num-1].push_back(block);

}

bool GameBoard::isTipsCell(int col, int row)
{
    if(m_selRow<0 || m_selCol<0 )
        return false;

    if( !m_nums[m_selCol][m_selRow] )
        return (col==m_selCol || row==m_selRow);

    int num = m_nums[m_selCol][m_selRow]->number();
    if( conflict(col, row, num) )
        return true;

    return (m_nums[col][row] != nullptr);
}

void GameBoard::candidate(int col, int row)
{
    if( m_candidateNum ){
        m_candidateNum->candidate(false);
        m_candidateNum = nullptr;
    }

    if( m_nums[col][row] ){
        return;
    }

    int candNum = 0;
    for(int num=1; num<=9; num++){
        if( !conflict(col, row, num) ){
            if(candNum) return;
            candNum = num;
        }
    }

    if( !candNum ) return;

    if( m_candidateNum ) 
        m_candidateNum->candidate(false);

    m_candidateNum = m_blocks[candNum-1].back();
    m_candidateNum->candidate(true);
    m_candidateNum->raise();
}