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
#include "build/notify.h"

typedef unsigned char byte;

using namespace boost::filesystem;

class imageVolumeLoader:public notifyClass
{
public:
	imageVolumeLoader();
	~imageVolumeLoader();

	static void imageVolumeLoader::loadDataSet(std::string path_directory, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, byte * * voxels, int &w, int &h, int &d, int threshold, float pixSize);//loadDataSet(std::string path);
	//static void loadDataSet( std::vector<std::string> pathfiles, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, byte * * voxels, int &w, int &h, int &d, int threshold, float pixSize, bool smoothing=false );
	static void loadDataSet( std::vector<std::string> pathfiles, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, byte * * voxels, int &w, int &h, int &d, int threshold, float pixSize, bool smoothing=false, int smoothingAmount=0 , int cropL=0, int cropR=0, int cropT=0, int cropB=0);
	static void processImage(std::string pathBmp, float pixSize, float z, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud,int threshold,byte * voxels=NULL, bool smoothing=false, int smoothingAmount=0, int cropL=0, int cropR=0, int cropT=0, int cropB=0);
	
	static void imageVolumeLoader::correctCenter(pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, float centerX, float centerY, float centerZ, float cogX, float cogY, float cogZ);
static	void cog(byte * voxels, int&w, int &h, int &d, float &cogX, float &cogY, float &cogZ);
	//static bool smoothing;
	//static int smoothingInt;

};

