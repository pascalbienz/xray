#include "matVoxel.h"
#include "build/precisionTiming.h"

#include <iostream>
#include <vector>


using namespace std;

/* Indexing */
#define getAt(x,y,z) (x)+(y)*w+(z)*w*h

#define getZ(i) (i)/(w*h)
#define getX(i) (i)%w
#define getY(i) ((i)%(w*h))/w


/* Voxel neighbors : 27 indexed */
#define convertBlockDev27(x,y,z) (x+1) + (y+1) * 3 + (z+1)*9
#define idX(i) (- 1 + (i) % 3)
#define idY(i) (- 1 + ((i) % 9)/3)
#define idZ(i) (- 1 + (i) / 9)


/*



			UP

			z
			         NORTH
			|     y
			|   /
	EAST	d  / h       WEST
			| /
			|/
			|-------------> x

	SOUTH	w

			DOWN

	*/


matVoxel::matVoxel()
{
	init = false;
	nA=nB=nC=nD=0;
}


matVoxel::~matVoxel()
{
}


/*
Used for the template matching algorithm

numFace : 0 yz, 1 xz, 2 xy
Checks if every voxels of a face is object only
*/
bool matVoxel::checkFaces(int x, int y, int z, int numFace)
{
	for (int i = 0; i < 9; i++)
	{
		int offset1 = (i % 3) - 1;
		int offset2 = (i / 3) - 1;

		switch (numFace)
		{
		case 0:
			if (voxelTypes[getAt(x, y + offset1, z + offset2)] == OBJECT)
				return false;
			break;
		case 1:
			if (voxelTypes[getAt(x + offset1, y, z + offset2)] == OBJECT)
				return false;
			break;
		case 2:
			if (voxelTypes[getAt(x + offset1, y + offset2, z)] == OBJECT)
				return false;
			break;

		}
	}

	return true;
}

/*
In general : Lazy evaluation and no loops to ensure max speed

Specific check for class A (equivalent to applying several matchOne with class A)
*/
bool matVoxel::classA(int x, int y, int z)
{

	return	(voxelTypes[getAt(x, y, z)] == OBJECT)&&((voxelTypes[getAt(x + 1, y, z)] == OBJECT&&checkFaces(x - 1, y, z, 0)) ||
		(voxelTypes[getAt(x - 1, y, z)] == OBJECT&&checkFaces(x + 1, y, z, 0)) ||
		(voxelTypes[getAt(x, y + 1, z)] == OBJECT&&checkFaces(x, y - 1, z, 1)) ||
		(voxelTypes[getAt(x, y - 1, z)] == OBJECT&&checkFaces(x, y + 1, z, 1)) ||
		(voxelTypes[getAt(x, y, z + 1)] == OBJECT&&checkFaces(x, y, z - 1, 2)) ||
		(voxelTypes[getAt(x, y, z - 1)] == OBJECT&&checkFaces(x, y, z + 1, 2)));

}


/*
	Returns the number of 6 adjacent background voxels
*/
int matVoxel::is6adjacent(int x, int y, int z)
{
	return (voxelTypes[getAt(x + 1, y, z)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y + 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y - 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y, z - 1)] == BACKGROUND);
}

/*
	Returns the number of 18 ajdacent background voxels
*/
int matVoxel::is18adjacent(int x, int y, int z)
{
	return (is6adjacent(x, y, z)) +
		(voxelTypes[getAt(x + 1, y + 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y - 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y + 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y - 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y + 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y + 1, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y - 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y - 1, z - 1)] == BACKGROUND);
}

/*
Returns wether the voxel is adjacent to a background one
*/
int matVoxel::is26adjacent(int x, int y, int z)
{
	return (is18adjacent(x, y, z)) +
		(voxelTypes[getAt(x + 1, y + 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y + 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y + 1, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y + 1, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y - 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y - 1, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x + 1, y - 1, z - 1)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y - 1, z - 1)] == BACKGROUND)
		;
}


enum { UP, DOWN, NORTH, SOUTH, EAST, WEST, };

/*
Checks the borderness property of a voxel, related to a specific direction. ie. north south east west up and down
*/
int matVoxel::isBorder(int x, int y, int z, int direction)
{
	if (voxelTypes[getAt(x, y, z)]==BACKGROUND)
		return false;

	switch (direction)
	{
	case UP:
		return (voxelTypes[getAt(x, y, z + 1)] == BACKGROUND);
		break;
	case EAST:
		return (voxelTypes[getAt(x + 1, y, z)] == BACKGROUND);
		break;
	case NORTH:
		return (voxelTypes[getAt(x, y + 1, z)] == BACKGROUND);
		break;
	case WEST:
		return (voxelTypes[getAt(x - 1, y, z)] == BACKGROUND);
		break;
	case SOUTH:
		return (voxelTypes[getAt(x, y - 1, z)] == BACKGROUND);
		break;
	case DOWN:
		return (voxelTypes[getAt(x, y, z - 1)] == BACKGROUND);
		break;
	}
}

/*
This function is used to generate templates by rotation.
Inside a class, templates are simply rotations of themselves (90 degrees according to axis)
*/
void rotate(int&x, int&y, int nbRotations)
{

	for (int i = 0; i < nbRotations; i++)
	{
		int temp = x;
		x = y;
		y = -temp;
	}

}

/*
Match a voxel with an element of class
*/
bool matVoxel::matchOne(int x, int y, int z, int * data)
{
	for (int i = 0; i < 27; i++)
	{
		if (data[i] != voxelTypes[getAt(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9)] && data[i]!=EMPTY)//voxelTypes[getAt(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9)] != EMPTY)
			return false;
	}
	return true;
}

/*
Returns wether the point is a simple point or not. (i.e. it can be removed easily from the object currently being
skeletonized, without destroying topology or centricity..)
*/
bool matVoxel::simplePoint(int x, int y, int z)
{
	return topConnect26(x,y,z)&&topConnect6(x,y,z);
	for (int i = 0; i < 27; i++)
	{
		//cout << x - 1 + i % 3 << "-" << y - 1 + (i % 9) / 3 << "-" << z - 1 + i / 9<<endl;

		if (voxelTypes[getAt(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9)]==BACKGROUND||i==13)
			continue;
		if (is26adjacent(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9) ==0)
		{
			//disp(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9
			//	);
			return true;
		}
	}

	return false;

}

/* x, y, z */
int list6Neighbor[] =
{0,0,-1,
0,-1,0,
-1,0,0,
1,0,0,
0,1,0,
0,0,1};

/*int directions6Neighbor[] =
{
-1,0,0,
1,0,0,
0,-1,0,
0,1,0,

-1, 0, 0,
1, 0, 0,
0, 0, -1,
0, 0, 1,

0, -1, 0,
0, 1, 0,
0, 0, -1,
0, 0, 1,

0, -1, 0,
0, 1, 0,
0, 0, -1,
0, 0, 1,

-1, 0, 0,
1, 0, 0,
0, 0, -1,
0, 0, 1,

-1, 0, 0,
1, 0, 0,
0, -1, 0,
0, 1, 0,

};*/

//int direction


/*
 For multi-threading, it is crucial to make sure memory accessed by threads are
	exclusive when writing. The tmp structure just serves as a temporary
	reusable holder for the data, preventing re-allocation.
	The hardcoded 30 is here the max number of threads that can work
	simulataneously. In this case, parallelization is
	most efficient when the number of threads is equal to
	the number of cores, i.e. 4-8.
*/

struct tmp
{

std::vector<int> list;//(30);
int listF[30];
//int startList;//=0;
//int nextList;//=0;

bool check[27];// = { false };

tmp()

{
	list.reserve(30);
	//startList=0;
	for(int i=0;i<27;i++)
		check[i]=0;
	for(int i=0;i<30;i++)
		listF[i]=0;
}

};

tmp tmpContainers[20]; // max num thread

/*
Returns if the voxel is a tail or end-point. X-----
*/
bool matVoxel::isTail(int x, int y, int z)
{
	int nObj = 0;
	
	tmp * opt =&tmpContainers[omp_get_thread_num()];

	for (int i = 0; i < 27; i++)
	{
		if (i == 13)
			continue;
		if (voxelTypes[getAt(x + idX(i), y + idY(i), z + idZ(i))] == OBJECT)
		{
			nObj++;
			opt->check[i]=true;
		}
		else
			opt->check[i]=false;
	}

	return (nObj == 1);// || (nObj == 2 && ((::check[10] && ::check[14]) ^ (::check[10] && ::check[22]) ||
		//(::check[16] && ::check[12]) ^ (::check[22] && ::check[12]) ||
		//(::check[16] && ::check[1]) ^ (::check[14] && ::check[1])));
}


char directions1[27][27*3];
char directionL1[27]={0};

char directions2[27][27*3];
char directionL2[27]={0};

/*
 In order to accelerate the characterization of points, generates the set of available direction for each of the voxels
 in a 0-27 indexed neighbor set
*/
void generateSetOfPossibleDirections()
{
	for (int c = 0; c < 27; c++)
	{
		 directionL1[c]=0;
		 directionL2[c]=0;
	}

	for (int c = 0; c < 27; c++)
	{

		for (int i = 0; i < 27; i++)
		{
			int _x = idX(c) + idX(i);
			int _y = idY(c) + idY(i);
			int _z = idZ(c) + idZ(i);
			if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0) || convertBlockDev27(_x, _y, _z) == c)
				continue;
		
			directions1[c][directionL1[c]*3]=_x;
			directions1[c][directionL1[c]*3+1]=_y;
			directions1[c][directionL1[c]*3+2]=_z;

			directionL1[c]++;
		}

	}


	for (int c = 0; c < 27; c++)
	{

		for (int i = 0; i < 6; i++)
		{
			int _x = idX(c) + list6Neighbor[i * 3];
			int _y = idY(c) + list6Neighbor[i * 3 + 1];
			int _z = idZ(c) + list6Neighbor[i * 3 + 2];
			if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0) || (abs(_x) + abs(_y) + abs(_z)>2)|| convertBlockDev27(_x, _y, _z) == c)
				continue;


			directions2[c][directionL2[c]*3]=_x;
			directions2[c][directionL2[c]*3+1]=_y;
			directions2[c][directionL2[c]*3+2]=_z;

			directionL2[c]++;

		}

	}

}

/*
Checking connectivity of the 6-adjacent object neighbours within the set of 26-adjacents object
Note : Already optimized with custom list.
Discarding impossible moves by using pre-calculated
directions for every 26 voxels.
*/
bool matVoxel::topConnect26(int x, int y, int z)
{

	tmp * opt =&tmpContainers[omp_get_thread_num()];

	/*for (int i = 0; i < 27; i++)
	{
	//cout << x - 1 + i % 3 << "-" << y - 1 + (i % 9) / 3 << "-" << z - 1 + i / 9<<endl;

	if (voxelTypes[getAt(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9)] == BACKGROUND || i == 13)
	continue;
	if (is26adjacent(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9) == 0)
	{
	//disp(x - 1 + i % 3, y - 1 + (i % 9) / 3, z - 1 + i / 9
	//	);
	return true;
	}
	}*/

	int nObj = 0;
	int checkPos = 0;

	int startList=0;
	int nextList=0;

	/*for (int i = 0; i < 6; i++)
	{
	if (voxelTypes[getAt(x + list6Neighbor[i * 3], y + list6Neighbor[i * 3 + 1], z + list6Neighbor[i * 3 + 2])] == BACKGROUND)
	{
	nBack++;
	checkPos = convertBlockDev27(list6Neighbor[i * 3],list6Neighbor[i * 3 + 1],list6Neighbor[i * 3 + 2]);
	}
	}*/

	for (int i = 0; i < 27; i++)
	{
		if (i == 13)
			continue;
		if (voxelTypes[getAt(x + idX(i), y + idY(i), z + idZ(i))] == OBJECT)
		{
			nObj++;
			checkPos = i;
		}
	}

	if (nObj == 0)//||nObj==1)
	{
		return false;
		//return true;
	}
	else
	{
		//disp(x, y, z);
		//std::vector<int> list;
		//::list.clear();
		//::list.push_back(checkPos);
		startList=0;
		opt->listF[startList] = checkPos;
		nextList=1;


		//for (int i = 0;i<)
		//bool check[27] = { false };

		for (int i = 0; i < 27; i++)
			opt->check[i] = false;

		int c = 0;
		int nb = 1;
		while (nextList!=startList)//::list.size() > 0)
		{
			c = opt->listF[startList];//::list.at(0);//back();
			opt->check[c] = true;
			//::list.erase(::list.begin());//pop_back();
			startList++;
			/*for (int i = 0; i < 27; i++)
			{
				int _x = idX(c) + idX(i);
				int _y = idY(c) + idY(i);
				int _z = idZ(c) + idZ(i);
				if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0))
					continue;
				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == OBJECT&&!opt->check[convertBlockDev27(_x, _y, _z)])
				{
					opt->check[convertBlockDev27(_x, _y, _z)] = true;
					nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));
					opt->listF[nextList] = convertBlockDev27(_x, _y, _z);
					nextList++;
				}
			}*/

			for(int i=0;i<directionL1[c];i++)
			{
				int _x=directions1[c][i*3];
				int _y=directions1[c][i*3+1];
				int _z=directions1[c][i*3+2];

				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == OBJECT&&!opt->check[convertBlockDev27(_x, _y, _z)])
				{
					opt->check[convertBlockDev27(_x, _y, _z)] = true;
					nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));
					opt->listF[nextList] = convertBlockDev27(_x, _y, _z);
					nextList++;
				}
			}
		}

		if (nb == nObj)
			return true;
		else
		{
			//disp(x, y, z);
			return false;
		}
	}


}

//bool check[27] = { false };


/*
Checking connectivity of 6-adjacent background neighbours within the set of 18-adjacent background vox
Comments below are the unoptimized version of the lines following, for reference.
(i.e. not thread-safe, less efficient data structure, unnecessary memory allocation )
*/
bool matVoxel::topConnect6(int x, int y, int z)
{


	int nBack = 0;
	int checkPos = 0;

	for (int i = 0; i < 6; i++)
	{
		if (voxelTypes[getAt(x + list6Neighbor[i * 3], y + list6Neighbor[i * 3 + 1], z + list6Neighbor[i * 3 + 2])] == BACKGROUND)
		{
			nBack++;
			checkPos = convertBlockDev27(list6Neighbor[i * 3], list6Neighbor[i * 3 + 1], list6Neighbor[i * 3 + 2]);
		}
	}



	if (nBack == 0)
	{
		return false;
	}
	else
	{
		int tID=omp_get_thread_num();
		tmp * opt =&tmpContainers[omp_get_thread_num()];

		//OutputDebugString((boost::lexical_cast<std::string>(tID)+"\n").c_str());

		for (int i = 0; i < 27; i++)
			opt->check[i] = false;

		//::list.clear();
		//::list.push_back(checkPos);

		int startList = 0;
		opt->listF[startList] = checkPos;
		int nextList = 1;

		//for (int i = 0;i<)
		
		int c = 0;
		int nb = 1;
		while (startList!=nextList)//::list.size() > 0)
		{
			/*c = ::list.at(0);;
			::check[c] = true;
			::list.erase(::list.begin());*/

			c = opt->listF[startList];//::list.at(0);//back();
			opt->check[c] = true;
			//::list.erase(::list.begin());;
			startList++;

			for (int i = 0; i < 6; i++)
			{
				int _x = idX(c) + list6Neighbor[i * 3];
				int _y = idY(c) + list6Neighbor[i * 3 + 1];
				int _z = idZ(c) + list6Neighbor[i * 3 + 2];
				if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0) || (abs(_x) + abs(_y) + abs(_z)>2))
					continue;
				int n = convertBlockDev27(_x, _y, _z);
				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == BACKGROUND&&!opt->check[n])
				{
					opt->check[n] = true;
					if (n == 4 || n == 10 || n == 12 || n == 14 || n == 16 || n == 22)
						nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));

					opt->listF[nextList] = convertBlockDev27(_x, _y, _z);
					nextList++;
				}
			}

			/*for (int i = 0; i < directionL2[c]; i++)
			{
				int _x=directions2[c][i*3];
				int _y=directions2[c][i*3+1];
				int _z=directions2[c][i*3+2];


				int n = convertBlockDev27(_x, _y, _z);
				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == BACKGROUND&&!opt->check[n])
				{
					opt->check[n] = true;
					if (n == 4 || n == 10 || n == 12 || n == 14 || n == 16 || n == 22)
						nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));

					opt->listF[nextList] = convertBlockDev27(_x, _y, _z);
					nextList++;
				}
			}*/
		}

		if (nb == nBack)
			return true;
		else
		{
			//disp(x, y, z);
			return false;
		}
	}



	
}


/*
Checks if the corresponding voxel belongs to one of the classes
*/
bool matVoxel::matchClass(int x, int y, int z)
{
	if (classA(x, y, z))
	{
		nA++;
		return true;
	}

	for (int i = 0; i < 12; i++)
	{
		if (matchOne(x, y, z, matchB + i * 27))
		{

			//disp(matchB + i * 27);
			nB++;

			return true;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (matchOne(x, y, z, matchC + i * 27))
		{

			//disp(matchC + i * 27);
			nC++;

			return true;
		}
	}

	for (int i = 0; i < 12; i++)
	{
		if (matchOne(x, y, z, matchD + i * 27))
		{
			//disp(matchD + i * 27);
			nD++;

			return true;
		}
	}

	return false;
}


/*
Displaying the voxel and its neighbor in console
*/
void matVoxel::disp(int * data)
{

	for (int y = 2; y >= 0; y--)
	{

		for (int z = 0; z < 3; z++)
		{

			for (int x = 0; x < 3; x++)
			{
				int i = x + y * 3 + z * 9;
				if (data[i] == 0)
					cout << yellow;
				else if (data[i] == 1)
					cout << green;
				else
					cout << red;
				cout << data[i];
				cout << black;
			}
			cout << "   ";
		}
		cout << endl;
	}
	cout << endl;

	cout << endl;
}

/*
Displays a voxel on the console, starting with lower y,z,x
*/
void matVoxel::disp(int x_, int y_, int z_)
{
	cout << " x: " << x_ << " y: " << y_ << " z: " << z_ << endl;

	for (int y = 2; y >= 0; y--)
	{

		for (int z = 0; z < 3; z++)
		{

			for (int x = 0; x < 3; x++)
			{
				int i = x_+x-1 + (y_+y-1) * w + (z_+z-1)*w*h;
				if (voxelTypes[i] == 0)
					cout << yellow;
				else if (voxelTypes[i] == 1)
					cout << green;
				else
					cout << red;
				cout << voxelTypes[i];
				cout << black;
			}
			cout << "   ";
		}
		cout << endl;
	}
	cout << endl;

	cout << endl;
}


/*
Generates templates and run the skeletonization

*/
void matVoxel::skeletonize(byte * voxels, int w, int h, int d)
{

	assert(voxels!=NULL);

	if (!init)
	{

		//int class_possible[27*12];

		int initialB[] = {
			EMPTY, EMPTY, EMPTY,
			BACKGROUND, EMPTY, EMPTY,
			BACKGROUND, BACKGROUND, EMPTY,

			EMPTY, OBJECT, EMPTY,
			BACKGROUND, OBJECT, OBJECT,
			BACKGROUND, BACKGROUND, EMPTY,

			EMPTY, EMPTY, EMPTY,
			BACKGROUND, EMPTY, EMPTY,
			BACKGROUND, BACKGROUND, EMPTY
		};

		int initialC[] = {
			EMPTY, BACKGROUND, BACKGROUND,
			EMPTY, BACKGROUND, BACKGROUND,
			EMPTY, EMPTY, EMPTY,

			EMPTY, BACKGROUND, BACKGROUND,
			OBJECT, OBJECT, BACKGROUND,
			EMPTY, OBJECT, EMPTY,

			EMPTY, EMPTY, EMPTY,
			EMPTY, OBJECT, EMPTY,
			EMPTY, EMPTY, EMPTY
		};


		int initialD[] = {
			EMPTY, EMPTY, EMPTY,
			OBJECT, BACKGROUND, BACKGROUND,
			EMPTY, EMPTY, EMPTY,

			EMPTY, EMPTY, EMPTY,
			BACKGROUND, OBJECT, BACKGROUND,
			EMPTY, EMPTY, EMPTY,

			EMPTY, EMPTY, EMPTY,
			BACKGROUND, BACKGROUND, BACKGROUND,
			EMPTY, EMPTY, EMPTY,
		};

		int nbRotationsZ = 0;
		int nbRotationsX = 0;
		int nbRotationsY = 0;
		int offset = 0;




		/*

		Generate class B, C & D

		*/

		for (int i = 0; i < 3; i++)
		{
			nbRotationsY = i == 1 ? 1 : 0;
			nbRotationsX = i == 2 ? 1 : 0;

			for (nbRotationsZ = 0; nbRotationsZ < 4; nbRotationsZ++)
			{


				for (int x = -1; x <= 1; x++)
				for (int y = -1; y <= 1; y++)
				for (int z = -1; z <= 1; z++)
				{
					int xrotate = x;
					int yrotate = y;
					int zrotate = z;

					rotate(yrotate, zrotate, nbRotationsX);
					rotate(xrotate, zrotate, nbRotationsY);
					rotate(xrotate, yrotate, nbRotationsZ);

					matchB[(xrotate + 1) + 3 * (yrotate + 1) + 9 * (zrotate + 1) + offset * 27] = initialB[(x + 1) + 3 * (y + 1) + 9 * (z + 1)];
					matchD[(xrotate + 1) + 3 * (yrotate + 1) + 9 * (zrotate + 1) + offset * 27] = initialD[(x + 1) + 3 * (y + 1) + 9 * (z + 1)];

					if (i == 0 || i == 2)
					{
						matchC[(xrotate + 1) + 3 * (yrotate + 1) + 9 * (zrotate + 1) + (offset >= 4 ? (offset - 4) * 27 : offset * 27)] = initialC[(x + 1) + 3 * (y + 1) + 9 * (z + 1)];
					}

				}

				offset++;

			}

		}

		cout << " Class B " << endl;

		for (int i = 0; i < 12 * 27; i += 27)
			disp(matchB + i);

		cout << " Class C " << endl;

		for (int i = 0; i < 8 * 27; i += 27)
			disp(matchC + i);

		cout << " Class D " << endl;

		for (int i = 0; i < 12 * 27; i += 27)
			disp(matchD + i);


	}

	this->voxels = voxels;
	this->voxelTypes = new byte[w*h*d];//VoxelType[w*h*d];
	this->w = w;
	this->h = h;
	this->d = d;


	for (int i = 0; i < w*h*d; i++)
	{
		if (voxels[i] == 0)
			voxelTypes[i] = BACKGROUND;
		else
			voxelTypes[i] = OBJECT;
	}


	/*for (int i = 0; i < 27; i++)
	{
		voxelTypes[getAt(50 - 1 + i % 3, 50 - 1 + (i % 9) / 3, 50 - 1 + i / 9)] = (VoxelType)matchC[i + 27];

	}*/

	//algo();

	//omp_set_num_threads(4);

	DWORD start=GetTickCount();

	algo2();

	OutputDebugString((boost::lexical_cast<string>((GetTickCount()-start))+"\n").c_str());

}

/*
 The algorithm for skeletonization
 Parallelized with openmp
*/
void matVoxel::algo2()
{
	generateSetOfPossibleDirections();

	omp_set_nested(1);
	omp_set_num_threads(4);

	notify_( ((ostringstream&)(ostringstream() << std::endl<< "Beginning skeletonization" )).str());

	std::vector<int> adjacenttobackground;
		adjacenttobackground.reserve(10000000);

	//std::vector<int> markers;markers.reserve(10000000);
	int * markers = new int[10000000]; // max num of points to be simultaneously removed
	int nbMarkers=0;

	//std::vector<int> voxels_correct(10000000);

	int startX=MAXINT,endX=0,startY=MAXINT,endY=0,startZ=MAXINT,endZ=0;

	//reduction(Min : startX,startY,startZ) reduction(Max :endX,endY,endZ)

	DWORD start=GetTickCount();

	#pragma omp parallel for
	for (int z = 1; z < d - 1; z++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			for (int y = 1; y < h - 1; y++)
			{
				if (voxelTypes[getAt(x, y, z)] == OBJECT)
				{
					
					#pragma omp flush(startX,startY,startZ,endX,endY,endZ)

					#pragma omp critical
					{
					startX = min(startX,x);
					startY = min(startY, y);
					startZ = min(startZ, z);

					endX = max(endX, x);
					endY = max(endY, y);
					endZ = max(endZ, z);
					}
				}
			}
		}
	}

	OutputDebugString((boost::lexical_cast<string>((GetTickCount()-start))+"\n").c_str());

	/*startX--;
	startY--;
	startZ--;

	endX++;
	endY++;
	endZ++;*/

	bool iteration = true;

	int id=0;

	int del_=0;

	while (iteration)
	{
		notify_( ((ostringstream&)(ostringstream()<< "  Phase : " << id++ <<"" )).str());
		notify_( ((ostringstream&)(ostringstream()<< "  Deleted : "<<del_<<"")).str());

		iteration=false;

		start=GetTickCount();

		for (int d_ = 0; d_ < 6; d_++)
		{
			notify_(((ostringstream&)(ostringstream() << "     Direction " << d_ << "")).str());
			//OutputDebugString((boost::lexical_cast<string>(omp_get_num_threads())+"\n").c_str());
			//#pragma omp parallel
			{
			
				// schedule(dynamic,1)
			
			//omp_set_num_threads(4);

			#pragma omp parallel for shared(nbMarkers) schedule(static,1)
			for (int z = startZ; z <= endZ; z++)
			{
				//if(z==1)
				//OutputDebugString((boost::lexical_cast<string>(omp_get_num_threads())+"\n").c_str());
				//#pragma omp for nowait
				for (int x = startX; x <= endX; x++)
				{
					//#pragma omp for nowait
					for (int y = startY; y <= endY; y++)
					{
						if (voxelTypes[getAt(x, y, z)] == OBJECT&&isBorder(x, y, z, d_)&&simplePoint(x,y,z)&&!isTail(x,y,z))
						{
							//notify_(boost::lexical_cast<string>(omp_get_thread_num())+"\n");
							//#pragma omp critical
							//markers.push_back(getAt(x, y, z));
							#pragma omp critical
							{
							markers[nbMarkers]=getAt(x,y,z);
							nbMarkers++;
							}

						}
					}
				}
			}

			}
			//OutputDebugString((boost::lexical_cast<string>(omp_get_num_threads())+"\n").c_str());
			//random_shuffle(markers.begin(), markers.end());
			OutputDebugString((boost::lexical_cast<string>((GetTickCount()-start))+"\n").c_str());

			start=GetTickCount();

			bool cont=true;

			int nbIter=0;

			while (cont)
			{
				cont=false;
				nbIter++;

				//OutputDebugString((boost::lexical_cast<string>(nbIter)+"\n").c_str());

				for (int i = 0; i < nbMarkers;i++)//markers.size(); i++)
				{

					int x = getX(markers[i]);
					int y = getY(markers[i]);
					int z = getZ(markers[i]);


					if (voxelTypes[getAt(x, y, z)]==OBJECT&&simplePoint(x, y, z))//&&!isTail(x,y,z))
					{

						voxelTypes[getAt(x, y, z)] = BACKGROUND;

						iteration = true;
						cont = true;

						//markers.erase(markers.begin()+i);
						//i--;

						del_++;
					}

				}

			}

			adjacenttobackground.clear();
			//markers.clear();

			nbMarkers=0;

			OutputDebugString(("-"+boost::lexical_cast<string>((GetTickCount()-start))+"\n").c_str());
		}

	}


	delete [] markers;
	
}

/*
For reference only, this is the first algorithm for skeletonization, that
works by template matching of voxels to find simple points. It could
theoretically work well with parallelization
*/
void matVoxel::algo()
{
	std::vector<int> adjacenttobackground;

	std::vector<int> removedvoxels;

	std::vector<int> voxels_correct;

	bool extloop = true, inloop = true;

	int ite = 0;

	int phase_num = 0;

	int total_d = 0;

	while (extloop)
	{

		cout << "Phase : " << phase_num++ << endl;
		cout << "Class : " << nA << " / " << nB << " / " << nC << " / " << nD << endl;
		cout << "Deleted : " << total_d;
		cout << endl;

		extloop = false;

		inloop = true;

		for (int z = 1; z < d - 1; z++)
		{
			for (int x = 1; x < w - 1; x++)
			{
				for (int y = 1; y < h - 1; y++)
				{
					if (voxelTypes[getAt(x, y, z)] == OBJECT&&is6adjacent(x, y, z)>0)
					{

						adjacenttobackground.push_back(getAt(x, y, z));

					}
				}
			}
		}


		while (inloop)
		{

			ite++;

			inloop = false;

			//std::random_shuffle(adjacenttobackground.begin(), adjacenttobackground.end());

			for (int i = 0; i < adjacenttobackground.size(); i++)
			{
				int x = getX(adjacenttobackground.at(i));
				int y = getY(adjacenttobackground.at(i));
				int z = getZ(adjacenttobackground.at(i));

				if (matchClass(x, y, z) && voxelTypes[getAt(x, y, z)] == OBJECT)//classA(x, y, z) || classB(x, y, z) || classC(x, y, z) || classD(x, y, z))
				{
					//disp(x,y,z);
					//if (true)
					if (simplePoint(x, y, z) && !isTail(x, y, z))
						//if (!isTail(x,y,z))
					{
						extloop = true;
						inloop = true;

						//disp(x, y, z);

						voxelTypes[getAt(x, y, z)] = BACKGROUND; //[x, y, z];


						removedvoxels.push_back(x + y*w + z*w*h);
						adjacenttobackground.erase(begin(adjacenttobackground) + i);
						i--;

						total_d++;
					}

				}
			}


			for (int i = 0; i < removedvoxels.size(); i++)
			{
				int x = getX(removedvoxels.at(i));
				int y = getY(removedvoxels.at(i));
				int z = getZ(removedvoxels.at(i));

				voxelTypes[getAt(x, y, z)] = BACKGROUND; //[x, y, z];

			}
			total_d += removedvoxels.size();
			removedvoxels.clear();

		}

		for (int i = 0; i < adjacenttobackground.size(); i++)
			voxels_correct.push_back(adjacenttobackground.at(i));
		adjacenttobackground.clear();

	}

	for (int i = 0; i < voxels_correct.size(); i++)
	{
		int x = getX(voxels_correct.at(i));
		int y = getY(voxels_correct.at(i));
		int z = getZ(voxels_correct.at(i));

		voxelTypes[getAt(x, y, z)] = OBJECT; //[x, y, z];

	}
}

int w, h, d, cogX, cogY, cogZ;


/*
Squared distance to a point
*/
float distCompSq(float u1, float u2, float u3, float v1, float v2, float v3)
{
	return (v1 - u1)*(v1 - u1) + (v2 - u2)*(v2 - u2) + (v3 - u3)*(v3 - u3);
}

/* Distance comparison function
*/
bool sortDist(int i, int j) { return ::distCompSq(getX(i), getY(i), getZ(i), cogX, cogY, cogZ)>::distCompSq(getX(j), getY(j), getZ(j), cogX, cogY, cogZ); }

/*
Simple computation of the center of gravity, using voxel gray value as a weighting
factor
*/
void matVoxel::cog()
{


	cogX=0,cogY=0,cogZ=0;

	float tot=0;

	for (int z = 1; z < d - 1; z++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			for (int y = 1; y < h - 1; y++)
			{
				if (voxels[getAt(x, y, z)]>0)
				{
					cogX+=voxels[getAt(x, y, z)]*x;
					cogY+=voxels[getAt(x, y, z)]*y;
					cogZ+=voxels[getAt(x, y, z)]*z;

					tot+=voxels[getAt(x, y, z)];

				}
			}
		}
	}

	cogX/=tot;
	cogY/=tot;
	cogZ/=tot;

	::w=w;
	::h=h;
	::d=d;

	::cogX=cogX;
	::cogY=cogY;
	::cogZ=cogZ;

}

/*
Compute the end / tails points of the spiral, as well as the center of gravity of the spiral
Order them according to their distance to it.
*/
void matVoxel::isolatePoints(std::vector<int> &markers_endpoints)
{

	for (int z = 1; z < d - 1; z++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			for (int y = 1; y < h - 1; y++)
			{
				if (voxelTypes[getAt(x, y, z)] == OBJECT&&isTail(x, y, z))
				{

					markers_endpoints.push_back(getAt(x, y, z));

				}
			}
		}
	}

	
	std::sort(markers_endpoints.begin(),markers_endpoints.end(),::sortDist);

	/*if (markers_endpoints.size() > 0)
	{
	

	//markers_endpoints.resize(2);

	findPath();

	}*/

}




//float pixSize=11.80303;//11.8;

//int * visited;
std::map<int, int> visited;

struct CompareDistance
{
	bool operator()(const int t1, const int t2) const
	{
		return ::visited[t1] > ::visited[t2];
	}
};

/*
Performs a filtering on the raw voxel data of the skeleton to remove artefacts, i.e. small branches
end points or tail points that are incorrect. The idea is to compute the longest shortest path starting
from the end of the spiral. This is retrieved for now by computing the furthest endpoint of the spiral to the center.
That should correspond to it, while a more robust approach has to be found. (It could be possible to have a branch)
*/
int matVoxel::findPath(int startIndex,pcl::PointCloud<pcl::PointXYZI>* path_cloud)
{

	std::vector<int> p;

	std::priority_queue<int,vector<int>, CompareDistance> list;

	
	list.push(startIndex);//markers_endpoints[0]);
	

	//::visited=new int[w*h*d];

	#pragma omp parallel for
	for (int i = 0; i<w*h*d;i++)
		if (voxelTypes[i]==OBJECT)
		{
			#pragma omp critical
			{
			::visited[i]=MAXINT;//(i,MAXINT);//[i] = MAXINT;
			}
		}

	//int * prev = new int[w*h*d];

	map<int,int> prev;

	/*for (int i = 0; i<w*h*d; i++)
		if (voxelTypes[i] == OBJECT)
			prev[i] = -1;*/

	int x,y,z;

	int c = 0;
	int nb = 1;

	::visited[list.top()] = 0;
	prev[list.top()]=-1;

	while (list.size() > 0)
	{
		c = list.top();

		x=getX(c);
		y=getY(c);
		z=getZ(c);

		if (list.size()>1)
			cout << "---\n";

		list.pop();

		for (int i = 0; i < 27; i++)
			{
				int _x = idX(i);
				int _y = idY(i);
				int _z = idZ(i);
				//if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0))
				//	continue;
				float dist = ::visited[c] + sqrtf(_x*_x+_y*_y+_z*_z);//1;

				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == OBJECT&&dist<::visited[getAt(x + _x, y + _y, z + _z)])
				{
					::visited[getAt(x + _x, y + _y, z + _z)] = dist;
					prev[getAt(x + _x, y + _y, z + _z)]=c;
					list.push(getAt(x + _x, y + _y, z + _z));
				}
			}


	}

	//path_cloud.reset(new pcl::PointCloud<pcl::PointXYZI>());

	int r=::visited[c];

	if(path_cloud)
	while (prev[c] != -1)
	{
		pcl::PointXYZI basic_point;

		basic_point.x = (float)(getX(c))*pixSize - cogX*pixSize;// centerX;
		basic_point.y = (float)(getY(c))*pixSize - cogY*pixSize;// centerY;
		basic_point.z = (float)(getZ(c))*pixSize - cogZ*pixSize;// centerZ;
		basic_point.intensity = ::visited[c];//255;
		c=prev[c];

		path_cloud->points.push_back(basic_point);
	}

	
	return ::visited[c];

}

/*
Uses the voxel skeleton to create a B-Spline fitting.
The order of the spline is unusually high, because of the computation time required to either
fit a curve using linear-least square fitting, or do a dimensionality reduction
*/
void matVoxel::fitCurve(int order, pcl::PointCloud<pcl::PointXYZI>::Ptr path_cloud, pcl::PointCloud<pcl::PointXYZRGB>::Ptr nurb,Wm5::BSplineCurve3d * * cur)
{

	/*int order=5;

	nurb.reset(new pcl::PointCloud<pcl::PointXYZI>());

	for (int i = 0; i<order; i++)
		nurb->points.push_back(path_cloud->points[i]);

	for (int i = order; i<path_cloud->size();i++)
	{
		float x=path_cloud->points[i].x + nurb->points[i - 1].x*order-nurb->points[i-order].x;
		float y=path_cloud->points[i].y + nurb->points[i - 1].y*order-nurb->points[i-order].y;
		float z=path_cloud->points[i].z + nurb->points[i - 1].z*order-nurb->points[i-order].z;

		pcl::PointXYZI point;
		point.x=x;
		point.y=y;
		point.z=z;
		point.intensity=path_cloud->points[i].intensity;
		nurb->points.push_back(point);
	}*/



	pcl::on_nurbs::NurbsDataCurve data;
	for (int i = 0; i < path_cloud->size(); i++)
	{
		pcl::PointXYZI &p=path_cloud->at(i);
		data.interior.push_back(Eigen::Vector3d(p.x,p.y,p.z));
	}

	pcl::on_nurbs::FittingCurve::Parameter param;
	param.smoothness=0.000001;


	//int order=100;

	//cur= ON_NurbsCurve::New();//(3,false,order,path_cloud->size());//pcl::on_nurbs::FittingCurve::initNurbsCurvePCA(order,data.interior,path_cloud->size());

	//ON_3dPoint * points=new ON_3dPoint[path_cloud->size()];

	//cur.ReserveCVCapacity(path_cloud->size());

	Wm5::Vector3d * pts_path = new Wm5::Vector3d[path_cloud->size()];

	for (int i = 0; i < path_cloud->size(); i++)
	{
		pcl::PointXYZI &p = path_cloud->at(i);
		//cur.SetCV(i,ON_3dPoint(p.x, p.y, p.z));
		//points[i]=ON_3dPoint(p.x, p.y, p.z);
		
		//pts_path[i*3] = p.x;
		//pts_path[i * 3+1] = p.y;
		//pts_path[i * 3+2] = p.z;
		pts_path[i]=Wm5::Vector3d(p.x,p.y,p.z);
	}

	//cur->CreateClampedUniformNurbs(3,order,path_cloud->size(),points,1);

	//Wm5::BSplineCurveFitf fit(3,path_cloud->size(),pts_path,3,path_cloud->size()/30);

	float fraction=1./5;

	int ctrlP=path_cloud->size()*fraction;

	Wm5::Vector3d * pts_path_reduce = new Wm5::Vector3d[ctrlP];

	//Wm5::BSplineReduction3d(path_cloud->size(), pts_path, 3, fraction, ctrlP, pts_path_reduce);
	//cur=new Wm5::BSplineCurve3d (ctrlP,pts_path_reduce,3,false,true);
	*cur = new Wm5::BSplineCurve3d(path_cloud->size(), pts_path, order, false, true);

	//cur.ReserveKnotCapacity(path_cloud->size());
	//ON_MakeClampedUniformKnotVector(order, path_cloud->size(),cur.m_knot);

	/*pcl::on_nurbs::FittingCurve fit(&data,cur);
	fit.assemble(param);
	fit.solve();*/

	//nurb.reset(new pcl::PointCloud<pcl::PointXYZRGB>());

	//pcl::on_nurbs::Triangulation::convertCurve2PointCloud(*cur,nurb,10);
	Wm5::Vector3d position;
	for (int i = 0; i < path_cloud->size()*10; i++)
	{
		position=(*cur)->GetPosition((double)i / path_cloud->size()/10);

		pcl::PointXYZRGB basic_point;

		basic_point.x = position[0];
		basic_point.y = position[1];
		basic_point.z = position[2];
		basic_point.r = 255;
		basic_point.g = 255;
		basic_point.b = 0;


		nurb->points.push_back(basic_point);
	}

}

/*
Performs a trilinear interpolation based on the 8 adjacent voxels
Done by doing 1 D interpolation in x, y and z directions
*/
float matVoxel::triLinear(float x, float y, float z)
{
	int x_0 = floor(x);
	int y_0 = floor(y);
	int z_0 = floor(z);
	int x_1 = ceil(x);
	int y_1 = ceil(y);
	int z_1 = ceil(z);
	float vec000=voxels[getAt(x_0, y_0, z_0)];
	float vec001 = voxels[getAt(x_0, y_0, z_0)];
	float vec010 = voxels[getAt(x_0, y_1, z_0)];
	float vec011 = voxels[getAt(x_0, y_1, z_1)];
	float vec100 = voxels[getAt(x_1, y_0, z_0)];
	float vec101 = voxels[getAt(x_1, y_0, z_1)];
	float vec110 = voxels[getAt(x_1, y_1, z_0)];
	float vec111 = voxels[getAt(x_1, y_1, z_1)];

	float c00=vec000*(1-(x-x_0))+vec100*(x-x_0);
	float c10=vec010*(1-(x-x_0))+vec110*(x-x_0);
	float c01=vec001*(1-(x-x_0))+vec101*(x-x_0);
	float c11=vec011*(1-(x-x_0))+vec111*(x-x_0);

	float c0 = c00*(1 - (y - y_0)) + c10*(y - y_0);
	float c1 = c01*(1 - (y - y_0)) + c11*(y - y_0);

	return c0*(1 - (z-z_0))+c1*(z-z_0);
}



typedef struct
{
	float           x, y, z;
} Point;


/*
* TriCubic - tri-cubic interpolation at point, p.
*   inputs:
*     p - the interpolation point.
*     volume - a pointer to the float volume data, stored in x,
*              y, then z order (x index increasing fastest).
*     xDim, yDim, zDim - dimensions of the array of volume data.
*   returns:
*     the interpolated value at p.
*   note:
*     NO range checking is done in this function.
*/
float TriCubic(Point p, byte *volume, int xDim, int yDim, int zDim)
{
	int             x, y, z;
	register int    i, j, k;
	float           dx, dy, dz;
	register byte *pv;
	float           u[4], v[4], w[4];
	float           r[4], q[4];
	float           vox = 0;
	int             xyDim;

	xyDim = xDim * yDim;

	x = (int)p.x, y = (int)p.y, z = (int)p.z;
	if (x < 0 || x >= xDim || y < 0 || y >= yDim || z < 0 || z >= zDim)
		return (0);

	dx = p.x - (float)x, dy = p.y - (float)y, dz = p.z - (float)z;
	pv = volume + (x - 1) + (y - 1) * xDim + (z - 1) * xyDim;

# define CUBE(x)   ((x) * (x) * (x))
# define SQR(x)    ((x) * (x))
	/*
	#define DOUBLE(x) ((x) + (x))
	#define HALF(x)   ...
	*
	* may also be used to reduce the number of floating point
	* multiplications. The IEEE standard allows for DOUBLE/HALF
	* operations.
	*/

	/* factors for Catmull-Rom interpolation */

	u[0] = -0.5 * CUBE(dx) + SQR(dx) - 0.5 * dx;
	u[1] = 1.5 * CUBE(dx) - 2.5 * SQR(dx) + 1;
	u[2] = -1.5 * CUBE(dx) + 2 * SQR(dx) + 0.5 * dx;
	u[3] = 0.5 * CUBE(dx) - 0.5 * SQR(dx);

	v[0] = -0.5 * CUBE(dy) + SQR(dy) - 0.5 * dy;
	v[1] = 1.5 * CUBE(dy) - 2.5 * SQR(dy) + 1;
	v[2] = -1.5 * CUBE(dy) + 2 * SQR(dy) + 0.5 * dy;
	v[3] = 0.5 * CUBE(dy) - 0.5 * SQR(dy);

	w[0] = -0.5 * CUBE(dz) + SQR(dz) - 0.5 * dz;
	w[1] = 1.5 * CUBE(dz) - 2.5 * SQR(dz) + 1;
	w[2] = -1.5 * CUBE(dz) + 2 * SQR(dz) + 0.5 * dz;
	w[3] = 0.5 * CUBE(dz) - 0.5 * SQR(dz);

	for (k = 0; k < 4; k++)
	{
		q[k] = 0;
		for (j = 0; j < 4; j++)
		{
			r[j] = 0;
			for (i = 0; i < 4; i++)
			{
				if(pv-volume<xDim*yDim*zDim)
					r[j] += u[i] * *pv;
				pv++;
			}
			q[k] += v[j] * r[j];
			pv += xDim - 4;
		}
		vox += w[k] * q[k];
		pv += xyDim - 4 * xDim;
	}
	return (vox < 0 ? 0.0 : vox);
}


void matVoxel::retrieveCorrectCenter(std::vector<cv::Point> &contour_inter, std::vector<std::vector<cv::Point>> &contour_list, float centerX, float centerY)
{

	cv::Point2f c_;
	float ac = 0;
	float mindist = VTK_FLOAT_MAX;

	/*
	compute the centers of the contours and select the one which is the closest to the center point of the image
	it allows to discard the potential artifacts visible in the image
	*/

	for (int k = 0; k < contour_list.size(); k++)
	{

		//if (contour_points[k].size()>longestContour.size())
		//	longestContour = contour_points[k];

		c_.x = 0;
		c_.y = 0;
		ac = 0;

		for (int u = 0; u < contour_list[k].size(); u++)
		{
			c_.x += contour_list[k].at(u).x;
			c_.y += contour_list[k].at(u).y;
			ac++;
		}

		c_.x /= ac;
		c_.y /= ac;

		float dist = distCompSq(c_.x, c_.y, 0, centerX, centerY, 0);

		if (dist < mindist)
		{
			contour_inter = contour_list[k];//contour_points[k];
			mindist = dist;
		}

	}

	

}

/*
	Given the plane defined by orthogonal axis ax1 and ax2, compute the image of the cross-section
*/
void matVoxel::computePlaneMat(cv::Mat & image, double length, Wm5::Vector3d &point, Eigen::Vector3d &ax1, Eigen::Vector3d &ax2 )
{
	
	#pragma omp parallel for
	for (int a_1 = 0; a_1 < (int)length; a_1++)
	{
		uchar * p = image.ptr<uchar>(a_1);
		for (int a_2 = 0; a_2 < length; a_2++)
		{
			float cx = point[0] + ax1[0]*a_1 + ax2[0]*a_2;
			float cy = point[1] + ax1[1]*a_1 + ax2[1]*a_2;
			float cz = point[2] + ax1[2]*a_1 + ax2[2]*a_2;

			int vX = floor(cx + 0.5); // round to nearest integer
			int vY = floor(cy + 0.5);
			int vZ = floor(cz + 0.5);

			Point pos;
			pos.x = cx;
			pos.y = cy;
			pos.z = cz;

			#pragma omp critical
			{
			if (getAt(vX, vY, vZ) > 0 && getAt(vX, vY, vZ) < w*h*d)
				//p[a_2]=voxels[getAt(vX,vY,vZ)];
				//p[a_2]=triLinear(cx,cy,cz);
				p[a_2] = TriCubic(pos, voxels, w, h, d);
			else
				p[a_2] = 0;
			}

		}
	}
}



double matVoxel::getLength(Wm5::BSplineCurve3d * curve, int nbSampling)
{
	Wm5::Vector3d point=point=curve->GetPosition(0);

	double acc=0;

	for (int i = 1; i < nbSampling; i++)
	{
		Wm5::Vector3d point2=curve->GetPosition((double)i/nbSampling);
		acc+=(point2-point).Length();
		point=point2;
	}

	return acc;

}

double matVoxel::getLength(pcl::PointCloud<pcl::PointXYZI>::Ptr cl)
{
	
	double acc=0;

	for (int i = 0; i < cl->size()-1; i++)
	{
		double x=cl->points[i].x-cl->points[i+1].x;
		double y=cl->points[i].y-cl->points[i+1].y;
		double z=cl->points[i].z-cl->points[i+1].z;
		double d=std::sqrt(x*x+y*y+z*z);
		acc+=d;
	}

	return acc;

}

void matVoxel::plans(pcl::visualization::PCLVisualizer * viewer, string path,pcl::PointCloud<pcl::PointXYZI>::Ptr center_line,int nbSampling,Wm5::BSplineCurve3d * cur,std::vector<std::pair<double,double>> &y_values, double length, int thresholdMeasure, double endPerThreshold, double &posEnd, double &meanW, double &meanH, bool processImages)
{
	/* ax + by + cz + d = 0*/

	double a, b, c,u;

	//double length=200;

	float angle=0;

	double meanWidth=0,meanHeight=0;
	int accTotal=0;

	int counter=0;

	for (int i = nbSampling; i >=0; i -= 1)
	{
	
		counter++;

		if((i)%(nbSampling/9)==0)
			notify_("Step "+boost::lexical_cast<std::string>(floor((double)(i)/nbSampling*10+0.1)+1));

		/*
			Retrieve position for the ith sample, and compute the tangent
		*/

		perfTiming::startCounter();

		Wm5::Vector3d tan;
		Wm5::Vector3d point;
	
		tan=cur->GetTangent((double)i/nbSampling);
		point=cur->GetPosition((double)i/nbSampling);

		//y_values.push_back(std::pair<double,double>(cur->GetLength(0,(double)i/nbSampling),cur->GetCurvature((double)i/nbSampling)));

		a=tan.X();
		b=tan.Y();
		c=tan.Z();

		u=-point[0]*a-point[1]*b-point[2]*c; // Plane equation


		/*
			Get orthonormal basis for the plane
			(Origin at current position on the curve)
		*/
		Eigen::Vector3d normal(a, b, c);
		normal.normalize();
		Eigen::Vector3d ax1(normal.unitOrthogonal());//1, 0.5, _isnanf(-u / c)?1:-u/c);

		ax1 /= ax1.norm();
		Eigen::Vector3d ax2;
		ax2 = normal.cross(ax1);


		/*

		Displays the plane every 10 step

		*/
		if(viewer!=NULL&&(int)i%10==0)
		{
			pcl::PointCloud<pcl::PointXYZ>::Ptr cl(new pcl::PointCloud<pcl::PointXYZ>());

			cl->points.push_back(pcl::PointXYZ(point[0] + length*ax1.x() / 2 + length*ax2.x() / 2, point[1] + length*ax1.y() / 2 + +length*ax2.y() / 2, point[2] + length*ax1.z() / 2 + length*ax2.z() / 2));
			cl->points.push_back(pcl::PointXYZ(point[0] + length*ax1.x() / 2 - length*ax2.x() / 2, point[1] + length*ax1.y() / 2 + -length*ax2.y() / 2, point[2] + length*ax1.z() / 2 - length*ax2.z() / 2));
			cl->points.push_back(pcl::PointXYZ(point[0] - length*ax1.x() / 2 - length*ax2.x() / 2, point[1] - length*ax1.y() / 2 - +length*ax2.y() / 2, point[2] - length*ax1.z() / 2 - length*ax2.z() / 2));
			cl->points.push_back(pcl::PointXYZ(point[0] - length*ax1.x() / 2 + length*ax2.x() / 2, point[1] - length*ax1.y() / 2 + +length*ax2.y() / 2, point[2] - length*ax1.z() / 2 + length*ax2.z() / 2));
		
		//if ((int)i%10==0)
		viewer->addPolygon<pcl::PointXYZ>(cl,200,100,0,"poly"+boost::lexical_cast<std::string>(i));
		}

		perfTiming::stopShow("Axis computation");

		perfTiming::startCounter();

		/*
		Coordinate conversion and translation of the starting point
		(From world coordinates to volume)

			|
			|
		ax1 |      +
			|
			|______________>
					ax2		


		*/

		point[0] += - ax1[0]*length / 2 - ax2[0]*length / 2;
		point[1] += - ax1[1]*length / 2 - ax2[1]*length / 2;
		point[2] += - ax1[2]*length / 2 - ax2[2]*length / 2;

		point[0]/=pixSize;
		point[1] /= pixSize;
		point[2] /= pixSize;

		point[0] += cogX;
		point[1] += cogY;
		point[2] += cogZ;
		
		ax1 /= pixSize;
		ax2 /= pixSize;

		perfTiming::stopShow("Axis correction");
		perfTiming::startCounter();
		
		cv::Mat image(length,length,CV_8UC1);

		int thresholdBorders = 100;

		computePlaneMat(image, length, point, ax1, ax2);

		perfTiming::stopShow("Image");
		perfTiming::startCounter();
				
		std::vector<std::vector<cv::Point>> contour_list;

		cv::Mat thresholded;

		cv::threshold(image,thresholded,thresholdBorders,255,CV_THRESH_BINARY);
		cv::findContours(thresholded, contour_list, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		cv::Point2d center, pca_1, pca_2;
		double e1, e2;

		std::vector<cv::Point> contour_inter;

		retrieveCorrectCenter(contour_inter,contour_list,length/2,length/2);

		perfTiming::stopShow("Center");

		perfTiming::startCounter();
		

		if(contour_inter.size()>0)
		{

		
		pca(image, center, pca_1, pca_2, e1, e2, thresholdBorders, contour_inter);

		
		

		/*
		CV PCA for ref

		std::vector<cv::Point> points_pc;

		//if (i%10==0)
		//cv::imwrite(path + "/cross" + "/im_" + boost::lexical_cast<std::string>(i)+".bmp", image);

		//cv::Mat dst(1, image.rows*image.cols, CV_32F);
		//image.clone().reshape(1, 1).convertTo(dst.row(0), CV_32F);

		cv::Mat prepared_image(points_pc.size(),2,CV_64F);//CV_32F);//image.rows*image.cols,2,CV_64FC1);
		for (int k = 0; k < points_pc.size();k++)//image.rows*image.cols; k++)
		{

			prepared_image.at<double>(k, 0) = points_pc.at(k).x;
			prepared_image.at<double>(k, 1) = points_pc.at(k).y;

		}

		//cv::PCA principal_components(dst,cv::Mat(),CV_PCA_DATA_AS_ROW,2);
		cv::PCA principal_components(prepared_image, cv::Mat(), CV_PCA_DATA_AS_ROW);

		cv::cvtColor(image, image, CV_GRAY2RGB);

		cv::Point2d center(principal_components.mean.at<double>(0, 0), principal_components.mean.at<double>(0, 1));

		cv::Point2d pca_1(principal_components.eigenvectors.at<double>(0, 0), principal_components.eigenvectors.at<double>(0, 1));
		cv::Point2d pca_2(principal_components.eigenvectors.at<double>(1, 0), principal_components.eigenvectors.at<double>(1, 1));*/

			if(processImages)
			{

					cv::Mat imageCopy(image);

					cv::cvtColor(imageCopy, image, CV_GRAY2RGB);

					cv::circle(imageCopy,center,3,CV_RGB(255,0,0),1);
					cv::line(imageCopy,center,center+pca_1*e1,CV_RGB(0,255,0));
					cv::line(imageCopy,center,center+pca_2*e2,CV_RGB(0,0,255));

					cv::drawContours(imageCopy,contour_list,-1,cv::Scalar(255,0,0));

				
		/*
				Rotation and translation of the image

				*/

				affineTransform(image,angle, pca_1, center);

				affineTransform(imageCopy,angle, pca_1, center);


				double wI=0,hI=0;

				getWidthHeight(image, center, e1, e2,wI,hI,thresholdMeasure);

				meanH=meanHeight;
				meanW=meanWidth;

				meanWidth*=accTotal;
				meanHeight*=accTotal;

				meanWidth+=wI;
				meanHeight+=hI;

				accTotal++;

				meanWidth/=accTotal;
				meanHeight/=accTotal;

				posEnd=(double)(nbSampling-i)/nbSampling;

				if(wI>(1+endPerThreshold)*meanWidth&&accTotal>200) //&&hI>(1+endPerThreshold)*meanHeight) // at least a few iterations needed to prevent unexpected exit of the algorithm
				{
					
					return;
				}

				/*

				Showing edges, computed axis and center

				*/
							

				if(path.compare("")!=0)
				cv::imwrite(path + "/cross" + "/im_" + boost::lexical_cast<std::string>(i)+".bmp", imageCopy);


			}

		Wm5::Vector3d p_=cur->GetPosition((double)i/nbSampling);

		pcl::PointXYZI point_center;
		point_center.x=p_[0];point_center.y=p_[1];point_center.z=p_[2];
		point_center.x += (center.x - (double)image.cols / 2)*pixSize*(ax2.x());
		point_center.y += (center.x - (double)image.cols / 2)*pixSize*(ax2.y());
		point_center.z += (center.x - (double)image.cols / 2)*pixSize*(ax2.z());
		point_center.x += (center.y - (double)image.rows / 2)*pixSize*(ax1.x());
		point_center.y += (center.y - (double)image.rows / 2)*pixSize*(ax1.y());
		point_center.z += (center.y - (double)image.rows / 2)*pixSize*(ax1.z());
		/*point_center.r = 100;
		point_center.g = 200;
		point_center.b=200;*/
		point_center.intensity=0;

		

	
		//if(counter>3) // the first computed points generally are incorrect
		center_line->push_back(point_center);

		perfTiming::stopShow("Other");

		perfTiming::startCounter();

		}


	

	}

	//cout << cur->GetLength(0,1);

	//for (int x=0;x<length;x++)
	//for (int y = 0; y < length; y++);
}

/*

The skeleton stops before the actual start of the curve, the function projects the tangent vector in order to get the real start

Should be fixed (wrong path taken)

*/
void matVoxel::projectBack(Wm5::BSplineCurve3d * cur, pcl::PointCloud<pcl::PointXYZI>::Ptr line,int threshold, int length)
{
	Wm5::Vector3d tan;
	Wm5::Vector3d pointVoxel, point, oldpoint;
	
	tan = -cur->GetTangent(0);
	point = cur->GetPosition(0);

	pcl::PointXYZI start(1);
	start.x=point[0];
	start.y=point[1];
	start.z=point[2];

	line->push_back(start);

	oldpoint=point;
	//point+=tan;

	pointVoxel=point;

	pointVoxel[0] /=pixSize;
	pointVoxel[1] /= pixSize;
	pointVoxel[2] /= pixSize;

	pointVoxel[0] += cogX;
	pointVoxel[1] += cogY;
	pointVoxel[2] += cogZ;

	//tan/=pixSize;

	cv::Mat image(length,length,CV_8UC1);

	while(voxels[(int)(getAt(floor(pointVoxel[0]+0.5),floor(pointVoxel[1]+0.5),floor(pointVoxel[2]+0.5)))]>threshold)
	{

		//y_values.push_back(std::pair<double,double>(cur->GetLength(0,(double)i/nbSampling),cur->GetCurvature((double)i/nbSampling)));

		double a=tan.X();
		double b=tan.Y();
		double c=tan.Z();


		/*
			Get orthonormal basis for the plane
			(Origin at current position on the curve)
		*/
		Eigen::Vector3d normal(a, b, c);
		normal.normalize();
		Eigen::Vector3d ax1(normal.unitOrthogonal());//1, 0.5, _isnanf(-u / c)?1:-u/c);

		ax1 /= ax1.norm();
		Eigen::Vector3d ax2;
		ax2 = normal.cross(ax1);


		ax1 /= pixSize;
		ax2 /= pixSize;


		pointVoxel[0] += - ax1[0]*length / 2 - ax2[0]*length / 2;
		pointVoxel[1] += - ax1[1]*length / 2 - ax2[1]*length / 2;
		pointVoxel[2] += - ax1[2]*length / 2 - ax2[2]*length / 2;


		

		computePlaneMat(image, length, pointVoxel, ax1, ax2);

		cv::threshold(image,image,threshold,255,CV_THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contour_list;
		cv::findContours(image, contour_list, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		cv::Point2d center, pca_1, pca_2;
		double e1, e2;
		std::vector<cv::Point> contour_inter;
		retrieveCorrectCenter(contour_inter,contour_list,length/2,length/2);
		if(contour_inter.size()>0)
		{
			pca(image,center,pca_1,pca_2,e1,e2,threshold,contour_inter);
		}

		pcl::PointXYZI point_center;
		
		point[0] += (center.x - (double)image.cols / 2)*pixSize*(ax2.x());
		point[1] += (center.x - (double)image.cols / 2)*pixSize*(ax2.y());
		point[2] += (center.x - (double)image.cols / 2)*pixSize*(ax2.z());
		point[0] += (center.y - (double)image.rows / 2)*pixSize*(ax1.x());
		point[1] += (center.y - (double)image.rows / 2)*pixSize*(ax1.y());
		point[2] += (center.y - (double)image.rows / 2)*pixSize*(ax1.z());

		point_center.x=point[0];point_center.y=point[1];point_center.z=point[2];

		line->push_back(point_center);

		tan=point-oldpoint;
		pointVoxel[0]=(point[0]/pixSize+cogX);
		pointVoxel[1]=(point[1]/pixSize+cogY);
		pointVoxel[2]=(point[2]/pixSize+cogZ);

		oldpoint=point;
		point+=tan;

	}

	pcl::PointXYZI end(200);
	end.x=point[0];
	end.y=point[1];
	end.z=point[2];

	
	line->push_back(end);
}

void matVoxel::projectBack(Wm5::BSplineCurve3d * cur, pcl::PointCloud<pcl::PointXYZI>::Ptr line,int threshold)
{
	Wm5::Vector3d tan;
	Wm5::Vector3d point;
	
	tan = -cur->GetTangent(0);
	point = cur->GetPosition(0);

	tan/=pixSize;

	pcl::PointXYZI start(1);
	start.x=point[0];
	start.y=point[1];
	start.z=point[2];

	line->push_back(start);
	
	
	point[0] /=pixSize;
	point[1] /= pixSize;
	point[2] /= pixSize;

	point[0] += cogX;
	point[1] += cogY;
	point[2] += cogZ;

	

	while(voxels[(int)(getAt(floor(point[0]+0.5),floor(point[1]+0.5),floor(point[2]+0.5)))]>threshold)
	{
		point+=tan;
	}

	pcl::PointXYZI end(200);
	end.x=(point[0]-cogX)*pixSize;
	end.y=(point[1]-cogY)*pixSize;
	end.z=(point[2]-cogZ)*pixSize;

	
	line->push_back(end);
}


void matVoxel::pca(cv::Mat image, cv::Point2d &center, cv::Point2d &vec1, cv::Point2d &vec2, double &e1, double &e2, int threshold, std::vector<cv::Point> contour)
{
	double meanx=0, meany=0;
	double xx=0, xy=0, yy=0;
	double total=0;
	if (contour.size()==0)
	{

		// not the optimal way to parallelize, but results in performance boost

		#pragma omp parallel for
		for (int i = 0; i < image.rows; i++)
		{
			uchar * p = image.ptr<uchar>(i);
			for (int j = 0; j < image.cols; j++)
			{
				if (p[j]>threshold)
				{
					#pragma omp flush(meanx,meany,xx,xy,yy,total)

					#pragma omp critical
					{
						meanx += j;
						meany += i;
						xx += j*j;
						xy += i*j;
						yy += j*j;
						total++;
					}
				}
			}
		}

	}
	else
	{
		for (int i = 0; i < contour.size(); i++)
		{
			meanx += contour[i].x;
			meany += contour[i].y;
			xx += contour[i].x*contour[i].x;
			xy += contour[i].x*contour[i].y;
			yy += contour[i].y*contour[i].y;
			total++;
		}
	}

	meanx/=total;
	meany/=total;
	xx /= total;
	xy /= total;
	yy /= total;

	double covxx=xx-meanx*meanx;
	double covxy=xy-meanx*meany;
	double covyy=yy-meany*meany;

	Eigen::Matrix2d cov;
	cov(0,0)=covxx;
	cov(0,1)=covxy;
	cov(1,0)=covxy;
	cov(1,1)=covyy;

	Eigen::EigenSolver<Eigen::Matrix2d> solveEigenV(cov);

	center = cv::Point2d(meanx, meany);

	vec1.x=solveEigenV.eigenvectors().col(0).x().real();
	vec1.y = solveEigenV.eigenvectors().col(0).y().real();

	vec2.x = solveEigenV.eigenvectors().col(1).x().real();
	vec2.y = solveEigenV.eigenvectors().col(1).y().real();

	e1=solveEigenV.eigenvalues()[0].real();
	e2=solveEigenV.eigenvalues()[1].real();


	if(e2>e1)
	{
		auto v=vec1;
		vec1=vec2;
		vec2=v;
	}

}

pcl::PolygonMesh::Ptr matVoxel::toPoly(pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud)
{

	pcl::PolygonMesh::Ptr mesh(new pcl::PolygonMesh());

	pcl::Vertices vert;

	for (int i = 0; i < pointCloud->size(); i++)
	{
		//std::ostringstream os;
		//os<<i;
		//viewer->addLine(test.path_cloud->points[i], test.path_cloud->points[i + 1], 144, 0, 0,os.str());
		//mesh.polygons.push_back(i);

		vert.vertices.push_back(i);

	}

	pcl::PCLPointCloud2 cl;


	pcl::toPCLPointCloud2(*pointCloud, cl);

	mesh->cloud = cl;

	mesh->polygons.push_back(vert);

	

	return mesh;


}

void matVoxel::skeletonToPoints(pcl::PointCloud<pcl::PointXYZI>* pointCloud)
{

	/*float centerX = w*pixSize / 2;
	float centerY = h*pixSize / 2;
	float centerZ = d*pixSize / 2;*/

	for (int v = 0; v < d; v++)
	for (int i = 0; i < h; i++)
	{

		for (int j = 0; j < w; j++)
		{

			//if (image.at<uchar>(i, j)>10)
			if (voxelTypes[getAt(j, i, v)] == OBJECT)//>0)
			{

				pcl::PointXYZI basic_point;

				basic_point.x = (float)j*pixSize - cogX*pixSize;//centerX;
				basic_point.y = (float)i*pixSize - cogY*pixSize;//centerY;
				basic_point.z = (float)v*pixSize - cogZ*pixSize;//centerZ;
				basic_point.intensity = voxels[j + i*w + v*h*w];


				pointCloud->points.push_back(basic_point);
			}


		}
	}

	

//	cout << "Points tail : " << markers_endpoints.size();
}


void matVoxel::vectorToPointCloud(pcl::PointCloud<pcl::PointXYZI>::Ptr pointCloud, std::vector<int> vec)
{
	

	for (int i = 0; i < vec.size(); i++)
	{
		pcl::PointXYZI basic_point;

		int p = vec.at(i);

		basic_point.x = (float)(getX(p))*pixSize - cogX*pixSize;// centerX;
		basic_point.y = (float)getY(p)*pixSize - cogY*pixSize;// centerY;
		basic_point.z = (float)getZ(p)*pixSize - cogZ*pixSize;// centerZ;
		basic_point.intensity = (float)i/vec.size()*255;//255;


		pointCloud->points.push_back(basic_point);
	}
}


/*
A simple way to compute an approximation of the height and width of the object in the center of the image,
along axis x and y
*/
void matVoxel::getWidthHeight( cv::Mat param1, cv::Point2d center, double e1, double e2, double &wI, double &hI , int threshold)
{
	int centerX=center.x;
	int centerY=center.y;

int x1=center.x;
	while(x1<param1.cols)
	{
		if(*param1.ptr<uchar>(centerY,x1)<threshold)
			break;
		x1++;
	}

int x2=center.x;

	while(x2>0)
	{
		if(*param1.ptr<uchar>(centerY,x2)<threshold)
			break;
		x2--;
	}

	int y1=center.y;
	while(y1<param1.rows)
	{
		if(*param1.ptr<uchar>(y1,centerX)<threshold)
			break;
		y1++;
	}

	int y2=center.y;

	while(y2>0)
	{
		if(*param1.ptr<uchar>(y2,centerX)<threshold)
			break;
		y2--;
	}

	wI=x1-x2;
	hI=y1-y2;
}


void matVoxel::affineTransform(cv::Mat image, float angle, cv::Point2d &pca_1, cv::Point2d &center )
{
	//float angle = atan2(pca_1.y, pca_1.x);
	angle = atan2(pca_1.y, pca_1.x);
	cv::Mat rotation=cv::getRotationMatrix2D(center,atan2(pca_1.y,pca_1.x)/M_PI*180+90,1);

	cv::warpAffine(image,image,rotation,cv::Size(max(image.rows,image.cols),max(image.rows,image.cols)));

	cv::Mat translate(2,3,CV_32FC1);

	translate.at<float>(0, 0) = 1;
	translate.at<float>(1, 0) = 0;
	translate.at<float>(0, 1) = 0;
	translate.at<float>(1, 1) = 1;
	translate.at<float>(0, 2) = -center.x+image.cols/2;
	translate.at<float>(1, 2) = -center.y+image.rows/2;

	cv::warpAffine(image, image, translate, cv::Size(max(image.rows, image.cols), max(image.rows, image.cols)));	
}


