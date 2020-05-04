#include "FileHandler.h"

#include <fstream>
#include <string>
#include <boost/algorithm/string/split.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QProgressDialog>

using namespace std;

int64_t FileHandler::getProcess()
{
	return myProcess;
}

//读txt
void FileHandler::readTXT(QString filePath, int index)
{
	PointInfos point;//存储信息

	//转换为char *
	QByteArray cpath = filePath.toLocal8Bit();
	char *name = cpath.data();
	qDebug() << filePath;

	//ifstream fin(name, ios::binary);
	FILE *fp;
	if (fopen_s(&fp, name, "rb") != 0)
	{
		qDebug() << "DataFile does not exist!!!";
	}
	else
	{
		//文件大小

		//fin.seekg(0, ios_base::end);
		//point.fileSize = fin.tellg();
		//fin.clear();//重置
		//fin.seekg(std::ios::beg);//重置为开头
		rewind(fp);
		int point_size = 0;
		QString file_full, file_name, file_path, file_suffix;
		QFileInfo fileinfo;
		fileinfo = QFileInfo(filePath);
		file_name = fileinfo.fileName();//文件名
		file_path = fileinfo.absolutePath();
		_fseeki64(fp,0,SEEK_END);
		int64_t size = _ftelli64(fp);
		point.fileSize = size;
		_fseeki64(fp, 0, SEEK_SET);

		//读取点云结构
		//int col_temp = 0;
		//char a[512];
		//string sa;
		//fin.getline(a, 512, '\n');
		//stringstream ssa;
		//ssa << a;
		//while (ssa >> sa)
		//{
		//	col_temp++;
		//}
		int col_temp = 0;
		char s[200];
		fgets(s,200,fp);
		vector<string> ssplit;
		boost::split(ssplit,s,boost::is_any_of(" "),boost::token_compress_on);
		col_temp = ssplit.size();
		_fseeki64(fp,0, SEEK_SET);

		point.structureSize = col_temp;
		point.fileName = file_name;
		point.index = index;

		//fin.clear();//重置
		//fin.seekg(std::ios::beg);//重置为开头

		qDebug() << point.structureSize;
		if (point.structureSize == 6)
		{
			point.structure = QString("xyzrgb");
			//double max_x = DBL_MIN, min_x = DBL_MAX, max_y = DBL_MIN, min_y = DBL_MAX, max_z = DBL_MIN, min_z = DBL_MAX;
			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr p(new pcl::PointCloud<pcl::PointXYZRGB>);
			pcl::PointXYZRGB t;
			//stringstream ss;
			emit beginToRead(point.fileSize, file_name);
			int r = 0, g = 0, b = 0;
			float x = 0.0, y = 0.0, z = 0.0;
			char xs[100], ys[100], zs[100], rs[100], gs[100], bs[100];
			while (fscanf(fp,"%s %s %s %s %s %s\n", xs,ys,zs,rs,gs,bs)==6 &&!feof(fp))
			{
				//qDebug() << stod(xs) << endl << ys << endl << zs << endl << rs << endl;
				//string s;
				//myProcess = fin.tellg();
				//int r, g, b;
				//getline(fin, s, '\n');
				//point_size++;
				//ss << s;
				//ss >> t.x >> t.y >> t.z >> r >> g >> b;
				t.x = stof(xs); t.y = stof(ys); t.z = stof(zs);
				r = stoi(rs); g = stoi(gs); b = stoi(bs);
				//qDebug() << x << y << z << r << g << b;
				if (myProcess % 20000 == 0)
					qDebug() << t.x << t.y << t.z << r << g << b;
				myProcess = _ftelli64(fp);
				if (myProcess > size - 100)
				qDebug() << myProcess << endl;
				int32_t frgb = 0;
				frgb = (int)r << 16 | (int)g << 8 | (int)b;
				t.rgb = *reinterpret_cast<float*>(&frgb);
				p->push_back(t);
				//ss.clear();
				//ss.str("");
				////计算包围盒
				//min_x = min_x < t.x ? min_x : t.x;
				//max_x = max_x > t.x ? max_x : t.x;
				//min_y = min_y < t.y ? min_y : t.y;
				//max_y = max_y > t.y ? max_y : t.y;
				//min_z = min_z < t.z ? min_z : t.z;
				//max_z = max_z > t.z ? max_z : t.z;
			}
			//fin.close();
			fclose(fp);
			myProcess = size;//针对某些文件读不到末尾
			for (int64_t i = 0; i < p->size(); i++) {
				vertices->push_back(osg::Vec3f(p->points[i].x, p->points[i].y, p->points[i].z));
				colors->push_back(osg::Vec4f(p->points[i].r / 255.0, p->points[i].r / 255.0, p->points[i].r / 255.0, 1.0));
			}
			qDebug() << "ok";

			//point.max_x = max_x; point.max_y = max_y; point.max_z = max_z;
			//point.min_x = min_x; point.min_y = min_y; point.min_z = min_z;
			//point.center_x = (max_x + min_x) / 2; point.center_y = (max_y + min_y) / 2; point.center_z = (max_z + min_z) / 2;
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

//读osg
void FileHandler::readOSG(QString filePath, int index)
{
	QString file_full, file_name, file_path, file_suffix;
	QFileInfo fileinfo;
	fileinfo = QFileInfo(filePath);
	file_name = fileinfo.fileName();//文件名
	file_path = fileinfo.absolutePath();//文件路径
	emit beginToRead(10, file_name);

	PointInfos point;//存储信息

	point.fileName = file_name;
	point.index = index;

	myProcess = 1;

	QByteArray cpath = filePath.toLocal8Bit();
	char *filename = cpath.data();

	myProcess = 4;

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename);


	myProcess = 9;
	//addList(file_name);

	//fileNames.push_back(file_name);
	//filePathes.push_back(filePath);
	//resentFiles.insert(filePath);

	//g_widget->addNode(node, file_name);
	emit toUpdateOSG(filePath, file_name, node, point);
}

//读取文件
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

//单个文件读取
void FileHandler::readOneFile(QString filePath, int index)
{
	QString file_full, file_name, file_path, file_suffix;
	QFileInfo fileinfo;
	fileinfo = QFileInfo(filePath);
	//文件名
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
