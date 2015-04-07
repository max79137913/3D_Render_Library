#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>




class Position3 
{

public:

	float pos[3]; //x,y,z
	
	

	// Constructor
	Position3();
	// Copy Constructor

	// Destructor
	~Position3();

	// operator overload
	Position3 & operator= (const Position3 & otherPosition);
	Position3& operator *= (const float scale);
	Position3 operator * (const float scale);
	Position3& operator += (const Position3& right);
	Position3 operator + (const Position3& right);
	Position3& operator -= (const Position3& right);
	Position3 operator - (const Position3& right);

	//method
	void	setPosition(float x,float y, float z); 
	

};

class Plane
{
	public:
	
	Position3 position[4];

	Plane();
		// Destructor
	~Plane();


	//operator overloading
	Plane operator - (const Plane& right);

	// method 
	void	setPlane(float *arr); 
	void    setPlane(Position3 p1,Position3 p2,Position3 p3,Position3 p4);

	void    setPlane(Position3 *positionlist);
	//Plane    subdivide(const Plane& plane, int iterations = 1);


};


std::vector<Plane> subdivide(const Plane& plane, int iterations);