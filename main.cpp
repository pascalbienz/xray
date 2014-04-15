/* \author Geoffrey Biggs */

// TEST GIT

#include <iostream>

#include <boost/thread/thread.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <pcl/filters/passthrough.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_cylinder.h>
#include "bmpConverter.h"
#include "matVoxel.h"
#include <pcl/visualization/pcl_plotter.h>

#include <pcl/PolygonMesh.h>

// --------------
// -----Help-----
// --------------
void
printUsage(const char* progName)
{
	std::cout << "\n\nUsage: " << progName << " [options]\n\n"
		<< "Options:\n"
		<< "-------------------------------------------\n"
		<< "-h           this help\n"
		<< "-s           Simple visualisation example\n"
		<< "-r           RGB colour visualisation example\n"

		<< "\n\n";
}


boost::shared_ptr<pcl::visualization::PCLVisualizer> simpleVis(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
{
	// --------------------------------------------
	// -----Open 3D viewer and add point cloud-----
	// --------------------------------------------

	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(cloud, "intensity");

	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZI>(cloud, handler, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.5, "sample cloud");
	//viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1, "sample cloud");
	viewer->addCoordinateSystem(1.0);
	viewer->initCameraParameters();
	return (viewer);
}



boost::shared_ptr<pcl::visualization::PCLVisualizer> shapesVis(pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr cloud)
{
	// --------------------------------------------
	// -----Open 3D viewer and add point cloud-----
	// --------------------------------------------
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
	viewer->addPointCloud<pcl::PointXYZRGB>(cloud, rgb, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
	viewer->addCoordinateSystem(1.0);
	viewer->initCameraParameters();

	//------------------------------------
	//-----Add shapes at cloud points-----
	//------------------------------------
	viewer->addLine<pcl::PointXYZRGB>(cloud->points[0],
		cloud->points[cloud->size() - 1], "line");
	viewer->addSphere(cloud->points[0], 0.2, 0.5, 0.5, 0.0, "sphere");

	//---------------------------------------
	//-----Add shapes at other locations-----
	//---------------------------------------
	pcl::ModelCoefficients coeffs;
	coeffs.values.push_back(0.0);
	coeffs.values.push_back(0.0);
	coeffs.values.push_back(1.0);
	coeffs.values.push_back(0.0);
	viewer->addPlane(coeffs, "plane");
	coeffs.values.clear();
	coeffs.values.push_back(0.3);
	coeffs.values.push_back(0.3);
	coeffs.values.push_back(0.0);
	coeffs.values.push_back(0.0);
	coeffs.values.push_back(1.0);
	coeffs.values.push_back(0.0);
	coeffs.values.push_back(5.0);
	viewer->addCone(coeffs, "cone");

	return (viewer);
}





unsigned int text_id = 0;
void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event,
	void* viewer_void)
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer = *static_cast<boost::shared_ptr<pcl::visualization::PCLVisualizer> *> (viewer_void);
	if (event.getKeySym() == "r" && event.keyDown())
	{
		std::cout << "r was pressed => removing all text" << std::endl;

		char str[512];
		for (unsigned int i = 0; i < text_id; ++i)
		{
			sprintf(str, "text#%03d", i);
			viewer->removeShape(str);
		}
		text_id = 0;
	}
}

void mouseEventOccurred(const pcl::visualization::MouseEvent &event,
	void* viewer_void)
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer = *static_cast<boost::shared_ptr<pcl::visualization::PCLVisualizer> *> (viewer_void);
	if (event.getButton() == pcl::visualization::MouseEvent::LeftButton &&
		event.getType() == pcl::visualization::MouseEvent::MouseButtonRelease)
	{
		std::cout << "Left mouse button released at position (" << event.getX() << ", " << event.getY() << ")" << std::endl;

		char str[512];
		sprintf(str, "text#%03d", text_id++);
		viewer->addText("clicked here", event.getX(), event.getY(), str);
	}
}

boost::shared_ptr<pcl::visualization::PCLVisualizer> interactionCustomizationVis()
{
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addCoordinateSystem(1.0);

	viewer->registerKeyboardCallback(keyboardEventOccurred, (void*)&viewer);
	viewer->registerMouseCallback(mouseEventOccurred, (void*)&viewer);

	return (viewer);
}


void loop(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer)
{

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZI>);
	pcl::copyPointCloud(*cloud, *cloud_filtered);

	pcl::PassThrough<pcl::PointXYZI> pass;
	pass.setInputCloud(cloud_filtered);
	pass.setFilterFieldName("y");
	//  pass.setFilterLimits (-0.1, 0.1);
	pass.setFilterLimits(-500, 500);
	pass.filter(*cloud_filtered);

	pass.setInputCloud(cloud_filtered);
	pass.setFilterFieldName("x");
	//  pass.setFilterLimits (-0.1, 0.1);
	pass.setFilterLimits(-500, 500);
	pass.filter(*cloud_filtered);

	pass.setInputCloud(cloud);
	pass.setFilterFieldName("x");
	//  pass.setFilterLimits (-0.1, 0.1);
	pass.setFilterLimits(-1000, 1000);
	pass.filter(*cloud);

	viewer->addPointCloud<pcl::PointXYZI>(cloud_filtered, "scene_cloud");
	viewer->spinOnce();
	//viewer->removePointCloud("sample cloud"); 
	viewer->spinOnce(100);




	// created RandomSampleConsensus object and compute the appropriated model
	pcl::SampleConsensusModelCylinder<pcl::PointXYZI, pcl::Normal>::Ptr		model_s(new pcl::SampleConsensusModelCylinder<pcl::PointXYZI, pcl::Normal>(cloud_filtered));

	std::vector<int> inliers;


	// Create the normal estimation class, and pass the input dataset to it
	pcl::NormalEstimation<pcl::PointXYZI, pcl::Normal> ne;
	ne.setInputCloud(cloud_filtered);

	// Create an empty kdtree representation, and pass it to the normal estimation object.
	// Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
	pcl::search::KdTree<pcl::PointXYZI>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZI>());
	ne.setSearchMethod(tree);

	// Output datasets
	pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);

	// Use all neighbors in a sphere of radius 3cm
	ne.setRadiusSearch(0.03);

	// Compute the features
	ne.compute(*cloud_normals);


	pcl::RandomSampleConsensus<pcl::PointXYZI> ransac(model_s);
	model_s->setInputNormals(cloud_normals);
	ransac.setMaxIterations(100);
	ransac.setDistanceThreshold(1000);
	ransac.computeModel();
	ransac.getInliers(inliers);

	cout << inliers.size();

	pcl::PointXYZ o;
	o.x = 1.0;
	o.y = 0;
	o.z = 0;
	Eigen::VectorXf coeffs;
	//coeffs.resize(7);
	ransac.getModelCoefficients(coeffs);
	pcl::ModelCoefficients mod;
	//mod.values.push_back(coeffs[0]);
	//mod.values.push_back(coeffs[1]);
	//mod.values.push_back(coeffs[2]);
	//mod.values.push_back(coeffs[3]);
	//mod.values.push_back(coeffs[4]);
	//mod.values.push_back(coeffs[5]);
	//mod.values.push_back(coeffs[6]);
	//cout << coeffs;
	mod.values.push_back(0);
	mod.values.push_back(0);
	mod.values.push_back(0);
	mod.values.push_back(10);
	mod.values.push_back(10);
	mod.values.push_back(10000);
	mod.values.push_back(100);
	viewer->addCylinder(mod, "cyl", 0);

	return;
	//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZI>);



	//pcl::copyPointCloud(*cloud, *cloud_filtered);

	float i;
	float j;
	float k;

	int a, b, c;

	cv::namedWindow("picture");

	// Creating trackbars uisng opencv to control the pcl filter limits
	cvCreateTrackbar("X_limit", "picture", &a, 30, NULL);
	cvCreateTrackbar("Y_limit", "picture", &b, 30, NULL);
	cvCreateTrackbar("Z_limit", "picture", &c, 30, NULL);

	// Starting the while loop where we continually filter with limits using trackbars and display pointcloud
	char last_c = 0;
	while (true && (last_c != 27))
	{

		/*pcl::copyPointCloud(*cloud_filtered, *cloud);

		// i,j,k Need to be adjusted depending on the pointcloud and its xyz limits if used with new pointclouds.

		i = 0.1*((float)a);
		j = 0.1*((float)b);
		k = 0.1*((float)c);

		// Printing to ensure that the passthrough filter values are changing if we move trackbars.

		cout << "i = " << i << " j = " << j << " k = " << k << endl;

		// Applying passthrough filters with XYZ limits

		pcl::PassThrough<pcl::PointXYZI> pass;
		/*pass.setInputCloud(cloud);
		pass.setFilterFieldName("y");
		//  pass.setFilterLimits (-0.1, 0.1);
		pass.setFilterLimits(-k, k);
		pass.filter(*cloud);

		pass.setInputCloud(cloud);
		pass.setFilterFieldName("x");
		// pass.setFilterLimits (-0.1, 0.1);
		pass.setFilterLimits(-j, j);
		pass.filter(*cloud);*/

		/*pass.setInputCloud(cloud);
		pass.setFilterFieldName("i");
		//  pass.setFilterLimits (-10, 10);
		pass.setFilterLimits(-i, i);
		pass.filter(*cloud);

		// Visualizing pointcloud
		viewer->addPointCloud<pcl::PointXYZI>(cloud, "scene_cloud");
		viewer->spinOnce();
		viewer->removePointCloud("scene_cloud");*/
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}



matVoxel test;

void pickPoint(const pcl::visualization::PointPickingEvent& pick);

// --------------
// -----Main-----
// --------------
int
main(int argc, char** argv)
{

	//matVoxel test;

	byte * voxels;
	int w, h, d;

	int threshold=100;
	float pixSize=11.803;

	/*int * testData = new int[100 * 100 * 100];
	for (int i = 0; i < 1000000; i++)
	{
	testData[i] = i * 1235748683453101 % 3;
	}
	test.skeletonize(testData, 100, 100, 100);*/

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);

	// --------------------------------------
	// -----Parse Command Line Arguments-----
	// --------------------------------------
	if (pcl::console::find_argument(argc, argv, "-h") >= 0)
	{
		printUsage(argv[0]);
		return 0;
	}
	bool simple(false), rgb(false), custom_c(false), normals(false),
		shapes(false), viewports(false), interaction_customization(false), transf = false;
	if (pcl::console::find_argument(argc, argv, "-s") >= 0)
	{
		simple = true;
		std::cout << "Simple visualisation example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-c") >= 0)
	{
		custom_c = true;
		std::cout << "Custom colour visualisation example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-r") >= 0)
	{
		rgb = true;
		std::cout << "RGB colour visualisation example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-n") >= 0)
	{
		normals = true;
		std::cout << "Normals visualisation example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-a") >= 0)
	{
		shapes = true;
		std::cout << "Shapes visualisation example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-v") >= 0)
	{
		viewports = true;
		std::cout << "Viewports example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-i") >= 0)
	{
		interaction_customization = true;
		std::cout << "Interaction Customization example\n";
	}
	else if (pcl::console::find_argument(argc, argv, "-p") >= 0)
	{



		std::cout << "Extract dir :\n";
		std::cout << argv[pcl::console::find_argument(argc, argv, "-p") + 1];
		imageVolumeLoader::loadDataSet(argv[pcl::console::find_argument(argc, argv, "-p") + 1], cloud, &voxels, w, h, d, threshold, pixSize);
		return 0;
	}
	else if (pcl::console::find_argument(argc, argv, "-j") >= 0)
	{


		voxels = NULL;
		std::cout << "Extract dir :\n";
		std::cout << argv[pcl::console::find_argument(argc, argv, "-j") + 1];
		imageVolumeLoader::loadDataSet(argv[pcl::console::find_argument(argc, argv, "-j") + 1], cloud, &voxels, w, h, d, threshold, pixSize);
		transf = true;
		//return 0;
	}
	else
	{
		printUsage(argv[0]);
		return 0;
	}

	// ------------------------------------
	// -----Create example point cloud-----
	// ------------------------------------
	pcl::PointCloud<pcl::PointXYZ>::Ptr basic_cloud_ptr(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr point_cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGB>);
	std::cout << "Genarating example point clouds.\n\n";
	// We're going to make an ellipse extruded along the z-axis. The colour for
	// the XYZRGB cloud will gradually go from red to green to blue.
	uint8_t r(255), g(15), b(15);
	for (float z(-1.0); z <= 1.0; z += 0.05)
	{
		for (float angle(0.0); angle <= 360.0; angle += 5.0)
		{
			pcl::PointXYZ basic_point;
			basic_point.x = 0.5 * cosf(pcl::deg2rad(angle));
			basic_point.y = sinf(pcl::deg2rad(angle));
			basic_point.z = z;
			basic_cloud_ptr->points.push_back(basic_point);

			pcl::PointXYZRGB point;
			point.x = basic_point.x;
			point.y = basic_point.y;
			point.z = basic_point.z;
			uint32_t rgb = (static_cast<uint32_t>(r) << 16 |
				static_cast<uint32_t>(g) << 8 | static_cast<uint32_t>(b));
			point.rgb = *reinterpret_cast<float*>(&rgb);
			point_cloud_ptr->points.push_back(point);

			//std::cout << basic_point.x<<"\n";
		}
		if (z < 0.0)
		{
			r -= 12;
			g += 12;
		}
		else
		{
			g -= 12;
			b += 12;
		}
	}
	basic_cloud_ptr->width = (int)basic_cloud_ptr->points.size();
	basic_cloud_ptr->height = 1;
	point_cloud_ptr->width = (int)point_cloud_ptr->points.size();
	point_cloud_ptr->height = 1;

	// ----------------------------------------------------------------
	// -----Calculate surface normals with a search radius of 0.05-----
	// ----------------------------------------------------------------
	pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> ne;
	ne.setInputCloud(point_cloud_ptr);
	pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZRGB>());
	ne.setSearchMethod(tree);
	pcl::PointCloud<pcl::Normal>::Ptr cloud_normals1(new pcl::PointCloud<pcl::Normal>);
	ne.setRadiusSearch(0.05);
	ne.compute(*cloud_normals1);

	// ---------------------------------------------------------------
	// -----Calculate surface normals with a search radius of 0.1-----
	// ---------------------------------------------------------------
	pcl::PointCloud<pcl::Normal>::Ptr cloud_normals2(new pcl::PointCloud<pcl::Normal>);
	ne.setRadiusSearch(0.1);
	ne.compute(*cloud_normals2);




	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;
	if (simple)
	{
		pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);

		if (pcl::io::loadPCDFile<pcl::PointXYZI>(argv[pcl::console::find_argument(argc, argv, "-s") + 1], *cloud) == -1) //* load the file
		{
			PCL_ERROR("Couldn't read file test_pcd.pcd \n");
			return (-1);
		}

		std::cout << "NB POINTS : " << cloud->points.size() << "\n";

		/*for (size_t i = 0; i < cloud->points.size(); ++i)
		std::cout << "    " << cloud->points[i].x
		<< " " << cloud->points[i].y
		<< " " << cloud->points[i].z << std::endl;*/

		viewer = simpleVis(cloud);

		loop(cloud, viewer);
	}
	else if (transf)
	{
		//matVoxel transform();

		test.pixSize=pixSize;

		::test.skeletonize(voxels, w, h, d);

		std::vector<int> markers_endpoints;

		::test.cog();

		::test.isolatePoints(markers_endpoints);

		pcl::PointCloud<pcl::PointXYZI>::Ptr path_cloud(new pcl::PointCloud<pcl::PointXYZI>);

		::test.findPath(markers_endpoints.at(0),path_cloud.get());

		pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZI>);

		float pixSize=11.80303;//11.8;

		test.skeletonToPoints(cloud2.get());

		imageVolumeLoader::correctCenter(cloud, (float)w / 2*pixSize, (float)h / 2*pixSize, (float)d / 2*pixSize, test.cogX*pixSize, test.cogY*pixSize, test.cogZ*pixSize);

		viewer = simpleVis(cloud);

		//pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> handler(cloud, "intensity");

		//boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
		//viewer->setBackgroundColor(0, 0, 0);
		//viewer->addPointCloud<pcl::PointXYZI>(cloud, handler, "sample cloud");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.2, "sample cloud");
		viewer->addPointCloud<pcl::PointXYZI>(cloud2, "cloud_");

		cloud2->width=cloud2->size();
		cloud2->height=1;

		std::string path = (std::string(argv[pcl::console::find_argument(argc, argv, "-j") + 1]));

		pcl::io::savePCDFileASCII(path+"/spiral.PCD", *cloud2);

		if (markers_endpoints.size()>0)
		{
		
			pcl::PointCloud<pcl::PointXYZI>::Ptr endpoints(new pcl::PointCloud<pcl::PointXYZI>());

		pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZI> single_color(endpoints, 0, 255, 0);
		viewer->addPointCloud<pcl::PointXYZI>(endpoints, single_color, "cloud endpoints");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "cloud endpoints");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 1, "cloud endpoints");


		//pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZI> single_color2(test.path_cloud, 255, 0, 0);
		pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> single_color2(path_cloud, "intensity");
		viewer->addPointCloud<pcl::PointXYZI>(path_cloud, single_color2, "cloud path");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "cloud path");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 1, "cloud path");

		pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurb(new pcl::PointCloud<pcl::PointXYZRGB>());

		Wm5::BSplineCurve3d * cur;

		test.fitCurve(100,path_cloud,nurb,&cur);

		pcl::PolygonMesh::Ptr mesh =test.toPoly(nurb);

		viewer->addPolylineFromPolygonMesh(*mesh);


		pcl::PointCloud<pcl::PointXYZI>::Ptr center_line(new pcl::PointCloud<pcl::PointXYZI>());


		std::vector<std::pair<double,double>> y_values;

		double pos=0,mean1=0,mean2=0;

		test.plans(viewer.get(),path,center_line,path_cloud->size()/10,cur,y_values,200,100,0.2,pos,mean1,mean2,true);

		//viewer->addPolylineFromPolygonMesh(test.toPoly(test.center_line),"center_line");

		viewer->addPointCloud<pcl::PointXYZI>(center_line, "cloud center line");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "cloud center line");
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 1, "cloud center line");


		pcl::visualization::PCLPlotter * plot=new pcl::visualization::PCLPlotter();
		plot->addPlotData(y_values);//addHistogramData(test.y_values);
		plot->setWindowSize(900, 600);
		plot->setYTitle("this is my own function");
		plot->plot();

		}

		viewer->spinOnce();
		//viewer->removePointCloud("sample cloud"); 
		viewer->spinOnce(100);

		

		viewer->registerPointPickingCallback(::pickPoint );


	}
	else if (shapes)
	{
		viewer = shapesVis(point_cloud_ptr);
	}
	else if (interaction_customization)
	{
		viewer = interactionCustomizationVis();
	}

	//--------------------
	// -----Main loop-----
	//--------------------
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}

void pickPoint(const pcl::visualization::PointPickingEvent& pick)
{
	int point = pick.getPointIndex();
	float x, y, z;

	pick.getPoint(x,y,z);

	/*if (point>=0&&point<path_cloud->size())
	cout << x << " - " << " - " << y << " - " << z << " " << ::test.path_cloud->points[point].intensity<<endl;*/
}


