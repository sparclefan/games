#ifndef __LLK_GAME_H_SPARCLE_2020_09_27
#define __LLK_GAME_H_SPARCLE_2020_09_27

#include "dropline.h"
#include <QObject>
#include <QTimer>

class CGameLLK : public QObject
{
	Q_OBJECT
public:
	CGameLLK();
	~CGameLLK();

    void NewGame(int c);
	int getIconId(int col, int row);

	void ResetGame();

	BOOL LifeChance();
	BOOL GetTip(int &tip1, int &tip2);
	BYTE GetIconType(int c, int r);

	void selectIcon(int idx);

	QString gameClassInfo() const;
	QString gameMessage() const;

	inline int rowNum() const{return m_nRows;};
	inline int colNum() const{return m_nCols;};
	inline int getTimer() const{return m_iTimer;};
	inline int getOrgTimer() const{return m_iOrgTimer;};
	inline vector<CPoint *> getDropLine() const{return m_dropLine.m_pts;};
	inline int score() const { return m_iScore;};

public slots:
	void onTimeout();

signals:
	void sigIconSelect(int idx);
	void sigDropLine();
	void sigInvalidIcon(int idx);
	void sigRepaint();
	void sigInfoChange();
	void sigScoreChange();
	void sigUpdateTimer();

private:
	void ReArrangeIcons();
	void InitGameCfg(int life, int tips, int timer);
	BOOL InitIcons(int col, int row, int nIcons);

	BOOL CanDrop(int id1, int id2);
	BOOL CanDrop(int c1, int r1, int c2, int r2);
	void DropIcons(int id1, int id2);
	void AfterDropBlock(int id1, int id2);
	void InvalidBlocks(int col1, int col2, int row1, int row2);

	BOOL SearchRight(int c, int r, int ec, int er);
	BOOL SearchDown(int c, int r, int ec, int er);
	BOOL SearchUp(int c, int r, int ec, int er);
	BOOL SearchLeft( int c, int r, int ec, int er );

	BOOL isCenter(int col, int row);
	BOOL isCorner(int col, int row);
	void MoveBlocksGather(int col, int row);
	void MoveBlocksScatter(int col, int row);
	void MoveBlocksRight(int col, int row, int ec, int er);
	void MoveBlocksUp(int col, int row, int ec, int er);
	void MoveBlocksLeft(int col, int row, int ec, int er);
	void MoveBlocksDown(int col, int row, int ec, int er);
	void NextLevel();
	BOOL IsSeccess();

	int GetIconBeExchange(int id);	
	BYTE GetNextIcon();
	BOOL getTip(int &tip1, int &tip2);

private:
    // int m_iTip1;
    // int m_iTip2;
	int m_iCacheRow;
	int m_iCacheCol;

	int m_iSelect1;
	int m_iSelect2;

	int m_iClass;
	int m_iOrgTimer;
	BOOL m_bTimeRun;
	int m_iScore;
	int m_iTimer;
	int m_nLife;
	int m_nTips;
	int m_iLevel;
	int m_nBlocks;
	int m_iDropBlock;

	CDropLine m_dropLine;
	int m_cIconNum;
	int m_nCols;
	int m_nRows;
	BYTE *m_pIcons;

	BYTE m_cLastIcon;
	QTimer m_timer;

};

#endif //__LLK_GAME_H_SPARCLE_2020_09_27