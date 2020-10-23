
#pragma execution_character_set("utf-8")

#include "ui_gamewin.h"
#include "gamewin.h"
#include <QDebug>

GameWin::GameWin()
:QMainWindow()
{
	ui = new Ui::GameWin();
	ui->setupUi(this);

	// for(int i=0; i<10; i++){
	// 	Card *card = Card::randomCard(ui->frame, TRUE, i==0);
	// 	card->setFace(FALSE);
	// 	card->move(50*i,50);
	// 	connect(card, SIGNAL(sigClicked(Card *, QPoint)), this, SLOT(onCardClicked(Card *, QPoint)) );
	// }
	setOptionSuitNum(4);

	m_orgPile.move(20,5);
	for(int i=0; i<10; i++)
	{
		m_workPiles[i].move(10+i*(DEFAULT_CARD_WIDTH+20), DEFAULT_CARD_HEIGHT+50);
		connect(&(m_workPiles[i]), SIGNAL(sigDragStart(Pile *, vector<Card *>*, QPoint)), this, SLOT(onDragStart(Pile *,vector<Card *>*, QPoint)) );
		connect(&m_workPiles[i], SIGNAL(sigAutoMoveCards(Pile *, Card *)), this, SLOT(tryAutoMove(Pile *, Card *)) );
	}

	m_nSucessPile = 0;
	for(int i=0; i<8; i++)
	{
		m_sucessPile[i].move(360+i*120, 5);
	}

	connect(ui->actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(&m_orgPile, SIGNAL(sigDeliver()), this, SLOT(deliver()));
	connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()));

	connect(ui->actionOptoin, SIGNAL(triggered()), this, SLOT(debug()));
}

GameWin::~GameWin()
{

}

void GameWin::debug()
{
	// qDebug()<<"movePile:";
	// m_movePile.debug();
	// qDebug()<<"workPiles:";
	// for(int i=0; i<10; i++){
	// 	qDebug()<<"============ "<<i<<"=========";
	// 	m_workPiles[i].debug();
	// }
	for(auto it=m_cards.cbegin(); it!=m_cards.cend(); it++)
	{
		(*it)->disconnect();
		(*it)->setVisible(FALSE);
	}

	for(int i=0; i<10; i++)
	{
		m_workPiles[i].removeAll();
	}

	for(int i=0; i<8; i++)
	{
		m_sucessPile[i].removeAll();
	}

	m_orgPile.removeAll();


	for(int i=0; i<8; i++)
	{
		for(int j=0; j<13; j++)
		{
			Card *card = m_cards.at(i*13+j);
			card->setFace();
			m_sucessPile[i].addCard(card);
		}
		m_sucessPile[i].show();
	}

	animateSuccess();
}

void GameWin::onCardClicked(Card *card, QPoint pt)
{
	card->setFace(!card->isFaceOn());
}

void GameWin::setOptionSuitNum(int num)
{
	vector<Card *> tmplist;
	m_cards.swap(tmplist);

	m_nOpSuit = num;
	for(int i=0; i<8; i++)
	{
		Card::createSuit(m_cards, (Suit)(i%m_nOpSuit), ui->frame, FALSE);
	}

	for(auto it=m_cards.cbegin(); it!=m_cards.cend(); it++)
	{
		(*it)->setVisible(FALSE);
	}
}

void GameWin::newGame()
{
	m_aniGroup.clear();
	m_aniGroup.disconnect(SIGNAL(finished()));

	for(auto it=m_cards.cbegin(); it!=m_cards.cend(); it++)
	{
		(*it)->disconnect();
		(*it)->setVisible(FALSE);
	}

	for(int i=0; i<10; i++)
	{
		m_workPiles[i].removeAll();
	}

	for(int i=0; i<8; i++)
	{
		m_sucessPile[i].removeAll();
	}

	m_orgPile.removeAll();

	Card::shuffle(&m_cards);
	Card::shuffle(&m_cards);
	m_orgPile.addCards(m_cards);
	m_orgPile.show(TRUE);

	m_bNewGame = TRUE;
	m_bDragged = FALSE;

	for(auto it=m_undoList.cbegin(); it!=m_undoList.cend();)
	{
		vector<Op *> *opList = *it;
		for(auto itop=opList->cbegin(); itop!=opList->cend();)
		{
			Op * op = *itop;
			delete op;
			itop = opList->erase(itop);
		}
		delete opList;
		it = m_undoList.erase(it);
	}

	vector<vector<Op *> *> tmpUndolist;
	m_undoList.swap(tmpUndolist);

	m_score = 500;
	m_nSucessPile = 0;

	QString strScore = QString("得分：%1").arg(m_score);
	ui->statusBar->showMessage(strScore);

}

void GameWin::deliver()
{
	m_aniGroup.clear();
	m_aniGroup.disconnect(SIGNAL(finished()));

	if(m_bNewGame){
		m_bNewGame = FALSE;
		for(int i=0; i<54; i++)
		{
			BOOL face= (i>=44);
			animateOneCard(m_orgPile, &m_workPiles[i%10], face, 200);
		}
	}else{
		vector<Op *> *undo = new vector<Op *>();
		for(int i=0; i<10; i++){
			Card *card = m_orgPile.getTailCard();
			if(card){
				animateOneCard(m_orgPile, &m_workPiles[i], TRUE, 300);
				vector<Card *> opcardlist;
				opcardlist.push_back(card);
				Op *op = new Op(&m_orgPile, &m_workPiles[i], opcardlist, FALSE);
				undo->push_back(op);
			}
		}
		if( undo->empty()){
			delete undo;
		}else{
			m_undoList.push_back(undo);
		}
	}

	connect( &m_aniGroup, SIGNAL(finished()), this, SLOT(onAnimateFinished()) );
	m_aniGroup.start();
}

void GameWin::undo()
{
	if( m_undoList.empty()) return;

	vector<Op *> *opList = m_undoList.back();
	m_undoList.pop_back();

	while( !opList->empty() ){
		Op *op = opList->back();
		opList->pop_back();
		op->undo();
		delete op;
	}

	m_score--;
	ui->statusBar->showMessage(QString("得分：%1").arg(m_score));

}

void GameWin::tryAutoMove(Pile *pile, Card *card)
{
	if(m_bDragged) return;

	Pile *matchPile = NULL;
	Pile *alterPile = NULL;
	Pile *emptyPile = NULL;
	for(int i=0; i<10; i++)
	{
		if( pile == (&m_workPiles[i]) ) continue;

		Card *tail = m_workPiles[i].getTailCard();
		if(tail==NULL){
			if(emptyPile==NULL){
				emptyPile = &m_workPiles[i];
			}
			continue;
		}

		int rank1 = card->rank();
		int rank2 = tail->rank();
		if( rank2 == rank1+1 ){
			if(card->suit()==tail->suit()){
				matchPile = &m_workPiles[i];
				break;
			}else if(alterPile==NULL){
				alterPile = &m_workPiles[i];
			}
		}		
	}

	Pile *toPile = matchPile?matchPile:(alterPile?alterPile:emptyPile);
	if(toPile == NULL) return;

	vector<Op *> *undo = new vector<Op *>();
	vector<Card *> movelist;
	pile->popCards(card, movelist);

	Card *tCard = m_pDragPile->getTailCard();
	BOOL brevface = FALSE;
	if( tCard && (!tCard->isFaceOn()) ){
		tCard->setFace(TRUE);
		brevface = TRUE;
	}
	Op *op = new Op(pile, toPile, movelist, brevface);
	undo->push_back(op);
	m_score --;
	ui->statusBar->showMessage(QString("得分：%1").arg(m_score));

	toPile->addCards(movelist);

	// 每次移完牌后，测试是否完成一套牌
	if( ((WorkPile *)toPile)->testSuccessSuit() ){
		for(int i=0; i<8; i++){
			if(m_sucessPile[i].count()==0){
				vector<Card *> cards;
				toPile->getTailCards(13, cards);
				Card *card = toPile->getTailCardById(14);
				BOOL rev = (card && (!card->isFaceOn()));
				Op *op = new Op(toPile, &m_sucessPile[i], cards, rev, TRUE);
				undo->push_back(op);
				break;
			}
		}
		successPile(toPile);
	}

	if(undo->empty()){
		delete undo;
	}else{
		m_undoList.push_back(undo);
	}

}

//====================== Drag and Drop ==========================
void GameWin::onDragStart(Pile *pile, vector<Card *>* cardlist, QPoint pt)
{
	m_pDragPile = pile;
	m_movePile.startDrag(*cardlist, pt); 
	Card *card = cardlist->front();
	m_bDragged = FALSE;

	// qDebug()<<"GameWin onDragStart";
	card->disconnect(SIGNAL(sigDrop(QPoint)));
	connect(card, SIGNAL(sigDrop(QPoint)), this, SLOT(onDrop(QPoint)));
}

Pile *GameWin::getDropPile(QPoint &pt)
{
	QPoint ptFrame = ui->frame->mapFromGlobal(pt);
	for(int i=0; i<10; i++)
	{
		int x = ptFrame.x();
		if( x>m_workPiles[i].x() && x<(m_workPiles[i].x()+DEFAULT_CARD_WIDTH) )
		{
			Card *tail = m_workPiles[i].getTailCard();
			if(tail==NULL){
				return &m_workPiles[i];
			}
			int rank1 = m_movePile.getHeadCard()->rank();
			int rank2 = tail->rank();
			if( rank2 == rank1+1){
				return &m_workPiles[i];
			}
		}
	}
	return m_pDragPile;
}

void GameWin::onDrop(QPoint pt)
{
	if( m_movePile.count()<1 )
		return;

	// qDebug()<<"GameWin onDrop";
	vector<Op *> *undo = new vector<Op *>();
	Pile *pile = getDropPile(pt);
	if(pile != m_pDragPile){
		m_bDragged = TRUE;
		Card *card = m_pDragPile->getTailCard();
		BOOL brevface = FALSE;
		if( card && (!card->isFaceOn()) ){
			card->setFace(TRUE);
			brevface = TRUE;
		}
		vector<Card *> list = m_movePile.cards();
		Op *op = new Op(m_pDragPile, pile, list, brevface);
		undo->push_back(op);
		m_score --;
		ui->statusBar->showMessage(QString("得分：%1").arg(m_score));
	}

	vector<Card *> list = m_movePile.cards();

	pile->addCards(list);
	m_movePile.removeAll();

	// 每次移完牌后，测试是否完成一套牌
	if( ((WorkPile *)pile)->testSuccessSuit() ){
		for(int i=0; i<8; i++){
			if(m_sucessPile[i].count()==0){
				vector<Card *> cards;
				pile->getTailCards(13, cards);
				Card *card = pile->getTailCardById(14);
				BOOL rev = (card && (!card->isFaceOn()));
				Op *op = new Op(pile, &m_sucessPile[i], cards, rev, TRUE);
				undo->push_back(op);
			}
		}
		successPile(pile);
	}

	if(undo->empty()){
		delete undo;
	}else{
		m_undoList.push_back(undo);
	}
}

void GameWin::successPile(Pile *pile)
{
	m_aniGroup.clear();
	m_aniGroup.disconnect(SIGNAL(finished()));

	m_pDropPile = pile;

	for(int i=0; i<8; i++){
		if(m_sucessPile[i].count()==0){
			// m_nSucessPile = (i+1);
			for(int j=0; j<13; j++){
				animateOneCard(*pile,&m_sucessPile[i],TRUE,100);
			}
			break;
		}
	}
	connect( &m_aniGroup, SIGNAL(finished()), this, SLOT(onPileSuccessFinished()) );
	m_aniGroup.start();

}

//=================== Animate ========================================
void GameWin::onAnimateFinished()
{
	int n = m_aniGroup.animationCount();
	for(int i=n-1; i>=0; i--)
	{
		CardAnimation *ani = (CardAnimation *)m_aniGroup.takeAnimation(i);
		if(ani){
			delete ani;
		}
	}
}

void GameWin::onPileSuccessFinished()
{
	Card *card = m_pDropPile->getTailCard();
	if( card && (!card->isFaceOn())){
		card->setFace(TRUE);
	}

	m_score += 100;
	ui->statusBar->showMessage(QString("得分：%1").arg(m_score));

	for(int i=0; i<8; i++){
		if(m_sucessPile[i].count()==0){
			return;
		}
	}
	animateSuccess();
	// if(m_nSucessPile==8){
	// 	animateSuccess();
	// }

}

void GameWin::animateOneCard(Pile &fromPile, Pile *toPile, BOOL face, int duration)
{
	Card *card = fromPile.popTail();
	if(card==NULL) return;

	QPoint pt= toPile->getNextCardPos();

	CardAnimation *ani = new CardAnimation(card, toPile, face);
	ani->setDuration(duration);
	ani->setStartValue(card->geometry());
	ani->setEndValue(QRect(pt.x(),pt.y(),card->width(), card->height()));
	m_aniGroup.addAnimation(ani);

}

void GameWin::animateSuccess()
{
	m_aniGroup.clear();
	m_aniGroup.disconnect(SIGNAL(finished()));

	srand((unsigned)time(NULL));
	int w = ui->frame->width();
	int h = ui->frame->height();
	for(int i=0; i<8; i++){
		QParallelAnimationGroup *pPag = new QParallelAnimationGroup();
		Card *card = m_sucessPile[i].popTail();
		while(card){
			QPropertyAnimation *ani = new QPropertyAnimation(card, "geometry");
			ani->setDuration(1000+rand()%1000);
			ani->setStartValue(card->geometry());

			int xEnd = rand()%(w-DEFAULT_CARD_WIDTH);
			int yEnd = 200+rand()%(h-200-DEFAULT_CARD_HEIGHT);

			ani->setEndValue(QRect(xEnd,yEnd,DEFAULT_CARD_WIDTH,DEFAULT_CARD_HEIGHT));

			// ani->setKeyValueAt(0.3, QRect(0,h,0,0));
			ani->setKeyValueAt(0.5, QRect(w/2, h, DEFAULT_CARD_WIDTH/3, DEFAULT_CARD_HEIGHT/3 ));

			ani->setEasingCurve(QEasingCurve::OutInCubic);
			// m_aniGroup.addAnimation(ani);
			pPag->addAnimation(ani);
			card = m_sucessPile[i].popTail();
		}
		m_aniGroup.addAnimation(pPag);
	}
	m_aniGroup.start();
}