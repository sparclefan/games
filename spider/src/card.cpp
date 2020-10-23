#pragma execution_character_set("utf-8")

#include "card.h"
#include <time.h>
#include <stdlib.h>

Card::Card(Suit suit, int rank, QWidget *parent, BOOL face)
: QLabel(parent), m_suit(suit), m_rank(rank), m_bInDrag(FALSE)
{
    m_bkimg.load(":/img/back.jpg");
    QString suitImg;
    switch(suit)
    {
    case Club:
        suitImg = "c";
        break;
    case Diamond:
        suitImg = "d";
        break;
    case Heart:
        suitImg = "h";
        break;
    case Spade:
        suitImg = "s";
        break;
    case Joker:
    default:
        suitImg = "joker";
        break;  
    }

    QString img = QString(":/img/%1%2.jpg").arg(suitImg).arg(rank+1);
    m_img.load(img);

    m_orgWidth = DEFAULT_CARD_WIDTH;
    m_orgHeight = DEFAULT_CARD_HEIGHT;
    
    setFace(face);
    restoreOrgSize();

}

Card::~Card()
{

}

void Card::setFace(BOOL up)
{
    m_up = up;
    if(up){
        setPixmap(m_img.scaled(m_orgWidth, m_orgHeight));
    }else{
        setPixmap(m_bkimg.scaled(m_orgWidth, m_orgHeight));
    }
}

void  Card::resizeEvent(QResizeEvent *event)
{
    int w = width();
    int h = height();
    if(m_up){
        setPixmap(m_img.scaled(w, h));
    }else{
        setPixmap(m_bkimg.scaled(w, h));
    }
}

void Card::restoreOrgSize()
{
    setGeometry(QRect(x(), y(), m_orgWidth, m_orgHeight));
}

BOOL Card::check()
{
    if(m_suit<0){
        m_suit=Club;
        if(m_rank<0 || m_rank>12){
            m_rank = 0;
        }
        return FALSE;
    }else if(m_suit>Joker){
        m_suit = Joker;
        m_rank = 0;
        return FALSE;
    }else if(m_suit==Joker){
        if(m_rank<0 || m_rank>1){
            m_rank = 0;
            return FALSE;
        }
        return TRUE;
    }else{
        if(m_rank < 0 || m_rank>12){
            m_rank = 0;
            return FALSE;
        }
        return TRUE;
    }
}

void Card::mousePressEvent(QMouseEvent *event)
{
    if( (event->button()==Qt::LeftButton) && (!m_bInDrag))
    {
        m_bInDrag = TRUE;
        // qDebug()<<"card sigSelected";
        emit sigSelected(this, event->globalPos());
    }
    //QLabel::mousePressEvent(event);
}

void Card::mouseReleaseEvent(QMouseEvent *event)
{
    // QLabel::mouseReleaseEvent(event);

    if( event->button()==Qt::LeftButton )
    {
        if(m_bInDrag){
            m_bInDrag = FALSE;
            // qDebug()<<"card sigDrop";
            emit sigDrop(event->globalPos());
        }
        // qDebug()<<"card sigClicked";
        emit sigClicked(this);
    }
}

void Card::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sigDoubleClicked(this);
}

void Card::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bInDrag){
        // qDebug()<<"card sigDrag";
        emit sigDrag(this, event->globalPos());
    }
    //QLabel::mouseMoveEvent(event);
}

void Card::enterEvent(QEvent *event)
{
    emit sigEnter(this);
}

void Card::leaveEvent(QEvent *event)
{
    emit sigLeave(this);
}

Card *Card::randomCard(QWidget *parent, BOOL joker, BOOL seed)
{
    int maxrank = joker?53:51;

    if(seed){
        srand((unsigned)time(NULL));
    }
    int idex = rand()%maxrank;
    Suit suit = (Suit)(idex/13);
    int rank = idex%13;
    return new Card(suit, rank, parent);
}

void Card::createSuit(vector<Card *> &cardlist, Suit suit, QWidget *parent, BOOL face)
{
    for(int rank=0; rank<13; rank++){
        Card *card = new Card(suit,rank,parent,face);
        cardlist.push_back(card);
    }
}

void Card::createDeck(vector<Card *> &cardlist, QWidget *parent, BOOL joker, BOOL face)
{
    for(int suit=0; suit<4; suit++)
        for(int rank=0; rank<13; rank++)
        {
            Card *card = new Card((Suit)suit, rank, parent, face);
            cardlist.push_back(card);
        }
    if(joker){
        Card* joker0 = new Card(Joker,0,parent,face);
        Card* joker1 = new Card(Joker, 1, parent, face);
        cardlist.push_back(joker0);
        cardlist.push_back(joker1);
    }
}

void Card::shuffle(vector<Card *>* cardlist)
{
    vector<Card *> tmpList;
    srand((unsigned)time(NULL));
    while(!cardlist->empty()){
        int remain = cardlist->size();
        int idx = rand()%remain;
        tmpList.push_back(cardlist->at(idx));
        cardlist->erase(cardlist->begin()+idx);
    }

    for(auto it=tmpList.cbegin(); it != tmpList.cend(); it++)
    {
        cardlist->push_back(*it);
    }
}