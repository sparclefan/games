#pragma execution_character_set("utf-8")

#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <QThread>
#include <QMessageBox>

const char *levelDesc[]={"不变化","向下","向左","上下分离",
						"左右分离","上下集中","左右集中","上左下右",
						"左上右下","向外扩散","向内集中"};

CGameLLK::CGameLLK()
{
	m_cIconNum = 0;
	m_nCols = 0;
	m_nRows = 0;
	m_nBlocks = 0;
	m_iLevel = -1;
	m_pIcons = NULL;
	// m_iTip1 = -1;
	// m_iTip2 = -1;
	m_nTips = 0;
	m_nLife = 0;
	m_iTimer = 0;
	m_iScore = 0;
	m_iOrgTimer = 0;
	m_bTimeRun = FALSE;
	m_iClass = 0;

	m_iSelect1 = -1;
	m_iSelect2 = -1;
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()) );
}

CGameLLK::~CGameLLK()
{
	if( m_pIcons != NULL )
	{
		delete m_pIcons;
		m_pIcons = NULL;
	}
}

BOOL CGameLLK::InitIcons(int col, int row, int nIcons)
{
	if( (col*row)%2 != 0 )
		return FALSE;

	if( m_pIcons != NULL )
	{
		delete m_pIcons;
		m_pIcons = NULL;
	}

	m_pIcons = new BYTE[col*row];

	if( m_pIcons == NULL ) return FALSE;
	m_nCols = col;
	m_nRows = row;
	m_nBlocks = nIcons;

	m_cIconNum = nIcons;
	m_cLastIcon = 1;
	for( int i=0; i<row; i++ )
		for( int j=0; j<col; j++ )
		{
			m_pIcons[i*col+j] = GetNextIcon();
		}

	ReArrangeIcons();
	// m_iTip1 = -1;
	// m_iTip2 = -1;

	m_iDropBlock = 0;

	m_bTimeRun = TRUE;

	return TRUE;
}

BYTE CGameLLK::GetNextIcon()
{
	static BOOL isOdd = TRUE;

	BYTE rt = m_cLastIcon;
	if( !isOdd )
		m_cLastIcon ++;

	if( m_cLastIcon > m_cIconNum )
		m_cLastIcon = 1;

	isOdd = !isOdd;

	return rt;
}

void CGameLLK::ReArrangeIcons()
{
	if( m_nLife < 0 ) return;
	
	m_nLife --;

	srand( (unsigned)time( NULL ) );
	for( int i=0; i<(m_nCols*m_nRows); i++ )  
	{
		if( m_pIcons[i] == 0 ) continue;
		int c = GetIconBeExchange(i);
		if( (c == i )||(m_pIcons[c]==0) )
			continue;
		BYTE tmp = m_pIcons[i];
		m_pIcons[i] = m_pIcons[c];
		m_pIcons[c] = tmp;
	}
}

int CGameLLK::GetIconBeExchange(int id)
{
	int n = m_nCols*m_nRows - id;
	int rt = rand()%n + id;
	for(int i=0; (i<10)&&(m_pIcons[rt]==0); i++ )
		rt = rand()%n + id;

	return rt;
}

BYTE CGameLLK::GetIconType(int c, int r)
{
	if( m_pIcons == NULL ) return 0;
	if( (c<0)||(c>=m_nCols) ) return 0;
	if( (r<0)||(r>=m_nRows) ) return 0;

	return m_pIcons[r*m_nCols+c];
}

BOOL CGameLLK::CanDrop(int c1, int r1, int c2, int r2)
{
	BYTE b0 = GetIconType(c1, r1);
	if(b0==0) return FALSE;

	if( GetIconType(c2, r2) != b0 ) return FALSE;

	m_dropLine.Reset();
	m_dropLine.AddPoint(new CPoint(c1, r1) );
	if( SearchLeft( c1, r1, c2, r2 ) ) return TRUE;

	m_dropLine.Reset();
	m_dropLine.AddPoint(new CPoint(c1, r1) );
	if( SearchRight( c1, r1, c2, r2) ) return TRUE;

	m_dropLine.Reset();
	m_dropLine.AddPoint(new CPoint(c1, r1) );
	if( SearchUp(c1, r1, c2, r2) ) return TRUE;

	m_dropLine.Reset();
	m_dropLine.AddPoint(new CPoint(c1, r1) );
	return SearchDown(c1,r1,c2,r2);
}

BOOL CGameLLK::SearchLeft(int c, int r, int ec, int er)
{
	if( c == -1 ) return FALSE;

	if( GetIconType(c-1, r) != 0 )
		if( ((c-1)!=ec)||(r!=er) ) return FALSE;

	CPoint *pt = new CPoint(c-1, r);
	if( !m_dropLine.AddPoint(pt) )
		return FALSE;
	
	if( ((c-1)==ec) && (r==er) ) return TRUE;

	if( SearchUp( c-1, r, ec, er) )
		return TRUE;
	if( SearchDown(c-1, r, ec, er) )
		return TRUE;
	if( SearchLeft( c-1, r, ec, er) )
		return TRUE;

	m_dropLine.RemoveTail();
	return FALSE;
}

BOOL CGameLLK::SearchUp(int c, int r, int ec, int er)
{
	if( r == -1 ) return FALSE;

	if( GetIconType(c, r-1) != 0 )
		if( (c!=ec)||((r-1)!=er) ) return FALSE;

	CPoint *pt = new CPoint(c, r-1);
	if( !m_dropLine.AddPoint(pt) )
		return FALSE;
	
	if( (c==ec)&&((r-1)==er) ) return TRUE;

	if( SearchLeft( c, r-1, ec, er) )
		return TRUE;
	if( SearchRight( c, r-1, ec, er) )
		return TRUE;
	if( SearchUp( c, r-1, ec, er) )
		return TRUE;

	m_dropLine.RemoveTail();
	return FALSE;

}

BOOL CGameLLK::SearchDown(int c, int r, int ec, int er)
{
	if( r == m_nRows ) return FALSE;

	if( GetIconType(c, r+1) != 0 )
		if( (c!=ec)||((r+1)!=er) ) return FALSE;

	CPoint *pt = new CPoint(c, r+1);
	if( !m_dropLine.AddPoint(pt) )
		return FALSE;

	if( (c==ec) && ((r+1)==er) ) return TRUE;

	if( SearchLeft( c, r+1, ec, er ) )
		return TRUE;
	if( SearchRight( c, r+1, ec, er) )
		return TRUE;

	if( SearchDown(c, r+1, ec, er) )
		return TRUE;

	m_dropLine.RemoveTail();
	return FALSE;

}

BOOL CGameLLK::SearchRight(int c, int r, int ec, int er)
{
	if( c == m_nCols ) return FALSE;

	if( GetIconType(c+1, r) != 0 )
		if( ((c+1)!=ec) || (r!=er) ) return FALSE;

	CPoint *pt = new CPoint(c+1, r);
	if( !m_dropLine.AddPoint(pt) )
		return FALSE;

	if( ((c+1)==ec) && (r == er) ) return TRUE;

	if( SearchUp(c+1, r, ec, er) ) return TRUE;
	if( SearchDown(c+1,r, ec,er) ) return TRUE;

	if( SearchRight(c+1, r, ec, er) )
		return TRUE;
	m_dropLine.RemoveTail();
	return FALSE;

}

BOOL CGameLLK::CanDrop(int id1, int id2)
{
	int c1,r1,c2,r2;

	c1 = id1%m_nCols;
	r1 = id1/m_nCols;
	c2 = id2%m_nCols;
	r2 = id2/m_nCols;

	return CanDrop(c1,r1,c2,r2);

}

BOOL CGameLLK::GetTip(int &tip1, int &tip2)
{
	if( m_nTips <= 0 ) return FALSE;
	m_nTips --;
	emit sigInfoChange();
	return getTip(tip1, tip2);
}

BOOL CGameLLK::getTip(int &tip1, int &tip2)
{
	int n = m_nCols*m_nRows;
	for( int i=0; i<n; i++ )
	{
		if( m_pIcons[i] == 0 ) continue;
		for( int j=i+1; j<n; j++ )
		{
			if( m_pIcons[j] != m_pIcons[i] ) continue;
			if( CanDrop(i,j) )
			{
				tip1 = i;
				tip2 = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CGameLLK::IsSeccess()
{
	return m_iDropBlock >= m_nCols*m_nRows ;
}

void CGameLLK::NextLevel()
{
	if( m_iLevel == 10 )
	{
		m_timer.stop();
		QMessageBox::information(NULL,"恭喜","本难度所有关已通过!!!!");
		return;
	}
	m_iLevel ++;
	InitIcons(m_nCols, m_nRows, m_nBlocks);
	m_nLife += 2;
	m_nTips += 2;
	m_iTimer = m_iOrgTimer;
	m_bTimeRun = TRUE;

	emit sigRepaint();
	emit sigInfoChange();

	// m_timer.start(100);
}

void CGameLLK::MoveBlocksDown(int col, int row, int ec, int er)
{
	for( int i=row; i>er; i-- )
		m_pIcons[i*m_nCols+col] = m_pIcons[(i-1)*m_nCols+col];
	m_pIcons[er*m_nCols+col] = 0;
}

void CGameLLK::MoveBlocksLeft(int col, int row, int ec, int er)
{
	for( int i=col; i<ec; i++ )
		m_pIcons[row*m_nCols+i] = m_pIcons[row*m_nCols+i+1];
	m_pIcons[row*m_nCols+ec] = 0;
}

void CGameLLK::MoveBlocksUp(int col, int row, int ec, int er)
{
	for( int i=row; i<er; i++ )
		m_pIcons[i*m_nCols+col] = m_pIcons[(i+1)*m_nCols+col];
	m_pIcons[er*m_nCols+col] = 0;

}

void CGameLLK::MoveBlocksRight(int col, int row, int ec, int er)
{
	for( int i=col; i>ec; i-- )
		m_pIcons[row*m_nCols+i] = m_pIcons[row*m_nCols+i-1];
	m_pIcons[row*m_nCols+ec] = 0;
}

BOOL CGameLLK::isCenter(int col, int row)
{
	BOOL isCenterCol = FALSE;
	BOOL isCenterRow = FALSE;

	if(col==m_nCols/2){
		isCenterCol = TRUE;
	}

	if( (m_nCols%2 == 0)&&(col==(m_nCols/2-1)) ){
		isCenterCol = TRUE;
	}

	if(row==m_nRows/2){
		isCenterRow = TRUE;
	}

	if( (m_nRows%2 == 0)&&(row==(m_nRows/2-1)) ){
		isCenterRow = TRUE;
	}

	return (isCenterRow && isCenterCol);
}

BOOL CGameLLK::isCorner(int col, int row)
{
	if( col==0 && row==0 )
		return TRUE;

	if( col==0 && row == m_nRows-1 )
		return TRUE;

	if( col == (m_nCols-1) && (row == 0) )
		return TRUE;

	if( col==m_nCols-1 && row == m_nRows-1)
		return TRUE;

	return FALSE;
}

void CGameLLK::MoveBlocksScatter(int col, int row)
{

	int nextCol = col;
	int nextRow = row;

	int tmpCol = col;
	int tmpRow = row;

	int centerCol = (m_nCols % 2) ? m_nCols / 2 : m_nCols / 2 - 1;
	int centerRow = (m_nRows % 2) ? m_nRows / 2 : m_nRows / 2 - 1;

	if( col<= centerCol ){ //左边
		if( row <= centerRow ){  // 左上
			if ( (col < centerCol) && ( centerCol-col >= (centerRow-row)) ) {
				nextCol = col + 1;
				if(row < centerRow)
					tmpRow = row + 1;
			}
			else if (row <centerRow) {
				nextRow = row + 1;
				if (col < centerCol)
					tmpCol = col + 1;
			}
		}else{  // 左下
			if ((col < centerCol) && ( centerCol - col >= (centerRow - row))) {
				nextCol = col + 1;
				if (row > centerRow)
					tmpRow = row - 1;
			}
			else if (row > centerRow) {
				nextRow = row - 1;
				if (col < centerCol)
					tmpCol = col + 1;
			}
		}
	}else{  //右边
		if( row <= centerRow ){  // 右上
			if ( col > (centerCol) && ((col-centerCol)>=(centerRow-row))) {
				nextCol = col - 1;
				if (row < centerRow)
					tmpRow = row + 1;
			}
			else if (row < centerRow) {
				nextRow = row + 1;
				if (col > (centerCol))
					tmpCol = col - 1;
			}
		}else{  // 右下
			if (col > (centerCol ) && ((col-centerCol) >= (row-centerRow))) {
				nextCol = col - 1;
				if (row > (centerRow) )
					tmpRow = row - 1;
			}
			else if (row > (centerRow )) {
				nextRow = row - 1;
				if (col > (centerCol ))
					tmpCol = col - 1;
			}

		}
	}  

	if ((m_pIcons[nextRow*m_nCols + nextCol] == 0) &&
		(tmpCol >= 0 && tmpCol < m_nCols && tmpRow >= 0 && tmpRow < m_nRows) &&
		(tmpCol != col || tmpRow != row) &&
		(m_pIcons[tmpRow*m_nCols + tmpCol] > 0))
	{
		nextCol = tmpCol;
		nextRow = tmpRow;
	}

	m_pIcons[row*m_nCols+col] = m_pIcons[nextRow*m_nCols+nextCol];
	if(m_pIcons[nextRow*m_nCols+nextCol]==0 && (nextRow != m_iCacheRow || nextCol != m_iCacheCol ))  
		return;

	if(nextCol==m_iCacheCol && nextRow == m_iCacheRow){
		m_iCacheRow = row;
		m_iCacheCol = col;
	}

	if( isCenter(nextCol, nextRow)){
		m_pIcons[nextRow*m_nCols+nextCol] = 0;
		return;
	}

	MoveBlocksScatter(nextCol, nextRow);

}

void CGameLLK::MoveBlocksGather(int col, int row)
{
	int nextCol = col;
	int nextRow = row;
	int tmpCol = col;
	int tmpRow = row;

	if( col<= m_nCols/2 ){ //左边
		if( row <= m_nRows/2 ){  // 左上
			if(col>0 && (col>=row) ){
				nextCol = col-1;
				tmpRow = row-1;
			}else if(row>0){
				nextRow = row-1;
				tmpCol = col-1;
			}
		}else{  // 左下
			if( (col>0)&&( col>=(m_nRows-row) )){
				nextCol = col-1;
				tmpRow = row+1;
			}else if(row<m_nRows-1){
				nextRow = row+1;
				tmpCol = col - 1;
			}
		}
	}else{  //右边
		if( row <= m_nRows/2 ){  // 右上
			if( (col<m_nCols-1) && (m_nCols - col)>row){
				nextCol = col+1;
				tmpRow = row-1;
			}else if(row>0){
				nextRow = row-1;
				tmpCol = col + 1;
			}
		}else{  // 右下
			if((col<m_nCols-1) && (m_nCols - col)>=(m_nRows - row)){
				nextCol = col+1;
				tmpRow = row+1;
			}else if(row<m_nRows-1){
				nextRow = row+1;
				tmpCol = col + 1;
			}
		}
	}  

	if( (m_pIcons[nextRow*m_nCols+nextCol]==0) &&
		( tmpCol>=0 && tmpCol<m_nCols && tmpRow>=0 && tmpRow<m_nRows) &&
		( m_pIcons[tmpRow*m_nCols+tmpCol]>0) )
	{
		nextCol = tmpCol;
		nextRow = tmpRow;
	}			

	m_pIcons[row*m_nCols+col] = m_pIcons[nextRow*m_nCols+nextCol];

	
	if( (m_pIcons[nextRow*m_nCols+nextCol]==0) && (nextRow != m_iCacheRow || nextCol != m_iCacheCol ))  
		return;

	if(nextCol==m_iCacheCol && nextRow == m_iCacheRow){
		m_iCacheRow = row;
		m_iCacheCol = col;
	}

	if( isCorner(nextCol, nextRow)){
		m_pIcons[nextRow*m_nCols+nextCol] = 0;
		return;
	}

	MoveBlocksGather(nextCol, nextRow);

}

void CGameLLK::InitGameCfg(int life, int tips, int timer)
{
	m_nLife = life;
	m_nTips = tips;
	m_iTimer = timer;
	m_iOrgTimer = timer;
}

void CGameLLK::ResetGame()
{
	m_cIconNum = 0;
	m_nCols = 0;
	m_nRows = 0;
	m_nBlocks = 0;
	m_iLevel = -1;
	m_nTips = 0;
	m_nLife = 0;
	m_iTimer = 0;
	m_iScore = 0;
	m_iOrgTimer = 0;

	m_iSelect1 = -1;
	m_iSelect2 = -1;

	if( m_pIcons != NULL )
	{
		delete m_pIcons;
		m_pIcons = NULL;
	}

	// UpdateAllViews(NULL);
	emit sigRepaint();
	m_timer.stop();
}

void CGameLLK::NewGame(int c)
{
	switch(c)
	{
	case 1:
		InitIcons(12,7,20);
		InitGameCfg(5,8, 60*4*5);
		break;
	case 2:
		InitIcons(14,8,27);
		InitGameCfg(8, 10, 60*6*5);
		break;
	case 3:
		InitIcons(16, 9, 35);
		//InitIcons(16,9,1);
		InitGameCfg(50,52,60*10*5);
		break;
	case 4:
		// CDlgDefine dlg;
		// if( dlg.DoModal()!=IDOK )
		// 	return;
		// InitIcons(dlg.m_iCol, dlg.m_iRow, dlg.m_iIcon);
		// InitGameCfg(dlg.m_iLife, dlg.m_iTip, dlg.m_iTime);
		break;
	}
	m_iClass = c;
	m_iScore = 0;
	m_iLevel = 0;
//	emit sigInfoChange();
	emit sigRepaint();

	m_timer.start(100);

}

int CGameLLK::getIconId(int col, int row)
{
	int idx = row*m_nCols + col;
	return (int)m_pIcons[idx];
}

void CGameLLK::selectIcon(int idx)
{
	int t1=m_iSelect1;
	int t2=m_iSelect2;
	if( m_pIcons[idx] > 0 )
	{
		if( m_iSelect1 < 0 )
		{
			m_iSelect1 = idx;
			emit sigIconSelect(idx);
			return;
		}else if( m_iSelect2 < 0 && (m_iSelect1 != idx) ){

			if( m_pIcons[m_iSelect1] == m_pIcons[idx] )
			{
				m_iSelect2 = idx;
				if( CanDrop(m_iSelect1, m_iSelect2) )
				{
					emit sigIconSelect(idx);
					DropIcons( m_iSelect1, m_iSelect2);
					m_iSelect1 = -1;
					m_iSelect2 = -1;
				}else{
					m_iSelect1 = -1;
					m_iSelect2 = -1;
					emit sigInvalidIcon(t1);
					emit sigInvalidIcon(t2);
				}

			}else{
				m_iSelect1 = -1;
				m_iSelect2 = -1;
				emit sigInvalidIcon(t1);
				emit sigInvalidIcon(t2);
			}

		}else{
			m_iSelect2 = -1;
			m_iSelect1 = -1;
			emit sigInvalidIcon(t1);
			emit sigInvalidIcon(t2);
			// emit sigInvalidIcon(m_iSelect1);
			// emit sigInvalidIcon(m_iSelect2);
		}
	}else{
		m_iSelect1 = -1;
		m_iSelect2 = -1;
		emit sigInvalidIcon(t1);
		emit sigInvalidIcon(t2);

	}

}

void CGameLLK::DropIcons(int id1, int id2)
{
	emit sigDropLine();

	QThread::msleep(200);

	m_pIcons[id1] = 0;
	m_pIcons[id2] = 0;

	emit sigDropLine();
	emit sigInvalidIcon(id1);
	emit sigInvalidIcon(id2);

	AfterDropBlock(id1, id2);

	m_iTimer += 2;
	m_iScore += 10;

	emit sigScoreChange();

	m_iDropBlock += 2;
	if( IsSeccess() )
	{
		m_bTimeRun = FALSE;
		int exScore = m_nLife*100
			+ m_nTips*50 + m_iTimer 
			+ m_iLevel*1000;

		QString msg = QString(" 根据剩余的提示、生命值、时间和关数可获得%1分").arg(exScore);
		QMessageBox::information(NULL, "恭喜你, 过关成功!!!", msg);

		m_iScore += exScore;
		emit sigScoreChange();

		NextLevel();
	}

	int t1, t2;
	if( !getTip(t1, t2) )
	{
		if( !LifeChance() )
		{
			QMessageBox::information(NULL, "Game Over!", "已经无牌可消除，游戏结束!!");
			ResetGame();
			return;
		}
		//ReArrangeIcons();
		//emit sigRepaint();
		//emit sigInfoChange();
	}

}

BOOL CGameLLK::LifeChance()
{
	if (m_nLife <= 0) return FALSE;
	ReArrangeIcons();
	emit sigRepaint();
	emit sigInfoChange();
	return TRUE;
}

void CGameLLK::InvalidBlocks(int col1, int col2, int row1, int row2)
{
	for(int col=col1; col<=col2; col++){
		for(int row=row1; row<=row2; row++){
			emit sigInvalidIcon(row*m_nCols+col);
		}
	}
}

void CGameLLK::AfterDropBlock(int id1, int id2)
{
	int col1 = id1%m_nCols;
	int row1 = id1/m_nCols;
	int col2 = id2%m_nCols;
	int row2 = id2/m_nCols;

	int id = -1;
	switch( m_iLevel )
	{
	case 1: // 向下
		MoveBlocksDown(col1, row1, col1, 0);
		InvalidBlocks(col1, col1, 0, row1);
		if( (col1==col2)&&(row1>row2) )
			row2++;
		MoveBlocksDown(col2, row2, col2, 0);
		InvalidBlocks(col2, col2, 0, row2);
		break;

	case 2:  // 向左
		MoveBlocksLeft(col1, row1, m_nCols-1, row1);
		InvalidBlocks(col1, m_nCols-1, row1, row1);
		if( (row1==row2)&&(col1<col2) )
			col2--;
		MoveBlocksLeft(col2, row2, m_nCols-1, row2);
		InvalidBlocks(col2, m_nCols-1, row2, row2);
		break;

	case 3: // 上下分离
		if( row1 > m_nRows/2 )
		{
			MoveBlocksDown(col1, row1, col1, m_nRows/2);
			InvalidBlocks(col1, col1, m_nRows/2, row1);
			if( (col1==col2)&&(row1>row2)&&(row2>m_nRows/2) )
				row2 ++;
		}else
		{
			MoveBlocksUp(col1, row1, col1, m_nRows/2 );
			InvalidBlocks(col1, col1, row1, m_nRows/2);
			if( (col1==col2)&&(row1<row2)&&(row2<m_nRows/2) )
				row2 --;
		}

		if( row2 > m_nRows/2 )
		{
			MoveBlocksDown(col2, row2, col2, m_nRows/2);
			InvalidBlocks(col2, col2, m_nRows/2, row2);
		}else
		{
			MoveBlocksUp(col2, row2, col2, m_nRows/2 );
			InvalidBlocks(col2, col2, row2, m_nRows/2);
		}
		break;

	case 4: // 左右分离
		if( col1 > m_nCols/2 )
		{
			MoveBlocksRight(col1, row1, m_nCols/2, row1);
			InvalidBlocks(m_nCols/2, col1, row1, row1 );
			if( (row1==row2)&&(col1>col2)&&(col2>m_nCols/2) )
				col2 ++;
		}else
		{
			MoveBlocksLeft(col1, row1, m_nCols/2, row1 );
			InvalidBlocks(col1, m_nCols/2, row1, row1);
			if( (row1==row2)&&(col1<col2)&&(col2<m_nCols/2) )
				col2 --;
		}

		if( col2 > m_nCols/2 )
		{
			MoveBlocksRight(col2, row2, m_nCols/2, row2);
			InvalidBlocks(m_nCols/2, col2, row2, row2 );
		}else
		{
			MoveBlocksLeft(col2, row2, m_nCols/2, row2 );
			InvalidBlocks(col2, m_nCols/2, row2, row2);
		}
		break;

	case 5: // 上下集中
		if( row1 < m_nRows/2 )
		{
			MoveBlocksDown(col1, row1, col1, 0);
			InvalidBlocks(col1, col1, 0, row1);
			if( (col1==col2)&&(row1>row2)&&(row2<m_nRows/2) )
				row2 ++;
		}else  //( row1 > pDoc->m_nRows/2 )
		{
			MoveBlocksUp(col1, row1, col1, m_nRows-1 );
			InvalidBlocks(col1, col1, row1, m_nRows-1);
			if( (col1==col2)&&(row1<row2)&&(row2>=m_nRows/2) )
				row2 --;
		}

		if( row2 < m_nRows/2 )
		{
			MoveBlocksDown(col2, row2, col2, 0);
			InvalidBlocks(col2, col2, 0, row2);
		}else   //( row2 > pDoc->m_nRows/2 )
		{
			MoveBlocksUp(col2, row2, col2, m_nRows-1 );
			InvalidBlocks(col2, col2, row2, m_nRows-1);
		}
		break;

	case 6:  // 左右集中
		if( col1 < m_nCols/2 )
		{
			MoveBlocksRight(col1, row1, 0, row1);
			InvalidBlocks(0, col1, row1, row1);
			if( (row1==row2)&&(col1>col2)&&(col2<m_nCols/2) )
				col2 ++;
		}else 
		{
			MoveBlocksLeft(col1, row1, m_nCols-1, row1 );
			InvalidBlocks(col1, m_nCols-1, row1, row1);
			if( (row1==row2)&&(col1<col2)&&(col2>=m_nCols/2) )
				col2 --;
		}

		if( col2 < m_nCols/2 )
		{
			MoveBlocksRight(col2, row2, 0, row2);
			InvalidBlocks(0, col2, row2, row2);
		}else 
		{
			MoveBlocksLeft(col2, row2, m_nCols-1, row2 );
			InvalidBlocks(col2, m_nCols-1, row2, row2);
		}
		break;

	case 7:  //上左下右
		if( row1 < m_nRows/2 )
		{
			MoveBlocksLeft(col1, row1, m_nCols-1, row1 );
			InvalidBlocks(col1, m_nCols-1, row1, row1);
			if ((row1 == row2) && (col1<col2) )
					col2 --;
		}else  
		{
			MoveBlocksRight(col1, row1, 0, row1);
			InvalidBlocks(0, col1, row1, row1);
			if( (row1==row2)&&(col1>col2) )
				col2 ++;
		}

		if( row2 < m_nRows/2 )
		{
			MoveBlocksLeft(col2, row2, m_nCols-1, row2 );
			InvalidBlocks(col2, m_nCols-1, row2, row2) ;
		}else   
		{
			MoveBlocksRight(col2, row2, 0, row2);
			InvalidBlocks(0, col2, row2, row2);
		}
		break;

	case 8:  // 左上右下
		if( col1 < m_nCols/2 )
		{
			MoveBlocksUp(col1, row1, col1, m_nRows-1 );
			InvalidBlocks(col1, col1, row1, m_nRows-1);
			if( (col1==col2)&&(row1<row2) )
				row2 --;
		}else 
		{
			MoveBlocksDown(col1, row1, col1, 0);
			InvalidBlocks(col1, col1, 0, row1);
			if( (col1==col2)&&(row1>row2) )
				row2 ++;
		}

		if( col2 < m_nCols/2 )
		{
			MoveBlocksUp(col2, row2, col2, m_nRows-1 );
			InvalidBlocks(col2, col2, row2, m_nRows-1);
		}else 
		{
			MoveBlocksDown(col2, row2, col2, 0);
			InvalidBlocks(col2, col2, 0, row2);
		}
		break;

	case 9:  // 向外扩散
		m_iCacheCol = col2;
		m_iCacheRow = row2;
		MoveBlocksScatter(col1, row1);
		MoveBlocksScatter(m_iCacheCol, m_iCacheRow);
		emit sigRepaint();
		break;
	case 10: // 向内集中
		m_iCacheCol = col2;
		m_iCacheRow = row2;
		MoveBlocksGather(col1, row1);
		MoveBlocksGather(m_iCacheCol, m_iCacheRow);
		emit sigRepaint();
		break;
	}
}

QString CGameLLK::gameClassInfo() const
{
	switch(m_iClass)
	{
	case 1:
		return "难度 初级";
		break;
	case 2:
		return "难度 中级";
		break;
	case 3:
		return "难度 高级";
		break;
	}
	return "";
}

QString CGameLLK::gameMessage() const
{
	if(m_iLevel<0){
		return QString("F1-F2-F3开始游戏！");
	}

	return QString("关卡 %1-%2 生命值 %3 提示 %4").arg(m_iLevel).arg(levelDesc[m_iLevel]).arg(m_nLife).arg(m_nTips);
}

void CGameLLK::onTimeout()
{
	if(!m_bTimeRun) return;

	m_iTimer--;

	emit sigUpdateTimer();

	if(m_iTimer<=0){
		m_timer.stop();
		QMessageBox::information(NULL, "Game Over!", "时间耗尽，游戏结束!!");
		ResetGame();
		return;		
	}
}