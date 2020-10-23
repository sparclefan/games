#ifndef __TERRIS_TERRISVIEW_H_SPARCLE_2020_10_22
#define __TERRIS_TERRISVIEW_H_SPARCLE_2020_10_22

#include <QWidget>
#include "terrisgame.h"
#include <QPainter>
#include <QKeyEvent>

class TerrisView : public QWidget
{
    Q_OBJECT
public:
    TerrisView(QWidget *parent);
    ~TerrisView();    
    inline TerrisGame *game() { return &m_game;};

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    TerrisGame m_game;
};

#endif //__TERRIS_TERRISVIEW_H_SPARCLE_2020_10_22