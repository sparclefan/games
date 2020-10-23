#pragma execution_character_set("utf-8")

#include "pile.h"
#include <QDebug>

Pile::Pile()
:m_ox(0),m_oy(0)
{

}

Pile::~Pile()
{

}

void Pile::debug()
{
    qDebug("Pile pos: %d  %d cards: %d", m_ox, m_oy, m_cards.size());
    for(auto it=m_cards.cbegin(); it != m_cards.cend(); it++)
    {
        Card *card = *it;
        qDebug()<<card->suit()<<"-"<<card->rank()<<"  pos: "<<card->x()<<","<<card->y()<<"  ";
    }
}

void Pile::move(int dx, int dy)
{
    m_ox = dx;
    m_oy = dy;
    reArrange();
}

void Pile::show(BOOL visible)
{
    for(auto it=m_cards.cbegin(); it != m_cards.cend(); it++)
    {
        (*it)->setVisible(visible);
    }
}

void Pile::addCard(Card *card)
{
    addCardImpl(card);
    reArrange();
}

void Pile::addCards(vector<Card *> &cards)
{
    for(auto it=cards.cbegin(); it != cards.cend(); it++)
    {
        addCard(*it);
    }
}

void Pile::removeAll()
{
    vector<Card *> tmplist;
    m_cards.swap(tmplist);
}

void Pile::popCards(Card *fromCard, vector<Card *> &cardlist)
{
    // if( idex>=m_cards.size() || idex<0 ){
    //     return NULL;
    // }
    auto it= find(m_cards.cbegin(), m_cards.cend(), fromCard);
    if(it==m_cards.cend()){
        // qDebug()<<"can not find card";
        return;
    }

    // qDebug()<<"find card";

    for(; it != m_cards.cend(); )
    {
        // (*it)->disconnect();
        // (*it)->disconnect(SIGNAL(sigClicked(Card *, QPoint)));
        // (*it)->disconnect(SIGNAL(sigEnter(Card *)));
        // (*it)->disconnect(SIGNAL(sigLeave(Card *)));

        cardlist.push_back(*it);
        m_cards.erase(it);
    }

    reArrange();
}

Card *Pile::popTail()
{
    if(m_cards.empty())
        return NULL;

    Card *card = m_cards.back();
    m_cards.pop_back();
    reArrange();
    return card;
}

void Pile::getTailCards(int num, vector<Card *> &cardlist)
{
    if( num>m_cards.size())
        return;

    int idex = m_cards.size()-num;
    for(auto it=m_cards.cbegin()+idex; it != m_cards.cend(); it++)
    {
        cardlist.push_back(*it);
    }
}

Card * Pile::getTailCardById(int num)
{
    if( num>m_cards.size())
        return NULL;

    int idex = m_cards.size()-num;
    auto it = m_cards.cbegin()+idex;
    return (*it);
}

Card * Pile::getTailCard()
{
    if(m_cards.empty())
        return NULL;
    return m_cards.back();
}

Card * Pile::getHeadCard()
{
    if(m_cards.empty())
        return NULL;
    return m_cards.front();
}

Op::Op(Pile *pilefrom, Pile *pileto, vector<Card *> &cardlist, BOOL revlast)
:m_pilefrom(pilefrom),m_pileto(pileto),m_revlast(revlast)
{
    m_cards = cardlist;
}

void Op::undo()
{
    if(m_revlast){
        Card *card = m_pilefrom->getTailCard();
        if(card != NULL)
            card->setFace(FALSE);
    }

    // qDebug()<<"===========undo========";
    // m_pileto->debug();
    Card *head = m_cards.front();

    // qDebug()<<"try to find card: "<<head->suit()<<"-"<<head->rank();

    vector<Card *> cardlist;
    m_pileto->popCards(head, cardlist);
    m_pilefrom->addCards(m_cards);
    // qDebug()<<"--------------------------";
}

CardAnimation::CardAnimation(Card *card, Pile *toPile, BOOL face)
:QPropertyAnimation(card, "geometry"), m_card(card), m_toPile(toPile), m_face(face)
{
    connect(this, SIGNAL(finished()), this, SLOT(endAnimate()));
}

void CardAnimation::endAnimate()
{
    m_card->setFront();
    m_card->setFace(m_face);
    if(m_toPile){
        m_toPile->addCard(m_card);
    }
}