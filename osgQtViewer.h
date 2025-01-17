#ifndef OSGQTVIWER_H
#define OSGQTVIWER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QThread>
#include <QMetaType>
#include <QMutex> 

#include "QtOSGWidget.h"
#include "osgQtType.h"
#include "FileHandler.h"

#include <map>
#include <vector>
#include <set>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

QT_BEGIN_NAMESPACE
namespace Ui { class osgQtViewer; }
QT_END_NAMESPACE

//extern QtOSGWidget *g_widget;

//extern std::map<QString, PointInfos> pointsInfos;

using namespace std;

class osgQtViewer : public QWidget
{
    Q_OBJECT

public:
	osgQtViewer(QWidget *parent = nullptr);
    ~osgQtViewer();

	void init();

	void addList(QString fileName);

private:
    Ui::osgQtViewer *ui;

	QMutex mutex;

	QStringList fileNames;
	QStringList filePathes;
	set<QString> resentFiles;

	map<QString, PointInfos> pointsInfos;

	QtOSGWidget *g_widget;

	FileHandler *fileHandler;

protected:
	void initMenu();

	void readResentFiles();

	void saveResentFiles();

	//void readTXT(QString filePath, int index);

	//void readOSG(QString filePath, int index);

	void removeNode();

	virtual void keyPressEvent(QKeyEvent *event);

private slots:
	void selectFile();

	//void readFiles(QStringList files_temp);

	//void readOneFile(QString fileName, int index);

	void updateTXT(QString filePath, QString fileName, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec4Array> colors, PointInfos point);

	void updateOSG(QString filePath, QString fileName, osg::ref_ptr<osg::Node> node, PointInfos point);

	void openResent(QAction* act);

	void checkChange(QTreeWidgetItem *item, int column);

	void showIfos(QTreeWidgetItem * item, int column);

	void showProcess(int64_t size, QString fileName);

signals:
	void selected(QStringList files_temp, int currentSize);

	void openResent(QString fileName, int index);

};
//Q_DECLARE_METATYPE(osg::ref_ptr<osg::Vec3Array>);
//Q_DECLARE_METATYPE(osg::ref_ptr<osg::Vec4Array>);
//Q_DECLARE_METATYPE(PointInfos);

#endif // OSGQTVIWER_H
