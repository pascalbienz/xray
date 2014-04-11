#pragma once

#include <QRubberBand>

class Resizable_rubber_band : public QWidget {
public:
	Resizable_rubber_band(QWidget* parent = 0);
	QRubberBand* rubberband;

private:
	
	void resizeEvent(QResizeEvent *);

};