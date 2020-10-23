#pragma execution_character_set("utf-8")

#include "spiderpiles.h"
#include <algorithm>

WorkPile::WorkPile()
:Pile(), m_nexty(0), m_linespace(DEFAULT_LINE_SPACE)
{

}

WorkPile::~WorkPile()
{

}

void WorkPile::move(int dx, int dy)
{
    m_ox = dx;
    m_oy = dy;
    m_nexty = dy;
    reArrange();
}

void WorkPile::reArrange()
{
    if(m_cards.empty())
        return;

    Card *card = *(m_cards.cbegin());
    int frameh = card->parentWidget()->height();
    int cardh = card->height();
    m_nexty = m_oy;

    BOOL calcsp = FALSE;
    int idx = 0;
    for(auto it=m_cards.cbegin(); it!=m_cards.cend(); it++)
    {
        card = *it;
        card->move(m_ox, m_nexty);
        if(card->isFaceOn()){
            if(!calcsp){
                m_linespace = (frameh-cardh-m_nexty)/(m_cards.size()-idx);
                if(m_linespace>DEFAULT_LINE_SPACE)
                    m_linespace = DEFAULT_LINE_SPACE;
                calcsp = TRUE;
            }
            m_nexty += m_linespace;
        }else{
            m_nexty += 5;
        }
        idx ++;
    }

}

QPoint WorkPile::getNextCardPos()
{
    if(m_cards.empty())
        return QPoint(m_ox, m_oy);
    else
        return QPoint(m_ox, m_nexty);
}

void WorkPile::addCardImpl(Card *card)
{
    card->move(m_ox, m_oy);
    card->setFront();
    // card->disconnect(SIGNAL(sigSelected(Card *, QPoint)));
    card->disconnect();
    connect(card,SIGNAL(sigSelected(Card *, QPoint)), this, SLOT(onCardSelect(Card *, QPoint)));
    connect(card,SIGNAL(sigEnter(Card *)), this, SLOT(enterEvent(Card *)));
    connect(card,SIGNAL(sigLeave(Card *)), this, SLOT(leaveEvent(Card *)));
    // connect(card,SIGNAL(sigDoubleClicked(Card *)), this, SLOT(cardDoubleClicked(Card *)));
    connect(card,SIGNAL(sigClicked(Card *)), this, SLOT(cardClicked(Card *)));
    m_cards.push_back(card);
}

BOOL WorkPile::canDrag(Card *card)
{
    if( !card->isFaceOn())
        return FALSE;
    auto it= find(m_cards.cbegin(), m_cards.cend(), card);
    if(it==m_cards.cend()){
        return FALSE;
    }
    for(; (it !=m_cards.cend())&&( (it+1) != m_cards.cend() ); it++){
        Card *nextCard = *(it+1);
        if( nextCard->suit() != (*it)->suit() )
            return FALSE;
        if( nextCard->rank()+1 != (*it)->rank())
            return FALSE;
    }
    return TRUE;
}

void WorkPile::onCardSelect(Card *card, QPoint pt)
{
    if( !canDrag(card))
        return;

    vector<Card *> dragList;
    popCards(card, dragList);
    // qDebug()<<"WorkPile onCardSelect  sigDragStart";
    emit sigDragStart(this, &dragList, pt);
}

void WorkPile::enterEvent(Card *card)
{
    if(m_linespace>=50) return;

    auto it = find(m_cards.cbegin(), m_cards.cend(), card);
    if(it+1==m_cards.cend())  //最后一张
        return;

    int ox = (*it)->x();
    int oy = (*it)->y()+50;
    for(; it!=m_cards.cend(); it++)
    {
        (*it)->move(ox, oy);
        oy += m_linespace;
    }
}

void WorkPile::leaveEvent(Card *card)
{
    reArrange();
}

void WorkPile::cardDoubleClicked(Card *card)
{
    if( canDrag(card) ){
        emit sigAutoMoveCards(this, card);
    }
}

void WorkPile::cardClicked(Card *card)
{
    // qDebug()<<"WorkPile cardClicked";
    if( canDrag(card) ){
        emit sigAutoMoveCards(this, card);
    }
}

BOOL WorkPile::testSuccessSuit()
{
    if(m_cards.size()<13)
        return FALSE;

    int i=0;
    for(auto it=m_cards.cend()-1; (it != m_cards.cbegin())&&(i<12); it--,i++)
    {
        Card *card = *it;
        Card *lastCard = *(it-1);

        if( !(card->isFaceOn()&&lastCard->isFaceOn()) ) {
            return FALSE;
        }

        if( lastCard->suit() != card->suit())
            return FALSE;

        if( card->rank()+1 != lastCard->rank() )
            return FALSE;
    }
    return TRUE;
}

//===========================================================================
DragPile::DragPile()
:Pile(), dragStartX(0), dragStartY(0)
{

}

DragPile::~DragPile()
{

}

void DragPile::startDrag(vector<Card *> &cardlist, QPoint pt)
{
    removeAll();
    Card *card = *cardlist.cbegin();
    move(card->x(), card->y());
    addCards(cardlist);
    m_startPos = pt;
    dragStartX = m_ox;
    dragStartY = m_oy;

    // qDebug()<<"DragPile stargDrag";
    // card->disconnect();
    card->disconnect(SIGNAL(sigDrag(Card *, QPoint)));
    connect(card, SIGNAL(sigDrag(Card *, QPoint)), this, SLOT(onDrag(Card *, QPoint)) );
}

void DragPile::onDrag(Card *card, QPoint pt)
{
    int dx = pt.x()-m_startPos.x();
    int dy = pt.y()-m_startPos.y();

    // qDebug()<<"DragPile onDrag";
    // card->disconnect(SIGNAL(sigClicked(Card *)));

    move(dragStartX+dx, dragStartY+dy);
}

void DragPile::reArrange()
{
    if(m_cards.empty()) return;

    int ox = m_ox;
    int oy = m_oy;
    int linespace = DEFAULT_LINE_SPACE;
    for(auto it=m_cards.cbegin(); it!=m_cards.cend(); it++)
    {
        (*it)->move(ox, oy);
        oy += linespace;
    }

}

void DragPile::addCardImpl(Card *card)
{
    card->move(m_ox, m_oy);
    card->setFront();
    m_cards.push_back(card);
}

//==========================================================
void OrgPile::addCardImpl(Card *card)
{
    int n = m_cards.size();
    card->restoreOrgSize();
    card->move(m_ox+n, m_oy);
    card->setFace(FALSE);
    card->setFront();
    // card->disconnect(SIGNAL(sigSelected(Card *, QPoint)));
    card->disconnect();
    connect(card, SIGNAL(sigClicked(Card *)), this, SLOT(onClicked()) );
    m_cards.push_back(card);
}

QPoint OrgPile::getNextCardPos()
{
    if( m_cards.empty())
        return QPoint(m_ox, m_oy);
    Card *last = m_cards.back();
    return QPoint(last->x()+1, m_oy);
}

//====================================================
void SucessPile::addCardImpl(Card *card)
{
    card->move(m_ox, m_oy);
    card->setFront();
    m_cards.push_back(card);
}
