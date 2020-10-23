#include "block.h"

Blocks::Blocks(int type)
:m_type(type), m_ox(0), m_oy(0),m_bBackup(FALSE)
{
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++)
        {
            m_data[i][j] = BLOCKTYPES[type*4+i][j];
        }
    }
    trim();
}

void Blocks::Backup()
{
    memcpy(m_bk, m_data, sizeof m_data);
    m_bBackup = TRUE;
}

void Blocks::Restore()
{
    if(!m_bBackup) return;
    memcpy(m_data, m_bk, sizeof m_data);
    m_bBackup = FALSE;
}

void Blocks::Rotate()
{
    BYTE result[4][4];
    for(int i=0; i<4; i++)
      for(int j=0; j<4; j++)
        result[i][j] = m_data[j][3-i];

    for(int i=0; i<4; i++)
      for(int j=0; j<4; j++)
        m_data[i][j] = result[i][j];

    trim();
}

BOOL Blocks::trimTop()
{
    for(int i=0; i<4; i++)
      if( m_data[i][0] > 0 )
        return FALSE;

    for(int i=0; i<4; i++)
      for(int j=0; j<3; j++)
        m_data[i][j] = m_data[i][j+1];

    for(int i=0; i<4; i++)
      m_data[i][3] = 0;

    return TRUE;
}

BOOL Blocks::trimLeft()
{
    for(int i=0; i<4; i++)
      if( m_data[0][i] > 0 )
        return FALSE;

    for(int i=0; i<3; i++)
      for(int j=0; j<4; j++)
        m_data[i][j] = m_data[i+1][j];

    for(int i=0; i<4; i++)
      m_data[3][i] = 0;

    return TRUE;
}

void Blocks::trim()
{
    for(int i=0; i<4; i++)
      if( !trimTop() ) break;

    for(int i=0; i<4; i++)
      if( !trimLeft() ) break;
}

///===================== draw ==========================
void Blocks::draw(QPainter &painter, int blockWidth, int margin)
{
    for(int i=0; i<4; i++)
    {
      for(int j=0; j<4; j++)
      {
        if( m_data[i][j] == 0 ) continue;
        drawBlock(painter, m_ox+j, m_oy+i, m_data[i][j], blockWidth, margin);
      }
    }  
}


void Blocks::drawBlock(QPainter &painter, int x, int y, int type, int blockWidth, int margin)
{
  if (type == 0)
    return;

  painter.fillRect(x * blockWidth + margin + 1, y * blockWidth + margin + 1, blockWidth - 2, blockWidth - 2, TYPE_COLOR[type - 1]);

}