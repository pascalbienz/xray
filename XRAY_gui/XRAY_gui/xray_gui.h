#ifndef XRAY_GUI_H
#define XRAY_GUI_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QDirIterator>
#include "ui_xray_gui.h"
#include <QStringListModel>
#include <QShortcut>
#include <QAbstractItemModel>
#include <QPointer>
#include <QLabel>
#include <QProgressBar>
#include <QHash>

#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <pcl/filters/passthrough.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_cylinder.h>
#include <pcl/visualization/pcl_plotter.h>
#include "build/notify.h"


class XRAY_gui : public QMainWindow, public iobserverClass
{
	Q_OBJECT

public:
	XRAY_gui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~XRAY_gui();

	QPointer<QStringListModel> listFiles;

	QLabel * labelStatus;
	QProgressBar * progressBarStatus;

	QPixmap image;

	pcl::visualization::PCLVisualizer * viewer; 

	QHash<QString, int *> volumes;

	QHash<QString, void *> pointClouds;

	void showImage(int i);

	void fitImage();

	void updateNotify(std::string message);

public	slots: void on_toolButton_clicked();

	void on_horizontalSlider_valueChanged();

	void resizeEvent(QResizeEvent *);

void deleteItem();


void itemcurrentChanged ( const QModelIndex & current);
void on_loadVolumePushButton_clicked();
void update();


void secureThreadUpdate(QString data);
QString getFile(int i);
private:
	Ui::XRAY_guiClass ui;
	QLabel * usagelabelStatus;
};

#endif // XRAY_GUI_H
