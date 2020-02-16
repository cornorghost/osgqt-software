#ifndef OSGQTVIWER_H
#define OSGQTVIWER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QThread>

#include "QtOSGWidget.h"
#include "osgQtType.h"

#include <map>
#include <vector>
#include <set>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

QT_BEGIN_NAMESPACE
namespace Ui { class osgQtViewer; }
QT_END_NAMESPACE

extern QtOSGWidget *g_widget;

extern std::map<QString, PointInfos> pointsInfos;

using namespace std;

class osgQtViewer : public QWidget
{
    Q_OBJECT

public:
	osgQtViewer(QWidget *parent = nullptr);
    ~osgQtViewer();

	void init();

	void addList(QString fileName);

	QtOSGWidget *g_widget;
	QThread* m_objThread;
private:
    Ui::osgQtViewer *ui;
	QStringList fileNames;
	QStringList filePathes;
	set<QString> resentFiles;
	map<QString, PointInfos> pointsInfos;

protected:
	void initMenu();

	void readResentFiles();

	void saveResentFiles();

	void readTXT(QString filePath, int index);

	void readOSG(QString filePath, int index);

	void removeNode();

	virtual void keyPressEvent(QKeyEvent *event);

private slots:
	void selectFile();

	void readFiles(QStringList files_temp);

	void readOneFile(QString fileName, int index);

	void openResent(QAction* act);

	void checkChange(QTreeWidgetItem *item, int column);

	void showIfos(QTreeWidgetItem * item, int column);

signals:
	void selected(QStringList files_temp);

};
#endif // OSGQTVIWER_H
