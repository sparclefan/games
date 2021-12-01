#include "gameboard.h"
#include "ui_frameinfo.h"
#include <QPropertyAnimation>


#define GRID_LINE 30

GameInfoBoard::GameInfoBoard(QWidget *parent)
:QWidget(parent)
{
    ui_info = new Ui::InfoBoard();
    ui_info->setupUi(this);
}

GameInfoBoard::~GameInfoBoard()
{

}

GameBoard::GameBoard(QWidget *parent)
:QFrame(parent), m_blockWidth(0), m_blockHeight(0), m_score(0), m_info(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    srand((unsigned)time(NULL));

    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {
        m_blocks[col][row] = nullptr;
    }

    // m_blocks[0][3] = new NumBlock(this, 2);
    // m_blocks[1][3] = new NumBlock(this, 4);
    // m_blocks[2][3] = new NumBlock(this, 8);

    newNum();
    newNum();

    connect(&m_moveAniGroup, SIGNAL(finished()), this, SLOT(finishMoveAnimate()));

}

GameBoard::~GameBoard()
{

}

void GameBoard::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();

    m_blockWidth = (size.width() - GRID_LINE*5)/4;
    m_blockHeight = (size.height() - GRID_LINE*5)/4;

    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {

        int x = col*(m_blockWidth+GRID_LINE) + GRID_LINE;
        int y = row*(m_blockHeight+GRID_LINE) + GRID_LINE;
        QRect rect(x, y, m_blockWidth, m_blockHeight);

        if( m_blocks[col][row] ){
            m_blocks[col][row]->setGeometry(rect);
        }

    }
    animateNewBlocks();

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

    QRect rect = frameRect();
    painter.fillRect(rect, QColor(0xbb,0xad, 0xa0));

    QColor color(0xfa, 0xf8, 0xef, 0.2*255);
    painter.setPen(color);
    painter.setBrush(QBrush(color));


    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {
        // painter.fillRect( blockRect(col, row), QColor(0xfa, 0xf8, 0xef, 0.2*255));
        painter.drawRoundedRect(blockRect(col, row), 20.0, 20.0, Qt::RelativeSize);
    }

    QFrame::paintEvent(event);
}

void GameBoard::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_Left:
        moveAction(LEFT);
        break;
    case Qt::Key_Right:
        moveAction(RIGHT);
        break;
    case Qt::Key_Up:
        moveAction(UP);
        break;
    case Qt::Key_Down:
        moveAction(DOWN);
        break;
    default:
        break;            
    }
}

void GameBoard::moveAction(Direction dir)
{
    m_moveAniGroup.clear();
    QParallelAnimationGroup *dropAniGroup = new QParallelAnimationGroup();
    QParallelAnimationGroup *mergeAniGroup = new QParallelAnimationGroup();
    QParallelAnimationGroup *postDropGroup = new QParallelAnimationGroup();

    switch(dir)
    {
    case LEFT:
        for(int row=0; row<4; row++){
            lineDrop(Loc(0,row), Loc(1,row), Loc(2,row), Loc(3,row), dropAniGroup);
            lineMerge(Loc(0,row), Loc(1,row), Loc(2,row), Loc(3,row), mergeAniGroup);
            lineDrop(Loc(0,row), Loc(1,row), Loc(2,row), Loc(3,row), postDropGroup);
        }
        break;
    case RIGHT:
        for(int row=0; row<4; row++){
            lineDrop(Loc(3,row), Loc(2,row), Loc(1,row), Loc(0,row), dropAniGroup);
            lineMerge(Loc(3,row), Loc(2,row), Loc(1,row), Loc(0,row), mergeAniGroup);
            lineDrop(Loc(3,row), Loc(2,row), Loc(1,row), Loc(0,row), postDropGroup);
        }
        break;
    case UP:
        for(int col=0; col<4; col++){
            lineDrop(Loc(col,0), Loc(col,1), Loc(col,2), Loc(col,3), dropAniGroup);
            lineMerge(Loc(col,0), Loc(col,1), Loc(col,2), Loc(col,3), mergeAniGroup);
            lineDrop(Loc(col,0), Loc(col,1), Loc(col,2), Loc(col,3), postDropGroup);
        }
        break;
    case DOWN:
        for(int col=0; col<4; col++){
            lineDrop(Loc(col,3), Loc(col,2), Loc(col,1), Loc(col,0), dropAniGroup);
            lineMerge(Loc(col,3), Loc(col,2), Loc(col,1), Loc(col,0), mergeAniGroup);
            lineDrop(Loc(col,3), Loc(col,2), Loc(col,1), Loc(col,0), postDropGroup);
        }
        break;
    }

    if( dropAniGroup->animationCount() > 0 )
        m_moveAniGroup.addAnimation(dropAniGroup);
    if( mergeAniGroup->animationCount() > 0 )
        m_moveAniGroup.addAnimation(mergeAniGroup);
    if( postDropGroup->animationCount() > 0 )
        m_moveAniGroup.addAnimation(postDropGroup);

    if( m_moveAniGroup.animationCount() > 0 )
        m_moveAniGroup.start();
    else{
        if( isGameOver())
            animateGameOver();
    }

}

void GameBoard::onRestartGame()
{
    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {
        if( m_blocks[col][row]){
            delete(m_blocks[col][row]);
            m_blocks[col][row] = nullptr;
        }
    }
    if(m_info){
        delete(m_info);
        m_info = nullptr;
    } 


    srand((unsigned)time(NULL));
    setFocus();

    newNum();
    newNum();
    animateNewBlocks();

    m_score = 0;
    emit sigScoreChanged(m_score);

}

bool GameBoard::isGameOver()
{
    if( emptyCount() ) return false;

    for(int row=0; row<4; row++)
    for(int col=1; col<4; col++)
    {
        if( m_blocks[col][row] && m_blocks[col][row]->equals(m_blocks[col-1][row]))
            return false;
    }

    for(int col=0; col<4; col++)
    for(int row=1; row<4; row++)
    {
        if( m_blocks[col][row] && m_blocks[col][row]->equals(m_blocks[col][row-1]))
            return false;
    }

    return true;
}

void GameBoard::animateGameOver()
{
    if(m_info){
        delete(m_info);
        m_info = nullptr;
    }
    m_info = new GameInfoBoard(this);
    connect(m_info->ui_info->pb_restart, SIGNAL(clicked()), this, SLOT(onRestartGame()));
    m_info->show();
    QPropertyAnimation *ani = new QPropertyAnimation(m_info, "geometry");
    QRect rect(0, 0, width(), height());
    ani->setDuration(500);
    ani->setStartValue(QRect(rect.center(), QSize(0,0)));
    ani->setEndValue(rect);
    ani->start();
}

void GameBoard::finishMoveAnimate()
{
    if( newNum() )
    {
        animateNewBlocks();
    }
}

void GameBoard::animateNewBlocks()
{
    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {
        if( m_blocks[col][row] && m_blocks[col][row]->needAnimate() ){
                m_blocks[col][row]->endNewBlock();
                QPropertyAnimation *ani = new QPropertyAnimation(m_blocks[col][row], "geometry");
                QRect endRect = m_blocks[col][row]->geometry();
                ani->setDuration(200);
                ani->setStartValue(QRect(endRect.center(), QSize(0,0)));
                ani->setEndValue(endRect);
                ani->start();
        }
    }

}

void GameBoard::moveBlock(Loc src, Loc dst, QAnimationGroup *group)
{
    NumBlock *block = getBlock(src);
    QRect startRect = blockRect(src.col, src.row);
    QRect endRect = blockRect(dst.col, dst.row);
    setBlock(dst, block);
    setBlock(src, nullptr);
    // QPropertyAnimation *ani = new QPropertyAnimation(block, "geometry");
    // ani->setDuration(100);
    // ani->setStartValue(startRect);
    // ani->setEndValue(endRect);
    MoveAnimation *ani = new MoveAnimation(block, startRect, endRect);
    group->addAnimation(ani);
}

void GameBoard::mergeBlock(Loc src, Loc dst, QAnimationGroup *group)
{
    int score = getBlock(dst)->number();
    MergeAnimation *mergeAni = new MergeAnimation(getBlock(src), getBlock(dst));
    group->addAnimation(mergeAni);
    setBlock(src, nullptr);
    m_score += (score*2);
    emit sigScoreChanged(m_score);
}


void GameBoard::lineDrop(Loc loc0, Loc loc1, Loc loc2, Loc loc3, QAnimationGroup *group)
{
    if( getBlock(loc1) && (!getBlock(loc0)) ){
        moveBlock(loc1, loc0, group);
    }

    if( getBlock(loc2) && (!getBlock(loc1) ) ){
        if( !getBlock(loc0) ){
            moveBlock(loc2, loc0, group);
        }else{
            moveBlock(loc2, loc1, group);
        }
    }

    if( getBlock(loc3) && (!getBlock(loc2)) ){
        if( !getBlock(loc1) ){
            if( !getBlock(loc0) ){
                moveBlock(loc3, loc0, group);
            }else{
                moveBlock(loc3, loc1, group);
            }
        }else{
            moveBlock(loc3, loc2, group);
        }
    }
}

void GameBoard::lineMerge(Loc loc0, Loc loc1, Loc loc2, Loc loc3, QAnimationGroup *group)
{
    NumBlock *block0 = getBlock(loc0);
    NumBlock *block1 = getBlock(loc1);
    NumBlock *block2 = getBlock(loc2);
    NumBlock *block3 = getBlock(loc3);

    if( block0 && (block0->equals(block1)) ){
        mergeBlock(loc1, loc0, group);
        if(  block2 && block2->equals(block3) ){
            mergeBlock(loc3, loc2, group);
        }
    }else if( block1 && block1->equals(block2) ){
        mergeBlock(loc2, loc1, group);
    }else if( block2 && block2->equals(block3) ){
        mergeBlock(loc3, loc2, group);
    }    
}


int GameBoard::emptyCount()
{
    int count=0;
    for(int i=0; i<4; i++)
    for(int j=0; j<4; j++)
    {
        if( !m_blocks[i][j]) count ++;
    }
    return count;
}

bool GameBoard::newNum()
{
    int count = emptyCount();
    if( !count) return false;

    int n = (rand()%100)>70?4:2;

    int idx = rand()%count;
    int id=0;
    for(int row=0; row<4; row++)
    for(int col=0; col<4; col++)
    {
        if( !m_blocks[col][row] ){
            if( id == idx ){
                m_blocks[col][row] = new NumBlock(this, n);
                m_blocks[col][row]->setGeometry(blockRect(col, row));
                m_blocks[col][row]->show();
                return true;
            }
            id ++;
        }
    }

    return false;
}