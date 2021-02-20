#ifndef __SPIDER_SPIDERPILES_H_SPARCLE_2020_10_14
#define __SPIDER_SPIDERPILES_H_SPARCLE_2020_10_14

#include "pile.h"

#define DEFAULT_LINE_SPACE 80
#define SHOW_SUIT_SPACE 70

class WorkPile : public Pile
{
    Q_OBJECT
public:
    WorkPile();
    ~WorkPile();

    virtual QPoint getNextCardPos();
    BOOL testSuccessSuit();
    void move(int dx, int dy);

public slots:
    void onCardSelect(Card *card, QPoint pt);
    void enterEvent(Card *card);
    void leaveEvent(Card *card);
    void cardDoubleClicked(Card *card);
    void cardClicked(Card *card);

signals:
    void sigDragStart(Pile *, vector<Card *>*, QPoint);
    void sigAutoMoveCards(Pile *, Card *);

protected:
    virtual void reArrange();
    virtual void addCardImpl(Card *card);

private:
    BOOL canDrag(Card *card);

    int m_nexty;
    int m_linespace;

};

class DragPile : public Pile
{
    Q_OBJECT
public:
    DragPile();
    ~DragPile();

    void startDrag(vector<Card *> &cardlist, QPoint pt);

public slots:
    void onDrag(Card *card, QPoint pt);

protected:
    virtual void reArrange();
    virtual void addCardImpl(Card *card);

private:
    QPoint m_startPos;
    int dragStartX;
    int dragStartY;

};

class OrgPile : public Pile
{
    Q_OBJECT
public:
    OrgPile(){};
    ~OrgPile(){};

    virtual QPoint getNextCardPos();
public slots:
    void onClicked(){emit sigDeliver();};

signals:
    void sigDeliver();

protected:
    virtual void addCardImpl(Card *card);

};

class SucessPile : public Pile
{
public:
    QPoint getNextCardPos(){ return QPoint(m_ox, m_oy);};

protected:
    virtual void addCardImpl(Card *card);
};


#endif //__SPIDER_SPIDERPILES_H_SPARCLE_2020_10_14