#include "numblock.h"

const QColor conColors[] = {
    QColor(0xff, 0xff, 0xff),   // Standby
    QColor(0x5d, 0x5d, 0x6d),   // Origin
    QColor(0xf2, 0xb1, 0x79),   // Try
    QColor(0x3d, 0xe5, 0x2f),   // Selected
    QColor(0x1c, 0x2c, 0x1c),   // SetOn
    QColor(0xee, 0xee, 0x1c),   // Candidate
    QColor(0xff, 0x00, 0x00)    // Conflict
};


NumBlock::NumBlock(QWidget *parent, int num)
: QWidget(parent), m_num(num), m_flag(Standby), 
m_select(false), m_candidate(false)
{
    setWindowOpacity(0);
}

NumBlock::~NumBlock()
{

}

void NumBlock::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFont serifFont("Times", 24, QFont::Bold);

    if( m_flag == Standby)
        painter.setBrush(QColor(0xb0, 0xad, 0xa0));

    painter.drawRoundedRect(rect(), 20.0, 20.0, Qt::RelativeSize);

    painter.setFont(serifFont);

    int colorIdx = static_cast<int>(m_flag);
    if( m_select && (m_flag != Conflict) && (m_flag != Try) ) colorIdx = static_cast<int>(BlockFlag::Selected);
    else if( m_candidate) colorIdx = static_cast<int>(BlockFlag::Candidate);

    painter.setPen(conColors[colorIdx]);

    painter.drawText(rect(), Qt::AlignCenter, QString("%1").arg(m_num));
}

///==============================================================
MoveAnimation::MoveAnimation(NumBlock *block, QRect &startRect, QRect &endRect)
:QPropertyAnimation(block, "geometry"), m_block(block), m_endRect(endRect), m_endflag(false)
{
    setDuration(300);
    setStartValue(startRect);
    setEndValue(endRect);
    connect(this, SIGNAL(finished()), this, SLOT(endAnimate()));
}

void MoveAnimation::endAnimate()
{
    m_endflag = true;
}

MoveAnimation::~MoveAnimation()
{
    if( !m_endflag){
        m_block->setGeometry(m_endRect);
    }
}

