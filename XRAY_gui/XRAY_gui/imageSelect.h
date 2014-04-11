#pragma once
#include <QLabel>
#include <QPoint>
#include <QRubberBand>
#include <QMouseEvent>

class imageSelect : public QLabel
{
	Q_OBJECT

public:
	explicit imageSelect( QWidget * parent=0, Qt::WindowFlags f=0 )
		: QLabel( parent, f ) {rubberBand=NULL;}
	explicit imageSelect( const QString & text, QWidget * parent=0, Qt::WindowFlags f=0 )
		: QLabel( text, parent, f ) {rubberBand=NULL;}

private:
	QPoint origin;
	QRubberBand* rubberBand;

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

