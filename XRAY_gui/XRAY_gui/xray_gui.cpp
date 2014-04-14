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
#include "OptionsLoader.h"
#include "Resizable_rubber_band.h"


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

	//viewer->spin();

	/*selectionRectangle=new Resizable_rubber_band(ui.labelImage);
	selectionRectangle->move(0,0);
	selectionRectangle->resize(100,100);*/
}

XRAY_gui::~XRAY_gui()
{

}

/*
http://stackoverflow.com/questions/11933883/sort-filenames-naturally-with-qt

modified : find latest number in file and sort according to it
if no number if present, uses normal comparison
*/

inline int findNumberPart(const QString& sIn)
{
	QString s = "";
	int i = sIn.length();
	
	bool stop=false;

	while (--i >= 0)
	{
		if (!sIn[i].isNumber())
			if(!stop)
				continue;
			else
				break;
		else
		{
				s = sIn[i]+s;
				stop=true;
		}
		
		
	}
	if (s == "")
		return -1;
	return s.toInt();
}

bool naturalSortCallback(const QString& s1, const QString& s2)
{
	int idx1 = findNumberPart(s1);
	int idx2 = findNumberPart(s2);
	if(idx1==-1||idx2==-1)
		return s1.compare(s2);
	else
	return (idx1 < idx2);
}

QString lastDir="";

void XRAY_gui::on_toolButton_clicked()
{

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             lastDir,
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);

	lastDir=dir;

	loadDir(dir);

}

void XRAY_gui::loadDir(QString dir)
{


	QDir dirFilter= QDir::QDir(dir);
	dirFilter.setFilter(QDir::Files|QDir::NoDotAndDotDot);

	dirFilter.setNameFilters(QStringList()<<"*.bmp");
	dirFilter.filter();

	/*dirFilter.setSorting(QDir::LocaleAware|QDir::Name);
	dirFilter.sorting();*/

	auto list=dirFilter.entryList();
	qSort(list.begin(), list.end(), naturalSortCallback);

	listFiles->setStringList(list);//dirFilter.entryList());
	//ui.listView->addItems(dirFilter.entryList());
	//ui.listView->setModel(listFiles);

	ui.lineEdit->setText(dirFilter.absolutePath());

	ui.horizontalSlider->setMaximum(listFiles->rowCount()-1);

	showImage(listFiles->rowCount()/2);


}

void XRAY_gui::deleteItem()
{

	/*int i=ui.listView->currentIndex().row();
	if(i!=-1)
	listFiles->removeRow(i);*/

	
	while(ui.listView->selectionModel()->selectedRows().count()>0)
		listFiles->removeRow(ui.listView->selectionModel()->selectedRows().at(0).row());

	ui.horizontalSlider->setMaximum(listFiles->rowCount()-1);

}

void XRAY_gui::on_horizontalSlider_valueChanged()
{

	if(listFiles!=NULL&&listFiles->rowCount()!=0)
	{

		showImage(ui.horizontalSlider->value());

		ui.namelabel_13->setText(getFile(ui.horizontalSlider->value()));

	}
}

void XRAY_gui::on_prethresholdspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_gaussianspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_previewcheckBox_stateChanged(int i)
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_lspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_rspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_tspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::on_bspinBox_valueChanged()
{
	on_horizontalSlider_valueChanged();
}

void XRAY_gui::showImage(int i)
{

	if(i>0&&i<listFiles->rowCount())
	{

		//image=QPixmap(getFile(i));

		if(ui.previewcheckBox->checkState()==Qt::Checked)
		{
		cv::Mat image_ = cv::imread(getFile(i).toStdString());

		image_.convertTo(image_,CV_8UC3);

		cv::threshold(image_,image_,ui.prethresholdspinBox->value(),255,0);

		if (ui.gaussianspinBox->value()>0)
			cv::GaussianBlur(image_, image_, cv::Size((ui.gaussianspinBox->value()/2)*2+1, (ui.gaussianspinBox->value()/2)*2+1), 0, 0);


		int left=ui.lspinBox->value();
		int top=ui.tspinBox->value();
		int width=image_.cols-left-ui.rspinBox->value();
		int height=image_.rows-top-ui.bspinBox->value();

		cv::rectangle(image_,cv::Rect(left,top,width,height),CV_RGB(100,200,230),1);

			image=QPixmap::fromImage(QImage(image_.data,image_.cols,image_.rows,QImage::Format_RGB888));
		}
		else
		{
			image=QPixmap(getFile(i));
		}

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

	ui.labelImage->setPixmap(image.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation));

	}

}

QString XRAY_gui::getFile(int i)
{
	if(i<listFiles->rowCount())
		return ui.lineEdit->text()+"/"+listFiles->stringList().at(i);
	else
		return QString();
}

void XRAY_gui::itemcurrentChanged ( const QModelIndex & current)
{

	showImage(ui.listView->currentIndex().row()-1);



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

	
	loadVolumeFromListFiles();

}

QFutureWatcher<void> * XRAY_gui::loadVolumeFromListFiles()
{
	pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud(new pcl::PointCloud<pcl::PointXYZI>);

	//std::vector<std::string> vec;

	parameters = new LoadingParam();

	parameters->matrix=new VolumeData();
	parameters->matrix->pixSize=ui.pixSizedoubleSpinBox->value();
	parameters->pointCloud=pointCloud;
	parameters->threshold=ui.prethresholdspinBox->value();
	parameters->cropL=ui.lspinBox->value();
	parameters->cropR=ui.rspinBox->value();
	parameters->cropT=ui.tspinBox->value();
	parameters->cropB=ui.bspinBox->value();
	parameters->Smoothing=ui.gaussianspinBox->value()>0?true:false;
	parameters->SmoothingInt=ui.gaussianspinBox->value();
	//parameters->pixSize=ui.pixSizedoubleSpinBox->value();

	for(int i=0;i<listFiles->stringList().size();i++)
		parameters->vec.push_back(getFile(i).toStdString());

	QFutureWatcher<void> * watcher;

	if(parameters->vec.size()>0)
	{



		watcher = new QFutureWatcher<void>();
		connect(watcher,SIGNAL(finished()),this,SLOT(handleLoading()));

		QFuture<void> result =
			QtConcurrent::run(&XRAY_gui::startLoading,*parameters);

		watcher->setFuture(result);

		//imageVolumeLoader::loadDataSet(vec,pointCloud,&(test.voxels),test.w,test.h,test.d,0,11,false,0);

		enableInterface(false);

	}
	else
	{
		delete parameters;
		return NULL;
	}


	return watcher;

}

void XRAY_gui::startLoading(LoadingParam &param)
{

	imageVolumeLoader::loadDataSet(param.vec,param.pointCloud,&(param.matrix->voxels),param.matrix->w,param.matrix->h,param.matrix->d,param.threshold,param.matrix->pixSize,param.Smoothing,param.SmoothingInt,param.cropL,param.cropR,param.cropT,param.cropB);

	float cogX,cogY,cogZ;

	imageVolumeLoader::cog(param.matrix->voxels, param.matrix->w,param.matrix->h, param.matrix->d,cogX,cogY,cogZ);

	imageVolumeLoader::correctCenter(param.pointCloud, (float)(param.matrix->w) / 2*param.matrix->pixSize, (float)(param.matrix->h) / 2*param.matrix->pixSize, (float)(param.matrix->d) / 2*param.matrix->pixSize, cogX*param.matrix->pixSize, cogY*param.matrix->pixSize, cogZ*param.matrix->pixSize);

	
}

void XRAY_gui::handleLoading()
{


	QString name="Volume "+QString::number(volumes.size());

	enableInterface(true);

	addCloud(parameters->pointCloud, name);
	
	volumes.insert(name,parameters->matrix);

	pointClouds.insert(name,parameters->pointCloud);//static_cast<void*>(parameters->pointCloud.get()));

	/*QList<QTreeWidgetItem*> item = ui.treeWidget->findItems("",Qt::MatchContains|Qt::MatchRecursive);//"3D",Qt::MatchContains|Qt::MatchRecursive);

	if (item.size() > 0)
	{
	
		auto nitem=new QTreeWidgetItem(QStringList(name));
	item.at(0)->addChild(nitem);
	ui.treeWidget->clearSelection();
	ui.treeWidget->setItemSelected(nitem,true);
	}*/

	addItemToTreeWidget(name,"");

	secureThreadUpdate("Points : "+QString::number(parameters->pointCloud->points.size()));

	delete parameters;

	viewer->resetCamera();

	//viewer->spin();
	//viewer->spinOnce(1,true);

	
	
}


void XRAY_gui::addCloud( pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, QString &name , float pointSize, float opacity) 
{
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(pointCloud, "intensity");
	viewer->addPointCloud<pcl::PointXYZI>((pcl::PointCloud<pcl::PointXYZI>::ConstPtr)pointCloud,handler,name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, pointSize, name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, opacity, name.toStdString());
	
}

void XRAY_gui::addCloud( pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud, QString &name , float pointSize, float opacity) 
{
	viewer->addPointCloud<pcl::PointXYZRGB>((pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr)pointCloud,name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, pointSize, name.toStdString());
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, opacity, name.toStdString());

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
	
	// Should ultimately be removed
	QCoreApplication::processEvents();
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
			ui.qvtkWidget->update();
			//viewer->spinOnce(1,true);
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
			ui.qvtkWidget->update();
			//viewer->spinOnce(1,true);
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


QString XRAY_gui::getActiveSkeletonName()
{
	QList<QTreeWidgetItem*> items=ui.treeWidget->selectedItems();
	if(items.size()>0)
	{
		if(algData[items[0]->text(0)])
		{
			return items[0]->text(0);
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

		emit finished();
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
		Skeletonize(currentData);
	}

}

QThread * XRAY_gui::Skeletonize( VolumeData * currentData ) 
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
		connect(threadworker,SIGNAL(getRes(matVoxel * , pcl::PointCloud<pcl::PointXYZI> * ,VolumeData * )),this,SLOT(skeletonLoading(matVoxel * , pcl::PointCloud<pcl::PointXYZI> * ,VolumeData * )));

		connect(threadworker, SIGNAL(finished()), thread, SLOT(quit()));
		connect(threadworker, SIGNAL(finished()), threadworker, SLOT(deleteLater()));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));


		thread->start();

		return thread;

		//imageVolumeLoader::correctCenter(pcl::PointCloud<pcl::PointXYZI>::Ptr((pcl::PointCloud<pcl::PointXYZI>*)getActiveCloud()), (float)vox->w / 2*currentData->pixSize, (float)vox->h / 2*currentData->pixSize, (float)vox->d / 2*currentData->pixSize, vox->cogX, vox->cogY, vox->cogZ);
	}

void XRAY_gui::skeletonLoading( matVoxel * voxel, pcl::PointCloud<pcl::PointXYZI>* cloud2,VolumeData * currentData )
{
	QString name="Skeleton MA "+QString::number(pointClouds.size());

	//imageVolumeLoader::correctCenter(((pcl::PointCloud<pcl::PointXYZI>::Ptr)getActiveCloud()), (float)(currentData->w) / 2*currentData->pixSize, (float)(currentData->h) / 2*currentData->pixSize, (float)(voxel->d) / 2*currentData->pixSize, voxel->cogX*currentData->pixSize, voxel->cogY*currentData->pixSize, voxel->cogZ*currentData->pixSize);

	pcl::PointCloud<pcl::PointXYZI>::Ptr c_(cloud2);

	addCloud(c_,name,3,0.8);

	pointClouds.insert(name,c_);//static_cast<void*>(cloud2));

	algData.insert(name,voxel);

	/*QList<QTreeWidgetItem*> item = ui.treeWidget->findItems(QString::fromStdString(getActiveCloudName()),Qt::MatchContains|Qt::MatchRecursive);//"skeletons",Qt::MatchContains|Qt::MatchRecursive);

	auto skeletonItem=new QTreeWidgetItem(QStringList(name));

	item.at(0)->addChild(skeletonItem);


	ui.treeWidget->clearSelection();
	ui.treeWidget->setItemSelected(skeletonItem,true);*/

	addItemToTreeWidget(name,QString::fromStdString(getActiveCloudName()));

	

	enableInterface(true);

	//viewer->spin();
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(getActiveCloud(), "intensity");
	viewer->updatePointCloud(getActiveCloud(),handler,getActiveCloudName());
	viewer->updateCamera();
	ui.qvtkWidget->update();
	//viewer->spinOnce(1,true);

	secureThreadUpdate("Skeleton points : "+QString::number(c_->points.size()));

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

	//viewer->spinOnce(1,true);


}


void XRAY_gui::on_cleanPathpushButton_clicked()
{

	matVoxel * skeleton_=getActiveSkeleton();

	if(skeleton_!=NULL)
	{

		pruning(skeleton_);

		
		//viewer->spinOnce(1,true);

	}

}


void XRAY_gui::pruning(matVoxel * skeleton_)
{

	secureThreadUpdate("Finding paths");
	QCoreApplication::processEvents();

	std::vector<int> endpoints_indices;

		skeleton_->isolatePoints(endpoints_indices);

		pcl::PointCloud<pcl::PointXYZI>::Ptr endpointsPointCloud(new pcl::PointCloud<pcl::PointXYZI>());

		skeleton_->vectorToPointCloud(endpointsPointCloud,endpoints_indices);

		if(endpointsPointCloud->size()>0)
		{


			QString name="End points "+QString::number(pointClouds.size());

			/*QList<QTreeWidgetItem*> item = ui.treeWidget->findItems(getActiveSkeletonName(),Qt::MatchContains|Qt::MatchRecursive);//"points",Qt::MatchContains|Qt::MatchRecursive);

			pointClouds.insert(name,endpointsPointCloud);

			item.at(0)->addChild(new QTreeWidgetItem(QStringList(name)));*/

			pointClouds.insert(name,endpointsPointCloud);

			addItemToTreeWidget(name,getActiveSkeletonName(),false);

			addCloud(endpointsPointCloud,name,5,0.1);

			secureThreadUpdate("End/Tail points : "+QString::number(endpointsPointCloud->points.size()));
			QCoreApplication::processEvents();
		}


		if(endpointsPointCloud->size()>0)
		{

			pcl::PointCloud<pcl::PointXYZI>::Ptr longestShortestPathCloud(new pcl::PointCloud<pcl::PointXYZI>());

			skeleton_->findPath(endpoints_indices.at(0),longestShortestPathCloud);

			QString name="Path cloud "+QString::number(pointClouds.size());

			addItemToTreeWidget(name,getActiveSkeletonName());//"Paths"

			pointClouds.insert(name,longestShortestPathCloud);


			addCloud(longestShortestPathCloud,name,3,0.8);


			secureThreadUpdate("Path points : "+QString::number(longestShortestPathCloud->points.size()));
			QCoreApplication::processEvents();
		}
}


void XRAY_gui::addItemToTreeWidget(QString itemName, QString parentName, bool afterSelect)
{

	QList<QTreeWidgetItem*> itemParent = ui.treeWidget->findItems(parentName,Qt::MatchContains|Qt::MatchRecursive);
	if(itemParent.size()>0)
	{
	auto itemChild=new QTreeWidgetItem(QStringList(itemName));
	itemParent.at(0)->addChild(itemChild);
	if(afterSelect)
	{
		ui.treeWidget->clearSelection();
		ui.treeWidget->setItemSelected(itemChild,true);
		ui.treeWidget->expandAll();
	}}

}

void XRAY_gui::on_splinepushButton_4_clicked()
{

	//matVoxel * currentSkeleton=getActiveSkeleton();

	auto cloud=getActiveCloud();

	if(cloud!=NULL)
	{

		if(cloud->points.size()>1000000)
		{
			QMessageBox::information(this,"Error","Too many points (probably trying to fit a curve to a volume)");
			return;
		}
	
		if(cloud->points.size()>0)
		fitCurve(cloud);

	}


}

Wm5::BSplineCurve3d * XRAY_gui::fitCurve(pcl::PointCloud<pcl::PointXYZI>::Ptr cl)
{

	secureThreadUpdate("Fitting curve : "+QString::number( cl->size() )+" points.");
	QCoreApplication::processEvents();

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurbPoints(new pcl::PointCloud<pcl::PointXYZRGB>());

	Wm5::BSplineCurve3d * curve;

	matVoxel::fitCurve(ui.orderspinBox->value(),cl,nurbPoints,&curve);

	pcl::PolygonMesh::Ptr mesh = matVoxel::toPoly(nurbPoints);

	QString name="Spline "+QString::number(meshes.size());

	//QList<QTreeWidgetItem*> item = ui.treeWidget->findItems(QString::fromStdString(getActiveCloudName()),Qt::MatchContains|Qt::MatchRecursive);//"B-Splines",Qt::MatchContains|Qt::MatchRecursive);

	meshes.insert(name,mesh.get());

	curves.insert(name,curve);

	addItemToTreeWidget(name,QString::fromStdString(getActiveCloudName()),true);



	viewer->addPolylineFromPolygonMesh(*mesh,name.toStdString());



//	viewer->updateCamera();
	//viewer->spinOnce(1,true);

	return curve;

}

void XRAY_gui::on_actionExport_settings_triggered()
{

	QString path=QFileDialog::getSaveFileName(this, tr("Save settings"),
		"","",&QString("*.xml"));

	QMap<QString,QString> listParameters;


	listParameters.insert("Path",ui.lineEdit->text());
	if(listFiles&&listFiles->rowCount()>0)
	{
		listParameters.insert("FirstImage",listFiles->stringList().at(0));
		listParameters.insert("LastImage",listFiles->stringList().last());
	}
	listParameters.insert("PixSize",QString::number(ui.pixSizedoubleSpinBox->value()));
	listParameters.insert("PreThresholding",QString::number(ui.prethresholdspinBox->value()));
	listParameters.insert("GaussianEdgeRefine",QString::number(ui.gaussianspinBox->value()));
	listParameters.insert("PathSmoothingOrder",QString::number(ui.orderspinBox->value()));
	listParameters.insert("PathReconstructionSubdivisions",QString::number(ui.subdividespinBox->value()));
	listParameters.insert("PathReconstructionFramesPerSubdivision",QString::number(ui.framesperdivspinBox->value()));
	listParameters.insert("DetectionOfEndPercentageThreshold",QString::number(ui.endthresholdspinBox->value()));
	listParameters.insert("PlaneDimensions",QString::number(ui.planedimspinBox->value()));
	listParameters.insert("CropLeft",QString::number(ui.lspinBox->value()));
	listParameters.insert("CropRight",QString::number(ui.rspinBox->value()));
	listParameters.insert("CropTop",QString::number(ui.tspinBox->value()));
	listParameters.insert("CropBottom",QString::number(ui.bspinBox->value()));


	OptionsLoader::writeXml(path,listParameters);
}

void XRAY_gui::on_actionImport_settings_triggered()
{

	QString path=QFileDialog::getOpenFileName(this, tr("Open settings"),
		"","",&QString("*.xml"));

	QMap<QString,QString> listParameters=OptionsLoader::loadXmlFile(path);


	//ui.lineEdit->setText(listParameters["Path"]);
	loadDir(listParameters["Path"]);
	if(listFiles&&listFiles->rowCount()>0)
	{
		int start=listFiles->stringList().indexOf(listParameters["FirstImage"]);
		int end=listFiles->stringList().indexOf(listParameters["LastImage"]);
		QStringList list=listFiles->stringList().mid(start,end-start+1);
		listFiles->setStringList(list);
	}
	ui.pixSizedoubleSpinBox->setValue(listParameters["PixSize"].toDouble());
	ui.prethresholdspinBox->setValue(listParameters["PreThresholding"].toDouble());
	ui.gaussianspinBox->setValue(listParameters["GaussianEdgeRefine"].toDouble());
	ui.orderspinBox->setValue(listParameters["PathSmoothingOrder"].toDouble());
	ui.subdividespinBox->setValue(listParameters["PathReconstructionSubdivisions"].toDouble());
	ui.framesperdivspinBox->setValue(listParameters["PathReconstructionFramesPerSubdivision"].toDouble());
	ui.endthresholdspinBox->setValue(listParameters["DetectionOfEndPercentageThreshold"].toDouble());
	ui.planedimspinBox->setValue(listParameters["PlaneDimensions"].toDouble());
	ui.lspinBox->setValue(listParameters["CropL"].toInt());
	ui.rspinBox->setValue(listParameters["CropR"].toInt());
	ui.tspinBox->setValue(listParameters["CropT"].toInt());
	ui.bspinBox->setValue(listParameters["CropB"].toInt());



	//OptionsLoader::writeXml(path,listParameters);
}

void XRAY_gui::on_runpushButton_clicked()
{
	run();
}

void XRAY_gui::run()
{

	auto threadVolume = loadVolumeFromListFiles();

	if(!threadVolume)
		return;

	while(!threadVolume->isFinished())
	{
		QCoreApplication::processEvents();
		Sleep(100);
	}

	if(getActiveCloud())
	{
		std::string volumecloudName=getActiveCloudName();

	QThread * th=Skeletonize(getActiveVolume());

	if(!th)
		return;

	while(!th->isFinished())
	{
		QCoreApplication::processEvents();
		Sleep(100);
	}
	
	QCoreApplication::processEvents();

	matVoxel * skeleton=getActiveSkeleton();//algData.values().last();

	pruning(skeleton);

	QCoreApplication::processEvents();

	if(QString::fromStdString(getActiveCloudName()).contains("Path"))
	{

		auto pathCloud=getActiveCloud();

		Wm5::BSplineCurve3d * curve = fitCurve(pathCloud);

		pcl::PointCloud<pcl::PointXYZI>::Ptr center_line(new pcl::PointCloud<pcl::PointXYZI>());

		std::vector<std::pair<double,double>> y_values;

		secureThreadUpdate("Correction of the path center");
		QCoreApplication::processEvents();

		double endPositiononCurve=0;

		double meanW=0, meanH=0;

		skeleton->plans(NULL,"",center_line,pathCloud->size()/2,curve,y_values,ui.planedimspinBox->value(),ui.prethresholdspinBox->value(),(double)ui.endthresholdspinBox->value()/100,endPositiononCurve,meanW,meanH,true);

		QString name="Center Line "+QString::number(pointClouds.size());
		pointClouds.insert(name,center_line);
		addCloud(center_line,name,5,0.8);
		addItemToTreeWidget(name,QString::fromStdString(volumecloudName));

		pathCloud=getActiveCloud();

		curve = fitCurve(pathCloud);

		pcl::PointCloud<pcl::PointXYZI>::Ptr endLine(new pcl::PointCloud<pcl::PointXYZI>());

		skeleton->projectBack(curve,endLine,ui.prethresholdspinBox->value(),ui.planedimspinBox->value());

		name="Starting points "+QString::number(pointClouds.size());
		pointClouds.insert(name,endLine);
		addCloud(endLine,name,5,0.8);
		addItemToTreeWidget(name,QString::fromStdString(volumecloudName));

		pcl::PointCloud<pcl::PointXYZRGB>::Ptr endLine2(new pcl::PointCloud<pcl::PointXYZRGB>());
		pcl::copyPointCloud(*endLine,*endLine2);
		pcl::PolygonMesh::Ptr mesh = matVoxel::toPoly(endLine2);
		meshes.insert(name,mesh.get());
		viewer->addPolylineFromPolygonMesh(*mesh,name.toStdString());


		secureThreadUpdate("Mean height : "+QString::number(meanH)+" um Mean width : "+QString::number(meanW)+" um");

	}


	}


}

/*void XRAY_gui::correctCenterLine()
{

}*/