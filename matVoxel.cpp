#include "matVoxel.h"

#include <iostream>
#include <vector>


using namespace std;

/* Indexing */
#define getAt(x,y,z) (x)+(y)*w+(z)*w*h

#define getZ(i) (i)/(w*h)
#define getX(i) (i)%w
#define getY(i) ((i)%(w*h))/w

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



int matVoxel::is6adjacent(int x, int y, int z)
{
	return (voxelTypes[getAt(x + 1, y, z)] == BACKGROUND) +
		(voxelTypes[getAt(x - 1, y, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y + 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y - 1, z)] == BACKGROUND) +
		(voxelTypes[getAt(x, y, z + 1)] == BACKGROUND) +
		(voxelTypes[getAt(x, y, z - 1)] == BACKGROUND);
}

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

/* Returns wether the voxel is adjacent to a background one
(Lazy evaluation and no loops to ensure max speed)
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
Checks the borderness of a voxel, related to a specific direction. ie. north south east west up and down
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

#define convertBlockDev27(x,y,z) (x+1) + (y+1) * 3 + (z+1)*9
#define idX(i) (- 1 + (i) % 3)
#define idY(i) (- 1 + ((i) % 9)/3)
#define idZ(i) (- 1 + (i) / 9)

std::vector<int> list(30);
int listF[30];
int startList=0;
int nextList=0;

bool check[27] = { false };

/*
Returns if the voxel is a tail or end-point. X-----
*/
bool matVoxel::isTail(int x, int y, int z)
{
	int nObj = 0;
	
	

	for (int i = 0; i < 27; i++)
	{
		if (i == 13)
			continue;
		if (voxelTypes[getAt(x + idX(i), y + idY(i), z + idZ(i))] == OBJECT)
		{
			nObj++;
			::check[i]=true;
		}
		else
			::check[i]=false;
	}

	return (nObj == 1);// || (nObj == 2 && ((::check[10] && ::check[14]) ^ (::check[10] && ::check[22]) ||
		//(::check[16] && ::check[12]) ^ (::check[22] && ::check[12]) ||
		//(::check[16] && ::check[1]) ^ (::check[14] && ::check[1])));
}

/*
Checking connectivity of the 6-adjacent object neighbours within the set of 26-adjacents object
Note : Already optimized with custom list. To be done : simple discard of impossible moves by pre-calculating
directions for every 26 voxels. (Would allow for around 4 times speed up)
*/
bool matVoxel::topConnect26(int x, int y, int z)
{

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
		::listF[startList] = checkPos;
		nextList=1;


		//for (int i = 0;i<)
		//bool check[27] = { false };

		for (int i = 0; i < 27; i++)
			::check[i] = false;

		int c = 0;
		int nb = 1;
		while (nextList!=startList)//::list.size() > 0)
		{
			c = ::listF[startList];//::list.at(0);//back();
			::check[c] = true;
			//::list.erase(::list.begin());//pop_back();
			startList++;
			for (int i = 0; i < 27; i++)
			{
				int _x = idX(c) + idX(i);
				int _y = idY(c) + idY(i);
				int _z = idZ(c) + idZ(i);
				if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0))
					continue;
				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == OBJECT&&!::check[convertBlockDev27(_x, _y, _z)])
				{
					::check[convertBlockDev27(_x, _y, _z)] = true;
					nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));
					::listF[nextList] = convertBlockDev27(_x, _y, _z);
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
		for (int i = 0; i < 27; i++)
			::check[i] = false;

		//::list.clear();
		//::list.push_back(checkPos);


		startList = 0;
		::listF[startList] = checkPos;
		nextList = 1;

		//for (int i = 0;i<)
		
		int c = 0;
		int nb = 1;
		while (startList!=nextList)//::list.size() > 0)
		{
			/*c = ::list.at(0);//back();
			::check[c] = true;
			::list.erase(::list.begin());//pop_back();*/

			c = ::listF[startList];//::list.at(0);//back();
			::check[c] = true;
			//::list.erase(::list.begin());//pop_back();
			startList++;

			for (int i = 0; i < 6; i++)
			{
				int _x = idX(c) + list6Neighbor[i * 3];
				int _y = idY(c) + list6Neighbor[i * 3 + 1];
				int _z = idZ(c) + list6Neighbor[i * 3 + 2];
				if (abs(_x) == 2 || abs(_y) == 2 || abs(_z) == 2 || (_x == 0 && _y == 0 && _z == 0) || (abs(_x) + abs(_y) + abs(_z)>2))
					continue;
				int n = convertBlockDev27(_x, _y, _z);
				if (voxelTypes[getAt(x + _x, y + _y, z + _z)] == BACKGROUND&&!::check[n])
				{
					::check[n] = true;
					if (n == 4 || n == 10 || n == 12 || n == 14 || n == 16 || n == 22)
						nb++;
					//::list.push_back(convertBlockDev27(_x, _y, _z));

					::listF[nextList] = convertBlockDev27(_x, _y, _z);
					nextList++;
				}
			}
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
Displays a voxel on the console, starting with lower y
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
void matVoxel::skeletonize(int * voxels, int w, int h, int d)
{

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
	this->voxelTypes = new VoxelType[w*h*d];
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

	algo2();

}


void matVoxel::algo2()
{

	std::vector<int> adjacenttobackground;
		adjacenttobackground.reserve(10000000);

	std::vector<int> markers;markers.reserve(10000000);

	//std::vector<int> voxels_correct(10000000);

	bool iteration = true;

	int id=0;

	int del_=0;

	while (iteration)
	{
		notify_( ((ostringstream&)(ostringstream()<< "Phase : " << id++ <<"" )).str());
		notify_( ((ostringstream&)(ostringstream()<< "Deleted : "<<del_<<"")).str());

		iteration=false;

		for (int d_ = 0; d_ < 6; d_++)
		{
			notify_(((ostringstream&)(ostringstream() << "Direction " << d_ << "")).str());

			for (int z = 1; z < d - 1; z++)
			{
				for (int x = 1; x < w - 1; x++)
				{
					for (int y = 1; y < h - 1; y++)
					{
						if (voxelTypes[getAt(x, y, z)] == OBJECT&&isBorder(x, y, z, d_)&&simplePoint(x,y,z)&&!isTail(x,y,z))
						{

							markers.push_back(getAt(x, y, z));

						}
					}
				}
			}

			//random_shuffle(markers.begin(), markers.end());

			bool cont=true;

			while (cont)
			{
				cont=false;
				
				for (int i = 0; i < markers.size(); i++)
				{

					int x = getX(markers.at(i));
					int y = getY(markers.at(i));
					int z = getZ(markers.at(i));


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
			markers.clear();


		}

	}

	
}

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
Compute the end / tails points of the spiral, as well as the center of gravity of the spiral
Order them according to their distance to it.
*/
void matVoxel::isolatePoints()
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

	std::sort(markers_endpoints.begin(),markers_endpoints.end(),::sortDist);

	if (markers_endpoints.size() > 0)
	{
	

	//markers_endpoints.resize(2);

	findPath();

	}

}




float pixSize=11.80303;//11.8;

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
void matVoxel::findPath()
{

	std::vector<int> p;

	std::priority_queue<int,vector<int>, CompareDistance> list;

	
	list.push(markers_endpoints[0]);
	

	//::visited=new int[w*h*d];

	for (int i = 0; i<w*h*d;i++)
		if (voxelTypes[i]==OBJECT)
			::visited[i]=MAXINT;//(i,MAXINT);//[i] = MAXINT;

	//int * prev = new int[w*h*d];

	map<int,int> prev;

	for (int i = 0; i<w*h*d; i++)
		if (voxelTypes[i] == OBJECT)
			prev[i] = -1;

	int x,y,z;

	int c = 0;
	int nb = 1;

	::visited[list.top()] = 0;

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

	path_cloud.reset(new pcl::PointCloud<pcl::PointXYZI>());

	while (prev[c] != -1)
	{
		pcl::PointXYZI basic_point;

		basic_point.x = (float)(getX(c))*::pixSize - cogX;// centerX;
		basic_point.y = (float)(getY(c))*::pixSize - cogY;// centerY;
		basic_point.z = (float)(getZ(c))*::pixSize - cogZ;// centerZ;
		basic_point.intensity = ::visited[c];//255;
		c=prev[c];

		path_cloud->points.push_back(basic_point);
	}

	

}

/*
Uses the voxel skeleton to create a B-Spline fitting.
The order of the spline is unusually high, because of the computation time required to either
fit a curve using linear-least square fitting, or do a dimensionality reduction
*/
void matVoxel::fitCurve()
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


	int order=100;

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
	cur = new Wm5::BSplineCurve3d(path_cloud->size(), pts_path, order, false, true);

	//cur.ReserveKnotCapacity(path_cloud->size());
	//ON_MakeClampedUniformKnotVector(order, path_cloud->size(),cur.m_knot);

	/*pcl::on_nurbs::FittingCurve fit(&data,cur);
	fit.assemble(param);
	fit.solve();*/

	nurb.reset(new pcl::PointCloud<pcl::PointXYZRGB>());

	//pcl::on_nurbs::Triangulation::convertCurve2PointCloud(*cur,nurb,10);
	Wm5::Vector3d position;
	for (int i = 0; i < path_cloud->size()*10; i++)
	{
		position=cur->GetPosition((float)i / path_cloud->size()/10);

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
float TriCubic(Point p, int *volume, int xDim, int yDim, int zDim)
{
	int             x, y, z;
	register int    i, j, k;
	float           dx, dy, dz;
	register int *pv;
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



void matVoxel::plans(boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer, string path)
{
	/* ax + by + cz + d = 0*/

	center_line.reset(new pcl::PointCloud<pcl::PointXYZRGB>());

	float a, b, c,u;

	float length=200;

	int nbSampling=path_cloud->size()/10;	

	float angle=0;

	for (float i = 0; i < nbSampling; i += 1)
	{
		//ON_3dVector tan;
		//ON_3dPoint point;
		Wm5::Vector3d tan;
		Wm5::Vector3d point;
		//cur->EvTangent(i,point,tan);
		tan=cur->GetTangent(i/nbSampling);
		point=cur->GetPosition(i/nbSampling);

		//x_values.push_back(i);
		y_values.push_back(std::pair<double,double>(cur->GetLength(0,i/nbSampling),cur->GetCurvature(i/nbSampling)));

		a=tan.X();
		b=tan.Y();
		c=tan.Z();

		u=-point[0]*a-point[1]*b-point[2]*c;

		Eigen::Vector3d normal(a, b, c);
		normal.normalize();
		Eigen::Vector3d ax1(normal.unitOrthogonal());//1, 0.5, _isnanf(-u / c)?1:-u/c);
		//Eigen::AngleAxisd rot(angle,normal);
		//ax1=rot*ax1;
		ax1 /= ax1.norm();
		Eigen::Vector3d ax2;
		ax2 = normal.cross(ax1);

		//normal*=100;
		//ax1*=30;
		//ax2*=30;

		pcl::PointCloud<pcl::PointXYZ>::Ptr cl(new pcl::PointCloud<pcl::PointXYZ>());

		cl->points.push_back(pcl::PointXYZ(point[0] + length*ax1.x() / 2 + length*ax2.x() / 2, point[1] + length*ax1.y() / 2 + +length*ax2.y() / 2, point[2] + length*ax1.z() / 2 + length*ax2.z() / 2));
		/*cl->points.push_back(pcl::PointXYZ(point.x + length*ax2.x() / 2, point.y + length*ax2.y() / 2, point.z + length*ax2.z() / 2));
		cl->points.push_back(pcl::PointXYZ(point.x - length*ax1.x() / 2, point.y - length*ax1.y() / 2, point.z - length*ax1.z() / 2));
		cl->points.push_back(pcl::PointXYZ(point.x - length*ax2.x() / 2, point.y - length*ax2.y() / 2, point.z - length*ax2.z() / 2));*/
		cl->points.push_back(pcl::PointXYZ(point[0] + length*ax1.x() / 2 - length*ax2.x() / 2, point[1] + length*ax1.y() / 2 + -length*ax2.y() / 2, point[2] + length*ax1.z() / 2 - length*ax2.z() / 2));
		cl->points.push_back(pcl::PointXYZ(point[0] - length*ax1.x() / 2 - length*ax2.x() / 2, point[1] - length*ax1.y() / 2 - +length*ax2.y() / 2, point[2] - length*ax1.z() / 2 - length*ax2.z() / 2));
		cl->points.push_back(pcl::PointXYZ(point[0] - length*ax1.x() / 2 + length*ax2.x() / 2, point[1] - length*ax1.y() / 2 + +length*ax2.y() / 2, point[2] - length*ax1.z() / 2 + length*ax2.z() / 2));



		if ((int)i%10==0&&false)
		viewer->addPolygon<pcl::PointXYZ>(cl,200,100,0,"poly"+boost::lexical_cast<std::string>(i));

		
		point[0] += cogX - ax1.x()*length / 2 - ax2.x()*length / 2;
		point[1] += cogY - ax1.y()*length / 2 - ax2.y()*length / 2;
		point[2] += cogZ - ax1.z()*length / 2 - ax2.z()*length / 2;

		point[0]/=pixSize;
		point[1] /= pixSize;
		point[2] /= pixSize;

		
		ax1 /= pixSize;
		ax2 /= pixSize;


		cv::Mat image(length,length,CV_8UC1);

		uchar * p;

		std::vector<cv::Point> points_pc;

		int threshold = 50;

		for (int a_1 = 0; a_1 < length; a_1++)
		{
			p = image.ptr<uchar>(a_1);
		for (int a_2 = 0; a_2 < length; a_2++)
		{
			float cx = point[0] + ax1.x()*a_1 + ax2.x()*a_2;
			float cy = point[1] + ax1.y()*a_1 + ax2.y()*a_2;
			float cz = point[2] + ax1.z()*a_1 + ax2.z()*a_2;

			int vX=floor(cx+0.5);
			int vY=floor(cy+0.5);
			int vZ=floor(cz+0.5);

			Point pos;
			pos.x=cx;
			pos.y=cy;
			pos.z=cz;

			if (getAt(vX, vY, vZ)<w*h*d)
			//p[a_2]=voxels[getAt(vX,vY,vZ)];
			//p[a_2]=triLinear(cx,cy,cz);
			p[a_2] = TriCubic(pos,voxels,w,h,d);
			else
				p[a_2]=0;

			

		}
		}

		
		//if (i%10==0)
		//cv::imwrite(path + "/cross" + "/im_" + boost::lexical_cast<std::string>(i)+".bmp", image);

		//cv::Mat dst(1, image.rows*image.cols, CV_32F);
		//image.clone().reshape(1, 1).convertTo(dst.row(0), CV_32F);

		std::vector<std::vector<cv::Point>> contour_points;

		cv::Mat thresholded;

		cv::threshold(image,thresholded,threshold,255,CV_THRESH_BINARY);

		cv::findContours(thresholded, contour_points, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		cv::Point2d center, pca_1, pca_2;
		double e1, e2;

		std::vector<cv::Point> contour_inter;

		cv::Point2f c_;
		float ac=0;
		float mindist=VTK_FLOAT_MAX;

		for (int k = 0; k < contour_points.size(); k++)
		{

			//if (contour_points[k].size()>longestContour.size())
			//	longestContour = contour_points[k];

			c_.x= 0;
			c_.y = 0;
			ac=0;

			for (int u = 0; u < contour_points[k].size(); u++)
			{
				c_.x += contour_points[k].at(u).x;
				c_.y += contour_points[k].at(u).y;
				ac++;
			}

			c_.x/=ac;
			c_.y/=ac;

			float dist=distCompSq(c_.x, c_.y, 0, length / 2, length / 2,0);

			if (dist < mindist)
			{
				contour_inter=contour_points[k];
				mindist=dist;
			}

		}

		pca(image, center, pca_1, pca_2, e1, e2, threshold, contour_inter);

		/*cv::Mat prepared_image(points_pc.size(),2,CV_64F);//CV_32F);//image.rows*image.cols,2,CV_64FC1);
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

		cv::cvtColor(image, image, CV_GRAY2RGB);

		cv::circle(image,center,3,CV_RGB(255,0,0),1);
		cv::line(image,center,center+pca_1*e1,CV_RGB(0,255,0));
		cv::line(image,center,center+pca_2*e2,CV_RGB(0,0,255));

		cv::drawContours(image,contour_points,-1,cv::Scalar(255,0,0));


		//float angle = atan2(pca_1.y, pca_1.x);
		angle = atan2(pca_1.y, pca_1.x);
		cv::Mat rotation=cv::getRotationMatrix2D(center,atan2(pca_1.y,pca_1.x)/M_PI*180+90,1);

		//cv::warpAffine(image,image,rotation,cv::Size(max(image.rows,image.cols),max(image.rows,image.cols)));

		cv::Mat translate(2,3,CV_32FC1);

		translate.at<float>(0, 0) = 1;
		translate.at<float>(1, 0) = 0;
		translate.at<float>(0, 1) = 0;
		translate.at<float>(1, 1) = 1;
		translate.at<float>(0, 2) = -center.x+image.cols/2;
		translate.at<float>(1, 2) = -center.y+image.rows/2;

		//cv::warpAffine(image, image, translate, cv::Size(max(image.rows, image.cols), max(image.rows, image.cols)));

		cv::imwrite(path + "/cross" + "/im_" + boost::lexical_cast<std::string>(i)+".bmp", image);

		Wm5::Vector3d p_=cur->GetPosition(i/nbSampling);

		pcl::PointXYZRGB point_center;
		point_center.x=p_[0];point_center.y=p_[1];point_center.z=p_[2];
		point_center.x += (center.x - image.cols / 2)*pixSize*(ax2.x());
		point_center.y += (center.x - image.cols / 2)*pixSize*(ax2.y());
		point_center.z += (center.x - image.cols / 2)*pixSize*(ax2.z());
		point_center.x += (center.y - image.rows / 2)*pixSize*(ax1.x());
		point_center.y += (center.y - image.rows / 2)*pixSize*(ax1.y());
		point_center.z += (center.y - image.rows / 2)*pixSize*(ax1.z());
		point_center.r = 100;
		point_center.g = 200;
		point_center.b=200;

		center_line->push_back(point_center);


		//pcl::ModelCoefficients coeffs;
		//coeffs.values.push_back(a);
		//coeffs.values.push_back(b);
		//coeffs.values.push_back(c);
		//coeffs.values.push_back(u);

		//viewer->addPlane(coeffs,point.x,point.y,point.z, boost::lexical_cast<std::string>(i));//->addPolygon()

	}

	cout << cur->GetLength(0,1);

	//for (int x=0;x<length;x++)
	//for (int y = 0; y < length; y++);
}

void matVoxel::projectBack()
{
	Wm5::Vector3d tan;
	Wm5::Vector3d point;
	
	tan = cur->GetTangent(1);
	point = cur->GetPosition(1);
}

void matVoxel::pca(cv::Mat image, cv::Point2d &center, cv::Point2d &vec1, cv::Point2d &vec2, double &e1, double &e2, int threshold, std::vector<cv::Point> contour)
{
	double meanx=0, meany=0;
	double xx=0, xy=0, yy=0;
	double total=0;
	if (contour.size()==0)
	{

		uchar * p;
		for (int i = 0; i < image.rows; i++)
		{
			p = image.ptr<uchar>(i);
			for (int j = 0; j < image.cols; j++)
			{
				if (p[j]>threshold)
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

}

pcl::PolygonMesh matVoxel::toPoly(pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud)
{

	pcl::PolygonMesh mesh;

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

	mesh.cloud = cl;

	mesh.polygons.push_back(vert);

	

	return mesh;


}

void matVoxel::skeletonToPoints(pcl::PointCloud<pcl::PointXYZI>* pointCloud, int * voxels, int w, int h, int d, float pixSize)
{

	float centerX = w*pixSize / 2;
	float centerY = h*pixSize / 2;
	float centerZ = d*pixSize / 2;

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

	endpoints.reset(new pcl::PointCloud<pcl::PointXYZI>());

	for (int i = 0; i < markers_endpoints.size(); i++)
	{
		pcl::PointXYZI basic_point;

		int p = markers_endpoints.at(i);

		basic_point.x = (float)(getX(p))*pixSize - cogX*pixSize;// centerX;
		basic_point.y = (float)getY(p)*pixSize - cogY*pixSize;// centerY;
		basic_point.z = (float)getZ(p)*pixSize - cogZ*pixSize;// centerZ;
		basic_point.intensity = (float)i/markers_endpoints.size()*255;//255;


		endpoints->points.push_back(basic_point);
	}

	cout << "Points tail : " << markers_endpoints.size();
}
