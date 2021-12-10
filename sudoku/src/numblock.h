#ifndef __numblock_h_Sparcle_2021_11_12
#define __numblock_h_Sparcle_2021_11_12

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>

class NumBlock : public QWidget
{
    Q_OBJECT
public:
    NumBlock(QWidget *parent, int num);
    ~NumBlock();

    enum BlockFlag {
        Empty,
        Origin,
        SetOn,
        Try
    };

    inline bool equals(NumBlock *block) { return (block && (m_num==block->m_num));};
    inline void setNumber(int number) { m_num = number; };
    inline int number() const { return m_num;};
    inline void setFlag(BlockFlag flag) { m_flag = flag; };
    inline void select(bool bSelect) {m_select = bSelect; };
    inline bool isTry() { return m_flag==Try; };
    inline void conflict(bool bConflict){ m_conflict = bConflict;};
    inline bool isConflict() { return m_conflict;};
    inline bool empty() { return m_num==0; };
    inline bool isOrigin() { return m_flag == Origin;};

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual QColor numberColor();
    virtual QColor bgColor();

private:
    BlockFlag m_flag;
    int m_num;
    bool m_select;
    bool m_conflict;
};


// 移动动画  为了确保每次移动能完全到位，监定结束信号
class MoveAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    MoveAnimation(NumBlock *block, QRect &startRect, QRect &endRect);
    ~MoveAnimation();

public slots:
    void endAnimate();

private:
    NumBlock *m_block;
    QRect m_endRect;
    bool m_endflag;
};


#endif //__numblock_h_Sparcle_2021_11_12