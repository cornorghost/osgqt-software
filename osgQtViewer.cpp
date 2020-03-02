#include "osgQtViewer.h"
#include "ui_osgQtViewer.h"
#include "QtOSGWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QAction>  
#include <QLayout> 
#include <QProgressDialog>

#include <fstream>

#include <pcl/common/common.h>

using namespace std;

osgQtViewer::osgQtViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::osgQtViewer)
{
    ui->setupUi(this);
}

osgQtViewer::~osgQtViewer()
{
	saveResentFiles();
    delete ui;
}

//初始化菜单栏
void osgQtViewer::initMenu()
{
	QMenuBar *m_bar = new QMenuBar(this);

	QMenu *fileMenu = new QMenu(tr("&File"));

	QAction *openAct = new QAction(tr("&open"));

	openAct->setShortcut(tr("ctrl+O"));

	QAction *saveAct = new QAction(tr("&save"));

	saveAct->setShortcut(tr("ctrl+S"));

	fileMenu->addAction(openAct);

	fileMenu->addAction(saveAct);

	QMenu *fileRsent = new QMenu(tr("&ResentFiles"));

	//最近文件
	readResentFiles();

	set<QString>::iterator it; //定义前向迭代器
	for (it = resentFiles.begin(); it != resentFiles.end(); it++)
	{
		if (fileRsent->actions().size() > 6) break;

		QAction *resent = new QAction(*it);

		fileRsent->addAction(resent);

	}

	fileMenu->addMenu(fileRsent);

	m_bar->addMenu(fileMenu);

	connect(openAct, SIGNAL(triggered()), this, SLOT(selectFile())); //选择文件

	connect(fileRsent, SIGNAL(triggered(QAction*)), this, SLOT(openResent(QAction*)));
}

//初始化
void osgQtViewer::init()
{
	qDebug() << "in init";
	qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	//g_widget = new QtOSGWidget(ui->widget);
	
	//创建osg视图
	g_widget = new QtOSGWidget();

	ui->gridLayout->addWidget(g_widget);

	g_widget->setMinimumWidth(900);

	//设置文件读取为线程
	fileHandler = new FileHandler;

	m_objThread = new QThread();

	fileHandler->moveToThread(m_objThread);

	m_objThread->start();

	//初始化显示栏treewidget
	ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读不可编辑
	ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置表格为整行选中
	ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//设置允许多个选中

	QPushButton *add = new QPushButton(g_widget);//在osg试图创建按钮

	initMenu();

	//设置信息栏
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->verticalHeader()->hide();


	qRegisterMetaType<osg::ref_ptr<osg::Vec3Array>>("osg::ref_ptr<osg::Vec3Array>");
	qRegisterMetaType<osg::ref_ptr<osg::Vec4Array>>("osg::ref_ptr<osg::Vec4Array>");
	qRegisterMetaType<osg::ref_ptr<osg::Node>>("osg::ref_ptr<osg::Node>");
	qRegisterMetaType<PointInfos>("PointInfos");
	qRegisterMetaType<int64_t>("int64_t");
	
	//函数信号
	connect(this, SIGNAL(selected(QStringList, int)), fileHandler, SLOT(readFiles(QStringList, int))); //读取文件

	connect(this, SIGNAL(openResent(QString, int)), fileHandler, SLOT(readOneFile(QString, int))); //读取最近文件

	connect(fileHandler, &FileHandler::toUpdateTXT, this, &osgQtViewer::updateTXT);//更新txt

	connect(fileHandler, &FileHandler::toUpdateOSG, this, &osgQtViewer::updateOSG);//更新osg

	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *,int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));//显示与隐藏节点

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(showIfos(QTreeWidgetItem *, int)));//显示信息

	connect(fileHandler, SIGNAL(beginToRead(int64_t, QString)), this, SLOT(showProcess(int64_t, QString)));//显示进度条

}

//读取文件后添加到显示栏
void osgQtViewer::addList(QString fileName)
{
	//qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));
	qDebug() << "addlist";
	//定义一个树形组  
	QTreeWidgetItem* group1 = new QTreeWidgetItem(ui->treeWidget);
	group1->setText(0, fileName);    //树形控件显示的文本信息  
	group1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);   //设置树形控件子项的属性  
	group1->setCheckState(0, Qt::Checked); //初始状态被选中 

	ui->treeWidget->resizeColumnToContents(0);
	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));
}

//读取最近文件
void osgQtViewer::readResentFiles()
{
	qDebug() << "inresentfile";
	QString path("settingFiles");
	QDir dir;
	if (!dir.exists(path))
	{
		//路径不存在，则创建路径
		if (!dir.mkdir(path)) 
		{
			qDebug() << "failed";
			return;
		}
	}

	QFile f("./settingFiles/resent.init");
	if (!f.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		return;
	}
	qDebug() << "resentfile";
	QTextStream in(&f);

	while (!in.atEnd()) //未到文件尾
	{
		QString line = in.readLine();
		resentFiles.insert(line);
		qDebug() << line;
	}
	f.close();
	qDebug() << resentFiles.size();
}

//保存最近文件
void osgQtViewer::saveResentFiles()
{
	qDebug() << "savefile";
	qDebug() << resentFiles.size();
	QFile f("./settingFiles/resent.init");
	if (!f.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly))
	{
		return;
	}
	QTextStream out(&f);
	out.seek(f.size());
	set<QString>::iterator it; //定义前向迭代器
	int size = 0;
	for (it = resentFiles.end(); it != resentFiles.begin(); it--)
	{
		if (++size > 5) break;
		qDebug() << *it;
		out << *it<<"\n";
	}
	f.close();

}

////读txt
//void osgQtViewer::readTXT(QString filePath, int index)
//{
//	PointInfos point;//存储信息
//
//	//转换为char *
//	QByteArray cpath = filePath.toLocal8Bit();
//	char *name = cpath.data();
//	qDebug() << filePath;
//
//	ifstream fin(name, ios::binary);
//	if (!fin)
//	{
//		qDebug() << "DataFile does not exist!!!";
//	}
//	else
//	{
//		//文件大小
//
//		//fin.seekg(0, ios_base::end);
//		//point.fileSize = fin.tellg();
//		//fin.clear();//重置
//		//fin.seekg(std::ios::beg);//重置为开头
//
//		int point_size = 0;
//		QString file_full, file_name, file_path, file_suffix;
//		QFileInfo fileinfo;
//		fileinfo = QFileInfo(filePath);
//		file_name = fileinfo.fileName();//文件名
//		file_path = fileinfo.absolutePath();
//		point.fileSize = fileinfo.size();
//
//		//读取点云结构
//		int col_temp = 0;
//		char a[512];
//		string sa;
//		fin.getline(a, 512, '\n');
//		stringstream ssa;
//		ssa << a;
//		while (ssa >> sa)
//		{
//			col_temp++;
//		}
//		point.structureSize = col_temp;
//		point.fileName = file_name;
//		point.index = index;
//
//		fin.clear();//重置
//		fin.seekg(std::ios::beg);//重置为开头
//
//		qDebug() << point.structureSize;
//		if (point.structureSize == 6)
//		{
//			point.structure = QString("xyzrgb");
//			double max_x=DBL_MIN, min_x=DBL_MAX, max_y= DBL_MIN, min_y = DBL_MAX, max_z= DBL_MIN, min_z = DBL_MAX;
//			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
//			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
//			pcl::PointCloud<pcl::PointXYZRGB>::Ptr p(new pcl::PointCloud<pcl::PointXYZRGB>);
//			pcl::PointXYZRGB t;
//			string s;
//			stringstream ss;
//			while (!fin.eof())
//			{
//				double r, g, b;
//				getline(fin, s, '\n');
//				point_size++;
//				ss << s;
//				ss >> t.x >> t.y >> t.z >> r >> g >> b;
//				vertices->push_back(osg::Vec3f(t.x, t.y, t.z));
//				colors->push_back(osg::Vec4f(r / 255.0, g / 255.0, b / 255.0, 1.0));
//				if(fin.tellg()%200000==0)
//				qDebug() << t.x << t.y << t.z << r << g << b;
//				int32_t frgb = 0;
//				frgb = (int)r << 16 | (int)g << 8 | (int)b;
//				t.rgb = *reinterpret_cast<float*>(&frgb);
//				p->push_back(t);
//				s.clear();
//				ss.clear();
//				//计算包围盒
//				min_x = min_x < t.x ? min_x : t.x;
//				max_x = max_x > t.x ? max_x : t.x;
//				min_y = min_y < t.y ? min_y : t.y;
//				max_y = max_y > t.y ? max_y : t.y;
//				min_z = min_z < t.z ? min_z : t.z;
//				max_z = max_z > t.z ? max_z : t.z;
//			}
//			ss.clear();
//			fin.close();
//			qDebug() << "ok";
//
//			point.max_x = max_x; point.max_y = max_y; point.max_z = max_z;
//			point.min_x = min_x; point.min_y = min_y; point.min_z = min_z;
//			point.center_x = (max_x + min_x) / 2; point.center_y = (max_y + min_y) / 2; point.center_z = (max_z + min_z) / 2;
//			point.pointSize = point_size;
//			point.point = p;
//
//			pointsInfos[file_name] = point;
//
//			addList(point.fileName);
//			fileNames.push_back(file_name);
//			filePathes.push_back(filePath);
//			resentFiles.insert(filePath);
//			g_widget->addNode(vertices, colors, file_name);
//		}
//
//	}
//}
//
////读osg
//void osgQtViewer::readOSG(QString filePath, int index)
//{
//	QString file_full, file_name, file_path, file_suffix;
//	QFileInfo fileinfo;
//	fileinfo = QFileInfo(filePath);
//	file_name = fileinfo.fileName();//文件名
//	file_path = fileinfo.absolutePath();//文件路径
//
//	PointInfos point;//存储信息
//
//	point.fileName = file_name;
//	point.index = index;
//
//	QByteArray cpath = filePath.toLocal8Bit();
//	char *filename = cpath.data();
//
//	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);
//
//	addList(file_name);
//
//	fileNames.push_back(file_name);
//	filePathes.push_back(filePath);
//	resentFiles.insert(filePath);
//
//	g_widget->addNode(node, file_name);
//}

//选择文件
void osgQtViewer::selectFile()
{
	//定义文件对话框类
	QFileDialog *fileDialog = new QFileDialog(this);
	//定义文件对话框标题
	fileDialog->setWindowTitle(QString::fromLocal8Bit("打开点云文件"));
	//设置默认文件路径
	fileDialog->setDirectory(".");
	//设置文件过滤器
	QStringList nameFileters;
	nameFileters.push_back(tr("(*.osg *.txt *.pcd *.las)"));
	nameFileters.push_back(tr("ALL FILES(*)"));
	fileDialog->setNameFilters(nameFileters);
	//设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	//设置视图模式
	fileDialog->setViewMode(QFileDialog::Detail);
	//打印所有选择的文件的路径
	if (fileDialog->exec() == QDialog::Accepted)
	{
		QStringList files_temp;
		files_temp = fileDialog->selectedFiles();
		//打印选定文件信息
		for (int i = 0; i < files_temp.size(); i++)
		{
			QString file_full, file_name, file_path, file_suffix;
			QFileInfo fileinfo;
			fileinfo = QFileInfo(files_temp[i]);
			//文件名
			//file_name = fileinfo.fileName();
			//文件后缀
			file_suffix = fileinfo.suffix();
			if (file_suffix != "osg" && file_suffix != "txt" && file_suffix != "pcd" && file_suffix != "las")//如果文件类型不符合
			{
				QMessageBox::StandardButton reply;
				reply = QMessageBox::critical(this, tr("OPEN ERROR"), "file not supported", QMessageBox::Abort);
				return;
			}
			//绝对路径
			//file_path = fileinfo.absolutePath();
			//qDebug() << " " << file_name << " " << file_path << " " << file_suffix;
		}

		emit selected(files_temp,fileNames.size());//开始读取文件信号
	}

}

////读取文件
//void osgQtViewer::readFiles(QStringList files_temp)
//{
//	for (int i = 0; i < files_temp.size(); i++)
//	{
//		int index;
//		index = fileNames.size();
//		qDebug() << index;
//		readOneFile(files_temp[i], index);
//	}
//}
//
////单个文件读取
//void osgQtViewer::readOneFile(QString filePath, int index)
//{
//	QString file_full, file_name, file_path, file_suffix;
//	QFileInfo fileinfo;
//	fileinfo = QFileInfo(filePath);
//	//文件名
//	file_name = fileinfo.fileName();
//	file_suffix = fileinfo.suffix();
//	qDebug() << fileinfo.fileName();
//
//	if (file_suffix == QString("txt"))
//	{
//		readTXT(filePath, index);
//	}
//	else if (file_suffix == QString("osg"))
//	{
//		readOSG(filePath, index);
//	}
//}

/*--------------------------------------------------------------------------------------*/
//添加显示进度条
void osgQtViewer::showProcess(int64_t size, QString fileName)
{
	qDebug() << "in process";
	qDebug() << size;
	QProgressDialog *progressDlg = new QProgressDialog(this);
	progressDlg->blockSignals(false);
	progressDlg->setWindowModality(Qt::WindowModal);
	progressDlg->setMinimumDuration(0);//dialog出现需等待的时间
	progressDlg->setWindowTitle("Please Wait...");
	progressDlg->setLabelText(fileName);
	progressDlg->setCancelButtonText("Cancel");
	progressDlg->setRange(0, size);

	int64_t process = fileHandler->getProcess();
	do {
		process = fileHandler->getProcess();
		//qDebug() << i;
		if (progressDlg->wasCanceled() || process > size - 3) break;
		//qDebug() << fileHandler->getProcess();
		progressDlg->setValue(process);
	} while (process <= size);

	qDebug() << process << " " << size;
	progressDlg->setValue(size);
	qDebug() << "out process";
}


//读取txt
void osgQtViewer::updateTXT(QString filePath, QString fileName, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec4Array> colors, PointInfos point)
{
	qDebug() << "txt";
	pointsInfos[fileName] = point;
	addList(fileName);
	fileNames.push_back(fileName);
	filePathes.push_back(filePath);
	resentFiles.insert(filePath);
	g_widget->addNode(vertices, colors, fileName);
}

//读取osg
void osgQtViewer::updateOSG(QString filePath, QString fileName, osg::ref_ptr<osg::Node> node, PointInfos point)
{
	qDebug() << "updatosg";
	addList(fileName);

	fileNames.push_back(fileName);
	filePathes.push_back(filePath);
	resentFiles.insert(filePath);

	g_widget->addNode(node, fileName);
}

//显示栏选中状态动作
void osgQtViewer::checkChange(QTreeWidgetItem *item, int column)
{
	//qDebug() << "in";
	if (Qt::Checked == item->checkState(column))
	{
		//qDebug() << ui->treeWidget->indexOfTopLevelItem(item);
		g_widget->setVisible(ui->treeWidget->indexOfTopLevelItem(item) * 2);
		g_widget->setVisible(ui->treeWidget->indexOfTopLevelItem(item) * 2+1);
	}
	else
	{
		//qDebug() << ui->treeWidget->indexOfTopLevelItem(item);
		g_widget->setUnvisible(ui->treeWidget->indexOfTopLevelItem(item) * 2);
		g_widget->setUnvisible(ui->treeWidget->indexOfTopLevelItem(item) * 2+1);
	}
}

void osgQtViewer::removeNode()
{
	qDebug() << "delete";
	QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();//获得当前节点

	for (int i = 0; i < items.size(); i++) {
		QTreeWidgetItem *item = items[i];
		if (NULL == item)//没有选择节点
			return;
		qDebug() << item->text(0);
		QTreeWidgetItem* parent = item->parent();//获得当前节点的父节点
		if (NULL == parent)//item就是top节点
		{
			////删除子节点
			//if (item->childCount() != 0)
			//{
			//	QTreeWidgetItemIterator it(item);
			//	while (*it) {
			//		item->removeChild(*it);
			//		++it;
			//	}
			//}

			int index;
			index = ui->treeWidget->indexOfTopLevelItem(item);
			ui->treeWidget->takeTopLevelItem(index);
			fileNames.removeAt(index);
			filePathes.removeAt(index);
			ui->tableWidget->clearContents();
			//清理显示栏
			ui->tableWidget->setRowCount(0);
			ui->tableWidget->setColumnCount(2);
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

			g_widget->deleteNode(index * 2);
			g_widget->deleteNode(index * 2+1);
		}
		else //parent才是
		{
			int index;
			index = parent->indexOfChild(item);
			parent->removeChild(item);
			g_widget->deleteNode(index * 2);
			g_widget->deleteNode(index * 2+1);
		}
	}
	
}

void osgQtViewer::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
	{
		removeNode();
	}
}


//显示信息栏同时高亮包围盒
void osgQtViewer::showIfos(QTreeWidgetItem * item, int column)
{
	qDebug() << item->isSelected();
	QString name = item->text(0);
	QString suffix = QFileInfo(name).suffix();
	PointInfos infos = pointsInfos[name];
	int rows = ui->tableWidget->rowCount();

	ui->tableWidget->clearContents();

	if (suffix == QString("txt"))
	{
		ui->tableWidget->setRowCount(7);
		ui->tableWidget->setColumnCount(2);

		ui->tableWidget->setItem(0, 0, new QTableWidgetItem("name"));
		ui->tableWidget->setItem(0, 1, new QTableWidgetItem(infos.fileName));

		ui->tableWidget->setItem(1, 0, new QTableWidgetItem("fileSize"));
		qDebug() << infos.fileSize;
		ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(infos.fileSize)));

		ui->tableWidget->setItem(2, 0, new QTableWidgetItem("structureSize"));
		ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::number(infos.structureSize)));

		ui->tableWidget->setItem(3, 0, new QTableWidgetItem("pointSize"));
		ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::number(infos.pointSize)));

		ui->tableWidget->setItem(4, 0, new QTableWidgetItem("boudary"));
		ui->tableWidget->setItem(4, 1, new QTableWidgetItem("max_x:"+QString::number(infos.max_x) + "\n" + "max_y:" + QString::number(infos.max_y) + "\n"+ "max_z:" + QString::number(infos.max_z) + "\n"
			"min_x:" + QString::number(infos.min_x) + "\n"+ "min_y:" + QString::number(infos.min_y) + "\n"+ "min_z:" + QString::number(infos.min_z)));

		ui->tableWidget->setItem(5, 0, new QTableWidgetItem("center"));
		ui->tableWidget->setItem(5, 1, new QTableWidgetItem("center_x:" + QString::number(infos.center_x) + "\n"
			+ "center_y:" + QString::number(infos.center_y) + "\n" 
			+ "center_z:" + QString::number(infos.center_z)));

		ui->tableWidget->setItem(6, 0, new QTableWidgetItem("index"));
		ui->tableWidget->setItem(6, 1, new QTableWidgetItem(QString::number(infos.index)));

		ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		ui->tableWidget->horizontalHeader()->setMinimumSectionSize(125);
	}
	//高亮包围盒
	if(item->checkState(column)==Qt::Checked) g_widget->setVisible(ui->treeWidget->indexOfTopLevelItem(item)*2+1);
}

//最近文件选取读取,打开
void osgQtViewer::openResent(QAction* act)
{
	qDebug() << "resentread";
	QString filepath = act->text();
	//readOneFile(filepath,fileNames.size());
	emit openResent(filepath, fileNames.size());
	
}