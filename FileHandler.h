#pragma once

#include <QObject>
#include <QString>

#include <osg/ref_ptr>
#include <osg/Point>
#include <osgDB/ReadFile>

#include "osgQtType.h"

class FileHandler :public QObject
{
	Q_OBJECT
	//public:
	//	explicit FileHandler(QObject *parent = nullptr);

	protected:
		void readTXT(QString filePath, int index);

		void readOSG(QString filePath, int index);

	public slots:
		void readFiles(QStringList files_temp, int currentSize);

		void readOneFile(QString fileName, int index);

	signals:
		void toUpdateTXT(QString filePath, QString fileName, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec4Array> colors, PointInfos point);

		void toUpdateOSG(QString filePath, QString fileName, osg::ref_ptr<osg::Node> node, PointInfos point);

};

