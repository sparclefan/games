#include "dropline.h"

CDropLine::CDropLine()
{

}

CDropLine::~CDropLine()
{
	Reset();
}

void CDropLine::Reset()
{
	while (!m_pts.empty()) {
		CPoint *pt = m_pts.back();
		if (pt) {
			delete pt;
		}
		m_pts.pop_back();
	}

}

BOOL CDropLine::AddPoint(CPoint *pt)
{
    if( m_pts.empty()){
        m_pts.push_back(pt);
        return TRUE;
    }

	CPoint *p = m_pts.back();
	if( (p->x != pt->x )&&(p->y != pt->y) )   // 不是相邻的点
		return FALSE;

	m_pts.push_back(pt);

	if( GetCorners() > 2 )
	{
        m_pts.pop_back();
		if( pt != NULL )
			delete pt;
		return FALSE;
	}

	return TRUE;
}

int CDropLine::GetCorners()
{
	int corner = 0;
	CPoint *pt=NULL, *lastpt=NULL;
	Direction d, lastd = DirUnknown;
    for( auto iter = m_pts.cbegin(); iter != m_pts.cend(); iter++){
		pt = *iter;
		if( lastpt == NULL )
		{
			lastpt = pt;
			continue;
		}

		d = GetDirection( lastpt, pt );
		if( lastd != DirUnknown )
		{
			if( d != lastd )
				corner ++;
		}

		lastd = d;
		lastpt = pt;
	}
	return corner;
}

Direction CDropLine::GetDirection(CPoint *p1, CPoint *p2)
{
	if( p1->x == p2->x )
	{
		if( p1->y < p2->y )
			return DirDown;
		if( p1->y > p2->y )
			return DirUp;
	}
	if( p1->y == p2->y )
	{
		if( p1->x < p2->x )
			return DirRight;
		if( p1->x > p2->x )
			return DirLeft;
	}
	return DirUnknown;
}

void CDropLine::RemoveTail()
{
	if( m_pts.empty() )
		return;

	CPoint *pt = m_pts.back();
	if( pt != NULL );
		delete pt;
    m_pts.pop_back();
}
