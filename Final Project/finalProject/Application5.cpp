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

/////////////// new final project include
#include "OBJParser.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
////////////////
/////////////////////
#include "Position3.h"
#include <array>
#include <string>
#include <vector>
#include <algorithm>
/////////////////////

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
/*
//////////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzNewDisplay(&m_pSampleDisplay[i], GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);
	}
/////
*/
	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes, &dispClass); 
	/*
//////////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzGetDisplayParams(m_pSampleDisplay[i], &xRes, &yRes, &dispClass); 
	}
////	 
*/

	status |= GzInitDisplay(m_pDisplay); 
/*
///////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzInitDisplay(m_pSampleDisplay[i]); 
	}

////
*/
	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay); 
/*
///////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzNewRender(&m_pSampleRender[i], GZ_Z_BUFFER_RENDER, m_pSampleDisplay[i]); 
	}
///
*/
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

/*
/////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzPutCamera(m_pSampleRender[i], &camera); 
	}

//
*/
#endif 

	/* Start Renderer */
	status |= GzBeginRender(m_pRender);
/*
////
	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzBeginRender(m_pSampleRender[i]);
	}
//
*/
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
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
		nameListShifters[0]=GZ_AASHIFTX;
		valueListShifters[0]=(GzPointer)&AAFilter[i][0];
		nameListShifters[1]=GZ_AASHIFTY;
		valueListShifters[1]=&AAFilter[i][1];

	    status |= GzPutAttribute(m_pSampleRender[i], 2, nameListShifters, valueListShifters);
	}
//
*/



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
/*
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	    status |= GzPutAttribute(m_pSampleRender[i], 3, nameListLights, valueListLights);
	}
//
*/
        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= GzPutAttribute(m_pRender, 1, nameListLights, valueListLights);
/*
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	    status |= GzPutAttribute(m_pSampleRender[i], 1, nameListLights, valueListLights);
	}
//
*/

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

/*
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	     status |= GzPutAttribute(m_pSampleRender[i], 6, nameListShader, valueListShader);
	}
//
*/


//	status |= GzPushMatrix(m_pRender, scale);    (watch  this !!!)

/*
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	    status |= GzPushMatrix(m_pSampleRender[i], scale);  
	}
//
//	status |= GzPushMatrix(m_pRender, rotateY); 
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	    status |= GzPushMatrix(m_pSampleRender[i], rotateY); 
	}
//
//	status |= GzPushMatrix(m_pRender, rotateX); 
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	   status |= GzPushMatrix(m_pSampleRender[i], rotateX); 
	}
//
*/

	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

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
////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	 status |= GzInitDisplay(m_pSampleDisplay[i]); 
	}
//
*/	
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
if(i==3||i==5)
{
		 normalList[y][0]=0.2;
		 normalList[y][1]=0.5;
		 normalList[y][2]=0.1;
}
else
{		 normalList[y][0]=0;
		 normalList[y][1]=0;
		 normalList[y][2]=0;
}
		   uvList[y][0]=0;
		   uvList[y][1]=0;

            
        }//vertex for loop


	  valueListTriangle[0] = (GzPointer)vertexList; 
	  valueListTriangle[1] = (GzPointer)normalList; 
	  valueListTriangle[2] = (GzPointer)uvList; 
	  GzPutTriangle(m_pRender, 3, nameListTriangle, valueListTriangle); 

	
}
//////***************/////  divide all the plane into patches

vector<vector<Plane>> wallList;

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

	if(it != posList.end()) {}  // find it 
    else 
	{
		 Position3 walltmpPosition;

		 walltmpPosition=wallPosition[2];

		 Vertex v = myOBJ.getVertex(currentVertex);
		 wallPosition[2].setPosition(v.x,v.y,v.z); 

		 wallPosition[3]=walltmpPosition;

		/////////////
		// Vertex v = myOBJ.getVertex(currentVertex);
		// wallPosition[3].setPosition(v.x,v.y,v.z); 

    }
	

	}


	Plane p;

	p.setPlane(wallPosition);

	vector<Plane> subd = subdivide(p, 2);

	wallList.push_back(subd);

	//wallList.insert(wallList.end(),subd.begin(),subd.end());

}

/////**************//////
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
/*
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


	
	//* Walk through the list of triangles, set color 
	//* and render each triangle 
	
	while( fscanf(infile, "%s", dummy) == 1) { 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 

	    
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     * NOTE: this sequence matches the nameList token sequence
	    
	     valueListTriangle[0] = (GzPointer)vertexList; 
		 valueListTriangle[1] = (GzPointer)normalList; 
		 valueListTriangle[2] = (GzPointer)uvList; 
	//	 GzPutTriangle(m_pRender, 3, nameListTriangle, valueListTriangle); 

////
			for(i=0;i<AAKERNEL_SIZE;i++)
	{
	//GzPutTriangle(m_pSampleRender[i], 3, nameListTriangle, valueListTriangle); 
	}
//
											}//while loop 

/////////////////////////////////////// weight the all 6 image 

	GzIntensity tmpr[6],tmpg[6],tmpb[6],tmpa[6];
	GzDepth tmpz[6];
	GzIntensity antiR,antiG,antiB;	
		int x,y;
		
		for(y=0;y<m_nHeight;y++)
		{
			for(x=0;x<m_nWidth;x++)
			{
			GzGetDisplay(m_pSampleDisplay[0],x,y,&tmpr[0],&tmpg[0],&tmpb[0],&tmpa[0],&tmpz[0]);
			GzGetDisplay(m_pSampleDisplay[1],x,y,&tmpr[1],&tmpg[1],&tmpb[1],&tmpa[1],&tmpz[1]);
			GzGetDisplay(m_pSampleDisplay[2],x,y,&tmpr[2],&tmpg[2],&tmpb[2],&tmpa[2],&tmpz[2]);
			GzGetDisplay(m_pSampleDisplay[3],x,y,&tmpr[3],&tmpg[3],&tmpb[3],&tmpa[3],&tmpz[3]);
			GzGetDisplay(m_pSampleDisplay[4],x,y,&tmpr[4],&tmpg[4],&tmpb[4],&tmpa[4],&tmpz[4]);
			GzGetDisplay(m_pSampleDisplay[5],x,y,&tmpr[5],&tmpg[5],&tmpb[5],&tmpa[5],&tmpz[5]);

			antiR=tmpr[0]*AAFilter[0][2]+tmpr[1]*AAFilter[1][2]+tmpr[2]*AAFilter[2][2]+tmpr[3]*AAFilter[3][2]+tmpr[4]*AAFilter[4][2]+tmpr[5]*AAFilter[5][2];
			antiG=tmpg[0]*AAFilter[0][2]+tmpg[1]*AAFilter[1][2]+tmpg[2]*AAFilter[2][2]+tmpg[3]*AAFilter[3][2]+tmpg[4]*AAFilter[4][2]+tmpg[5]*AAFilter[5][2];
			antiB=tmpb[0]*AAFilter[0][2]+tmpb[1]*AAFilter[1][2]+tmpb[2]*AAFilter[2][2]+tmpb[3]*AAFilter[3][2]+tmpb[4]*AAFilter[4][2]+tmpb[5]*AAFilter[5][2];

	//		GzPutDisplay(m_pDisplay,x,y,antiR,antiG,antiB,tmpa[0],tmpz[0]);
			}

		}

///////////////////////////////////////
*/	
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

	for(i=0;i<AAKERNEL_SIZE;i++)
	{
	status |= GzFreeRender(m_pSampleRender[i]); 
	status |= GzFreeDisplay(m_pSampleDisplay[i]);
	}
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



