#include "FileHandler.h"

#include <fstream>
#include <string>

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QProgressDialog>

using namespace std;

//��txt
void FileHandler::readTXT(QString filePath, int index)
{
	PointInfos point;//�洢��Ϣ

	//ת��Ϊchar *
	QByteArray cpath = filePath.toLocal8Bit();
	char *name = cpath.data();
	qDebug() << filePath;

	ifstream fin(name, ios::binary);
	if (!fin)
	{
		qDebug() << "DataFile does not exist!!!";
	}
	else
	{
		//�ļ���С

		//fin.seekg(0, ios_base::end);
		//point.fileSize = fin.tellg();
		//fin.clear();//����
		//fin.seekg(std::ios::beg);//����Ϊ��ͷ

		int point_size = 0;
		QString file_full, file_name, file_path, file_suffix;
		QFileInfo fileinfo;
		fileinfo = QFileInfo(filePath);
		file_name = fileinfo.fileName();//�ļ���
		file_path = fileinfo.absolutePath();
		point.fileSize = fileinfo.size();

		//��ȡ���ƽṹ
		int col_temp = 0;
		char a[512];
		string sa;
		fin.getline(a, 512, '\n');
		stringstream ssa;
		ssa << a;
		while (ssa >> sa)
		{
			col_temp++;
		}
		point.structureSize = col_temp;
		point.fileName = file_name;
		point.index = index;

		fin.clear();//����
		fin.seekg(std::ios::beg);//����Ϊ��ͷ

		qDebug() << point.structureSize;
		if (point.structureSize == 6)
		{
			point.structure = QString("xyzrgb");
			double max_x = DBL_MIN, min_x = DBL_MAX, max_y = DBL_MIN, min_y = DBL_MAX, max_z = DBL_MIN, min_z = DBL_MAX;
			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr p(new pcl::PointCloud<pcl::PointXYZRGB>);
			pcl::PointXYZRGB t;
			string s;
			stringstream ss;
			process.setLabelText(file_name);
			process.setRange(0, fileinfo.size());
			while (!fin.eof())
			{
				process.setValue(fin.tellg());
				double r, g, b;
				getline(fin, s, '\n');
				point_size++;
				ss << s;
				ss >> t.x >> t.y >> t.z >> r >> g >> b;
				vertices->push_back(osg::Vec3f(t.x, t.y, t.z));
				colors->push_back(osg::Vec4f(r / 255.0, g / 255.0, b / 255.0, 1.0));
				if (fin.tellg() % 200000 == 0)
					qDebug() << t.x << t.y << t.z << r << g << b;
				int32_t frgb = 0;
				frgb = (int)r << 16 | (int)g << 8 | (int)b;
				t.rgb = *reinterpret_cast<float*>(&frgb);
				p->push_back(t);
				s.clear();
				ss.clear();
				//�����Χ��
				min_x = min_x < t.x ? min_x : t.x;
				max_x = max_x > t.x ? max_x : t.x;
				min_y = min_y < t.y ? min_y : t.y;
				max_y = max_y > t.y ? max_y : t.y;
				min_z = min_z < t.z ? min_z : t.z;
				max_z = max_z > t.z ? max_z : t.z;
			}
			ss.clear();
			fin.close();
			qDebug() << "ok";

			point.max_x = max_x; point.max_y = max_y; point.max_z = max_z;
			point.min_x = min_x; point.min_y = min_y; point.min_z = min_z;
			point.center_x = (max_x + min_x) / 2; point.center_y = (max_y + min_y) / 2; point.center_z = (max_z + min_z) / 2;
			point.pointSize = point_size;
			point.point = p;

			//pointsInfos[file_name] = point;

			//addList(point.fileName);
			//fileNames.push_back(file_name);
			//filePathes.push_back(filePath);
			//resentFiles.insert(filePath);
			//g_widget->addNode(vertices, colors, file_name);
			emit toUpdateTXT(filePath, file_name, vertices, colors, point);
		}

	}
}

//��osg
void FileHandler::readOSG(QString filePath, int index)
{
	QString file_full, file_name, file_path, file_suffix;
	QFileInfo fileinfo;
	fileinfo = QFileInfo(filePath);
	file_name = fileinfo.fileName();//�ļ���
	file_path = fileinfo.absolutePath();//�ļ�·��

	PointInfos point;//�洢��Ϣ

	point.fileName = file_name;
	point.index = index;

	QByteArray cpath = filePath.toLocal8Bit();
	char *filename = cpath.data();

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);

	//addList(file_name);

	//fileNames.push_back(file_name);
	//filePathes.push_back(filePath);
	//resentFiles.insert(filePath);

	//g_widget->addNode(node, file_name);
	toUpdateOSG(filePath, file_name, node, point);
}

//��ȡ�ļ�
void FileHandler::readFiles(QStringList files_temp, int currentSize)
{
	qDebug() << "in thread";
	qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	for (int i = 0; i < files_temp.size(); i++)
	{
		qDebug() << currentSize+i;
		readOneFile(files_temp[i], currentSize + i);
	}
}

//�����ļ���ȡ
void FileHandler::readOneFile(QString filePath, int index)
{
	QString file_full, file_name, file_path, file_suffix;
	QFileInfo fileinfo;
	fileinfo = QFileInfo(filePath);
	//�ļ���
	file_name = fileinfo.fileName();
	file_suffix = fileinfo.suffix();
	qDebug() << fileinfo.fileName();

	if (file_suffix == QString("txt"))
	{
		readTXT(filePath, index);
	}
	else if (file_suffix == QString("osg"))
	{
		readOSG(filePath, index);
	}
}
