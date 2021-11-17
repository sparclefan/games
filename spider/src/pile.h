#ifndef __SPIDER_PILE_H_SPARCLE_2020_10_14
#define __SPIDER_PILE_H_SPARCLE_2020_10_14

#include "card.h"
#include <QObject>
#include <QPropertyAnimation>

class GameStorage;

class Pile : public QObject
{
    Q_OBJECT
public:
    Pile();
    ~Pile();

    void move(int dx, int dy);
    void show(BOOL visible=TRUE);
    void addCard(Card *card);
    void addCards(vector<Card *> &cards);
    void popCards(Card *fromCard, vector<Card *> &cardlist);
    void removeAll();
    Card *popTail();
    void getTailCards(int num, vector<Card *> &cardlist);
    Card * getTailCardById(int num);
    Card * getTailCard();
    Card * getHeadCard();

    void debug();

    virtual QPoint getNextCardPos(){return QPoint(m_ox, m_oy);};

    inline int count() const {return m_cards.size();};
    inline int x() const {return m_ox;};
    inline vector<Card *> cards() const & {return m_cards;};

protected:
    virtual void reArrange(){};
    virtual void addCardImpl(Card *card)=0;

    vector<Card *> m_cards;
    int m_ox;
    int m_oy;
};

//为支持 undo 功能，将所有移牌操作归纳为从一摞移到另一摞，undo时只需反向移动即可
class Op
{
friend class GameStorage;
public:
    Op(Pile *pilefrom, Pile *pileto, vector<Card *> &cardlist, BOOL revlast);
	Op(Pile *pilefrom, Pile *pileto, vector<Card *> &cardlist, BOOL revlast, BOOL revOrder);

    void undo();
    void redo();

    inline void setDeliver(){m_isDeliver = TRUE;}

private:
    Pile *m_pilefrom;
    Pile *m_pileto;
    vector<Card *> m_cards;
    BOOL m_revlast;
	BOOL m_revOrder;
    BOOL m_isDeliver = FALSE;
};

//单张牌移牌动画，主要为了关联finished消息, 动画结束后将 牌 加到目标牌摞上
class CardAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    CardAnimation(Card *card, Pile *toPile, BOOL face);

public slots:
    void endAnimate();

private:
    Card * m_card;
    Pile * m_toPile;
    BOOL m_face;
};

#endif //__SPIDER_PILE_H_SPARCLE_2020_10_14