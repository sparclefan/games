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

    enum BlockFlag{
        Standby,
        Origin,
        Try,
        Selected,
        SetOn,
        Candidate,
        Conflict
    };

    inline bool equals(NumBlock *block) { return (block && (m_num==block->m_num));};
    // inline bool needAnimate() const{ return m_newblock;};
    // inline void endNewBlock() { m_newblock = false; };
    inline int number() const { return m_num;};
    inline void setFlag(BlockFlag flag) { m_flag = flag; };
    inline void select(bool bSelect) {m_select = bSelect; };
    inline void candidate(bool bCandidate) { m_candidate = bCandidate;};
    inline bool isTry() { return m_flag==Try; };
    inline bool isConflict() { return m_flag==Conflict;};

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    BlockFlag m_flag;
    int m_num;
    bool m_select;
    bool m_candidate;
    // bool m_newblock;
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