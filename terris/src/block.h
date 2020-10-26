#ifndef __TERRIS_BLOCK_H_SPARCLE_2020_10_22
#define __TERRIS_BLOCK_H_SPARCLE_2020_10_22

#include <QDebug>
#include <QColor>
#include <QPainter>

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

const BYTE BLOCKTYPES[][4] = {
      {0,0,0,0},
      {1,1,1,1},
      {0,0,0,0},
      {0,0,0,0},
//----------------
      {0,0,0,0},
      {2,2,2,0},
      {0,2,0,0},
      {0,0,0,0},
//----------------
      {0,0,0,0},
      {0,3,3,0},
      {0,3,3,0},
      {0,0,0,0},
//----------------
      {4,4,0,0},
      {0,4,4,0},
      {0,0,0,0},
      {0,0,0,0},
//----------------
      {0,5,5,0},
      {5,5,0,0},
      {0,0,0,0},
      {0,0,0,0},
//----------------
      {0,0,0,0},
      {6,6,6,0},
      {0,0,6,0},
      {0,0,0,0},
//----------------
      {0,0,0,0},
      {7,7,7,0},
      {7,0,0,0},
      {0,0,0,0}
//----------------
};

const QColor TYPE_COLOR[] = {
      // QColorConstants::Blue,
      // QColorConstants::Cyan,
      // QColorConstants::Red,
      // QColorConstants::Gray,
      // QColorConstants::Yellow,
      // QColorConstants::Green,
      // QColorConstants::Magenta
      QColor(175, 211, 217),
      QColor(185, 181, 201),
      QColor(199, 224, 161),
      QColor(249, 200, 159),
      QColor(227, 206, 168),
      QColor(204, 51, 102),
      QColor(204, 153, 102 )
};

#define MAX_BLOCK_TYPE 7;

class Blocks 
{
public:
    Blocks(int type);

    void Backup();
    void Restore();

    void Rotate();
    inline void Down(){ m_oy++; };
    inline void Left(){ m_ox--; };
    inline void Right(){ m_ox++; };

    inline void setX(int x){ m_ox = x;};
    inline void setY(int y){ m_oy = y;};
    inline int getX() const { return m_ox;};
    inline int getY() const { return m_oy;};
    inline BYTE getData(int col, int row) const { return m_data[col][row]; };

    void draw(QPainter &painter, int blockWidth, int margin);

    static void drawBlock(QPainter &painter, int x, int y, int type, int blockWidth, int margin);

private:
    void trim();
    BOOL trimTop();
    BOOL trimLeft();

    BYTE m_data[4][4];
    BYTE m_bk[4][4];
    BOOL m_bBackup;

    int m_type;
    int m_ox;
    int m_oy;
};

#endif //__TERRIS_BLOCK_H_SPARCLE_2020_10_22