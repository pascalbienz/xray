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

class matVoxel:public notifyClass
{
public:
	matVoxel();
	~matVoxel();
	void skeletonize(int * voxels, int w, int h, int d);
	enum VoxelType  { BACKGROUND, OBJECT, EMPTY };

	VoxelType * voxelTypes;
	int * voxels,w,h,d;
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

	float cogX, cogY, cogZ;
	bool sortDist(int i, int j);

	void matVoxel::isolatePoints();
	void matVoxel::findPath();

	void matVoxel::algo();
	void matVoxel::algo2();

	void matVoxel::fitCurve();

	void matVoxel::skeletonToPoints(pcl::PointCloud<pcl::PointXYZI>* pointCloud, int * voxels, int w, int h, int d, float pixSize);
	pcl::PolygonMesh matVoxel::toPoly(pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud);


	pcl::PointCloud<pcl::PointXYZI>::Ptr endpoints;
	std::vector<int> markers_endpoints;

	pcl::PointCloud<pcl::PointXYZI>::Ptr path_cloud;
	std::vector<int> path;

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurb;

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr center_line;

	//ON_NurbsCurve* cur;
	Wm5::BSplineCurve3d *cur;

	void matVoxel::plans(boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer, std::string path);
	void matVoxel::projectBack();


	void matVoxel::pca(cv::Mat image,cv::Point2d &center, cv::Point2d &vec1, cv::Point2d &vec2, double &e1, double &e2, int threshold, std::vector<cv::Point> contour=std::vector<cv::Point>());

	float matVoxel::triLinear(float x, float y, float z);

	bool init;

	int nA,nB,nC,nD;

	//std::vector<double> x_values,y_values;
	std::vector<std::pair<double,double>> x_values, y_values;
};

