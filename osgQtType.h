#pragma once
#include <QString>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

typedef struct PointInfos
{
	QString fileName;
	QString structure;
	int structureSize;
	int pointSize;
	int64_t fileSize;
	float max_x;
	float max_y;
	float max_z;
	float min_x;
	float min_y;
	float min_z;
	float center_x;
	float center_y;
	float center_z;
	int index;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr point;
};

