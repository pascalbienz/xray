/********************************************************************************
** Form generated from reading UI file 'xray_gui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XRAY_GUI_H
#define UI_XRAY_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_XRAY_guiClass
{
public:
    QAction *actionImport_settings;
    QAction *actionExport_settings;
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QVTKWidget *qvtkWidget;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_8;
    QLabel *labelImage;
    QSlider *horizontalSlider;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    QListView *listView;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QDoubleSpinBox *pixSizedoubleSpinBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSpinBox *prethresholdspinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QSpinBox *gaussianspinBox;
    QPushButton *loadVolumePushButton;
    QDockWidget *dockWidget_2;
    QWidget *dockWidgetContents_3;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_5;
    QDockWidget *dockWidget_3;
    QWidget *dockWidgetContents_4;
    QFormLayout *formLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QSpinBox *skthresholdspinBox;
    QPushButton *skeletonizepushButton;
    QPushButton *cleanPathpushButton;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_6;
    QGridLayout *gridLayout_2;
    QSpinBox *orderspinBox;
    QLabel *label;
    QPushButton *splinepushButton_4;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_7;
    QGridLayout *gridLayout_4;
    QSpinBox *spinBox_3;
    QLabel *label_3;
    QPushButton *reconstruct_pushButton_5;
    QDockWidget *dockWidget_4;
    QWidget *dockWidgetContents_7;
    QVBoxLayout *verticalLayout_5;
    QTreeWidget *treeWidget;
    QPushButton *delpushButton;
    QDockWidget *dockWidget_5;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *verticalLayout_9;
    QTextBrowser *textBrowser;
    QDockWidget *propertiesdockWidget;
    QWidget *dockWidgetContents_5;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_9;
    QSpinBox *pointsizespinBox;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_8;
    QDoubleSpinBox *opacitydoubleSpinBox;

    void setupUi(QMainWindow *XRAY_guiClass)
    {
        if (XRAY_guiClass->objectName().isEmpty())
            XRAY_guiClass->setObjectName(QString::fromUtf8("XRAY_guiClass"));
        XRAY_guiClass->resize(1188, 902);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(XRAY_guiClass->sizePolicy().hasHeightForWidth());
        XRAY_guiClass->setSizePolicy(sizePolicy);
        XRAY_guiClass->setStyleSheet(QString::fromUtf8(""));
        actionImport_settings = new QAction(XRAY_guiClass);
        actionImport_settings->setObjectName(QString::fromUtf8("actionImport_settings"));
        actionExport_settings = new QAction(XRAY_guiClass);
        actionExport_settings->setObjectName(QString::fromUtf8("actionExport_settings"));
        centralWidget = new QWidget(XRAY_guiClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qvtkWidget = new QVTKWidget(tab);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));

        verticalLayout_2->addWidget(qvtkWidget);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_8 = new QVBoxLayout(tab_2);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        labelImage = new QLabel(tab_2);
        labelImage->setObjectName(QString::fromUtf8("labelImage"));
        sizePolicy.setHeightForWidth(labelImage->sizePolicy().hasHeightForWidth());
        labelImage->setSizePolicy(sizePolicy);
        labelImage->setMinimumSize(QSize(100, 100));
        labelImage->setAutoFillBackground(false);
        labelImage->setStyleSheet(QString::fromUtf8("background-color : black;"));
        labelImage->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(labelImage);

        horizontalSlider = new QSlider(tab_2);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);
        horizontalSlider->setTickPosition(QSlider::TicksBelow);
        horizontalSlider->setTickInterval(1);

        verticalLayout_8->addWidget(horizontalSlider);

        tabWidget->addTab(tab_2, QString());

        gridLayout_3->addWidget(tabWidget, 1, 1, 1, 1);

        XRAY_guiClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(XRAY_guiClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1188, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        XRAY_guiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(XRAY_guiClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        XRAY_guiClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(XRAY_guiClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        XRAY_guiClass->setStatusBar(statusBar);
        dockWidget = new QDockWidget(XRAY_guiClass);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setEnabled(true);
        lineEdit->setReadOnly(true);

        gridLayout->addWidget(lineEdit, 0, 0, 1, 1);

        toolButton = new QToolButton(groupBox);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        gridLayout->addWidget(toolButton, 0, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        listView = new QListView(groupBox);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setDefaultDropAction(Qt::MoveAction);
        listView->setMovement(QListView::Free);

        verticalLayout_3->addWidget(listView);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_3->addWidget(label_5);

        pixSizedoubleSpinBox = new QDoubleSpinBox(groupBox);
        pixSizedoubleSpinBox->setObjectName(QString::fromUtf8("pixSizedoubleSpinBox"));
        pixSizedoubleSpinBox->setDecimals(4);
        pixSizedoubleSpinBox->setValue(10);

        horizontalLayout_3->addWidget(pixSizedoubleSpinBox);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        prethresholdspinBox = new QSpinBox(groupBox);
        prethresholdspinBox->setObjectName(QString::fromUtf8("prethresholdspinBox"));
        prethresholdspinBox->setMaximum(255);
        prethresholdspinBox->setValue(100);

        horizontalLayout->addWidget(prethresholdspinBox);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_2->addWidget(label_4);

        gaussianspinBox = new QSpinBox(groupBox);
        gaussianspinBox->setObjectName(QString::fromUtf8("gaussianspinBox"));

        horizontalLayout_2->addWidget(gaussianspinBox);


        verticalLayout_3->addLayout(horizontalLayout_2);

        loadVolumePushButton = new QPushButton(groupBox);
        loadVolumePushButton->setObjectName(QString::fromUtf8("loadVolumePushButton"));

        verticalLayout_3->addWidget(loadVolumePushButton);

        listView->raise();
        loadVolumePushButton->raise();

        verticalLayout->addWidget(groupBox);

        dockWidget->setWidget(dockWidgetContents);
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
        dockWidget_2 = new QDockWidget(XRAY_guiClass);
        dockWidget_2->setObjectName(QString::fromUtf8("dockWidget_2"));
        dockWidget_2->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        gridLayout_6 = new QGridLayout(dockWidgetContents_3);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        groupBox_5 = new QGroupBox(dockWidgetContents_3);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_5 = new QGridLayout(groupBox_5);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));

        gridLayout_6->addWidget(groupBox_5, 0, 0, 1, 1);

        dockWidget_2->setWidget(dockWidgetContents_3);
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_2);
        dockWidget_3 = new QDockWidget(XRAY_guiClass);
        dockWidget_3->setObjectName(QString::fromUtf8("dockWidget_3"));
        dockWidget_3->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_4 = new QWidget();
        dockWidgetContents_4->setObjectName(QString::fromUtf8("dockWidgetContents_4"));
        formLayout = new QFormLayout(dockWidgetContents_4);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        groupBox_2 = new QGroupBox(dockWidgetContents_4);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_4->addWidget(label_6);

        skthresholdspinBox = new QSpinBox(groupBox_2);
        skthresholdspinBox->setObjectName(QString::fromUtf8("skthresholdspinBox"));
        skthresholdspinBox->setMaximum(255);
        skthresholdspinBox->setValue(100);

        horizontalLayout_4->addWidget(skthresholdspinBox);


        verticalLayout_4->addLayout(horizontalLayout_4);

        skeletonizepushButton = new QPushButton(groupBox_2);
        skeletonizepushButton->setObjectName(QString::fromUtf8("skeletonizepushButton"));

        verticalLayout_4->addWidget(skeletonizepushButton);

        cleanPathpushButton = new QPushButton(groupBox_2);
        cleanPathpushButton->setObjectName(QString::fromUtf8("cleanPathpushButton"));

        verticalLayout_4->addWidget(cleanPathpushButton);


        formLayout->setWidget(1, QFormLayout::SpanningRole, groupBox_2);

        groupBox_3 = new QGroupBox(dockWidgetContents_4);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_6 = new QVBoxLayout(groupBox_3);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(-1, -1, 0, -1);
        orderspinBox = new QSpinBox(groupBox_3);
        orderspinBox->setObjectName(QString::fromUtf8("orderspinBox"));
        orderspinBox->setMaximum(1000);
        orderspinBox->setValue(100);

        gridLayout_2->addWidget(orderspinBox, 0, 1, 1, 1);

        label = new QLabel(groupBox_3);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        gridLayout_2->setColumnStretch(1, 5);

        verticalLayout_6->addLayout(gridLayout_2);

        splinepushButton_4 = new QPushButton(groupBox_3);
        splinepushButton_4->setObjectName(QString::fromUtf8("splinepushButton_4"));

        verticalLayout_6->addWidget(splinepushButton_4);


        formLayout->setWidget(2, QFormLayout::SpanningRole, groupBox_3);

        groupBox_4 = new QGroupBox(dockWidgetContents_4);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_7 = new QVBoxLayout(groupBox_4);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(-1, -1, 0, -1);
        spinBox_3 = new QSpinBox(groupBox_4);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));
        spinBox_3->setMaximum(1000000);
        spinBox_3->setValue(1000);

        gridLayout_4->addWidget(spinBox_3, 0, 1, 1, 1);

        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_4->addWidget(label_3, 0, 0, 1, 1);

        gridLayout_4->setColumnStretch(1, 5);

        verticalLayout_7->addLayout(gridLayout_4);

        reconstruct_pushButton_5 = new QPushButton(groupBox_4);
        reconstruct_pushButton_5->setObjectName(QString::fromUtf8("reconstruct_pushButton_5"));

        verticalLayout_7->addWidget(reconstruct_pushButton_5);


        formLayout->setWidget(3, QFormLayout::SpanningRole, groupBox_4);

        dockWidget_3->setWidget(dockWidgetContents_4);
        groupBox_3->raise();
        groupBox_2->raise();
        groupBox_4->raise();
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_3);
        dockWidget_4 = new QDockWidget(XRAY_guiClass);
        dockWidget_4->setObjectName(QString::fromUtf8("dockWidget_4"));
        dockWidget_4->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_7 = new QWidget();
        dockWidgetContents_7->setObjectName(QString::fromUtf8("dockWidgetContents_7"));
        verticalLayout_5 = new QVBoxLayout(dockWidgetContents_7);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        treeWidget = new QTreeWidget(dockWidgetContents_7);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("Components"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(treeWidget);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setAutoExpandDelay(-1);
        treeWidget->setAnimated(true);
        treeWidget->setHeaderHidden(false);

        verticalLayout_5->addWidget(treeWidget);

        delpushButton = new QPushButton(dockWidgetContents_7);
        delpushButton->setObjectName(QString::fromUtf8("delpushButton"));

        verticalLayout_5->addWidget(delpushButton);

        dockWidget_4->setWidget(dockWidgetContents_7);
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget_4);
        dockWidget_5 = new QDockWidget(XRAY_guiClass);
        dockWidget_5->setObjectName(QString::fromUtf8("dockWidget_5"));
        dockWidget_5->setFloating(false);
        dockWidget_5->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        verticalLayout_9 = new QVBoxLayout(dockWidgetContents_2);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        textBrowser = new QTextBrowser(dockWidgetContents_2);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        verticalLayout_9->addWidget(textBrowser);

        dockWidget_5->setWidget(dockWidgetContents_2);
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(8), dockWidget_5);
        propertiesdockWidget = new QDockWidget(XRAY_guiClass);
        propertiesdockWidget->setObjectName(QString::fromUtf8("propertiesdockWidget"));
        propertiesdockWidget->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(propertiesdockWidget->sizePolicy().hasHeightForWidth());
        propertiesdockWidget->setSizePolicy(sizePolicy1);
        propertiesdockWidget->setMaximumSize(QSize(300, 90));
        propertiesdockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_5 = new QWidget();
        dockWidgetContents_5->setObjectName(QString::fromUtf8("dockWidgetContents_5"));
        verticalLayout_10 = new QVBoxLayout(dockWidgetContents_5);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_9 = new QLabel(dockWidgetContents_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_7->addWidget(label_9);

        pointsizespinBox = new QSpinBox(dockWidgetContents_5);
        pointsizespinBox->setObjectName(QString::fromUtf8("pointsizespinBox"));
        pointsizespinBox->setMaximum(50);
        pointsizespinBox->setValue(1);

        horizontalLayout_7->addWidget(pointsizespinBox);


        verticalLayout_10->addLayout(horizontalLayout_7);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_8 = new QLabel(dockWidgetContents_5);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        opacitydoubleSpinBox = new QDoubleSpinBox(dockWidgetContents_5);
        opacitydoubleSpinBox->setObjectName(QString::fromUtf8("opacitydoubleSpinBox"));
        opacitydoubleSpinBox->setMaximum(1);
        opacitydoubleSpinBox->setSingleStep(0.1);

        horizontalLayout_6->addWidget(opacitydoubleSpinBox);


        verticalLayout_10->addLayout(horizontalLayout_6);

        propertiesdockWidget->setWidget(dockWidgetContents_5);
        XRAY_guiClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), propertiesdockWidget);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionImport_settings);
        menuFile->addAction(actionExport_settings);

        retranslateUi(XRAY_guiClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(XRAY_guiClass);
    } // setupUi

    void retranslateUi(QMainWindow *XRAY_guiClass)
    {
        XRAY_guiClass->setWindowTitle(QApplication::translate("XRAY_guiClass", "XRAY Project", 0, QApplication::UnicodeUTF8));
        actionImport_settings->setText(QApplication::translate("XRAY_guiClass", "Import settings", 0, QApplication::UnicodeUTF8));
        actionExport_settings->setText(QApplication::translate("XRAY_guiClass", "Export settings", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("XRAY_guiClass", "3D", 0, QApplication::UnicodeUTF8));
        labelImage->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("XRAY_guiClass", "Images", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("XRAY_guiClass", "File", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("XRAY_guiClass", "Project", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("XRAY_guiClass", "Files", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("XRAY_guiClass", "...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("XRAY_guiClass", "Pixel size (\316\274m)", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("XRAY_guiClass", "Pre-thresholding", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("XRAY_guiClass", "Gaussian blur (Edge refining)", 0, QApplication::UnicodeUTF8));
        loadVolumePushButton->setText(QApplication::translate("XRAY_guiClass", "Load volumetric data", 0, QApplication::UnicodeUTF8));
        dockWidget_2->setWindowTitle(QApplication::translate("XRAY_guiClass", "Visualize", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("XRAY_guiClass", "Histogram", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("XRAY_guiClass", "Skeletonize", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("XRAY_guiClass", "Thresholding", 0, QApplication::UnicodeUTF8));
        skeletonizepushButton->setText(QApplication::translate("XRAY_guiClass", "Skeletonize", 0, QApplication::UnicodeUTF8));
        cleanPathpushButton->setText(QApplication::translate("XRAY_guiClass", "Clean Skeleton (Pruning)", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("XRAY_guiClass", "Smoothing", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("XRAY_guiClass", "Order", 0, QApplication::UnicodeUTF8));
        splinepushButton_4->setText(QApplication::translate("XRAY_guiClass", "Create B-Spline", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("XRAY_guiClass", "Path correction", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("XRAY_guiClass", "# Subdivisions of cross-section", 0, QApplication::UnicodeUTF8));
        reconstruct_pushButton_5->setText(QApplication::translate("XRAY_guiClass", "Reconstruct path", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->topLevelItem(0);
        ___qtreewidgetitem->setText(0, QApplication::translate("XRAY_guiClass", "Vol. data", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = ___qtreewidgetitem->child(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("XRAY_guiClass", "3D", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem1->child(0);
        ___qtreewidgetitem2->setText(0, QApplication::translate("XRAY_guiClass", "Skeletons", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem1->child(1);
        ___qtreewidgetitem3->setText(0, QApplication::translate("XRAY_guiClass", "Paths", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem1->child(2);
        ___qtreewidgetitem4->setText(0, QApplication::translate("XRAY_guiClass", "Points", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = ___qtreewidgetitem1->child(3);
        ___qtreewidgetitem5->setText(0, QApplication::translate("XRAY_guiClass", "B-Splines", 0, QApplication::UnicodeUTF8));
        treeWidget->setSortingEnabled(__sortingEnabled);

        delpushButton->setText(QApplication::translate("XRAY_guiClass", "Delete selection", 0, QApplication::UnicodeUTF8));
        dockWidget_5->setWindowTitle(QApplication::translate("XRAY_guiClass", "Info", 0, QApplication::UnicodeUTF8));
        propertiesdockWidget->setWindowTitle(QApplication::translate("XRAY_guiClass", "Properties", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("XRAY_guiClass", "Point size", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("XRAY_guiClass", "Opacity", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class XRAY_guiClass: public Ui_XRAY_guiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XRAY_GUI_H
