#include "stdafx.h"
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include "Hemicube.h"

Hemicube::Hemicube(){
	//need to put this cube into a list.
}
void Hemicube::crossProduct(Position3 a, Position3 b, Position3 &c){
	//c always needs to be up. Hence, b X a.
	c.pos[0] = b.pos[1] * a.pos[2] - b.pos[2] * a.pos[1];
	c.pos[1] = -(b.pos[0] * a.pos[2] - b.pos[2] * a.pos[0]);
	c.pos[2] = b.pos[0] * a.pos[1] - b.pos[1] * a.pos[0];
}
void Hemicube::normalize(Position3 &a){
	//c always needs to be up. Hence, b X a.
	float k = 1 / sqrtf(a.pos[0] * a.pos[0] +
		a.pos[1] * a.pos[1] + a.pos[2] * a.pos[2]);
	for (int i = 0; i < 3; i++){
		a.pos[i] = a.pos[i] * k;
	}
		
}
void Hemicube::setHemicube(Plane plane, int iteration){
	iterations = iteration;
	for (int i = 0; i < 4; i++){
		position[i] = plane.position[i];
	}
	//need nomral to cacalulate front view plane.
	Position3 a = position[1] - position[0];
	Position3 b = position[2] - position[0];
	crossProduct(a, b, normal);
	normalize(normal);
	//construct front view vertexes
	//find the center point using interpolation
	Position3 temp1,temp2;
	temp1.pos[0] = (position[1].pos[0] + position[0].pos[0]) / 2;
	temp1.pos[1] = (position[1].pos[1] + position[0].pos[1]) / 2;
	temp1.pos[2] = (position[1].pos[2] + position[0].pos[2]) / 2;
	temp2.pos[0] = (position[3].pos[0] + position[2].pos[0]) / 2;
	temp2.pos[1] = (position[3].pos[1] + position[2].pos[1]) / 2;
	temp2.pos[2] = (position[3].pos[2] + position[2].pos[2]) / 2;
	center.pos[0] = (temp1.pos[0] + temp2.pos[0]) / 2;
	center.pos[1] = (temp1.pos[1] + temp2.pos[1]) / 2;
	center.pos[2] = (temp1.pos[2] + temp2.pos[2]) / 2;
	//find center in front plane
	centerFront = center + normal;
	//find the front plane vertexes
	a = position[3] - position[0];
	b = position[1] - position[0];
	normalize(a);
	normalize(b);
	frontPosition[0] = centerFront - a - b;
	frontPosition[1] = centerFront - a + b;
	frontPosition[2] = centerFront + a + b;
	frontPosition[3] = centerFront + a - b;
	frontPlane.setPlane(frontPosition[0], frontPosition[1], frontPosition[2], frontPosition[3]);

	//divide plane into many patches and put them into a list.
	frontPatches = subdivide(frontPlane, iterations);
	std::vector<Plane> tempLeftPatches1, tempRightPatches1, tempTopPatches1, tempBottomPatches1, 
		tempLeftPatches2, tempRightPatches2, tempTopPatches2, tempBottomPatches2;
	normalLeft = b;
	normalRight.pos[0] = -b.pos[0];
	normalRight.pos[1] = -b.pos[1];
	normalRight.pos[2] = -b.pos[2];
	normalBottom = a;
	normalTop.pos[0] = -a.pos[0];
	normalTop.pos[1] = -a.pos[1];
	normalTop.pos[2] = -a.pos[2];
	//divide the side plane by 2 equal square
	leftPlane1.setPlane(frontPosition[3] - normal, frontPosition[3] - normalBottom - normal, frontPosition[3] - normalBottom, frontPosition[3]);
	leftPlane2.setPlane(frontPosition[0] - normal + normalBottom, frontPosition[0] - normal, frontPosition[0], frontPosition[0] + normalBottom);
	bottomPlane1.setPlane(frontPosition[0] - normal, frontPosition[0] - normal + normalLeft, frontPosition[0] + normalLeft, frontPosition[0]);
	bottomPlane2.setPlane(frontPosition[0] - normal + normalLeft, frontPosition[1] - normal, frontPosition[1], frontPosition[0] + normalLeft);
	rightPlane1.setPlane(frontPosition[1] - normal, frontPosition[1] - normal + normalBottom, frontPosition[1] + normalBottom, frontPosition[1]);
	rightPlane2.setPlane(frontPosition[1] - normal + normalBottom, frontPosition[2] - normal, frontPosition[2], frontPosition[1] + normalBottom);
	topPlane1.setPlane(frontPosition[2] - normal, frontPosition[2] - normal - normalLeft, frontPosition[2] - normalLeft, frontPosition[2]);
	topPlane2.setPlane(frontPosition[2] - normalLeft - normal, frontPosition[3] - normal, frontPosition[3], frontPosition[2] - normalLeft);
	leftPatches1 = subdivide(leftPlane1, iterations / 2);
	leftPatches2 = subdivide(leftPlane2, iterations / 2);
	rightPatches1 = subdivide(rightPlane1, iterations / 2);
	rightPatches2 = subdivide(rightPlane2, iterations / 2);
	topPatches1 = subdivide(topPlane1, iterations / 2);
	topPatches2 = subdivide(topPlane2, iterations / 2);
	bottomPatches1 = subdivide(bottomPlane1, iterations / 2);
	bottomPatches2 = subdivide(bottomPlane2, iterations / 2);
	//put all in one
	//patches.reserve(frontPatches.size() + leftPatches1.size() + leftPatches2.size() + 
	//	rightPatches1.size() + rightPatches2.size() + topPatches1.size() + topPatches2.size + bottomPatches1.size() + bottomPatches2.size());
	patches.insert(patches.end(),frontPatches.begin(),frontPatches.end());
	patches.insert(patches.end(), leftPatches1.begin(), leftPatches1.end());
	patches.insert(patches.end(), leftPatches2.begin(), leftPatches2.end());
	patches.insert(patches.end(), topPatches2.begin(), topPatches2.end());
	patches.insert(patches.end(), topPatches1.begin(), topPatches1.end());
	patches.insert(patches.end(), rightPatches1.begin(), rightPatches1.end());
	patches.insert(patches.end(), rightPatches2.begin(), rightPatches2.end());
	patches.insert(patches.end(), bottomPatches1.begin(), bottomPatches1.end());
	patches.insert(patches.end(), bottomPatches2.begin(), bottomPatches2.end());
	/*
	leftPatches.push_back(leftPatches1);
	leftPatches.push_back(leftPatches2);
	rightPatches.push_back(rightPatches1);
	rightPatches.push_back(rightPatches2);
	bottomPatches.push_back(bottomPatches1);
	bottomPatches.push_back(bottomPatches2);
	topPatches.push_back(topPatches1);
	topPatches.push_back(topPatches2);*/
}