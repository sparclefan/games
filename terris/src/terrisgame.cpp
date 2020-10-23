#include "terrisgame.h"

#define DROPDOWNTIME 50
#define MAXLEVEL 10
#define BLOCK_SCORE 7
#define LINE_SCORE 20

#define MIN_BLOCKWIDTH 20
#define MARGIN 20

TerrisGame::TerrisGame()
    : m_pThread(NULL),
      m_level(1), m_score(0), m_pCur(NULL), m_pNext(NULL), m_bGameOver(FALSE)
{
  memset(m_blocks, 0, sizeof m_blocks);
}

TerrisGame::~TerrisGame()
{
}

void TerrisGame::gameThread()
{
  while (!m_bGameOver)
  {
    if (canDown())
    {
      m_pCur->Down();
    }
    else
    {
      Drop();
      newBlock();
    }

    emit sigUpdate();

    QThread::msleep((MAXLEVEL - m_level) * DROPDOWNTIME);
    // qDebug()<<"game thread ...";
  }

  m_pThread = NULL;
}

void TerrisGame::newGame()
{
  if (m_pThread != NULL)
  {
    m_bGameOver = TRUE;
    m_pThread->exit();
    QThread::sleep(1);
  }

  memset(m_blocks, 0, sizeof m_blocks);
  m_level = 1;
  m_score = 0;
  m_bGameOver = FALSE;
  if (m_pCur != NULL)
  {
    delete m_pCur;
    m_pCur = NULL;
  }
  if (m_pNext != NULL)
  {
    delete m_pNext;
    m_pNext = NULL;
  }

  srand((unsigned)time(NULL));

  m_pThread = QThread::create(std::bind(&TerrisGame::gameThread, this));
  m_pThread->start();
}

void TerrisGame::newBlock()
{
  int type = rand() % MAX_BLOCK_TYPE;
  if (m_pNext != NULL)
  {
    m_pCur = m_pNext;
    m_pCur->setX((TOTALWIDTH - 4) / 2);
    m_pCur->setY(0);
  }

  m_pNext = new Blocks(type);
  m_pNext->setX(TOTALWIDTH + 2);
  m_pNext->setY(3);

  if (m_pCur != NULL && (!checkCur()))
  {
    m_bGameOver = TRUE;
    // m_pThread->exit();
  }
}

BOOL TerrisGame::checkCur()
{
  if (m_pCur == NULL)
    return FALSE;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      if (m_pCur->getData(i, j) == 0)
        continue;

      int xx = m_pCur->getX() + j;
      int yy = m_pCur->getY() + i;

      if (xx < 0 || xx >= TOTALWIDTH || yy < 0 || yy >= MAXLINES)
        return FALSE;

      if (m_blocks[yy][xx] != 0)
        return FALSE;
    }

  return TRUE;
}

BOOL TerrisGame::canDown()
{
  if (m_pCur == NULL)
    return FALSE;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      if (m_pCur->getData(i, j) == 0)
        continue;

      int xx = m_pCur->getX() + j;
      int yy = m_pCur->getY() + i + 1;

      if (xx < 0 || xx >= TOTALWIDTH || yy < 0 || yy >= MAXLINES)
        return FALSE;

      if (m_blocks[yy][xx] != 0)
        return FALSE;
    }

  return TRUE;
}

void TerrisGame::Drop()
{
  if (m_pCur == NULL)
    return;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      int oy = m_pCur->getY() + i;
      int ox = m_pCur->getX() + j;
      if (m_pCur->getData(i, j) == 0)
        continue;

      if (ox < 0 || ox >= TOTALWIDTH || oy < 0 || oy >= MAXLINES)
        continue;

      m_blocks[oy][ox] = m_pCur->getData(i, j);
    }
  delete m_pCur;
  m_pCur = NULL;

  m_score += BLOCK_SCORE;
  checkFillLine();

  m_level = m_score / 1000 + 1;
  if (m_level >= MAXLEVEL)
    m_level = 1;
}

void TerrisGame::killLine(int line)
{
  for (int i = 0; i < TOTALWIDTH; i++)
  {
    m_blocks[line][i] = 0;
  }

  for (int i = line; i > 0; i--)
    for (int j = 0; j < TOTALWIDTH; j++)
    {
      m_blocks[i][j] = m_blocks[i - 1][j];
    }
  for (int j = 0; j < TOTALWIDTH; j++)
  {
    m_blocks[0][j] = 0;
  }
}

void TerrisGame::checkFillLine()
{
  int nFillLine = 0;
  for (int i = 0; i < MAXLINES; i++)
  {
    BOOL bFill = TRUE;
    for (int j = 0; j < TOTALWIDTH; j++)
    {
      if (m_blocks[i][j] == 0)
      {
        bFill = FALSE;
        break;
      }
    }
    if (bFill)
    {
      nFillLine++;
      killLine(i);
    }
  }
  m_score += LINE_SCORE * (nFillLine * nFillLine);
}

BOOL TerrisGame::canLeft()
{
  if (m_pCur == NULL)
    return FALSE;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      if (m_pCur->getData(i, j) == 0)
        continue;

      int xx = m_pCur->getX() + j - 1;
      int yy = m_pCur->getY() + i;

      if (xx < 0 || xx >= TOTALWIDTH || yy < 0 || yy >= MAXLINES)
        return FALSE;

      if (m_blocks[yy][xx] != 0)
        return FALSE;
    }

  return TRUE;
}

BOOL TerrisGame::canRight()
{
  if (m_pCur == NULL)
    return FALSE;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      if (m_pCur->getData(i, j) == 0)
        continue;

      int xx = m_pCur->getX() + j + 1;
      int yy = m_pCur->getY() + i;

      if (xx < 0 || xx >= TOTALWIDTH || yy < 0 || yy >= MAXLINES)
        return FALSE;

      if (m_blocks[yy][xx] != 0)
        return FALSE;
    }

  return TRUE;
}

void TerrisGame::Left()
{
  if( canLeft()){
    m_pCur->Left();
    emit sigUpdate();
  }
}

void TerrisGame::Right()
{
  if( canRight()){
    m_pCur->Right();
    emit sigUpdate();
  }

}

void TerrisGame::Rotate()
{
  if(m_pCur==NULL) return;

  m_pCur->Backup();
  m_pCur->Rotate();
  if( !checkCur() ){
    m_pCur->Restore();
  }else{
    emit sigUpdate();
  }
}

void TerrisGame::Down()
{
  if(m_pCur==NULL) return;
  while( canDown() )
  {
    m_pCur->Down();
  }
  Drop();
  emit sigUpdate();
}

//================================== draw ==============================
void TerrisGame::draw(QPainter &painter, int totalWidth, int totalHeight)
{
  int iBlockWidthX = (totalWidth - MARGIN*2 - 4)*3/4/TOTALWIDTH;
  int iBlockWidthY = (totalHeight - MARGIN*2 - 4)/MAXLINES;

  int blockWidth = (iBlockWidthX<iBlockWidthY)?iBlockWidthX:iBlockWidthY;

  drawGameBoard(painter, blockWidth);

  if(m_pCur){
      m_pCur->draw(painter, blockWidth, MARGIN);
  }

  if(m_pNext){
      m_pNext->draw(painter, blockWidth, MARGIN);
  }

  drawScoreLevel(painter, blockWidth);
  
}

void TerrisGame::drawGameBoard(QPainter &painter, int blockWidth)
{
  painter.setPen( QPen(QBrush(QColorConstants::Black), 2) );
  painter.drawRect(MARGIN, MARGIN, blockWidth*TOTALWIDTH+4, blockWidth*MAXLINES+4);

  for(int i=0; i<MAXLINES; i++){
    for(int j=0; j <TOTALWIDTH; j++)
      Blocks::drawBlock(painter, j, i, m_blocks[i][j], blockWidth, MARGIN );
  }
}

void TerrisGame::drawScoreLevel(QPainter &painter, int blockWidth)
{
    int x = (TOTALWIDTH+2)*blockWidth+MARGIN;
    int y = 10*blockWidth+MARGIN;

    painter.setFont(QFont("Times New Roman", 14));
    painter.drawText(x, y, QString("Score %1").arg(m_score));

    y+= blockWidth;
    painter.drawText(x, y, QString("Level %1").arg(m_level));
}