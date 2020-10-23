#pragma execution_character_set("utf-8")

#include "infoview.h"
#include <QPainter>

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

    painter.fillRect(0,0,width(),height(), QColor(192,128,64));
    painter.setFont(QFont("华文彩云",28));
    painter.setPen(QColor(128,128,0));
    painter.drawText(10,100,m_pGame->gameClassInfo());

    painter.setFont(QFont("楷体",20));
    painter.setPen(QColor(255,255,0));
    painter.drawText(500, 100, m_pGame->gameMessage());

    painter.setFont(QFont("楷体",28));
    painter.setPen(QColor(0,0,192));
    QString score=QString("得分：%1").arg(m_pGame->score());
    painter.drawText(800, 180, score);

    int tmWidth = width()/2;
    int orgTm = m_pGame->getOrgTimer();
    if(orgTm>0){
        painter.drawRect(100, 230, tmWidth, 30);
        int tmLen = m_pGame->getTimer()*tmWidth/orgTm;
        painter.fillRect(101, 231, tmLen, 28, QColor(192,0,0));
    }


    painter.restore();

}

void InfoView::infoChange()
{
    repaint(QRect(0,0,width(), 200));
}

void InfoView::timeChange()
{
    int tmWidth = width()/2;
    repaint(QRect(99,229, tmWidth+2, 32));
}

void InfoView::scoreChange()
{
    repaint(QRect(799, 100, 500, 150));
}