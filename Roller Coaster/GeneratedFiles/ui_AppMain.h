/********************************************************************************
** Form generated from reading UI file 'AppMain.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPMAIN_H
#define UI_APPMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppMainClass
{
public:
    QAction *aExit;
    QAction *aLoadPath;
    QAction *aSavePath;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QVBoxLayout *mainLayout;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *AppMainClass)
    {
        if (AppMainClass->objectName().isEmpty())
            AppMainClass->setObjectName(QString::fromUtf8("AppMainClass"));
        AppMainClass->resize(1028, 769);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AppMainClass->sizePolicy().hasHeightForWidth());
        AppMainClass->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\273\237\346\255\243\351\273\221\351\253\224"));
        AppMainClass->setFont(font);
        AppMainClass->setLayoutDirection(Qt::LeftToRight);
        AppMainClass->setAutoFillBackground(true);
        AppMainClass->setStyleSheet(QString::fromUtf8(""));
        AppMainClass->setDockNestingEnabled(false);
        aExit = new QAction(AppMainClass);
        aExit->setObjectName(QString::fromUtf8("aExit"));
        aLoadPath = new QAction(AppMainClass);
        aLoadPath->setObjectName(QString::fromUtf8("aLoadPath"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/AppMain/Resources/Icons/openfile.ico"), QSize(), QIcon::Normal, QIcon::Off);
        aLoadPath->setIcon(icon);
        aLoadPath->setIconVisibleInMenu(true);
        aSavePath = new QAction(AppMainClass);
        aSavePath->setObjectName(QString::fromUtf8("aSavePath"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/AppMain/Resources/Icons/savefile.ico"), QSize(), QIcon::Normal, QIcon::Off);
        aSavePath->setIcon(icon1);
        centralWidget = new QWidget(AppMainClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        centralWidget->setMinimumSize(QSize(0, 0));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
				label->setFixedHeight(10);
				label->setText("Normal");

        verticalLayout->addWidget(label);

        mainLayout = new QVBoxLayout();
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

        verticalLayout->addLayout(mainLayout);

        AppMainClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(AppMainClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1028, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        AppMainClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(AppMainClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        AppMainClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(AppMainClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        AppMainClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(aLoadPath);
        menuFile->addAction(aSavePath);
        menuFile->addSeparator();
        menuFile->addAction(aExit);
        mainToolBar->addAction(aLoadPath);
        mainToolBar->addAction(aSavePath);

        retranslateUi(AppMainClass);

        QMetaObject::connectSlotsByName(AppMainClass);
    } // setupUi

    void retranslateUi(QMainWindow *AppMainClass)
    {
        AppMainClass->setWindowTitle(QCoreApplication::translate("AppMainClass", "AppMain", nullptr));
        aExit->setText(QCoreApplication::translate("AppMainClass", "Exit", nullptr));
        aLoadPath->setText(QCoreApplication::translate("AppMainClass", "Load Path", nullptr));
        aSavePath->setText(QCoreApplication::translate("AppMainClass", "Save Path", nullptr));
        label->setText(QCoreApplication::translate("AppMainClass", "TextLabel", nullptr));
        menuFile->setTitle(QCoreApplication::translate("AppMainClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AppMainClass: public Ui_AppMainClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPMAIN_H
