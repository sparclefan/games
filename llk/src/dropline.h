#ifndef __LLK_DROPLINE_H_SPARCLE_20200927
#define __LLK_DROPLINE_H_SPARCLE_20200927

#include <vector>

using namespace std;

#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum Direction{
	DirLeft,
	DirRight,
	DirUp,
	DirDown,
	DirUnknown
};

class CPoint 
{
public:
	CPoint(int ix, int iy):x(ix),y(iy) {};
    int x;
    int y;
};


class CDropLine  
{
public:
	void RemoveTail();
	BOOL AddPoint(CPoint *pt);
	void Reset();
	CDropLine();
	virtual ~CDropLine();

	vector<CPoint *> m_pts;
protected:
	Direction GetDirection(CPoint *p1, CPoint *p2);
	int GetCorners();
};

#endif //__LLK_DROPLINE_H_SPARCLE_20200927