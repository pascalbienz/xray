#ifndef UU_H
#define UU_H

#include <QtGui/QMainWindow>
#include "ui_uu.h"

class uu : public QMainWindow
{
	Q_OBJECT

public:
	uu(QWidget *parent = 0, Qt::WFlags flags = 0);
	~uu();

private:
	Ui::uuClass ui;
};

#endif // UU_H
