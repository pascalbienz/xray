#include "xray_gui.h"
#include "matVoxel.h"
#include "QVTKWidget.h"
#include <pcl\visualization\pcl_visualizer.h>
#include <vtkRenderWindow.h>
#include <windows.h>
#include <QTimer>
#include <QThread>
#include <psapi.h>
#include <qcoreapplication.h>
#include "bmpConverter.h"



XRAY_gui::XRAY_gui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	

	labelStatus = new QLabel("Message");
	statusBar()->addWidget(labelStatus);

	progressBarStatus = new QProgressBar();
	statusBar()->addWidget(progressBarStatus);

	usagelabelStatus = new QLabel("Performance :");
	statusBar()->addWidget(usagelabelStatus);

	QShortcut * shortcut = new QShortcut(QKeySequence(Qt::Key_Delete),ui.listView);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
	
	
	listFiles = new QStringListModel();

	ui.listView->setModel(listFiles);

	connect(ui.listView->selectionModel(), SIGNAL(currentChanged(const QModelIndex& , const QModelIndex&)), this, SLOT(itemcurrentChanged(const QModelIndex&)),Qt::QueuedConnection);


	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(1000);


	//vtkSmartPointer<vtkRenderWindow> renderWindow = viewer.getRenderWindow();

	imageVolumeLoader::observer_to=this;

	viewer = new pcl::visualization::PCLVisualizer();
	
	ui.qvtkWidget->SetRenderWindow(viewer->getRenderWindow());

	ui.treeWidget->expandAll();
}

XRAY_gui::~XRAY_gui()
{

}


void XRAY_gui::on_toolButton_clicked()
{

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             "",
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);


	QDir dirFilter= QDir::QDir(dir);
	dirFilter.setFilter(QDir::Files|QDir::NoDotAndDotDot);

	dirFilter.setNameFilters(QStringList()<<"*.bmp");
	dirFilter.filter();

	

	listFiles->setStringList(dirFilter.entryList());
	//ui.listView->addItems(dirFilter.entryList());
	//ui.listView->setModel(listFiles);

	ui.lineEdit->setText(dirFilter.absolutePath());

	
	ui.horizontalSlider->setMaximum(listFiles->rowCount()-1);


	showImage(listFiles->rowCount()/2);

	


}

void XRAY_gui::deleteItem()
{

	int i=ui.listView->currentIndex().row();
	if(i!=-1)
	listFiles->removeRow(i);

	ui.horizontalSlider->setMaximum(listFiles->rowCount()-1);

}

void XRAY_gui::on_horizontalSlider_valueChanged()
{

	if(listFiles!=NULL&&listFiles->rowCount()!=0)
	{

		showImage(ui.horizontalSlider->value());

	}
}

void XRAY_gui::showImage(int i)
{

	if(i>0&&i<listFiles->rowCount())
	{

		image=QPixmap(getFile(i));

		ui.labelImage->setPixmap(image);

		ui.horizontalSlider->setValue(i);

		fitImage();

	}

}

void XRAY_gui::resizeEvent(QResizeEvent *)
{

	fitImage();

}

void XRAY_gui::fitImage()
{

	if(ui.labelImage->pixmap())
	{

	int w=ui.labelImage->size().width();
	int h=ui.labelImage->size().height();

	w=std::min(image.width(),std::min(w,h));
	h=w;

	ui.labelImage->setPixmap(image.scaled(w,h,Qt::KeepAspectRatio));

	}

}

void XRAY_gui::itemcurrentChanged ( const QModelIndex & current)
{

	showImage(ui.listView->currentIndex().row()-1);



}

QString XRAY_gui::getFile(int i)
{
	return ui.lineEdit->text()+"/"+listFiles->stringList().at(i);
}

class ThreadLoader : public QThread
{
	Q_OBJECT

protected:
	void run()
	{

	}

signals:
	void finished();
};



void XRAY_gui::on_loadVolumePushButton_clicked()
{

	
	pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud(new pcl::PointCloud<pcl::PointXYZI>);

	//std::vector<std::string> vec;

	parameters = new LoadingParam();

	parameters->matrix=new VolumeData();
	parameters->matrix->pixSize=ui.pixSizedoubleSpinBox->value();
	parameters->pointCloud=pointCloud;
	parameters->threshold=ui.prethresholdspinBox->value();
	//parameters->pixSize=ui.pixSizedoubleSpinBox->value();

	for(int i=0;i<listFiles->stringList().size();i++)
		parameters->vec.push_back(getFile(i).toStdString());

	if(parameters->vec.size()>0)
	{

	

	QFutureWatcher<void> * watcher = new QFutureWatcher<void>();
	connect(watcher,SIGNAL(finished()),this,SLOT(handleLoading()));

	QFuture<void> result =
		QtConcurrent::run(&XRAY_gui::startLoading,*parameters);

	watcher->setFuture(result);

	//imageVolumeLoader::loadDataSet(vec,pointCloud,&(test.voxels),test.w,test.h,test.d,0,11,false,0);

	this->setEnabled(false);

	}
	else
		delete parameters;

}

void XRAY_gui::startLoading(LoadingParam &param)
{

	imageVolumeLoader::loadDataSet(param.vec,param.pointCloud,&(param.matrix->voxels),param.matrix->w,param.matrix->h,param.matrix->d,param.threshold,param.matrix->pixSize,param.Smoothing,param.SmoothingInt);

	
}

void XRAY_gui::handleLoading()
{


	QString name="Volume "+QString::number(volumes.size());

	this->setEnabled(true);

	addCloud(parameters->pointCloud, name);
	
	volumes.insert(name,parameters->matrix);

	pointClouds.insert(name,static_cast<void*>(parameters->pointCloud.get()));

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("3D",Qt::MatchContains|Qt::MatchRecursive);

	item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));

	//ui.treeWidget->insertTopLevelItem(


	delete parameters;

	viewer->resetCamera();

	viewer->spin();

	
}

void XRAY_gui::addCloud( pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, QString &name ) 
{
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(pointCloud, "intensity");
	viewer->addPointCloud<pcl::PointXYZI>((pcl::PointCloud<pcl::PointXYZI>::ConstPtr)pointCloud,handler,name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.5, name.toStdString());
	
}


void XRAY_gui::update()
{


	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS_EX pmc;

	// Print the process identifier.

	printf( "\nProcess ID: %u\n", GetCurrentProcessId() );

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, GetCurrentProcessId() );
	if (NULL == hProcess)
		return;

	if ( GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)) )
	{
		printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
		printf( "\tPeakWorkingSetSize: 0x%08X\n", 
			pmc.PeakWorkingSetSize );
		printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
		printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPeakPagedPoolUsage );
		printf( "\tQuotaPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPagedPoolUsage );
		printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPeakNonPagedPoolUsage );
		printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaNonPagedPoolUsage );
		printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
		printf( "\tPeakPagefileUsage: 0x%08X\n", 
			pmc.PeakPagefileUsage );
	}

	CloseHandle( hProcess );

	usagelabelStatus->setText( QString::number(pmc.PrivateUsage/1048576) + " M bytes");


}


void XRAY_gui::updateNotify(std::string message)
{
	QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest),
		Qt::LowEventPriority);
	QMetaObject::invokeMethod((QObject*)this, "secureThreadUpdate", Q_ARG(QString,QString::fromStdString(message)));
}

void XRAY_gui::secureThreadUpdate(QString data)
{

	ui.textBrowser->append(data);//+"\r\n");

}

bool _internal=false;

void XRAY_gui::on_treeWidget_itemSelectionChanged()
{
	_internal=true;
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			double tmp;
			//ui.propertiesdockWidget->setDisabled(false);
			ui.propertiesdockWidget->setEnabled(true);
			viewer->getPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, tmp, items[0]->text(0).toStdString());
			ui.pointsizespinBox->setValue(tmp);
			viewer->getPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY,tmp, items[0]->text(0).toStdString());
			ui.opacitydoubleSpinBox->setValue(tmp);
		}
		else
			ui.propertiesdockWidget->setEnabled(false);
	}
	_internal=false;
}

void XRAY_gui::on_pointsizespinBox_valueChanged(int i)
{
	if(_internal)
		return;
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, ui.pointsizespinBox->value(), items[0]->text(0).toStdString());
			viewer->spin();
		}
	}
}

void XRAY_gui::on_opacitydoubleSpinBox_valueChanged(double i)
{
	if(_internal)
		return;
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, ui.opacitydoubleSpinBox->value(), items[0]->text(0).toStdString());
			viewer->spin();
		}
	}
}

VolumeData * XRAY_gui::getActiveVolume()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(volumes[items[0]->text(0)])
		{
			return volumes[items[0]->text(0)];
		}
	}

	return NULL;
}

void * XRAY_gui::getActiveCloud()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			return pointClouds[items[0]->text(0)];
		}
	}

	return NULL;
}

matVoxel * XRAY_gui::getActiveSkeleton()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(algData[items[0]->text(0)])
		{
			return algData[items[0]->text(0)];
		}
	}

	return NULL;
}






ThreadLoaderSkeletonWorker::ThreadLoaderSkeletonWorker(matVoxel * vox, pcl::PointCloud<pcl::PointXYZI> * cloud2,VolumeData * currentData) : vox(vox), cloud2(cloud2),currentData(currentData)
	{

	}

void ThreadLoaderSkeletonWorker::run()
	{
		vox->skeletonize(currentData->voxels,currentData->w,currentData->h,currentData->d);

		vox->skeletonToPoints(cloud2, vox->voxels, vox->w, vox->h, vox->d, currentData->pixSize);

		cloud2->width=cloud2->size();
		cloud2->height=1;

		emit getRes(vox,cloud2,currentData);
	}



void XRAY_gui::on_skeletonizepushButton_clicked()
{

	VolumeData * currentData=getActiveVolume();

	if(currentData)
	{
		this->setEnabled(false);

		matVoxel *vox=new matVoxel();
		/*vox.voxels=currentData->voxels;
		vox.w=currentData->w;
		vox.h=currentData->h;
		vox.d=currentData->d;*/

		

		pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZI>());
		
		QThread * thread=new QThread();

		ThreadLoaderSkeletonWorker * threadworker = new ThreadLoaderSkeletonWorker(vox,cloud2.get(),currentData);

		threadworker->moveToThread(thread);

		connect(thread,SIGNAL(started()),threadworker,SLOT(run()));
		connect(threadworker,SIGNAL(getRes(matVoxel * , pcl::PointCloud<pcl::PointXYZI> * ,VolumeData * )),this,SLOT(skeleton(matVoxel * , pcl::PointCloud<pcl::PointXYZI> * ,VolumeData * )));
		
		connect(threadworker, SIGNAL(finished()), thread, SLOT(quit()));
		connect(threadworker, SIGNAL(finished()), threadworker, SLOT(deleteLater()));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		

		thread->start();

		//imageVolumeLoader::correctCenter(pcl::PointCloud<pcl::PointXYZI>::Ptr((pcl::PointCloud<pcl::PointXYZI>*)getActiveCloud()), (float)vox->w / 2*currentData->pixSize, (float)vox->h / 2*currentData->pixSize, (float)vox->d / 2*currentData->pixSize, vox->cogX, vox->cogY, vox->cogZ);

		
		

	}

}

void XRAY_gui::skeleton(matVoxel * voxel, pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2,VolumeData * currentData)
{
	QString name="Skeleton MA "+QString::number(pointClouds.size());

	addCloud(cloud2,name);

	pointClouds.insert(name,static_cast<void*>(cloud2.get()));

	algData.insert(name,voxel);

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("skeletons",Qt::MatchContains|Qt::MatchRecursive);

	item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));

	this->setEnabled(true);

	viewer->spin();

}