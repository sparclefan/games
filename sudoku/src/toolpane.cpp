#include "toolpane.h"
#include <QResizeEvent>
#include <QColor>

#define TOOL_SPACE 15

    //---------- toolpane color ----------
const  QColor conColorToolDisable(0xee, 0xee, 0xee);   // ToolDisable
const  QColor conColorToolEnable(0x1c, 0x1c, 0x1c);    // ToolEnable
const  QColor conColorToolRecommend(0xee, 0xee, 0x1c);    // ToolRecommend

ToolNum::ToolNum(QWidget *parent, int num)
: NumBlock(parent, num), m_recommend(false), m_setOnCount(0)
{
    setEnabled(false);
}

ToolNum::~ToolNum()
{

}

QColor ToolNum::numberColor()
{
    if( !isEnabled() ) return conColorToolDisable;

    if(m_recommend) return conColorToolRecommend;

    return conColorToolEnable;
}

QColor ToolNum::bgColor()
{
    return QColor(0xb0, 0xad, 0xa0);
}

bool ToolNum::setOn()
{
    if(m_setOnCount>=9){
        setEnabled(false);
        return false;
    }

    m_setOnCount++;
    if(m_setOnCount==9){
        setEnabled(false);
    }
    return true;
}

void ToolNum::mousePressEvent(QMouseEvent *event)
{
    emit numberClicked(number());
}


////////=============================================================

ToolPane::ToolPane(QWidget *parent)
:QFrame(parent), m_recommend(nullptr)
{
    for(int i=0; i<9; i++)
    {
        m_blocks[i] = new ToolNum(this, i+1);
        connect(m_blocks[i], SIGNAL(numberClicked(int)), this, SLOT(onNumberClicked(int)));
    }
}

ToolPane::~ToolPane()
{

}

void ToolPane::resizeEvent(QResizeEvent *event)
{
    int reserv = 0;
    QSize size = event->size();
    int blockWidth = (size.width() - TOOL_SPACE*10 - reserv )/9;
    int blockHeight = (size.height() - TOOL_SPACE*2);

    for(int col=0; col<9; col++)
    {
            QRect rect = QRect(col*(blockWidth+TOOL_SPACE), TOOL_SPACE, blockWidth, blockHeight);
            m_blocks[col]->setGeometry(rect);
    }
}

void ToolPane::resetAll()
{
    for(int i=0; i<9; i++)
        m_blocks[i]->resetCount();

    recommend(-1);
}

bool ToolPane::recommend(int num)
{
    if( m_recommend ){
        m_recommend->recommend(false);
        m_recommend->update();
        m_recommend = nullptr;
    }

    if( num<1 || num>9 ) return false;
    if( !m_blocks[num-1]->isEnabled() ) return false;

    m_recommend = m_blocks[num-1];
    m_recommend->recommend(true);
    m_recommend->update();

    return true;   
}

bool ToolPane::setOn(int num)
{
    if( num<1 || num>9 ) return false;
    return m_blocks[num-1]->setOn();
}

bool ToolPane::restoreNumber(int num)
{
    if( num<1 || num>9 ) return false;
    m_blocks[num-1]->restoreNumber();
    m_blocks[num-1]->update();
    return true;
}

void ToolPane::onNumberClicked(int num)
{
    emit numberClicked(num);
}

bool ToolPane::canSet(int num)
{
    return m_blocks[num-1]->isEnabled();
}
