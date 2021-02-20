#include "gameview.h"
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>

GameView::GameView(QWidget *parent)
:QWidget(parent)
{
    for(int i=0; i<ICONS_NUM; i++){
        QString iconUri = QString(":/icon/%1.png").arg(i+1);
        m_imgs[i].load(iconUri);
    }

    m_pPixmap = NULL;
    connect(&m_game, SIGNAL(sigIconSelect(int)), this, SLOT(onIconSelect(int)));
    connect(&m_game, SIGNAL(sigDropLine()), this, SLOT(onDropLine()));
    connect(&m_game, SIGNAL(sigInvalidIcon(int)), this, SLOT(repaintIcon(int)));
    connect(&m_game, SIGNAL(sigRepaint()), this, SLOT(repaintIconboard()));

    m_iTip1 = -1;
    m_iTip2 = -1;
    setFocusPolicy(Qt::StrongFocus);
}

GameView::~GameView()
{
    if( m_pPixmap != NULL){
        delete m_pPixmap;
        m_pPixmap = NULL;
    }

}

void GameView::onIconSelect(int idx)
{
    if(m_iTip1>=0){
        repaintIcon(m_iTip1);
        m_iTip1 = -1;
    }
    if(m_iTip2>=0){
        repaintIcon(m_iTip2);
        m_iTip2 = -1;
    }


    if(idx>=0){
        drawSelect(idx);
    }
}

void GameView::onDropLine()
{
    DrawInfo info;
    if( !caculateDrawInfo(info) ) return;
    QPainter painter(m_pPixmap);
    painter.save();
    painter.setPen(QPen(QBrush(QColor(255,255,0)), 3));
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    int x1, y1, x2, y2;

    vector<CPoint *> pts = m_game.getDropLine();
    CPoint *pt=NULL, *lastpt=NULL;
    for( auto iter = pts.cbegin(); iter != pts.cend(); iter++)
    {
		pt = *iter;
		if( lastpt == NULL )
		{
			lastpt = pt;
			continue;
		}

        x1 = info.margin_w + lastpt->x*info.colWidth + info.colWidth/2;
        y1 = info.margin_h + lastpt->y*info.rowHeight + info.rowHeight/2;
        x2 = info.margin_w + pt->x*info.colWidth + info.colWidth/2;
        y2 = info.margin_h + pt->y*info.rowHeight + info.rowHeight/2;

        painter.drawLine(x1,y1,x2,y2);
        lastpt = pt;
    }

    painter.restore();

    repaint();
}

void GameView::repaintIconboard()
{
    preparePixmap();
    repaint();
}

void GameView::onActionSimple()
{
    m_game.NewGame(1);
    repaintIconboard();
}

void GameView::onActionNormal()
{
    m_game.NewGame(2);
    repaintIconboard();
}

void GameView::onActionHard()
{
    m_game.NewGame(3);
    repaintIconboard();
}

void GameView::onPause()
{
    m_game.PauseContinue();
}

void GameView::resizeEvent(QResizeEvent *event)
{
    if(m_pPixmap==NULL) return;
    preparePixmap();
}

void GameView::paintEvent(QPaintEvent *event)
{
    if( m_pPixmap == NULL) return;

    QPainter painter(this);
    painter.save();

    QPixmap pix = *m_pPixmap;
    painter.drawPixmap(0,0, pix);

    painter.restore();

}

void GameView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_F1:
        onActionSimple();
        break;
    case Qt::Key_F2:
        onActionNormal();
        break;
    case Qt::Key_F3:
        onActionHard();
        break;
    case Qt::Key_F4:
        onPause();
        break;
    case Qt::Key_F5:
        if( m_game.GetTip(m_iTip1, m_iTip2) )
        {
            drawSelect(m_iTip1);
            drawSelect(m_iTip2);
        }
        break;
	case Qt::Key_F6:
		m_game.LifeChance();
		break;
    default:
        break;            
    }
}

void GameView::mouseReleaseEvent(QMouseEvent *event)
{
    if( m_pPixmap == NULL) return;

    int idx = hitTest(event->pos());
    if(idx >=0 ){
        m_game.selectIcon(idx);
    }
}


BOOL GameView::caculateDrawInfo(DrawInfo &info)
{
    info.totalWidth = width();
    info.totalHeight = height();

    if( info.totalWidth < DEFAULT_HORIZON_MARGIN*2 || info.totalHeight < DEFAULT_VERTICAL_MARGIN*2 ){
        return FALSE;
    }

    info.colnum = m_game.colNum();
    info.rownum = m_game.rowNum();
    // info.margin_w = DEFAULT_HORIZON_MARGIN;
    // info.margin_h = DEFAULT_VERTICAL_MARGIN;

    if( info.colnum<=0 || info.rownum<=0 )
        return FALSE;

    // info.colWidth = (info.totalWidth - info.margin_w*2)/info.colnum;
    // info.rowHeight = (info.totalHeight - info.margin_h*2)/info.rownum;

    info.colWidth = (info.totalWidth-4)/(info.colnum+1);
    info.margin_w = info.colWidth/2+2;
    info.rowHeight = (info.totalHeight-4)/(info.rownum+1);
    info.margin_h = info.rowHeight/2+2;

    if( info.colWidth > MAX_ICON_WIDTH ){
        info.colWidth = MAX_ICON_WIDTH;
        info.margin_w = (info.totalWidth - info.colWidth*info.colnum)/2;
    }

    if( info.rowHeight > MAX_ICON_HEIGTH){
        info.rowHeight = MAX_ICON_HEIGTH;
        info.margin_h = (info.totalHeight - info.rowHeight*info.rownum )/2;
    }

    if(info.colWidth > info.rowHeight){
        info.colWidth = info.rowHeight;
    }else{
        info.rowHeight = info.colWidth;
    }

    return TRUE;
}

int GameView::hitTest(QPoint pt)
{
    DrawInfo info;
    if( !caculateDrawInfo(info) ) return -1;

	int col =  (pt.x()-info.margin_w)/info.colWidth;
	int row = (pt.y()-info.margin_h)/info.rowHeight;

	if( (col<0)||(col>=info.colnum) )
		return -1;
	if( (row<0)||(row>=info.rownum) )
		return -1;

	return row*info.colnum+col;

}

void GameView::repaintIcon(int idx)
{
    if(idx<0) return;

    DrawInfo info;
    if( !caculateDrawInfo(info) ) return;

    int col = idx%info.colnum;
    int row = idx/info.colnum;

    QRect target(info.margin_w+col*info.colWidth, info.margin_h+row*info.rowHeight, info.colWidth, info.rowHeight);
    QRect source(0,0,ICONS_W,ICONS_H);

    QPainter painter(m_pPixmap);
    painter.fillRect(target, Qt::white);
    int iconId = m_game.getIconId(col, row);
    if(iconId>0){
        painter.drawImage(target, m_imgs[iconId-1], source );
    }
    painter.end();

 //   target.moveTop(target.top()+MESSAGE_BOARD_HEIGHT);
    repaint(target);
}

void GameView::drawSelect(int idx)
{
    DrawInfo info;
    if( !caculateDrawInfo(info) ) return;

    int col = idx%info.colnum;
    int row = idx/info.colnum;

    QRect source(0,0,ICONS_W,ICONS_H);
    QRect target(info.margin_w+col*info.colWidth, info.margin_h+row*info.rowHeight, info.colWidth, info.rowHeight);

    QPainter painter(m_pPixmap);
    // painter.save();
    // painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.fillRect(target, QColor(32,32,0,64));
    // painter.restore();

    painter.end();

 //   target.moveTop(target.top()+MESSAGE_BOARD_HEIGHT);
    repaint(target);
}

void GameView::preparePixmap()
{
    DrawInfo info;
    if( !caculateDrawInfo(info) ) return;

    if( m_pPixmap != NULL){
        delete m_pPixmap;
        m_pPixmap = NULL;
    }

    m_pPixmap = new QPixmap(QSize(info.totalWidth, info.totalHeight));
    m_pPixmap->fill(Qt::white);

    QRect source(0,0,ICONS_W,ICONS_H);

    QPainter painter(m_pPixmap);
    for(int row=0; row<info.rownum; row++){
        for(int col=0; col<info.colnum; col++){
            QRect target(info.margin_w+col*info.colWidth, info.margin_h+row*info.rowHeight, info.colWidth, info.rowHeight);
            int iconId = m_game.getIconId(col, row);
            if(iconId>0){
                painter.drawImage(target, m_imgs[iconId-1], source );
            }
        }
    }
    painter.end();
}