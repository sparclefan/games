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

    void merge();

    inline bool equals(NumBlock *block) { return (block && (m_num==block->m_num));};
    inline bool needAnimate() const{ return m_newblock;};
    inline void endNewBlock() { m_newblock = false; };

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QColor getColor();
    int m_num;
    bool m_newblock;
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


// 合并动画
class MergeAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    MergeAnimation(NumBlock *blockMerge, NumBlock *blockDst);
    ~MergeAnimation();

public slots:
    void endAnimate();

private:
    NumBlock *m_blockMerge;
    NumBlock *m_blockDst;

};

#endif //__numblock_h_Sparcle_2021_11_12