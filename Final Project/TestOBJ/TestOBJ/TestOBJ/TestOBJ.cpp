// TestOBJ.cpp : Defines the entry point for the console application.
//



/*
Objdemo.cpp
C++ Program to exercise OBJParser.cpp
usage: demo file.obj
call debugOn() to see LOTS of debugging information

Copyright 2006 Andrew aGoodney
This file is freely distributable and useable for any purpose as long as this
copyright statement is included and credit is given to the author.

If you make changes or improvement please consider releasing those so that others
may benefit.

*/

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>
#include "OBJParser.h"

/////////////////////
#include "Position3.h"
#include <array>
#include <string>
#include <vector>
/////////////////////
#define PI 3.141592

using namespace std;

int main(int argc, char *argv[])
{

    //check number of command line arguments
    if (argc != 2)
    {
        //usage();
        exit(-1);
    }
    //test to see if file exists
    ifstream objfile(argv[1], ios::in);
    if (!objfile)
    {
        cerr << "File " << argv[1] << "could not be opened." << endl; 
    }
    objfile.close();
    

	char ptr[]="cornell.obj";
	argv[1]=ptr;
    //instantiate obj parser
    OBJObject myOBJ;
    //myOBJ.debugOn();
    myOBJ.parse(argv[1]);
    
    int num_vert = myOBJ.getNumOfVertices();
    cout << "Parsed .obj file..." << endl;
    cout << "Found " << num_vert << " vertices..." << endl;
    for(int i=1;i<=num_vert;i++)
    {
       
       Vertex v = myOBJ.getVertex(i);
       cout << "Vertex: " << i << " " << v.x << "," << v.y << "," << v.z << endl;
       
    }
    int num_vn = myOBJ.getNumOfNormals();
    cout << "Found " << num_vn << " vertex normals" << endl;
    for(int i=1;i<=num_vn;i++)
    {
       
       VertexNormal vn = myOBJ.getVertexNormal(i);
       cout << "Vertex Normal: " << i << " " << vn.x << "," << vn.y << "," << vn.z << endl;
       
    }
    int num_vt = myOBJ.getNumOfTextures();
    cout << "Found " << num_vt << " vertex textures..." << endl;
    for(int i=1;i<=num_vt;i++)
    {
       
       VertexTexture vt = myOBJ.getVertexTexture(i);
       cout << "Vertex Texture: " << i << " " << vt.s << "," << vt.t << endl;
       
    }
    
    
    int num_face = myOBJ.getNumOfFaces();
    cout << "Found " << num_face << " faces..." << endl;
    
    Face f;
    
    for(int i=0;i<num_face;i++)
    {
        
        f = myOBJ.getFace(i);
        
        cout << "Face: " << i << " ";
        for(int y=0; y < f.numVertices; y++)
        {
            
            int currentVertex = f.faceVList[y];
            cout << currentVertex << "->";
            
        }//vertex for loop
        cout << f.faceVList[0] << endl;
        
    }//face for loop

///////////////////////////


	Plane p;
//	float arr[12]={0,0,0,1,0.5,0,1,1,0,-0.5,1.5,0};   array method
//	p.setPlane(arr);
	

	Position3 wallPosition[4];

	wallPosition[0].setPosition(0,0,0);
	wallPosition[1].setPosition(1,0.5f,0);
	wallPosition[2].setPosition(1,1,0);
	wallPosition[3].setPosition(-0.5f,1.5f,0);

	//p.setPlane(wallPosition[0],wallPosition[1],wallPosition[2],wallPosition[3]);
	p.setPlane(wallPosition);

//    Plane p = {  { 0, 0,0 }, { 1, 0.5f,0 }, { 1, 1,0 }, { -0.5f, 1.5f,0 }  };

	std::vector<Plane> subd = subdivide(p, 2);

	int x;
	x=5;

///////////////////////////

}
