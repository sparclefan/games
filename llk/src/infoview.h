#ifndef __LLK_INFOVIEW_H_SPARCLE_2020_10_09
#define __LLK_INFOVIEW_H_SPARCLE_2020_10_09

#include "game.h"
#include <QWidget>

class InfoView : public QWidget
{
    Q_OBJECT
public:
    InfoView(QWidget *parent):QWidget(parent){};
    ~InfoView(){};    

    void setGame(CGameLLK *pGame);

public slots:
    void infoChange();
    void timeChange();
    void scoreChange();
    void repaintAll(){repaint();};

protected:
   virtual void paintEvent(QPaintEvent *event);
//    virtual void resizeEvent(QResizeEvent *event);

private:
    CGameLLK *m_pGame;

};


#endif //__LLK_INFOVIEW_H_SPARCLE_2020_10_09