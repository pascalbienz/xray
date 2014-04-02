#include "xray_gui.h"
#include "matVoxel.h"
#include "QVTKWidget.h"
#include <pcl\visualization\pcl_visualizer.h>
#include <vtkRenderWindow.h>


XRAY_gui::XRAY_gui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	matVoxel test;

	labelStatus = new QLabel("Message");
	statusBar()->addWidget(labelStatus);

	progressBarStatus = new QProgressBar();
	statusBar()->addWidget(progressBarStatus);

	QShortcut * shortcut = new QShortcut(QKeySequence(Qt::Key_Delete),ui.listView);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
	
	listFiles = new QStringListModel();

	//vtkSmartPointer<vtkRenderWindow> renderWindow = viewer.getRenderWindow();

	
	ui.qvtkWidget->SetRenderWindow(viewer.getRenderWindow());
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
	ui.listView->setModel(listFiles);

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

		image=QPixmap(ui.lineEdit->text()+"/"+listFiles->stringList().at(i));

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

void XRAY_gui::on_listView_selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{

	showImage(ui.listView->currentIndex().row()-1);



}
