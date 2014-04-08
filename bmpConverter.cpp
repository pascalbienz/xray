#include "bmpConverter.h"

using namespace std;

//using namespace cv;

imageVolumeLoader::imageVolumeLoader()
{
}


imageVolumeLoader::~imageVolumeLoader()
{
}


//bool imageVolumeLoader::smoothing=true;
//int imageVolumeLoader::smoothingInt=10;


/*

To be used for tests purpose

*/
void imageVolumeLoader::loadDataSet( std::string path_directory, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, byte * * voxels, int &w, int &h, int &d, int threshold, float pixSize )
{
	path p(path_directory);   // p reads clearer than argv[1] in the following code

	//int threshold = 80;
	//float pixSize = 11.80303;//11.8;// *pow(10., -6);

	//smoothing=false;
	//smoothingInt=10;

	if (exists(p))    // does p actually exist?
	{
		if (is_directory(p))      // is p a directory?
		{

			cout << p << "Loading...\n";

			typedef vector<path> vec;             // store paths,
			vec v;                                // so we can sort them later

			copy(directory_iterator(p), directory_iterator(), back_inserter(v));

			sort(v.begin(), v.end());             // sort, since directory iteration
			// is not ordered on some file systems

			/*if (pointCloud == NULL)
				pointCloud = new pcl::PointCloud<pcl::PointXYZI>();*/

			/*if (voxels != NULL)
			{
			}*/

			cv::Mat image = cv::imread(v.at(5).string());

			(*voxels) = new byte[image.rows*image.cols*(v.end() - v.begin())];

			w=image.cols;
			h=image.rows;
			d = 0;// (v.end() - v.begin());
			for (vec::const_iterator it(v.begin()); it != v.end(); ++it)
				if (it->string().find(".bmp") != string::npos)
					d++;


			float z = -(float)d/2*pixSize;//(v.end()-v.begin())/2*pixSize;

			int count = 0;

			for (vec::const_iterator it(v.begin()); it != v.end(); ++it)
			{
				
				if (it->string().find(".bmp") != string::npos)
				{
					cout << "   " << *it << '\n';
					processImage(it->string(), pixSize, z, pointCloud,threshold,(*voxels)+count*image.rows*image.cols);
					z += pixSize;
					count++;
				}
			}

			//d=count;

			pointCloud->width = (int)pointCloud->points.size();
			pointCloud->height=1;

			pcl::io::savePCDFileBinary(path_directory+"/DATAFILE.PCD", *pointCloud);

		}

		else
			cout << p << "Should be a directory\n";
	}
	else
		cout << p << "does not exist\n";
}

void imageVolumeLoader::loadDataSet( std::vector<std::string> pathfiles, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, byte * * voxels, int &w, int &h, int &d, int threshold, float pixSize, bool smoothing, int smoothingAmount )
{

	if(pathfiles.size()>0)
	{

	

	cv::Mat image = cv::imread(pathfiles.at(0));

	(*voxels) = new byte[image.rows*image.cols*(pathfiles.size())];

	w=image.cols;
	h=image.rows;
	d = pathfiles.size();
	


	float z = -(float)d/2*pixSize;

	int count = 0;

	for (int it=0; it < pathfiles.size(); ++it)
	{

			notify_("   " + pathfiles[it]);
			processImage(pathfiles[it], pixSize, z, pointCloud,threshold,(*voxels)+count*image.rows*image.cols);
			z += pixSize;
			count++;
		}
	}

	//d=count;

	pointCloud->width = (int)pointCloud->points.size();
	pointCloud->height=1;


	

}

void imageVolumeLoader::correctCenter(pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, float centerX, float centerY, float centerZ, float cogX, float cogY, float cogZ)
{
	for (int i = 0; i < pointCloud->size(); i++)
	{

		pointCloud->points[i].x+=centerX-cogX;

		pointCloud->points[i].y += centerY - cogY;

		pointCloud->points[i].z += centerZ - cogZ;
	}
}



void imageVolumeLoader::processImage(string pathBmp, float pixSize, float z, pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, int threshold, byte * voxels, bool smoothing, int smoothingAmount)
{

	int width, height;

	cv::Mat image = cv::imread(pathBmp);

	if (smoothing)
	cv::GaussianBlur(image, image, cv::Size((smoothingAmount/2)*2+1, (smoothingAmount/2)*2+1), 0, 0);

	cv::Mat::MSize dimensions(image.size);

	float centerX=image.cols*pixSize/2, centerY=image.rows*pixSize/2;

	uchar*p;

	//Canny(image, image, 20, 20*3, 3);

	int channels = image.channels();

	std::cout << "Channels : " << channels << " / " << image.rows << " . " << image.cols;


	cv::Mat detected_edges;

	

	int edgeThresh = 1;
	int lowThreshold;
	int const max_lowThreshold = 10;
	int ratio = 3;
	int kernel_size = 3;


	for (int i = 0; i < image.rows; i++)
	{
		p = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols*channels; j+=3)
		{

			//if (image.at<uchar>(i, j)>10)
			if (p[j]>threshold)
			{

				pcl::PointXYZI basic_point;

				basic_point.x = (float)j*pixSize/channels - centerX;
				basic_point.y = (float)i*pixSize - centerY;
				basic_point.z = z;
				basic_point.intensity = p[j];//image.at<uchar>(i, j);

				voxels[j/channels+i*image.cols]=p[j];

				pointCloud->points.push_back(basic_point);
			}
			else
				voxels[j/channels+i*image.cols]=0;

			
		}
	}

	

}