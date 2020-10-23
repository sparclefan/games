#ifndef __LLK_GAMEVIEW_H_SPARCLE_2020_09_29
#define __LLK_GAMEVIEW_H_SPARCLE_2020_09_29

#include "game.h"
#include <QWidget>

#define ICONS_NUM   35
#define ICONS_W 43
#define ICONS_H 51
#define DEFAULT_HORIZON_MARGIN 100
#define DEFAULT_VERTICAL_MARGIN 100
#define MAX_ICON_WIDTH 180
#define MAX_ICON_HEIGTH 180

// #define MESSAGE_BOARD_HEIGHT 200

struct DrawInfo
{
    int totalWidth;
    int totalHeight;
    int colnum;
    int rownum;
    int margin_w;
    int margin_h;
    int colWidth;
    int rowHeight;
};

class GameView : public QWidget
{
    Q_OBJECT
public:
    GameView(QWidget *parent);
    ~GameView();    

    CGameLLK *game() {return &m_game;};

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

public slots:
    void onActionSimple();
    void onActionNormal();
    void onActionHard();

    void onIconSelect(int idx);
    void onDropLine();
    void repaintIcon(int idx);
    void repaintIconboard();

private:
    BOOL caculateDrawInfo(DrawInfo &info);
    int hitTest(QPoint pt);
    void drawSelect(int idx);
    void preparePixmap();

private:
    QPixmap *m_pMsgBoard;
    QPixmap *m_pPixmap;
    QImage m_imgs[ICONS_NUM];
    CGameLLK m_game;
    int m_iTip1;
    int m_iTip2;

};


#endif //__LLK_GAMEVIEW_H_SPARCLE_2020_09_29