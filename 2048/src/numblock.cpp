#include "numblock.h"

const QColor conColors[] = {
    QColor(0xee, 0xe4, 0xda),   // 2
    QColor(0xed, 0xe0, 0xc8),   // 4
    QColor(0xf2, 0xb1, 0x79),   // 8
    QColor(0xf5, 0x95, 0x63),   // 16
    QColor(0xf6, 0x7c, 0x5f),   // 32
    QColor(0xf6, 0x5e, 0x3b),   // 64
    QColor(0xed, 0xcf, 0x72),   // 128
    QColor(0xed, 0xcc, 0x61),   // 256
    QColor(0xed, 0xc8, 0x50),   // 512
    QColor(0xed, 0xc5, 0x3f),   // 1024
    QColor(0xed, 0xc2, 0x2e),   // 2048
    QColor(0xed, 0x70, 0x2e),   // 4096
    QColor(0xed, 0x4c, 0x2e)    // 8192
};

int g_defColorNum = sizeof(conColors)/sizeof(QColor);

NumBlock::NumBlock(QWidget *parent, int num)
: QWidget(parent), m_num(num), m_newblock(true)
{
    setWindowOpacity(0);
}

NumBlock::~NumBlock()
{

}

QColor NumBlock::getColor()
{
    if(m_num==0) return QColor(0xfa, 0xf8, 0xef, 0.2*255);

    for(int idx=0; idx<g_defColorNum; idx++){
        if( m_num == (2<<idx))
            return conColors[idx];
    }

    return Qt::red;    
}

void NumBlock::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFont serifFont("Times", 24, QFont::Bold);

    QColor color = getColor();

    painter.setPen(color);
    painter.setBrush(QBrush(color));

    // painter.fillRect(rect(), getColor());
    painter.drawRoundedRect(rect(), 20.0, 20.0, Qt::RelativeSize);

    if( m_num >= 8)
        painter.setPen(QColor(0xf9, 0xf6, 0xf2));
    else
        painter.setPen(QColor(0x77, 0x6e, 0x65));
    painter.setFont(serifFont);

    painter.drawText(rect(), Qt::AlignCenter, QString("%1").arg(m_num));

}

void NumBlock::merge()
{
    m_num *= 2;
    repaint();
}

///==============================================================
MoveAnimation::MoveAnimation(NumBlock *block, QRect &startRect, QRect &endRect)
:QPropertyAnimation(block, "geometry"), m_block(block), m_endRect(endRect), m_endflag(false)
{
    setDuration(100);
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


MergeAnimation::MergeAnimation(NumBlock *blockMerge, NumBlock *blockDst)
:QPropertyAnimation(blockMerge, "geometry"), m_blockMerge(blockMerge), m_blockDst(blockDst)
{
    setDuration(20);
    QRect rect = blockMerge->geometry();
    setStartValue(rect);
    setEndValue(QRect(rect.center(), QSize(0,0)));
    connect(this, SIGNAL(finished()), this, SLOT(endAnimate()));
    // printf("merge ...\n");
}

void MergeAnimation::endAnimate()
{
    m_blockDst->merge();
    delete(m_blockMerge);
    m_blockMerge = nullptr;
    // printf("delete merge\n");
}

MergeAnimation::~MergeAnimation()
{
    // Sometimes if play too quick, animation is clear before it's end
    if(m_blockMerge){
        m_blockDst->merge();
        delete(m_blockMerge);
        m_blockMerge = nullptr;
    } 
    // printf("--distruct\n");
}