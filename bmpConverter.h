#pragma once
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace boost::filesystem;

class bmpConverter
{
public:
	bmpConverter();
	~bmpConverter();

	static void bmpConverter::loadDataSet(std::string path_directory, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, int * * voxels, int &w, int &h, int &d);//loadDataSet(std::string path);
	static void processImage(std::string pathBmp, float pixSize, float z, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud,int threshold,int * voxels=NULL);
	
	static void bmpConverter::correctCenter(pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, float centerX, float centerY, float centerZ, float cogX, float cogY, float cogZ);

	static bool smoothing;
	static int smoothingInt;

};

