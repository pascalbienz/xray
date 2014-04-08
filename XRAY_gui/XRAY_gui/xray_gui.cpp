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
#include <QMessageBox>
#include <QScrollBar>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <stdlib.h>


XRAY_gui::XRAY_gui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
	connect(timer, SIGNAL(timeout()), this, SLOT(updateProcessDisp()));
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

/*class ThreadLoader : public QThread
{
	Q_OBJECT

protected:
	void run()
	{

	}

signals:
	void finished();
};*/



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

	enableInterface(false);

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

	enableInterface(true);

	addCloud(parameters->pointCloud, name);
	
	volumes.insert(name,parameters->matrix);

	pointClouds.insert(name,parameters->pointCloud);//static_cast<void*>(parameters->pointCloud.get()));

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("3D",Qt::MatchContains|Qt::MatchRecursive);

	if (item.size() > 0)
	{
	
	item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));

	}

	//ui.treeWidget->insertTopLevelItem(


	delete parameters;

	viewer->resetCamera();

	//viewer->spin();
	viewer->spinOnce(1,true);

	
}

void XRAY_gui::addCloud( pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, QString &name ) 
{
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(pointCloud, "intensity");
	viewer->addPointCloud<pcl::PointXYZI>((pcl::PointCloud<pcl::PointXYZI>::ConstPtr)pointCloud,handler,name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.5, name.toStdString());
	
}


void XRAY_gui::updateProcessDisp()
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

	ui.textBrowser->verticalScrollBar()->setSliderPosition(ui.textBrowser->verticalScrollBar()->maximum());

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
			//viewer->spin();
			viewer->updateCamera();
			viewer->spinOnce(1,true);
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
			//viewer->spin();
			viewer->updateCamera();
			viewer->spinOnce(1,true);
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

pcl::PointCloud<pcl::PointXYZI>::Ptr XRAY_gui::getActiveCloud()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			return pointClouds[items[0]->text(0)];
		}
	}

	return pcl::PointCloud<pcl::PointXYZI>::Ptr();
}

std::string XRAY_gui::getActiveCloudName()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(pointClouds[items[0]->text(0)])
		{
			return items[0]->text(0).toStdString();
		}
	}

	return "";
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

		vox->cog();

		vox->skeletonToPoints(cloud2);

		cloud2->width=cloud2->size();
		cloud2->height=1;

		emit getRes(vox,cloud2,currentData);
	}

void XRAY_gui::enableInterface(bool v)
{
	
		ui.dockWidget->setEnabled(v);
		ui.dockWidget_2->setEnabled(v);
		ui.dockWidget_3->setEnabled(v);
		ui.dockWidget_4->setEnabled(v);
		ui.dockWidget_5->setEnabled(v);
		ui.propertiesdockWidget->setEnabled(v);
	
}

void XRAY_gui::on_skeletonizepushButton_clicked()
{

	VolumeData * currentData=getActiveVolume();

	if(currentData)
	{
		enableInterface(false);

		ui.qvtkWidget->setEnabled(true);
		ui.qvtkWidget->setDisabled(false);

		matVoxel *vox=new matVoxel();
		vox->pixSize=currentData->pixSize;
		/*vox.voxels=currentData->voxels;
		vox.w=currentData->w;
		vox.h=currentData->h;
		vox.d=currentData->d;*/

		

		pcl::PointCloud<pcl::PointXYZI> * cloud2 = new pcl::PointCloud<pcl::PointXYZI>();
		
		QThread * thread=new QThread();

		ThreadLoaderSkeletonWorker * threadworker = new ThreadLoaderSkeletonWorker(vox,cloud2,currentData);

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

void XRAY_gui::skeleton(matVoxel * voxel, pcl::PointCloud<pcl::PointXYZI>* cloud2,VolumeData * currentData)
{
	QString name="Skeleton MA "+QString::number(pointClouds.size());

	imageVolumeLoader::correctCenter(((pcl::PointCloud<pcl::PointXYZI>::Ptr)getActiveCloud()), (float)(currentData->w) / 2*currentData->pixSize, (float)(currentData->h) / 2*currentData->pixSize, (float)(voxel->d) / 2*currentData->pixSize, voxel->cogX*currentData->pixSize, voxel->cogY*currentData->pixSize, voxel->cogZ*currentData->pixSize);

	pcl::PointCloud<pcl::PointXYZI>::Ptr c_(cloud2);

	addCloud(c_,name);

	pointClouds.insert(name,c_);//static_cast<void*>(cloud2));

	algData.insert(name,voxel);

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("skeletons",Qt::MatchContains|Qt::MatchRecursive);

	item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));

	

	enableInterface(true);

	//viewer->spin();
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(getActiveCloud(), "intensity");
	viewer->updatePointCloud(getActiveCloud(),handler,getActiveCloudName());
	viewer->updateCamera();
	viewer->spinOnce(1,true);

}

void XRAY_gui::on_delpushButton_clicked()
{
	
	QString dataname = QString::fromStdString(getActiveCloudName());

	if(dataname.compare("")==0)
		return;

	//if(getActiveCloud()!=NULL||getActiveSkeleton()!=NULL)
	{
		//QString dataname=QString::fromStdString(getActiveCloudName());
		//volumes.remove(dataname);
		viewer->removePointCloud(dataname.toStdString());
		
			
			pointClouds.remove(dataname);
			
			
	}

	if(getActiveVolume()!=NULL)
	{
		//QString dataname = QString::fromStdString(getActiveCloudName());

		delete[] getActiveVolume()->voxels;
		
		delete getActiveVolume();

		volumes.remove(dataname);
	}

	if(getActiveSkeleton()!=NULL)
	{
		if(algData[dataname]!=NULL)
		{
			delete[] algData[dataname]->voxelTypes;
			delete algData[dataname];
		}

		algData.remove(dataname);
	}

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems(dataname, Qt::MatchContains | Qt::MatchRecursive);

	if (item.size() > 0)
		//ui.treeWidget->takeTopLevelItem(item.at(0)->indexOfChild());//removeItemWidget(item.at(0),0);
	{
		delete item.at(0);
	}

	viewer->spinOnce(1,true);


}


void XRAY_gui::on_cleanPathpushButton_clicked()
{

	matVoxel * skeleton_=getActiveSkeleton();

	if(skeleton_!=NULL)
	{

		std::vector<int> endpoints_indices;

		skeleton_->isolatePoints(endpoints_indices);

		pcl::PointCloud<pcl::PointXYZI>::Ptr endpointsPointCloud(new pcl::PointCloud<pcl::PointXYZI>());

		skeleton_->vectorToPointCloud(endpointsPointCloud,endpoints_indices);

		if(endpointsPointCloud->size()>0)
		{


			QString name="End points "+QString::number(pointClouds.size());

			QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("points",Qt::MatchContains|Qt::MatchRecursive);

			pointClouds.insert(name,endpointsPointCloud);

			item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));


			addCloud(endpointsPointCloud,name);

		}


		if(endpointsPointCloud->size()>0)
		{

			pcl::PointCloud<pcl::PointXYZI>::Ptr longestShortestPathCloud(new pcl::PointCloud<pcl::PointXYZI>());

			skeleton_->findPath(endpoints_indices.at(0),longestShortestPathCloud);

			QString name="Path cloud "+QString::number(pointClouds.size());

			QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("Paths",Qt::MatchContains|Qt::MatchRecursive);

			pointClouds.insert(name,longestShortestPathCloud);

			item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));

			addCloud(longestShortestPathCloud,name);

		}

		
		viewer->spinOnce(1,true);

	}

}


void XRAY_gui::on_splinepushButton_4_clicked()
{

	matVoxel * currentSkeleton=getActiveSkeleton();

	if(currentSkeleton!=NULL)
	{

		/*if(->points.size()>1000000)
		{
			QMessageBox::information(this,"Error","Too many points (probably trying to fit a curve to a volume)");
			return;
		}*/
	/*
		pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurbPoints(new pcl::PointCloud<pcl::PointXYZRGB>());

		currentSkeleton->fitCurve(ui.orderspinBox->value(),path_clo);

	pcl::PolygonMesh::Ptr mesh =currentSkeleton->toPoly(currentSkeleton->nurb);

	QString name="Spline "+QString::number(meshes.size());

	QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("B-Splines",Qt::MatchContains|Qt::MatchRecursive);

	meshes.insert(name,mesh.get());

	item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));



	viewer->addPolylineFromPolygonMesh(*mesh,name.toStdString());

	viewer->spinOnce(1,true);*/


	}


}