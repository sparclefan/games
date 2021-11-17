#pragma execution_character_set("utf-8")

#ifndef __SPIDER_CARD_H_SPARCLE_2020_10_13
#define __SPIDER_CARD_H_SPARCLE_2020_10_13

#include <QLabel>
#include <QPoint>
#include <QMouseEvent>
#include <QPixmap>
#include <vector>
#include <QDebug>

using namespace std;

#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#define DEFAULT_CARD_WIDTH 200
#define DEFAULT_CARD_HEIGHT 300

// 花色定义，草花、方块、红心、黑桃、王
enum Suit
{
    Club,
    Diamond,
    Heart,
    Spade,
    Joker
};

class Card :public QLabel
{
    Q_OBJECT
public:
    Card(Suit suit, int rank, QWidget *parent=NULL, BOOL face=TRUE);
    ~Card();

    //恢复原始大小，动画时有可能改变大小
    void restoreOrgSize();
    void setFace(BOOL up=TRUE);
    void setFront(){raise();};

    inline Suit suit() const {return m_suit;};
    inline int rank() const {return m_rank;};
    inline BOOL isFaceOn() const {return m_up;};
    inline int id() const {return m_id;};
    inline void setId(int id) { m_id = id; };

    //###############各种发牌静态方法#####################

    //随机生成一张牌， parent:需要显示牌的控件，joker：是否包含大小王
    static Card *randomCard(QWidget *parent=NULL, BOOL joker=TRUE, BOOL seed=TRUE);
    
    //生成一整套指定花色的牌
    static void createSuit(vector<Card *> &cardlist, Suit suit=Club, QWidget *parent=NULL, BOOL face=FALSE);

    //生成一整副牌, parent:需要显示牌的控件，joker：是否包含大小王
    static void createDeck(vector<Card *> &cardlist, QWidget *parent=NULL, BOOL joker=TRUE, BOOL face=FALSE);

    //洗牌
    static void shuffle(vector<Card *>* cardlist);

signals:
    void sigSelected(Card *card, QPoint pt);
    void sigDrag(Card *card, QPoint pt);
    void sigDrop(QPoint pt);
    void sigClicked(Card *card);
    void sigDoubleClicked(Card *card);
    void sigEnter(Card *card);
    void sigLeave(Card *card);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    BOOL check();  //校验花色和牌点

    QPixmap m_bkimg;
    QPixmap m_img;
    Suit m_suit;
    int m_rank;
    BOOL m_up;

    // BOOL m_bInit;
    BOOL m_bInDrag;

    int m_orgWidth;
    int m_orgHeight;

    int m_id;  //唯一标识
};


#endif //__SPIDER_CARD_H_SPARCLE_2020_10_13