/********************************************************************************
** Form generated from reading UI file 'uu.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UU_H
#define UI_UU_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_uuClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *uuClass)
    {
        if (uuClass->objectName().isEmpty())
            uuClass->setObjectName(QString::fromUtf8("uuClass"));
        uuClass->resize(600, 400);
        menuBar = new QMenuBar(uuClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        uuClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(uuClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        uuClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(uuClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        uuClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(uuClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        uuClass->setStatusBar(statusBar);

        retranslateUi(uuClass);

        QMetaObject::connectSlotsByName(uuClass);
    } // setupUi

    void retranslateUi(QMainWindow *uuClass)
    {
        uuClass->setWindowTitle(QApplication::translate("uuClass", "uu", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class uuClass: public Ui_uuClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UU_H
