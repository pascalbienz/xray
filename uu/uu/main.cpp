#include "uu.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	uu w;
	w.show();
	return a.exec();
}
