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
#include <QtCore>

#include <matVoxel.h>
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


class VolumeData
{
public:
	int * voxels;
	int w,h,d;
	float pixSize;
};

class LoadingParam
{
public :
	VolumeData* matrix;
	int threshold;
	bool Smoothing;
	bool SmoothingInt;
	std::vector<std::string> vec;
	pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud;
	float pixSize;

};

class ThreadLoaderSkeletonWorker : public QObject
{
	Q_OBJECT

public :

	ThreadLoaderSkeletonWorker(matVoxel * vox, pcl::PointCloud<pcl::PointXYZI> * cloud2,VolumeData * currentData);


public slots:
	void run();

private:
	matVoxel * vox; pcl::PointCloud<pcl::PointXYZI>* cloud2;VolumeData * currentData;

signals:
	void getRes(matVoxel * vox, pcl::PointCloud<pcl::PointXYZI>* cloud2,VolumeData * currentData);
};

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

	QHash<QString, VolumeData *> volumes;

	QHash<QString, pcl::PointCloud<pcl::PointXYZI>::Ptr> pointClouds;

	QHash<QString, matVoxel *> algData;

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
	void addCloud( pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, QString &name );
	void handleLoading();
	static void startLoading(LoadingParam &param);
	void on_treeWidget_itemSelectionChanged();
	void on_pointsizespinBox_valueChanged(int i);
	void on_opacitydoubleSpinBox_valueChanged(double i);
	VolumeData * getActiveVolume();
	void on_skeletonizepushButton_clicked();
	pcl::PointCloud<pcl::PointXYZI>::Ptr getActiveCloud();
	matVoxel * getActiveSkeleton();
	void skeleton(matVoxel * voxel, pcl::PointCloud<pcl::PointXYZI>* cloud2,VolumeData * currentData);
	std::string getActiveCloudName();
	void enableInterface(bool v);
	void on_delpushButton_clicked();
private:
	Ui::XRAY_guiClass ui;
	QLabel * usagelabelStatus;
	LoadingParam * parameters;
};

#endif // XRAY_GUI_H
