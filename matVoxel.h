#pragma once
#include <pcl/common/common_headers.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include "ColorPrint.h"
#include <pcl/surface/on_nurbs/fitting_curve_pdm.h>
#include <pcl/surface/on_nurbs/triangulation.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/geometry/planar_polygon.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Wm5Core.h"
#include "Wm5Mathematics.h"
#include "build/notify.h"

typedef unsigned char byte;

class matVoxel:public notifyClass
{
public:
	matVoxel();
	~matVoxel();
	void skeletonize(byte * voxels, int w, int h, int d);
	//enum VoxelType  { BACKGROUND, OBJECT, EMPTY };
	class VoxelType
	{
	public:
	static byte const BACKGROUND=0;
	static byte const OBJECT=1;
	static byte const EMPTY=2;
	};

	static byte const BACKGROUND=0;
	static byte const OBJECT=1;
	static byte const EMPTY=2;

	byte * voxelTypes;
	byte * voxels;
	long long int w,h,d;
	float cogX, cogY, cogZ;
	int nA,nB,nC,nD;
	float pixSize;

	int matchA[27*6], matchB[27*12], matchC[27*8], matchD[27*12];

	int matVoxel::is26adjacent(int x, int y, int z);
	int matVoxel::is6adjacent(int x, int y, int z);
	int matVoxel::is18adjacent(int x, int y, int z);
	bool matVoxel::classA(int x, int y, int z);
	bool matVoxel::checkFaces(int x, int y, int z, int numFace);
	bool matVoxel::matchOne(int x, int y, int z, int * data);
	bool matVoxel::matchClass(int x, int y, int z);
	void matVoxel::disp(int * data);
	void matVoxel::disp(int x_, int y_, int z_);
	bool matVoxel::simplePoint(int x, int y, int z);
	bool matVoxel::topConnect26(int x, int y, int z);
	bool matVoxel::topConnect6(int x, int y, int z);
	bool matVoxel::isTail(int x, int y, int z);
	int matVoxel::isBorder(int x, int y, int z, int direction);

	bool sortDist(int i, int j);

	//void matVoxel::isolatePoints();
	void isolatePoints(std::vector<int> &markers_endpoints);
	//void matVoxel::findPath();
	void findPath(int startIndex,pcl::PointCloud<pcl::PointXYZI>::Ptr path_cloud);
	void matVoxel::algo();
	void matVoxel::algo2();

		
	void matVoxel::skeletonToPoints(pcl::PointCloud<pcl::PointXYZI>* pointCloud);//, byte * voxels, int w, int h, int d, float pixSize);
	void vectorToPointCloud(pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, std::vector<int> vec);
	
	void matVoxel::plans(pcl::visualization::PCLVisualizer* viewer, string path,pcl::PointCloud<pcl::PointXYZI>::Ptr center_line,int nbSampling,Wm5::BSplineCurve3d * cur,std::vector<std::pair<double,double>> &y_values, double length, int thresholdMeasure, double endPerThreshold, double &posEnd, double &meanW, double &meanH, bool processImages=false);

	void computePlaneMat(cv::Mat & image, double length, Wm5::Vector3d &point, Eigen::Vector3d &ax1, Eigen::Vector3d &ax2 );
	
	void projectBack(Wm5::BSplineCurve3d * cur, pcl::PointCloud<pcl::PointXYZI>::Ptr line,int threshold, int length);
	void projectBack(Wm5::BSplineCurve3d * cur, pcl::PointCloud<pcl::PointXYZI>::Ptr line,int threshold);
	void matVoxel::pca(cv::Mat image,cv::Point2d &center, cv::Point2d &vec1, cv::Point2d &vec2, double &e1, double &e2, int threshold, std::vector<cv::Point> contour=std::vector<cv::Point>());

	float matVoxel::triLinear(float x, float y, float z);
	void cog();


	static void fitCurve(int order, pcl::PointCloud<pcl::PointXYZI>::Ptr path_cloud, pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurb,Wm5::BSplineCurve3d * * cur);
	static pcl::PolygonMesh::Ptr matVoxel::toPoly(pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud);
	void retrieveCorrectCenter(std::vector<cv::Point> &contour_inter, std::vector<std::vector<cv::Point>> &contour_list, float centerX, float centerY);
	void getWidthHeight( cv::Mat param1, cv::Point2d center, double e1, double e2, double &wI, double &hI , int threshold);
	static	double getLength(Wm5::BSplineCurve3d * curve, int nbSampling);
	bool init;

	

};

