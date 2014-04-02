#include "xray_gui.h"
#include "matVoxel.h"
#include "QVTKWidget.h"
#include <pcl\visualization\pcl_visualizer.h>
#include <vtkRenderWindow.h>
#include <windows.h>
#include <QTimer>
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

void XRAY_gui::on_loadVolumePushButton_clicked()
{

	matVoxel test;
	pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud(new pcl::PointCloud<pcl::PointXYZI>);

	std::vector<std::string> vec;

	for(int i=0;i<listFiles->stringList().size();i++)
		vec.push_back(getFile(i).toStdString());

	imageVolumeLoader::loadDataSet(vec,pointCloud,&(test.voxels),test.w,test.h,test.d,0,11,false,0);

	viewer->addPointCloud<pcl::PointXYZI>((pcl::PointCloud<pcl::PointXYZI>::ConstPtr)pointCloud);
	viewer->spinOnce();

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
	QCoreApplication::postEvent(NULL, new QEvent(QEvent::UpdateRequest),
		Qt::LowEventPriority);
	QMetaObject::invokeMethod((QObject*)this, "secureThreadUpdate", Q_ARG(QString,QString::fromStdString(message)));
}

void XRAY_gui::secureThreadUpdate(QString data)
{

	ui.textBrowser->append(data+"\r\n");

}