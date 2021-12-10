#include "infopane.h"
#include <QPainter>

InfoPane::InfoPane(QWidget *parent)
:QFrame(parent), m_timePassed(0)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()) );
}

InfoPane::~InfoPane()
{

}

void InfoPane::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFont serifFont("Times", 24, QFont::Bold);
    painter.setFont(serifFont);

    int hour = m_timePassed/3600;
    int min = (m_timePassed%3600)/60;
    int sec = m_timePassed%60;

    QString tmStr = QString("%1:%2:%3").arg(hour).arg(min).arg(sec);

    QRect rectFrame = rect();
    QRect rectTm = QRect(rectFrame.center().x(), 0, rectFrame.width()/2, rectFrame.height());

    painter.drawText(rectTm, Qt::AlignCenter, tmStr);

}

void InfoPane::resizeEvent(QResizeEvent *event)
{

}

void InfoPane::onTimer()
{
    m_timePassed ++;
    repaint();
}

void InfoPane::startTimer()
{
    m_timePassed = 0;
    m_timer->start(1000);
}

void InfoPane::stopTimer()
{
    m_timer->stop();
}