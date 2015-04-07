#include "stdafx.h"
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include "Position3.h"




// Constructor and Desctructors
Position3::Position3() 
{
	int i;
	for(i=0;i<3;i++)
	{
	pos[i]=0;
	}
}

Position3::~Position3()
{
	
}
Position3 & Position3::operator= (const Position3 &otherPosition)
{
	int i;
	for(i=0;i<3;i++)
	{
		pos[i]=otherPosition.pos[i];
	}
	return *this;
}

Position3  &Position3::operator *= (const float scale)
{
   int i;
	for(i=0;i<3;i++)
	{
	pos[i]=pos[i]*scale;
	}
   return *this;
}

Position3 Position3::operator * ( const float scale)
{
    Position3 result = *this;

	int i;
	for(i=0;i<3;i++)
	{
		result.pos[i]=result.pos[i]*scale;
	}
    return result;
}

Position3 &Position3::operator += (const Position3& right)
{
	int i;
	for(i=0;i<3;i++)
	{
		pos[i]=pos[i]+right.pos[i];
	}

    return *this;
}

Position3 Position3::operator + (const Position3& right)
{
    Position3 result = *this;

	int i;
	for(i=0;i<3;i++)
	{
		result.pos[i]=result.pos[i]+right.pos[i];
	}

    return result;
}

Position3 &Position3::operator -= (const Position3& right)
{
	int i;
	for(i=0;i<3;i++)
	{
		pos[i]=pos[i]*right.pos[i];
	}
    return *this;
}
Position3 Position3::operator - (const Position3& right)
{
    Position3 result = *this;

	int i;
	for(i=0;i<3;i++)
	{
		result.pos[i]=result.pos[i]-right.pos[i];
	}

    return result;
}
void Position3::setPosition(float x,float y, float z)
{
	pos[0]=x;
	pos[1]=y;
	pos[2]=z;

}
Plane::Plane() 
{
	int i,j;
	for(int i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			position[i].pos[j]=0;
		}
	}
}
Plane::~Plane()
{
	
}
Plane Plane::operator - (const Plane& right)
{
    Plane result = *this;

	int i;
	for(i=0;i<3;i++)
	{
		result.position[i]=result.position[i]-right.position[i];
	}

    return result;
}
void Plane::setPlane(float *arr)
{
	int i,j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<3;i++)
		{
			position[j].pos[i]=arr[i+j*3];
		}
	}
}
void Plane::setPlane(Position3 p,Position3 p1,Position3 p2,Position3 p3)
{
	position[0]=p;
	position[1]=p1;
	position[2]=p2;
	position[3]=p3;

}

void Plane::setPlane(Position3 *positionlist)
{
	int i;
	for(i=0;i<4;i++)
	{
	position[i]=positionlist[i];
	}
}
/*
Plane  subdivide(const Plane& plane, int iterations = 1)
{
	 if (iterations == 0)
    {
         return plane ;
    }
	 else
	 {

		Position3 e1 = (Position3)plane.position[1] - (Position3)plane.position[0];
        Position3 e2 = (Position3)plane.position[2] - (Position3)plane.position[3];
        Position3 e3 = (Position3)plane.position[3] - (Position3)plane.position[0];
        Position3 e4 = (Position3)plane.position[2] - (Position3)plane.position[1];

        Position3 p1 = e1 * 0.5f + plane.position[0];
        Position3 p2 = e2 * 0.5f + plane.position[3];
        Position3 p3 = e3 * 0.5f + plane.position[0];
        Position3 p4 = e4 * 0.5f + plane.position[1];

        Position3 e5 = p2 - p1;
        Position3 p5 = e5 * 0.5f + p1;

	 
	 }

}
*/

/* vertices by index in quad:
    3---------2
    |         |
    |         |
    |         |
    0---------1
*/

std::vector<Plane> subdivide(const Plane& plane, int iterations = 1)
{

    if (iterations == 0)
    {
		std::vector<Plane> ivector;
		ivector.push_back(plane);
		return ivector ;
        
    }
    else
    {
       	Position3 e1 = (Position3)plane.position[1] - (Position3)plane.position[0];
        Position3 e2 = (Position3)plane.position[2] - (Position3)plane.position[3];
        Position3 e3 = (Position3)plane.position[3] - (Position3)plane.position[0];
        Position3 e4 = (Position3)plane.position[2] - (Position3)plane.position[1];

        Position3 p1 = e1 * 0.5f + plane.position[0];
        Position3 p2 = e2 * 0.5f + plane.position[3];
        Position3 p3 = e3 * 0.5f + plane.position[0];
        Position3 p4 = e4 * 0.5f + plane.position[1];

		

        Position3 e5 = p2 - p1;
		Position3 p5 = e5 * 0.5f + p1;


		Plane subplane[4];

		subplane[0].setPlane( plane.position[0], p1, p5, p3);
		subplane[1].setPlane( p1, plane.position[1], p4, p5 );
		subplane[2].setPlane( p5, p4, plane.position[2], p2);
		subplane[3].setPlane( p3, p5, p2, plane.position[3]);


		std::vector<Plane> result;
		result.push_back(subplane[0]);
		result.push_back(subplane[1]);
		result.push_back(subplane[2]);
		result.push_back(subplane[3]);

        if (iterations == 1)
        {
            return result;
        }
        else
        {
            std::vector<Plane> result2;

			for(int i = 0; i < result.size(); i++)
			{
				std::vector<Plane> subplanes = subdivide(result[i], iterations - 1);
                result2.insert(result2.end(), subplanes.begin(), subplanes.end());
			}

            return result2; // all the result plane
        }
    }
}
/*
int main()
{
	Plane p;
	float arr[12]={0,0,0,1,0.5,0,1,1,0,-0.5,1.5,0};

	p.setPlane(arr);

//    Plane p = {  { 0, 0,0 }, { 1, 0.5f,0 }, { 1, 1,0 }, { -0.5f, 1.5f,0 }  };

	std::vector<Plane> subd = subdivide(p, 2);

  
}*/