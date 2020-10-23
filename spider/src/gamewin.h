#pragma execution_character_set("utf-8")

#ifndef __SPIDER_GAMEWIN_H_SPARCLE_2020_10_13
#define __SPIDER_GAMEWIN_H_SPARCLE_2020_10_13

#include <QMainWindow>
#include "spiderpiles.h"
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

namespace Ui{
	class GameWin;
}

class GameWin : public QMainWindow
{
	Q_OBJECT
public:
	GameWin();
	~GameWin();

private slots:
	void onCardClicked(Card *card, QPoint pt);
	void onDragStart(Pile *pile, vector<Card *>*cardlist, QPoint pt);
	void newGame();
	void deliver();
	void onAnimateFinished();
	void onPileSuccessFinished();
	void onDrop(QPoint pt);
	void undo();
	void debug();
	void tryAutoMove(Pile *pile, Card *card);

private:
	void setOptionSuitNum(int num);
	Pile *getDropPile(QPoint &pt);
	void successPile(Pile *pile);

	//移动一张牌，从frompile到topile, 动画支持
	void animateOneCard(Pile &fromPile, Pile *toPile, BOOL face, int duration);

	void animateSuccess();

    Ui::GameWin *ui;

	QSequentialAnimationGroup m_aniGroup;

	vector<Card *> m_cards;
	vector<vector<Op *> *> m_undoList;

	DragPile m_movePile;
	Pile *m_pDragPile;
	Pile *m_pDropPile;
	OrgPile m_orgPile;

	WorkPile m_workPiles[10];
	SucessPile m_sucessPile[8];

	int m_nSucessPile;
	int m_nOpSuit;
	BOOL m_bNewGame;
	int m_score;

	BOOL m_bDragged;

};


#endif //__SPIDER_GAMEWIN_H_SPARCLE_2020_10_13