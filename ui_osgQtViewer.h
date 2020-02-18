/********************************************************************************
** Form generated from reading UI file 'osgQtViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OSGQTVIEWER_H
#define UI_OSGQTVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_osgQtViewer
{
public:
    QGridLayout *gridLayout_2;
    QTreeWidget *treeWidget;
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *osgQtViewer)
    {
        if (osgQtViewer->objectName().isEmpty())
            osgQtViewer->setObjectName(QString::fromUtf8("osgQtViewer"));
        osgQtViewer->resize(1280, 848);
        osgQtViewer->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_2 = new QGridLayout(osgQtViewer);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(2, 30, 2, 2);
        treeWidget = new QTreeWidget(osgQtViewer);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        treeWidget->setMinimumSize(QSize(0, 320));
        treeWidget->setMaximumSize(QSize(250, 50000));
        treeWidget->viewport()->setProperty("cursor", QVariant(QCursor(Qt::ArrowCursor)));
        treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        treeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        treeWidget->setAutoScrollMargin(16);
        treeWidget->header()->setDefaultSectionSize(100);

        gridLayout_2->addWidget(treeWidget, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 0, 1, 2, 1);

        tableWidget = new QTableWidget(osgQtViewer);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy1);
        tableWidget->setMinimumSize(QSize(0, 490));
        tableWidget->setMaximumSize(QSize(250, 16777215));
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(true);
        tableWidget->horizontalHeader()->setDefaultSectionSize(125);

        gridLayout_2->addWidget(tableWidget, 1, 0, 1, 1);


        retranslateUi(osgQtViewer);

        QMetaObject::connectSlotsByName(osgQtViewer);
    } // setupUi

    void retranslateUi(QWidget *osgQtViewer)
    {
        osgQtViewer->setWindowTitle(QApplication::translate("osgQtViewer", "osgQtViwer", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("osgQtViewer", "\346\250\241\345\236\213\347\273\223\346\236\204", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("osgQtViewer", "\345\261\236\346\200\247", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("osgQtViewer", "\345\200\274", nullptr));
    } // retranslateUi

};

namespace Ui {
    class osgQtViewer: public Ui_osgQtViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OSGQTVIEWER_H
