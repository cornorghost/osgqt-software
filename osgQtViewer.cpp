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

//��ʼ���˵���
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

	//����ļ�
	readResentFiles();

	set<QString>::iterator it; //����ǰ�������
	for (it = resentFiles.begin(); it != resentFiles.end(); it++)
	{
		if (fileRsent->actions().size() > 6) break;

		QAction *resent = new QAction(*it);

		fileRsent->addAction(resent);

	}

	fileMenu->addMenu(fileRsent);

	m_bar->addMenu(fileMenu);

	connect(openAct, SIGNAL(triggered()), this, SLOT(selectFile())); //ѡ���ļ�

	connect(fileRsent, SIGNAL(triggered(QAction*)), this, SLOT(openResent(QAction*)));
}

//��ʼ��
void osgQtViewer::init()
{
	qDebug() << "in init";
	qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	//g_widget = new QtOSGWidget(ui->widget);
	
	//����osg��ͼ
	g_widget = new QtOSGWidget();

	ui->gridLayout->addWidget(g_widget);

	g_widget->setMinimumWidth(900);

	//�����ļ���ȡΪ�߳�
	fileHandler = new FileHandler;

	m_objThread = new QThread();

	fileHandler->moveToThread(m_objThread);

	m_objThread->start();

	//��ʼ����ʾ��treewidget
	ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//ֻ�����ɱ༭
	ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//���ñ��Ϊ����ѡ��
	ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//����������ѡ��

	QPushButton *add = new QPushButton(g_widget);//��osg��ͼ������ť

	initMenu();

	//������Ϣ��
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->verticalHeader()->hide();


	qRegisterMetaType<osg::ref_ptr<osg::Vec3Array>>("osg::ref_ptr<osg::Vec3Array>");
	qRegisterMetaType<osg::ref_ptr<osg::Vec4Array>>("osg::ref_ptr<osg::Vec4Array>");
	qRegisterMetaType<osg::ref_ptr<osg::Node>>("osg::ref_ptr<osg::Node>");
	qRegisterMetaType<PointInfos>("PointInfos");
	qRegisterMetaType<int64_t>("int64_t");
	
	//�����ź�
	connect(this, SIGNAL(selected(QStringList, int)), fileHandler, SLOT(readFiles(QStringList, int))); //��ȡ�ļ�

	connect(this, SIGNAL(openResent(QString, int)), fileHandler, SLOT(readOneFile(QString, int))); //��ȡ����ļ�

	connect(fileHandler, &FileHandler::toUpdateTXT, this, &osgQtViewer::updateTXT);//����txt

	connect(fileHandler, &FileHandler::toUpdateOSG, this, &osgQtViewer::updateOSG);//����osg

	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *,int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));//��ʾ�����ؽڵ�

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(showIfos(QTreeWidgetItem *, int)));//��ʾ��Ϣ

	connect(fileHandler, SIGNAL(beginToRead(int64_t, QString)), this, SLOT(showProcess(int64_t, QString)));//��ʾ������

}

//��ȡ�ļ�����ӵ���ʾ��
void osgQtViewer::addList(QString fileName)
{
	//qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));
	qDebug() << "addlist";
	//����һ��������  
	QTreeWidgetItem* group1 = new QTreeWidgetItem(ui->treeWidget);
	group1->setText(0, fileName);    //���οؼ���ʾ���ı���Ϣ  
	group1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);   //�������οؼ����������  
	group1->setCheckState(0, Qt::Checked); //��ʼ״̬��ѡ�� 

	ui->treeWidget->resizeColumnToContents(0);
	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(checkChange(QTreeWidgetItem *, int)));
}

//��ȡ����ļ�
void osgQtViewer::readResentFiles()
{
	qDebug() << "inresentfile";
	QString path("settingFiles");
	QDir dir;
	if (!dir.exists(path))
	{
		//·�������ڣ��򴴽�·��
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

	while (!in.atEnd()) //δ���ļ�β
	{
		QString line = in.readLine();
		resentFiles.insert(line);
		qDebug() << line;
	}
	f.close();
	qDebug() << resentFiles.size();
}

//��������ļ�
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
	set<QString>::iterator it; //����ǰ�������
	int size = 0;
	for (it = resentFiles.end(); it != resentFiles.begin(); it--)
	{
		if (++size > 5) break;
		qDebug() << *it;
		out << *it<<"\n";
	}
	f.close();

}

////��txt
//void osgQtViewer::readTXT(QString filePath, int index)
//{
//	PointInfos point;//�洢��Ϣ
//
//	//ת��Ϊchar *
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
//		//�ļ���С
//
//		//fin.seekg(0, ios_base::end);
//		//point.fileSize = fin.tellg();
//		//fin.clear();//����
//		//fin.seekg(std::ios::beg);//����Ϊ��ͷ
//
//		int point_size = 0;
//		QString file_full, file_name, file_path, file_suffix;
//		QFileInfo fileinfo;
//		fileinfo = QFileInfo(filePath);
//		file_name = fileinfo.fileName();//�ļ���
//		file_path = fileinfo.absolutePath();
//		point.fileSize = fileinfo.size();
//
//		//��ȡ���ƽṹ
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
//		fin.clear();//����
//		fin.seekg(std::ios::beg);//����Ϊ��ͷ
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
//				//�����Χ��
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
////��osg
//void osgQtViewer::readOSG(QString filePath, int index)
//{
//	QString file_full, file_name, file_path, file_suffix;
//	QFileInfo fileinfo;
//	fileinfo = QFileInfo(filePath);
//	file_name = fileinfo.fileName();//�ļ���
//	file_path = fileinfo.absolutePath();//�ļ�·��
//
//	PointInfos point;//�洢��Ϣ
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

//ѡ���ļ�
void osgQtViewer::selectFile()
{
	//�����ļ��Ի�����
	QFileDialog *fileDialog = new QFileDialog(this);
	//�����ļ��Ի������
	fileDialog->setWindowTitle(QString::fromLocal8Bit("�򿪵����ļ�"));
	//����Ĭ���ļ�·��
	fileDialog->setDirectory(".");
	//�����ļ�������
	QStringList nameFileters;
	nameFileters.push_back(tr("(*.osg *.txt *.pcd *.las)"));
	nameFileters.push_back(tr("ALL FILES(*)"));
	fileDialog->setNameFilters(nameFileters);
	//���ÿ���ѡ�����ļ�,Ĭ��Ϊֻ��ѡ��һ���ļ�QFileDialog::ExistingFiles
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	//������ͼģʽ
	fileDialog->setViewMode(QFileDialog::Detail);
	//��ӡ����ѡ����ļ���·��
	if (fileDialog->exec() == QDialog::Accepted)
	{
		QStringList files_temp;
		files_temp = fileDialog->selectedFiles();
		//��ӡѡ���ļ���Ϣ
		for (int i = 0; i < files_temp.size(); i++)
		{
			QString file_full, file_name, file_path, file_suffix;
			QFileInfo fileinfo;
			fileinfo = QFileInfo(files_temp[i]);
			//�ļ���
			//file_name = fileinfo.fileName();
			//�ļ���׺
			file_suffix = fileinfo.suffix();
			if (file_suffix != "osg" && file_suffix != "txt" && file_suffix != "pcd" && file_suffix != "las")//����ļ����Ͳ�����
			{
				QMessageBox::StandardButton reply;
				reply = QMessageBox::critical(this, tr("OPEN ERROR"), "file not supported", QMessageBox::Abort);
				return;
			}
			//����·��
			//file_path = fileinfo.absolutePath();
			//qDebug() << " " << file_name << " " << file_path << " " << file_suffix;
		}

		emit selected(files_temp,fileNames.size());//��ʼ��ȡ�ļ��ź�
	}

}

////��ȡ�ļ�
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
////�����ļ���ȡ
//void osgQtViewer::readOneFile(QString filePath, int index)
//{
//	QString file_full, file_name, file_path, file_suffix;
//	QFileInfo fileinfo;
//	fileinfo = QFileInfo(filePath);
//	//�ļ���
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
//�����ʾ������
void osgQtViewer::showProcess(int64_t size, QString fileName)
{
	qDebug() << "in process";
	qDebug() << size;
	QProgressDialog *progressDlg = new QProgressDialog(this);
	progressDlg->blockSignals(false);
	progressDlg->setWindowModality(Qt::WindowModal);
	progressDlg->setMinimumDuration(0);//dialog������ȴ���ʱ��
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


//��ȡtxt
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

//��ȡosg
void osgQtViewer::updateOSG(QString filePath, QString fileName, osg::ref_ptr<osg::Node> node, PointInfos point)
{
	qDebug() << "updatosg";
	addList(fileName);

	fileNames.push_back(fileName);
	filePathes.push_back(filePath);
	resentFiles.insert(filePath);

	g_widget->addNode(node, fileName);
}

//��ʾ��ѡ��״̬����
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
	QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();//��õ�ǰ�ڵ�

	for (int i = 0; i < items.size(); i++) {
		QTreeWidgetItem *item = items[i];
		if (NULL == item)//û��ѡ��ڵ�
			return;
		qDebug() << item->text(0);
		QTreeWidgetItem* parent = item->parent();//��õ�ǰ�ڵ�ĸ��ڵ�
		if (NULL == parent)//item����top�ڵ�
		{
			////ɾ���ӽڵ�
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
			//������ʾ��
			ui->tableWidget->setRowCount(0);
			ui->tableWidget->setColumnCount(2);
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

			g_widget->deleteNode(index * 2);
			g_widget->deleteNode(index * 2+1);
		}
		else //parent����
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


//��ʾ��Ϣ��ͬʱ������Χ��
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
	//������Χ��
	if(item->checkState(column)==Qt::Checked) g_widget->setVisible(ui->treeWidget->indexOfTopLevelItem(item)*2+1);
}

//����ļ�ѡȡ��ȡ,��
void osgQtViewer::openResent(QAction* act)
{
	qDebug() << "resentread";
	QString filepath = act->text();
	//readOneFile(filepath,fileNames.size());
	emit openResent(filepath, fileNames.size());
	
}