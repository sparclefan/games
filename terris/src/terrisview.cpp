#include "terrisview.h"

TerrisView::TerrisView(QWidget *parent)
:QWidget(parent)
{
    connect(&m_game, SIGNAL(sigUpdate()), this, SLOT(repaint()));
    setFocusPolicy(Qt::StrongFocus);
}

TerrisView::~TerrisView()
{

}

void TerrisView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    m_game.draw(painter, width(), height());
}

void TerrisView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key)
    {
    case Qt::Key_Left:
        m_game.Left();
        break;
    case Qt::Key_Right:
        m_game.Right();
        break;
    case Qt::Key_Up:
        m_game.Rotate();
        break;
    case Qt::Key_Down:
        m_game.Down();
        break;
	case Qt::Key_F2:
		m_game.newGame();
		break;
    default:
        break;            
    }
}
