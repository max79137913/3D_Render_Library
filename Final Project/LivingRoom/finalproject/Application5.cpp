// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "disp.h"
#include "rend.h"
#include "formfactor.h"

/////////////// new final project include
#include "OBJParser.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>


using namespace std;
////////////////
/////////////////////

#include <array>
#include <string>
#include <vector>
#include <algorithm>
/////////////////////
//hemicube, removed position3.h since it is included in hemicube.h
//#include "Hemicube.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "ppot.asc"
#define OBJFILE  "cornell.obj"
#define OUTFILE "output.ppm"



float AAFilter[AAKERNEL_SIZE][3]= 			/* X, Y, coef */
{
-0.52, 0.38, 0.128,
0.41, 0.56, 0.119,
0.27, 0.08, 0.294,
-0.17, -0.29, 0.249,
0.58, -0.55, 0.104,
-0.31, -0.71, 0.106
};




extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */

void shade(GzCoord norm, GzCoord color);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes, dispClass;	/* display parameters */ 

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];

	GzToken     nameListShifters[2];// dx's and dy's name
	GzPointer   valueListShifters[2]; //    dx and dy value

	int			shaderType, interpStyle;
	float		specpower;
	int		status; 
 
	status = 0; 

	int i,j;

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes, &dispClass); 


	status |= GzInitDisplay(m_pDisplay); 

	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay); 



/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	0.0, 
	0.0,	3.25,	0.0,	-3.25, 
	0.0,	0.0,	3.25,	3.5, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	.7071,	.7071,	0.0, 
	0.0,	-.7071,	.7071,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateY = 
{ 
	.866,	0.0,	-0.5,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.5,	0.0,	.866,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
    camera.position[X] = 2;  //-3
    camera.position[Y] = 0; //-25
    camera.position[Z] = -14;  //-4

    camera.lookat[X] = 0;   // 7.8
    camera.lookat[Y] = 0;   //0.7
    camera.lookat[Z] = 1;   //6.5

    camera.worldup[X] = 0.0;  //-0.2
    camera.worldup[Y] = 1.0;   //1.0
    camera.worldup[Z] = 0.0;   //0.0

    camera.FOV = 63.7;              /* degrees *  63.7            /* degrees */

	status |= GzPutCamera(m_pRender, &camera); 


#endif 

	/* Start Renderer */
	status |= GzBeginRender(m_pRender);


	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

		/*
		Dx and Dy put in 
		*/
		int SHX=0;
		int SHY=0;

		nameListShifters[0]=GZ_AASHIFTX;
		valueListShifters[0]=(GzPointer)&SHX;
		nameListShifters[1]=GZ_AASHIFTY;
		valueListShifters[1]=(GzPointer)&SHY;
		status |= GzPutAttribute(m_pRender, 2, nameListShifters, valueListShifters);


        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= GzPutAttribute(m_pRender, 3, nameListLights, valueListLights);

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= GzPutAttribute(m_pRender, 1, nameListLights, valueListLights);


        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;
        interpStyle = GZ_FLAT;         /* Phong shading */
        valueListShader[1] = (GzPointer)&interpStyle;

        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 32;
        valueListShader[4] = (GzPointer)&specpower;

        nameListShader[5]  = GZ_TEXTURE_MAP;
#if 0   /* set up null texture function or valid pointer */
        valueListShader[5] = (GzPointer)0;
#else
        valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
#endif
        status |= GzPutAttribute(m_pRender, 6, nameListShader, valueListShader);


//	status |= GzPushMatrix(m_pRender, scale);  

//	status |= GzPushMatrix(m_pRender, rotateY); 

//	status |= GzPushMatrix(m_pRender, rotateX); 


	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

/////////////////////
/*Radiosity equation*/
vector<Plane> RadiosityEqu(vector<Plane>patchList)
{
	GzColor P;
	P[0] = P[1] = P[2] = 0.7;// tmp Kd

	//GzColor * B = new GzColor[patchList.size()];
	GzColor E ; // to get emmition from light source, most of them are dark 0
	vector<float> formfactorlist;
	Plane patch_i, patch_j ;
	GzColor sumJ;

	// Bi = Ei +Pi*sigma(Bj*Fij)
	for (int i = 0; i < patchList.size(); i++)
	{
		patch_i = patchList.at(i);
		formfactorlist = formfactor(patchList, patch_i);
		E[0] = patch_i.lightSource[0];
		E[1] = patch_i.lightSource[1];
		E[2] = patch_i.lightSource[2];
		sumJ[0] = sumJ[1] = sumJ[2] = 0.0;
		for (int j = 0; j < patchList.size(); j++)
		{
			if(j == i) // all j except i
				continue;
			patch_j = patchList.at(j);
			sumJ[0] += patch_j.radiosityValue[0]*formfactorlist.at(j);
			sumJ[1] += patch_j.radiosityValue[1]*formfactorlist.at(j);
			sumJ[2] += patch_j.radiosityValue[2]*formfactorlist.at(j);
		}
		// Bi = Ei +Pi*sigma(Bj*Fij)
		patch_i.radiosityValue[0] = E[0] + P[0]* sumJ[0];
		patch_i.radiosityValue[1] = E[1] + P[1]* sumJ[1];
		patch_i.radiosityValue[2] = E[2] + P[2]* sumJ[2];
		
		patchList.at(i).radiosityValue[0] = patch_i.radiosityValue[0];
		patchList.at(i).radiosityValue[1] = patch_i.radiosityValue[1];
		patchList.at(i).radiosityValue[2] = patch_i.radiosityValue[2];
	}
	return patchList;
}
/////////////////////

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status; 
	int         i;

	/* Initialize Display */
	status |= GzInitDisplay(m_pDisplay); 


	
	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;  


/////////////////////// objpath init


	char ptr[]="cornellsimple2.obj";
	char *objpath;
	objpath=ptr;
    //instantiate obj parser
    OBJObject myOBJ;
    //myOBJ.debugOn();
    myOBJ.parse(objpath);
    
/////////////////////// make objmeshes 
	for(i=0;i<myOBJ.getNumOfFaces();i++)
{
	Face f;
	f=myOBJ.getFace(i);
	
	 for(int y=0; y < f.numVertices; y++)
        {   
         int currentVertex = f.faceVList[y];
          

		 Vertex v = myOBJ.getVertex(currentVertex);
		 vertexList[y][0]=v.x;
		 vertexList[y][1]=v.y;
		 vertexList[y][2]=v.z;
if(i>3)
{
		 normalList[y][0]=0;
		 normalList[y][1]=0;
		 normalList[y][2]=0;
}
else
{		 normalList[y][0]=0.2;
		 normalList[y][1]=0.5;
		 normalList[y][2]=0.1;
}
		   uvList[y][0]=0;
		   uvList[y][1]=0;

            
        }//vertex for loop

	 /*
	  valueListTriangle[0] = (GzPointer)vertexList; 
	  valueListTriangle[1] = (GzPointer)normalList; 
	  valueListTriangle[2] = (GzPointer)uvList; 
	  GzPutTriangle(m_pRender, 3, nameListTriangle, valueListTriangle); 
	  */
	
}
//////***************/////  divide all the plane into patches

vector<vector<Plane>> wallList;

//////////////////////////
// patches list for all patches
vector<Plane> patcheList;
//////////////////////////

for(i=0;i<myOBJ.getNumOfFaces();i=i+2)
{
	Face f1,f2;
	f1=myOBJ.getFace(i);
	f2=myOBJ.getFace(i+1);
	int y;
	
	vector<int> posList;

	Position3 wallPosition[4];

	for(y=0;y<f1.numVertices;y++)
	{
	

         int currentVertex = f1.faceVList[y];
         
		 posList.push_back(currentVertex);

		 Vertex v = myOBJ.getVertex(currentVertex);

		 wallPosition[y].setPosition(v.x,v.y,v.z);
	
	}
	for(y=0;y<f2.numVertices;y++)
	{
	int currentVertex = f2.faceVList[y];
	vector<int>::iterator it = find(posList.begin(), posList.end(),currentVertex);

	if(it != posList.end()) {}
    else 
	{
		Position3 walltmpPosition;

		walltmpPosition = wallPosition[2];

		Vertex v = myOBJ.getVertex(currentVertex);
		wallPosition[2].setPosition(v.x, v.y, v.z);

		wallPosition[3] = walltmpPosition;
		 //Vertex v = myOBJ.getVertex(currentVertex);
		 //wallPosition[3].setPosition(v.x,v.y,v.z); 
    }


	}


	Plane p;

	p.setPlane(wallPosition);
	
	// give light source to a face
	if(i==0)
	{	
		p.lightSource[0] = 0.3;
		p.lightSource[1] = 0.3;
		p.lightSource[2] = 0.3;
		p.radiosityValue[0] = 0.3;
		p.radiosityValue[1] = 0.3;
		p.radiosityValue[2] = 0.3;
	}	
	else
	{	
		p.lightSource[0] = 0;
		p.lightSource[1] = 0;
		p.lightSource[2] = 0;
		p.radiosityValue[0] = 0;
		p.radiosityValue[1] = 0;
		p.radiosityValue[2] = 0;
	}	

	/* add to patches list */
	patcheList.push_back(p);

	vector<Plane> subd = subdivide(p, 2);

	wallList.push_back(subd);

	//wallList.insert(wallList.end(),subd.begin(),subd.end());

}

/////**************//////
//Constructing Hemicube
//calculate central point at each patch first
/////**************//////
///////////////////////
	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}



					
		// call radiosity equation several times
		int nTimes = 0;
		while(nTimes++ != 4)
		{
			patcheList = RadiosityEqu(patcheList);
		}

		// process triangle
		int n = 0;
		GzToken		nameListColor[3];		/* color type names */
		GzPointer	valueListColor[3];	/* color type rgb pointers */
		while(n < patcheList.size())
		{
			valueListColor[0] = (GzPointer)patcheList.at(n).radiosityValue; 
			nameListColor[0] = GZ_RGB_COLOR; 
			GzPutAttribute(m_pRender, 1, nameListColor, valueListColor);

			// tri 1
			vertexList[0][0] = patcheList.at(n).position[0].pos[0];
			vertexList[0][1] = patcheList.at(n).position[0].pos[1];
			vertexList[0][2] = patcheList.at(n).position[0].pos[2];

			vertexList[1][0] = patcheList.at(n).position[1].pos[0];
			vertexList[1][1] = patcheList.at(n).position[1].pos[1];
			vertexList[1][2] = patcheList.at(n).position[1].pos[2];

			vertexList[2][0] = patcheList.at(n).position[2].pos[0];
			vertexList[2][1] = patcheList.at(n).position[2].pos[1];
			vertexList[2][2] = patcheList.at(n).position[2].pos[2];

			valueListTriangle[0] = (GzPointer)vertexList; 
			//valueListTriangle[1] = (GzPointer)normalList; 
			//valueListTriangle[2] = (GzPointer)uvList; 
			GzPutTriangle(m_pRender, 1, nameListTriangle, valueListTriangle); 


			// tri 2
			vertexList[2][0] = patcheList.at(n).position[2].pos[0];
			vertexList[2][1] = patcheList.at(n).position[2].pos[1];
			vertexList[2][2] = patcheList.at(n).position[2].pos[2];

			vertexList[3][0] = patcheList.at(n).position[3].pos[0];
			vertexList[3][1] = patcheList.at(n).position[3].pos[1];
			vertexList[3][2] = patcheList.at(n).position[3].pos[2];

			vertexList[1][0] = patcheList.at(n).position[1].pos[0];
			vertexList[1][1] = patcheList.at(n).position[1].pos[1];
			vertexList[1][2] = patcheList.at(n).position[1].pos[2];

			valueListTriangle[0] = (GzPointer)vertexList; 
			GzPutTriangle(m_pRender, 1, nameListTriangle, valueListTriangle); 

			n++;
		}
	
///////////////////////////////////////

	GzFlushDisplay2File(outfile, m_pDisplay); 	/* write out or update display to file*/
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pDisplay);	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox( "The input file was not closed\n" );

	if( fclose( outfile ) )
      AfxMessageBox( "The output file was not closed\n" );
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 
	int i;

	status |= GzFreeRender(m_pRender); 
	status |= GzFreeDisplay(m_pDisplay);


	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



