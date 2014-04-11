#include "imageSelect.h"





void imageSelect::mousePressEvent(QMouseEvent *event)
{
	origin = event->pos();
	if (!rubberBand)
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	rubberBand->setGeometry(QRect(origin, QSize()));
	rubberBand->show();
}

void imageSelect::mouseMoveEvent(QMouseEvent *event)
{
	rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void imageSelect::mouseReleaseEvent(QMouseEvent *event)
{
	//rubberBand->hide();
	// determine selection, for example using QRect::intersects()
	// and QRect::contains().
}