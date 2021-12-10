#ifndef __TOOLPANE_H_Sparcle_2021_12_03
#define __TOOLPANE_H_Sparcle_2021_12_03

#include <QFrame>
#include "numblock.h"

class ToolNum : public NumBlock
{
    Q_OBJECT
public:
    ToolNum(QWidget *parent, int num);
    ~ToolNum();

    inline void resetCount(){ m_setOnCount=0; setEnabled(true); };
    inline void recommend(bool bRecommend) { m_recommend = bRecommend; };
    inline bool isRecommend() { return m_recommend; };
    inline void restoreNumber(){ m_setOnCount--; setEnabled(true); };
    bool setOn();

signals:
    void numberClicked(int num);

protected:
    virtual QColor numberColor();
    virtual QColor bgColor();
    virtual void mousePressEvent(QMouseEvent *event);

private:
    bool m_recommend;
    int m_setOnCount;
};

class ToolPane : public QFrame
{
    Q_OBJECT
public:
    ToolPane(QWidget *parent);
    ~ToolPane();

    void resetAll();
    bool recommend(int num);
    bool setOn(int num);
    bool restoreNumber(int num);
    bool canSet(int num);

signals:
    void numberClicked(int num);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private slots:
    void onNumberClicked(int num);

private:
    ToolNum *m_blocks[9];
    ToolNum *m_recommend;

};

#endif //__TOOLPANE_H_Sparcle_2021_12_03