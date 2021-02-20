#pragma execution_character_set("utf-8")

#include "infoview.h"
#include <QPainter>

#define INFO_MARGIN_TOP 30
#define INFO_MARGIN_LEFT 10
#define INFO_MARGIN_RIGHT 30
#define INFO_SPACE 20
#define INFO_TIMER_HEIGHT 30

void InfoView::setGame(CGameLLK *pGame)
{
    m_pGame=pGame;
    connect(m_pGame,SIGNAL(sigRepaint()),this,SLOT(repaintAll()));
    connect(m_pGame,SIGNAL(sigScoreChange()),this,SLOT(scoreChange()));
    connect(m_pGame,SIGNAL(sigInfoChange()),this,SLOT(infoChange()));
    connect(m_pGame,SIGNAL(sigUpdateTimer()),this,SLOT(timeChange()));
}

void InfoView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();

    int iWidth = width();
    BOOL assignParm = FALSE;
    if( m_width != iWidth ){
        m_width = iWidth;
        assignParm = TRUE;
    }

    QRect boundRect, boundRect2;
    painter.fillRect(0,0,width(),height(), QColor(192,128,64));
    painter.setFont(QFont("华文彩云",24));
    painter.setPen(QColor(128,128,0));
    painter.drawText(INFO_MARGIN_LEFT, INFO_MARGIN_TOP, width(),100, 
            Qt::AlignLeft|Qt::AlignTop, m_pGame->gameClassInfo(),&boundRect);
    if( m_scorePos==0 || assignParm ){
        m_scorePos = boundRect.right() + INFO_SPACE;
    }

    painter.setFont(QFont("楷体",28));
    painter.setPen(QColor(0,0,192));
    QString score=QString("得分：%1").arg(m_pGame->score(),-5,10,QLatin1Char(' '));
    painter.drawText(m_scorePos, INFO_MARGIN_TOP, width(),100, 
            Qt::AlignLeft|Qt::AlignTop, score, &boundRect);


    if( m_timerPos==0 || assignParm ){
        m_timerPos = boundRect.right() + INFO_SPACE;
    }

    if( m_timerWidth == 0 || assignParm ){
        m_timerWidth = m_width/4;
        // m_timerWidth = boundRect2.left()-m_timerPos- INFO_SPACE;
    }
    // int tmWidth = boundRect2.left()-m_timerPos- INFO_SPACE;   //width()/4;
    int tmPosY = INFO_MARGIN_TOP+(boundRect.height()-INFO_TIMER_HEIGHT)/2;
    painter.drawRect(m_timerPos, tmPosY, m_timerWidth, INFO_TIMER_HEIGHT);
    int orgTm = m_pGame->getOrgTimer();
    if(orgTm>0){        
        int tmLen = m_pGame->getTimer()*m_timerWidth/orgTm;
        if( m_pGame->isPause()){
            painter.fillRect(m_timerPos+1, tmPosY+1, tmLen, INFO_TIMER_HEIGHT-2, QColor(0,0,192));
        }else{
            painter.fillRect(m_timerPos+1, tmPosY+1, tmLen, INFO_TIMER_HEIGHT-2, QColor(192,0,0));
        }
    }

    if(m_infoPos==0 || assignParm){
        m_infoPos = m_timerPos + m_timerWidth + INFO_SPACE;
    }

    painter.setFont(QFont("楷体",20));
    painter.setPen(QColor(255,255,0));
    painter.drawText(m_infoPos, INFO_MARGIN_TOP, width()-m_infoPos-INFO_MARGIN_RIGHT,100, 
             Qt::AlignLeft|Qt::AlignTop, m_pGame->gameMessage(), &boundRect2);


    // if( m_pGame->isPause()){
    //     painter.setFont(QFont("宋体",20));
    //     painter.setPen(QColor(192,0,0));
    //     painter.drawText(50, 180, "暂停");
    // }

    // painter.setFont(QFont("楷体",20));
    // painter.setPen(QColor(255,255,0));
    // painter.drawText(500, 100, m_pGame->gameMessage());

    // painter.setFont(QFont("楷体",28));
    // painter.setPen(QColor(0,0,192));
    // QString score=QString("得分：%1").arg(m_pGame->score());
    // painter.drawText(800, 180, score);

    painter.restore();

}

void InfoView::infoChange()
{
    repaint(QRect(0,0,width(), 200));
}

void InfoView::timeChange()
{
    // int tmWidth = width()-m_timerPos-20;
    repaint(QRect(m_timerPos,INFO_MARGIN_TOP, m_timerWidth+2, 100));
}

void InfoView::scoreChange()
{
    repaint(QRect(m_scorePos, INFO_MARGIN_TOP, 500, 100));
}