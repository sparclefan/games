#include "numblock.h"

enum BlockNumberColor{
    Origin,
    Try,
    Selected,
    SetOn,
    Conflict,
    TrySelected,
};

const QColor conColors[] = {
    QColor(0x5d, 0x5d, 0x6d),   // Origin
    QColor(0xf2, 0xb1, 0x79),   // Try
    QColor(0x1d, 0xa5, 0x0f),   // Selected
    QColor(0x1c, 0x2c, 0x1c),   // SetOn
    QColor(0xff, 0x00, 0x00),   // Conflict
    QColor(0x4d, 0xf5, 0x3f)   // try & Selected
};


NumBlock::NumBlock(QWidget *parent, int num)
: QWidget(parent), m_num(num), m_flag(Empty), 
m_select(false), m_conflict(false)
{
    setWindowOpacity(0);
}

NumBlock::~NumBlock()
{

}

QColor NumBlock::numberColor()
{
    if( m_conflict)
        return conColors[BlockNumberColor::Conflict];

    if( m_select )
    {
        if( m_flag == Try) return conColors[BlockNumberColor::TrySelected];
        else return conColors[BlockNumberColor::Selected];
    }

    switch(m_flag)
    {
    case Origin:
        return conColors[BlockNumberColor::Origin];
    case SetOn:
        return conColors[BlockNumberColor::SetOn];
    case Try:
        return conColors[BlockNumberColor::Try];
    }

    return QColor(0,0,0);
}

QColor NumBlock::bgColor()
{
    // 100% transparent background
    return QColor(0,0,0,0);
}

void NumBlock::paintEvent(QPaintEvent *event)
{
    if(!m_num) return;

    QPainter painter(this);
    QFont serifFont("Times", 24, QFont::Bold);

    // if( m_flag == Tool)
    //     painter.setBrush(QColor(0xb0, 0xad, 0xa0));

    painter.setBrush(bgColor());
    painter.drawRoundedRect(rect(), 20.0, 20.0, Qt::RelativeSize);

    painter.setFont(serifFont);

    // int colorIdx = colorId();
    // static_cast<int>(m_flag);
    // if( m_select && (m_flag != Conflict) ){
    //     if (m_flag != Try ) colorIdx = static_cast<int>(BlockFlag::Selected);
    //     else  colorIdx = static_cast<int>(BlockFlag::TrySelected);
    // }
    // else if( m_candidate) colorIdx = static_cast<int>(BlockFlag::Candidate);

    painter.setPen(numberColor());

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

