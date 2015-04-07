/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"


#define X       0               /* array indicies for position vector */
#define Y       1
#define Z       2

#define R       0               /* array indicies for rbg vector */
#define G       1
#define B       2

#define U       0              /* array indicies for uv vector */
#define V       1
#define pi      3.1415926535897932384626433



short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

float unify(float x, float y, float z)
{
	float Uni;
//unitDivider[0]=
	Uni=sqrt((pow(x,2)+pow(y,2)+pow(z,2)));
	return Uni;
}


//Set by myself  Find the maximum   0=x,1=y,2=z;
int FindMax(float vertex[][3])
{


	if(vertex[0][Y]>=vertex[1][Y])
	{
		if(vertex[0][Y]>=vertex[2][Y])
		{	
			
			if(vertex[0][Y]==vertex[1][Y])// if 0,y==1,y
			{
				if(vertex[0][X]>vertex[1][X]){return 0;}
				else{return 1;}
			}		
		
			if(vertex[0][Y]==vertex[2][Y])//if 0,y==2,y
			{
				if(vertex[0][X]>vertex[2][X]){return 0;}
				else{return 2;}
			
			}
			
			return 0;
		
		}
		else{return 2;}
	
	
	}
	else
	{
		if(vertex[1][Y]>=vertex[2][Y])
		{
		
			if(vertex[1][Y]==vertex[2][Y]) ///1,y==2,y
			{
				if(vertex[1][X]>vertex[2][X]){return 1;}
				else{return 2;}
					
			}
			return 1;
		}
		else{return 2;}
	}
/*
	if(x>y)
	{	
		if(x>z){return 0;}	
		else return 2; 
	}
	else
	{    
		
		if(y>z){return 1;}
		else return 2;
	}
*/

} 
struct DDA
{
	GzCoord start;
	GzCoord end;
	GzCoord current;
	float slopex;
	float slopez;
	////////// 
	GzColor			 color; ///color
	GzCoord			 normal;// normal
	GzTextureIndex	 UV;    // uv 

};

struct SPAN
{
	GzCoord start;
	GzCoord end;
	GzCoord current;
    float slopez;
	////////// 
	GzColor			 color; ///color
	GzCoord			 normal; //normal
	GzTextureIndex	 UV;    // uv 
};



int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value


mat[1][1]=cos(degree*pi/180.0);
mat[1][2]=-(sin(degree*pi/180.0));
mat[2][1]=sin(degree*pi/180.0);
mat[2][2]=cos(degree*pi/180.0);




	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

mat[0][0]=cos(degree*pi/180.0);
mat[0][2]=sin(degree*pi/180.0);
mat[2][0]=-(sin(degree*pi/180.0));
mat[2][2]=cos(degree*pi/180.0);


	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

mat[0][0]=cos(degree*pi/180.0);
mat[0][1]=-(sin(degree*pi/180.0));
mat[1][0]=sin(degree*pi/180.0);
mat[1][1]=cos(degree*pi/180.0);


	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value


mat[0][3]=translate[0];
mat[1][3]=translate[1];
mat[2][3]=translate[2];
mat[3][3]=1;




	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value


mat[0][0]=scale[0];
mat[1][1]=scale[1];
mat[2][2]=scale[2];
mat[3][3]=1;




	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 

	*render= new GzRender[sizeof(GzRender)];  //malloc a renderer struct 
	(*render)->renderClass=renderClass;
	(*render)->display=display;
	(*render)->open=1;
	(*render)->matlevel=0;//  stack default



	if(renderClass==1){printf("Z_Buffer is ok.");}  //check for legal class GZ_Z_BUFFER_RENDER 


///////// init default camera 

	(*render)->camera.position[0]=13.2;
	(*render)->camera.position[1]=-8.7;
	(*render)->camera.position[2]=-14.8;

	(*render)->camera.lookat[0]=0.8;
	(*render)->camera.lookat[1]=0.7;
	(*render)->camera.lookat[2]=4.5;

	(*render)->camera.worldup[0]=-0.2;
	(*render)->camera.worldup[1]=1.0;
	(*render)->camera.worldup[2]=0.0;
	
	(*render)->camera.FOV=53.7;

	int i,j;
	
	for(j=0;j<4;j++){
	for(i=0;i<4;i++){
		(*render)->camera.Xiw[i][j]=0;
		(*render)->camera.Xpi[i][j]=0;
	}
	}
/*
	   
////set Xsp
//	float Zmax=2147483647;

	float Zmax=INT_MAX;

	float d;
 
  d = 1/tan (((*render)->camera.FOV*pi/180)/2);


  
	
	for(j=0;j<4;j++){
	for(i=0;i<4;i++){
		(*render)->Xsp[i][j]=0;
	}
	}

	(*render)->Xsp[0][0]=(display->xres/2);
	(*render)->Xsp[0][3]=(display->xres/2);
	(*render)->Xsp[1][1]=-(display->yres)/2;
	(*render)->Xsp[1][3]=(display->yres/2);
	(*render)->Xsp[2][2]=Zmax/d;
	(*render)->Xsp[3][3]=1;

*/


	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render; 

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
int i,j;
///// 
		int index;

	 for(j=0;j<render->display->yres;j++){
    for(i=0;i<render->display->xres;i++){

	index=i+(j*render->display->xres);
	 
	 render->display->fbuf[index].z=2147483647;

	 render->display->fbuf[index].red=0;
	 render->display->fbuf[index].green=0;
	 render->display->fbuf[index].blue=0;
	 render->display->fbuf[index].alpha=0;
	
											}
	 }

///// compute projection xform Xpi from camera definition 

  float d;
 
  d = 1/ tan ((render->camera.FOV*pi/180)/2);

	 render->camera.Xpi[0][0]=1;
	 render->camera.Xpi[1][1]=1;
	 render->camera.Xpi[2][2]=1;
	 render->camera.Xpi[3][3]=1;
	 render->camera.Xpi[3][2]=1/d;	


/////// compute Xiw	


	float IC[3];
	float absoluteIC;

	float X_axis[3],Y_axis[3],Z_axis[3];
	float upVector[3],up_Vector[3];

	 IC[X]=render->camera.lookat[X]-render->camera.position[X];
	 IC[Y]=render->camera.lookat[Y]-render->camera.position[Y];
	 IC[Z]=render->camera.lookat[Z]-render->camera.position[Z];
	
	 absoluteIC=sqrt((pow(IC[X],2)+pow(IC[Y],2)+pow(IC[Z],2)));

	 Z_axis[X]=IC[X]/absoluteIC;
	 Z_axis[Y]=IC[Y]/absoluteIC;
	 Z_axis[Z]=IC[Z]/absoluteIC;

	 upVector[X]=render->camera.worldup[X];
	 upVector[Y]=render->camera.worldup[Y];
	 upVector[Z]=render->camera.worldup[Z];

	 float dotResult=upVector[X]*Z_axis[X]+upVector[Y]*Z_axis[Y]+upVector[Z]*Z_axis[Z];
	/////set up up_Vector
	up_Vector[X]=upVector[X]-(dotResult*Z_axis[X]);
	up_Vector[Y]=upVector[Y]-(dotResult*Z_axis[Y]);
	up_Vector[Z]=upVector[Z]-(dotResult*Z_axis[Z]);

	float absoluteUp_;

	absoluteUp_=sqrt((pow(up_Vector[X],2)+pow(up_Vector[Y],2)+pow(up_Vector[Z],2)));


	Y_axis[X]=up_Vector[X]/absoluteUp_;
	Y_axis[Y]=up_Vector[Y]/absoluteUp_;
	Y_axis[Z]=up_Vector[Z]/absoluteUp_;
	
	X_axis[X]=(Y_axis[Y]*Z_axis[Z]-Y_axis[Z]*Z_axis[Y]);
	X_axis[Y]=(Y_axis[Z]*Z_axis[X]-Y_axis[X]*Z_axis[Z]);
	X_axis[Z]=(Y_axis[X]*Z_axis[Y]-Y_axis[Y]*Z_axis[X]);
	
	render->camera.Xiw[0][0]=X_axis[X];
	render->camera.Xiw[0][1]=X_axis[Y];
	render->camera.Xiw[0][2]=X_axis[Z];
	render->camera.Xiw[0][3]=-(X_axis[X]*render->camera.position[X]+X_axis[Y]*render->camera.position[Y]+X_axis[Z]*render->camera.position[Z]);
	render->camera.Xiw[1][0]=Y_axis[X];
	render->camera.Xiw[1][1]=Y_axis[Y];
	render->camera.Xiw[1][2]=Y_axis[Z];
	render->camera.Xiw[1][3]=-(Y_axis[X]*render->camera.position[X]+Y_axis[Y]*render->camera.position[Y]+Y_axis[Z]*render->camera.position[Z]);
	render->camera.Xiw[2][0]=Z_axis[X];
	render->camera.Xiw[2][1]=Z_axis[Y];
	render->camera.Xiw[2][2]=Z_axis[Z];
	render->camera.Xiw[2][3]=-(Z_axis[X]*render->camera.position[X]+Z_axis[Y]*render->camera.position[Y]+Z_axis[Z]*render->camera.position[Z]);
	render->camera.Xiw[3][0]=0;
	render->camera.Xiw[3][1]=0;
	render->camera.Xiw[3][2]=0;
	render->camera.Xiw[3][3]=1;



	////////init Ximage - put Xsp at base of stack, push on Xpi and Xiw 


	   
////set Xsp
//	float Zmax=2147483647;

	float Zmax=INT_MAX;

//	float d;

 
  d = 1/tan (((render)->camera.FOV*pi/180)/2);


  
	
	for(j=0;j<4;j++){
	for(i=0;i<4;i++){
		(render)->Xsp[i][j]=0;
	}
	}

	render->Xsp[0][0]=render->display->xres/2;
	render->Xsp[0][3]=render->display->xres/2;
	render->Xsp[1][1]=-(render->display->yres)/2;
	render->Xsp[1][3]=render->display->yres/2;
	render->Xsp[2][2]=Zmax/d;
	render->Xsp[3][3]=1;

/////////////////////////////////////////


 //GzPushMatrix(GzRender *render, GzMatrix	matrix)

	GzPushMatrix(render,render->Xsp);
	GzPushMatrix(render,render->camera.Xpi);
	GzPushMatrix(render,render->camera.Xiw);



	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
    render->camera.position[X]=camera->position[X];
	render->camera.position[Y]=camera->position[Y];
	render->camera.position[Z]=camera->position[Z];

	render->camera.lookat[X]=camera->lookat[X];
	render->camera.lookat[Y]=camera->lookat[Y];
	render->camera.lookat[Z]=camera->lookat[Z];

	render->camera.worldup[X]=camera->worldup[X];
	render->camera.worldup[Y]=camera->worldup[Y];
	render->camera.worldup[Z]=camera->worldup[Z];

	render->camera.FOV=camera->FOV;
	




	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{

	int size=MATLEVELS;
	
	if(render->matlevel>=size)
	{printf("Stack is full!");}
	else if(render->matlevel<0){printf("Stack is wrong!!");}



    else
	{	
		int i,j;
	
		for(j=0;j<4;j++){

			for(i=0;i<4;i++)
			{
			render->Ximage[render->matlevel][j][i]=matrix[j][i];
			}

						}
			render->matlevel++;

	}
                     

	
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/

	if(render->matlevel==0){printf("stack is emputy can't do the pop!");}


    else
	{
		render->matlevel--;
                            
    }



	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	int NumberOfAttributes=numAttributes;
	int i;

//	while(NumberOfAttributes>0){



	for(i=0;i<NumberOfAttributes;i++){

		GzToken Decider=nameList[i];


		if(Decider==GZ_RGB_COLOR)
		{

		GzColor *color=reinterpret_cast<GzColor*>(valueList[i]);
		render->flatcolor[0]=ctoi(color[0][0]);
		render->flatcolor[1]=ctoi(color[0][1]);
	    render->flatcolor[2]=ctoi(color[0][2]);
	
		}
		else if(Decider==GZ_DIRECTIONAL_LIGHT)
		{
			render->numlights=NumberOfAttributes;

			GzLight *dirlight=reinterpret_cast<GzLight*>(valueList[i]);
			///////////////////////////////////////
			render->lights[i].direction[X]=dirlight[0].direction[X];
			render->lights[i].direction[Y]=dirlight[0].direction[Y];
			render->lights[i].direction[Z]=dirlight[0].direction[Z];

			render->lights[i].color[R]=dirlight[0].color[R];
			render->lights[i].color[G]=dirlight[0].color[G];
			render->lights[i].color[B]=dirlight[0].color[B];
		}
		else if(Decider==GZ_AMBIENT_LIGHT)
		{
			GzLight *ambient=reinterpret_cast<GzLight*>(valueList[i]);
			//////////////////////////////////////////////////////////
			render->ambientlight.direction[X]=ambient[0].direction[X];
			render->ambientlight.direction[Y]=ambient[0].direction[Y];
			render->ambientlight.direction[Z]=ambient[0].direction[Z];
			render->ambientlight.color[R]=ambient[0].color[R];
			render->ambientlight.color[G]=ambient[0].color[G];
			render->ambientlight.color[B]=ambient[0].color[B];
		}
		else if(Decider==GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor *color=reinterpret_cast<GzColor*>(valueList[i]);
			render->Kd[0]=color[0][0];
			render->Kd[1]=color[0][1];
			render->Kd[2]=color[0][2];

		}
		else if(Decider==GZ_AMBIENT_COEFFICIENT)
		{
			GzColor *color=reinterpret_cast<GzColor*>(valueList[i]);
			render->Ka[0]=color[0][0];
			render->Ka[1]=color[0][1];
			render->Ka[2]=color[0][2];
		}
		else if(Decider==GZ_SPECULAR_COEFFICIENT)
		{
			GzColor *color=reinterpret_cast<GzColor*>(valueList[i]);
			render->Ks[0]=color[0][0];
			render->Ks[1]=color[0][1];
			render->Ks[2]=color[0][2];

		}
		else if(Decider==GZ_INTERPOLATE)
		{
			int *interpolate=static_cast<int*>(valueList[i]);
			render->interp_mode=*interpolate;
		}
		else if(Decider==GZ_DISTRIBUTION_COEFFICIENT)
		{
			float *sp=reinterpret_cast<float*>(valueList[i]);
			render->spec=*sp;
		}
		else if(Decider==GZ_TEXTURE_MAP)
		{
		   GzTexture tex=static_cast<GzTexture>(valueList[i]);
		   render->tex_fun=tex;  //transfer function
		}
		else if(Decider==GZ_AASHIFTX)
		{
			float *shiftx=reinterpret_cast<float*>(valueList[i]);
			render->dx=*shiftx;

			//render->dx=*(float*)valueList[i];
		}
		else if(Decider==GZ_AASHIFTY)
		{
			float *shifty=reinterpret_cast<float*>(valueList[i]);
			render->dy=*shifty;
		}
							}//for loop





	
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
{

/* numParts : how many names and values */
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space d
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 	
	if(render->interp_mode==GZ_FLAT){

	int NumberOfParts=numParts;
	int i,j;
//	int tokencounter;
//	while(NumberOfParts>0){

//		for(tokencounter=0;tokencounter<NumberOfParts;tokencounter++){

		//GzToken Decider=nameList[NumberOfParts-1];



		
		GzToken Decider=nameList[0];
		GzToken Decider2=nameList[1];
		
		GzCoord *tmp=static_cast<GzCoord*>(valueList[0]);
		GzCoord *normal=static_cast<GzCoord*>(valueList[1]);

		GzTextureIndex *uv=static_cast<GzTextureIndex*>(valueList[2]);  

		

		if(Decider==GZ_POSITION){}

		if(Decider2==GZ_NORMAL){}

   //float current[2],end[2],start[2],slope;
   float vertex[3][3];
   float w[3];
   int maxVertex,secondVertex,thirdVertex;
   DDA ED12,ED23,ED13;



		GzMatrix FMatrix;

		if(ComputeStack(render, render->matlevel, &FMatrix,0));// 0 stands for Vertexstack computation 

//////////////////////// ScreenMatrix
//float ScreenMatrix[4];


vertex[0][X]=FMatrix[0][0]*tmp[0][X]+FMatrix[0][1]*tmp[0][Y]+FMatrix[0][2]*tmp[0][Z]+FMatrix[0][3]*1;
vertex[0][Y]=FMatrix[1][0]*tmp[0][X]+FMatrix[1][1]*tmp[0][Y]+FMatrix[1][2]*tmp[0][Z]+FMatrix[1][3]*1;
vertex[0][Z]=FMatrix[2][0]*tmp[0][X]+FMatrix[2][1]*tmp[0][Y]+FMatrix[2][2]*tmp[0][Z]+FMatrix[2][3]*1;
w[0]=FMatrix[3][0]*tmp[0][X]+FMatrix[3][1]*tmp[0][Y]+FMatrix[3][2]*tmp[0][Z]+FMatrix[3][3]*1;

vertex[1][X]=FMatrix[0][0]*tmp[1][X]+FMatrix[0][1]*tmp[1][Y]+FMatrix[0][2]*tmp[1][Z]+FMatrix[0][3]*1;
vertex[1][Y]=FMatrix[1][0]*tmp[1][X]+FMatrix[1][1]*tmp[1][Y]+FMatrix[1][2]*tmp[1][Z]+FMatrix[1][3]*1;
vertex[1][Z]=FMatrix[2][0]*tmp[1][X]+FMatrix[2][1]*tmp[1][Y]+FMatrix[2][2]*tmp[1][Z]+FMatrix[2][3]*1;
w[1]=FMatrix[3][0]*tmp[1][X]+FMatrix[3][1]*tmp[1][Y]+FMatrix[3][2]*tmp[1][Z]+FMatrix[3][3]*1;

vertex[2][X]=FMatrix[0][0]*tmp[2][X]+FMatrix[0][1]*tmp[2][Y]+FMatrix[0][2]*tmp[2][Z]+FMatrix[0][3]*1;
vertex[2][Y]=FMatrix[1][0]*tmp[2][X]+FMatrix[1][1]*tmp[2][Y]+FMatrix[1][2]*tmp[2][Z]+FMatrix[1][3]*1;
vertex[2][Z]=FMatrix[2][0]*tmp[2][X]+FMatrix[2][1]*tmp[2][Y]+FMatrix[2][2]*tmp[2][Z]+FMatrix[2][3]*1;
w[2]=FMatrix[3][0]*tmp[2][X]+FMatrix[3][1]*tmp[2][Y]+FMatrix[3][2]*tmp[2][Z]+FMatrix[3][3]*1;
			

//////////////////final x,y,z
vertex[0][X]=vertex[0][X]/w[0];
vertex[0][Y]=vertex[0][Y]/w[0];
vertex[0][Z]=vertex[0][Z]/w[0];

vertex[1][X]=vertex[1][X]/w[1];
vertex[1][Y]=vertex[1][Y]/w[1];
vertex[1][Z]=vertex[1][Z]/w[1];

vertex[2][X]=vertex[2][X]/w[2];
vertex[2][Y]=vertex[2][Y]/w[2];
vertex[2][Z]=vertex[2][Z]/w[2];

/////////////  shift dx,dy for antialising
vertex[0][X]=vertex[0][X]-render->dx;
vertex[1][X]=vertex[1][X]-render->dx;
vertex[2][X]=vertex[2][X]-render->dx;

vertex[0][Y]=vertex[0][Y]-render->dy;
vertex[1][Y]=vertex[1][Y]-render->dy;
vertex[2][Y]=vertex[2][Y]-render->dy;


////////////////////////////////////////////////// bring Normal from mode space to image


		//float vertex[3][3];// set the normalized vertex
		float _w[3];
	
	

		
		GzMatrix NMatrix;
		float unitDivider[3];

		if(ComputeStack(render, render->matlevel, &NMatrix,1)); // 1 stands for normalstack computation 

////////////////////////////////////////////////Normalize the NMatrix

		NMatrix[0][3]=0;// remove Translation
		NMatrix[1][3]=0;
		NMatrix[2][3]=0;
					
unitDivider[0]=sqrt((pow(NMatrix[0][0],2)+pow(NMatrix[0][1],2)+pow(NMatrix[0][2],2)));
unitDivider[1]=sqrt((pow(NMatrix[1][0],2)+pow(NMatrix[1][1],2)+pow(NMatrix[1][2],2)));
unitDivider[2]=sqrt((pow(NMatrix[2][0],2)+pow(NMatrix[2][1],2)+pow(NMatrix[2][2],2)));


for(j=0;j<3;j++)
{
	for(i=0;i<3;i++)
	{
	NMatrix[j][i]=NMatrix[j][i]/unitDivider[j];
	}

}
////////////////////////////////////////////////		
render->_Normal[0][X]=NMatrix[0][0]*normal[0][X]+NMatrix[0][1]*normal[0][Y]+NMatrix[0][2]*normal[0][Z]+NMatrix[0][3]*1;
render->_Normal[0][Y]=NMatrix[1][0]*normal[0][X]+NMatrix[1][1]*normal[0][Y]+NMatrix[1][2]*normal[0][Z]+NMatrix[1][3]*1;
render->_Normal[0][Z]=NMatrix[2][0]*normal[0][X]+NMatrix[2][1]*normal[0][Y]+NMatrix[2][2]*normal[0][Z]+NMatrix[2][3]*1;
_w[0]=NMatrix[3][0]*normal[0][X]+NMatrix[3][1]*normal[0][Y]+NMatrix[3][2]*normal[0][Z]+NMatrix[3][3]*1;

render->_Normal[1][X]=NMatrix[0][0]*normal[1][X]+NMatrix[0][1]*normal[1][Y]+NMatrix[0][2]*normal[1][Z]+NMatrix[0][3]*1;
render->_Normal[1][Y]=NMatrix[1][0]*normal[1][X]+NMatrix[1][1]*normal[1][Y]+NMatrix[1][2]*normal[1][Z]+NMatrix[1][3]*1;
render->_Normal[1][Z]=NMatrix[2][0]*normal[1][X]+NMatrix[2][1]*normal[1][Y]+NMatrix[2][2]*normal[1][Z]+NMatrix[2][3]*1;
_w[1]=NMatrix[3][0]*normal[1][X]+NMatrix[3][1]*normal[1][Y]+NMatrix[3][2]*normal[1][Z]+NMatrix[3][3]*1;

render->_Normal[2][X]=NMatrix[0][0]*normal[2][X]+NMatrix[0][1]*normal[2][Y]+NMatrix[0][2]*normal[2][Z]+NMatrix[0][3]*1;
render->_Normal[2][Y]=NMatrix[1][0]*normal[2][X]+NMatrix[1][1]*normal[2][Y]+NMatrix[1][2]*normal[2][Z]+NMatrix[1][3]*1;
render->_Normal[2][Z]=NMatrix[2][0]*normal[2][X]+NMatrix[2][1]*normal[2][Y]+NMatrix[2][2]*normal[2][Z]+NMatrix[2][3]*1;
_w[2]=NMatrix[3][0]*normal[2][X]+NMatrix[3][1]*normal[2][Y]+NMatrix[3][2]*normal[2][Z]+NMatrix[3][3]*1;
			

//////////////////final norx,nory,norz
render->_Normal[0][X]=render->_Normal[0][X]/_w[0];
render->_Normal[0][Y]=render->_Normal[0][Y]/_w[0];
render->_Normal[0][Z]=render->_Normal[0][Z]/_w[0];

render->_Normal[1][X]=render->_Normal[1][X]/_w[1];
render->_Normal[1][Y]=render->_Normal[1][Y]/_w[1];
render->_Normal[1][Z]=render->_Normal[1][Z]/_w[1];

render->_Normal[2][X]=render->_Normal[2][X]/_w[2];
render->_Normal[2][Y]=render->_Normal[2][Y]/_w[2];
render->_Normal[2][Z]=render->_Normal[2][Z]/_w[2];
////////////////////////////////////////
GzColor finalcolor;

shade_equation(render,render->_Normal[0],&finalcolor);/* flat shade based on the norm of vert0 */
///////////////////////////////////////////	
		maxVertex=FindMax(vertex);

		// define first, second, third
		if(maxVertex==0)
		{	
			if(vertex[1][1]==vertex[2][1])
			{
				if(vertex[1][X]>vertex[2][X]){secondVertex=1; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=1;}
			
			}
			else if(vertex[1][1]>vertex[2][1]){secondVertex=1; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=1;}
		}
		else if(maxVertex==1)
		{
			if(vertex[0][1]==vertex[2][1])
			{
				if(vertex[0][X]>vertex[2][X]){secondVertex=0; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[2][1]){secondVertex=0; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=0;}
		}
		else if(maxVertex==2)
		{
			if(vertex[0][1]==vertex[1][1])
			{
			if(vertex[0][X]>vertex[1][X]){secondVertex=0; thirdVertex=1;}
			else{secondVertex=1; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[1][1]){secondVertex=0; thirdVertex=1;}
			else {secondVertex=1; thirdVertex=0;}
		
		}
		
		ED12.start[0]=vertex[thirdVertex][0];
		ED12.start[1]=vertex[thirdVertex][1];
		ED12.start[2]=vertex[thirdVertex][2];
		ED12.end[0]=vertex[secondVertex][0];
		ED12.end[1]=vertex[secondVertex][1];
		ED12.end[2]=vertex[secondVertex][2];
		ED12.current[0]=vertex[thirdVertex][0];
		ED12.current[1]=vertex[thirdVertex][1];
		ED12.current[2]=vertex[thirdVertex][2];

		ED12.slopex=(vertex[secondVertex][0]-vertex[thirdVertex][0])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
		ED12.slopez=(vertex[secondVertex][2]-vertex[thirdVertex][2])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
////////////////////////////////////////////////////////////
		ED13.start[0]=vertex[thirdVertex][0];
		ED13.start[1]=vertex[thirdVertex][1];
		ED13.start[2]=vertex[thirdVertex][2];
		ED13.end[0]=vertex[maxVertex][0];
		ED13.end[1]=vertex[maxVertex][1];
		ED13.end[2]=vertex[maxVertex][2];
		ED13.current[0]=vertex[thirdVertex][0];
		ED13.current[1]=vertex[thirdVertex][1];
		ED13.current[2]=vertex[thirdVertex][2];

		ED13.slopex=(vertex[maxVertex][0]-vertex[thirdVertex][0])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
		ED13.slopez=(vertex[maxVertex][2]-vertex[thirdVertex][2])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
/////////////////////////////////////////////////////////////

		ED23.start[0]=vertex[secondVertex][0];
		ED23.start[1]=vertex[secondVertex][1];
		ED23.start[2]=vertex[secondVertex][2];
		ED23.end[0]=vertex[maxVertex][0];
		ED23.end[1]=vertex[maxVertex][1];
		ED23.end[2]=vertex[maxVertex][2];
		ED23.current[0]=vertex[secondVertex][0];
		ED23.current[1]=vertex[secondVertex][1];
		ED23.current[2]=vertex[secondVertex][2];

		ED23.slopex=(vertex[maxVertex][0]-vertex[secondVertex][0])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
		ED23.slopez=(vertex[maxVertex][2]-vertex[secondVertex][2])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
////////////////////////////////////////////////////////////////////




		if(ED12.slopex<ED13.slopex)////////Left
		{	
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;
			int bughappen=0;
			int bughappen2=0;
	
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];
				
		


			if((DeltaY==0)&&(ED12.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs


				while(((ED12.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{ 

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED12.current[0];
			spanline.start[2]=ED12.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED12.current[0];
			spanline.current[2]=ED12.current[2];
			spanline.slopez=(ED13.current[2]-ED12.current[2])/(ED13.current[0]-ED12.current[0]);
			/////span start 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
		//	deltaX=floor(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED12.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
			//GzPutDisplay(render->display,spanline.current[0],ED12.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED12.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
	
			}
			spanXtimes++;
			deltaX=1;

		
			
														}
			spanYtimes++;
			DeltaY=1;
		}//firstwhile 
				
				
				if((ED12.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs
			
				DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
				if(DeltaY==0){DeltaY=1;}

			while((ED13.current[1]+DeltaY<=vertex[maxVertex][1])&&(bughappen==0)&&(bughappen2==0)){

				if(First1223)
				{
				   if(ED23.current[1]==ED12.current[1])
				  {
				  DeltaY=ceil(ED23.current[1])-ED23.current[1];
			    	if(DeltaY==0){DeltaY=1;}
				  }
				  else
				  {
				  DeltaY=(ED12.current[1]+1)-ED23.current[1];
				  }
				}
				else{	DeltaY=1;}



					
				First1223=false;
				
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
			//		else{DeltaY=1;}
				
			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED23.current[0];
			spanline.start[2]=ED23.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED23.current[0];
			spanline.current[2]=ED23.current[2];
			spanline.slopez=(ED13.current[2]-ED23.current[2])/(ED13.current[0]-ED23.current[0]);
			/////span start 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
	//		deltaX=floor(spanline.start[0])-spanline.start[0];
			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED23.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{


		//	GzPutDisplay(render->display,spanline.current[0],ED23.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED23.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
		

			}///if
			spanXtimes++;
			deltaX=1;

		
			
														}
			
					}
		
		}
		else /// Right
		{
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;

			int bughappen=0;
			int bughappen2=0;
		
		
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];


			if((DeltaY==0)&&(ED13.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs
				
				while(((ED13.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{
				

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);

			
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED12.current[0];
			spanline.end[2]=ED12.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED12.current[2]-ED13.current[2])/(ED12.current[0]-ED13.current[0]);
			/////span start 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];


			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			}
			spanXtimes++;
			deltaX=1;
			
														}//secondwhile
			spanYtimes++;
			DeltaY=1;
			}//firstwhile

					if((ED13.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs

						DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
						
						if(DeltaY==0){DeltaY=1;}

				while(ED13.current[1]+DeltaY<=vertex[maxVertex][1]&&(bughappen==0)&&(bughappen2==0))
					{

					
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
				//	else{DeltaY=1;}

				if(First1223)
				{
					 if(ED23.current[1]==ED12.current[1])
				  {
				  DeltaY=ceil(ED23.current[1])-ED23.current[1];
			    	if(DeltaY==0){DeltaY=1;}
				  }
				  else
				  {
				  DeltaY=(ED12.current[1]+1)-ED23.current[1];
				  }
				}
				else{	DeltaY=1;}
					
				First1223=false;
					
				

			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED23.current[0];
			spanline.end[2]=ED23.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED23.current[2]-ED13.current[2])/(ED23.current[0]-ED13.current[0]);
			/////span start 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			}
			spanXtimes++;
			deltaX=1;
			
														}//second while
						
	
		
					}//firstwhile



		}//else   



		
		}
		else if(render->interp_mode==GZ_COLOR)  
		{
				int NumberOfParts=numParts;
	int i,j;

		
		GzToken Decider=nameList[0];
		GzToken Decider2=nameList[1];
		
		GzCoord *tmp=static_cast<GzCoord*>(valueList[0]);
		 GzCoord *normal=static_cast<GzCoord*>(valueList[1]);
		  GzTextureIndex *uv=static_cast<GzTextureIndex*>(valueList[2]);

		if(Decider==GZ_POSITION){}

		if(Decider2==GZ_NORMAL){}

   //float current[2],end[2],start[2],slope;
   float vertex[3][3];
   float w[3];
   int maxVertex,secondVertex,thirdVertex;
   DDA ED12,ED23,ED13;



		GzMatrix FMatrix;

		if(ComputeStack(render, render->matlevel, &FMatrix,0));// 0 stands for Vertexstack computation 

//////////////////////// ScreenMatrix
//float ScreenMatrix[4];


vertex[0][X]=FMatrix[0][0]*tmp[0][X]+FMatrix[0][1]*tmp[0][Y]+FMatrix[0][2]*tmp[0][Z]+FMatrix[0][3]*1;
vertex[0][Y]=FMatrix[1][0]*tmp[0][X]+FMatrix[1][1]*tmp[0][Y]+FMatrix[1][2]*tmp[0][Z]+FMatrix[1][3]*1;
vertex[0][Z]=FMatrix[2][0]*tmp[0][X]+FMatrix[2][1]*tmp[0][Y]+FMatrix[2][2]*tmp[0][Z]+FMatrix[2][3]*1;
w[0]=FMatrix[3][0]*tmp[0][X]+FMatrix[3][1]*tmp[0][Y]+FMatrix[3][2]*tmp[0][Z]+FMatrix[3][3]*1;

vertex[1][X]=FMatrix[0][0]*tmp[1][X]+FMatrix[0][1]*tmp[1][Y]+FMatrix[0][2]*tmp[1][Z]+FMatrix[0][3]*1;
vertex[1][Y]=FMatrix[1][0]*tmp[1][X]+FMatrix[1][1]*tmp[1][Y]+FMatrix[1][2]*tmp[1][Z]+FMatrix[1][3]*1;
vertex[1][Z]=FMatrix[2][0]*tmp[1][X]+FMatrix[2][1]*tmp[1][Y]+FMatrix[2][2]*tmp[1][Z]+FMatrix[2][3]*1;
w[1]=FMatrix[3][0]*tmp[1][X]+FMatrix[3][1]*tmp[1][Y]+FMatrix[3][2]*tmp[1][Z]+FMatrix[3][3]*1;

vertex[2][X]=FMatrix[0][0]*tmp[2][X]+FMatrix[0][1]*tmp[2][Y]+FMatrix[0][2]*tmp[2][Z]+FMatrix[0][3]*1;
vertex[2][Y]=FMatrix[1][0]*tmp[2][X]+FMatrix[1][1]*tmp[2][Y]+FMatrix[1][2]*tmp[2][Z]+FMatrix[1][3]*1;
vertex[2][Z]=FMatrix[2][0]*tmp[2][X]+FMatrix[2][1]*tmp[2][Y]+FMatrix[2][2]*tmp[2][Z]+FMatrix[2][3]*1;
w[2]=FMatrix[3][0]*tmp[2][X]+FMatrix[3][1]*tmp[2][Y]+FMatrix[3][2]*tmp[2][Z]+FMatrix[3][3]*1;
			

//////////////////final x,y,z
vertex[0][X]=vertex[0][X]/w[0];
vertex[0][Y]=vertex[0][Y]/w[0];
vertex[0][Z]=vertex[0][Z]/w[0];

vertex[1][X]=vertex[1][X]/w[1];
vertex[1][Y]=vertex[1][Y]/w[1];
vertex[1][Z]=vertex[1][Z]/w[1];

vertex[2][X]=vertex[2][X]/w[2];
vertex[2][Y]=vertex[2][Y]/w[2];
vertex[2][Z]=vertex[2][Z]/w[2];

/////////////  shift dx,dy for antialising
vertex[0][X]=vertex[0][X]-render->dx;
vertex[1][X]=vertex[1][X]-render->dx;
vertex[2][X]=vertex[2][X]-render->dx;

vertex[0][Y]=vertex[0][Y]-render->dy;
vertex[1][Y]=vertex[1][Y]-render->dy;
vertex[2][Y]=vertex[2][Y]-render->dy;
////////////////////////////////////////////////// bring Normal from mode space to image


		//float vertex[3][3];// set the normalized vertex
		float _w[3];
	
	

		
		GzMatrix NMatrix;
		float unitDivider[3];

		if(ComputeStack(render, render->matlevel, &NMatrix,1)); // 1 stands for normalstack computation 

////////////////////////////////////////////////Normalize the NMatrix

		NMatrix[0][3]=0;// remove Translation
		NMatrix[1][3]=0;
		NMatrix[2][3]=0;
					
unitDivider[0]=sqrt((pow(NMatrix[0][0],2)+pow(NMatrix[0][1],2)+pow(NMatrix[0][2],2)));
unitDivider[1]=sqrt((pow(NMatrix[1][0],2)+pow(NMatrix[1][1],2)+pow(NMatrix[1][2],2)));
unitDivider[2]=sqrt((pow(NMatrix[2][0],2)+pow(NMatrix[2][1],2)+pow(NMatrix[2][2],2)));


for(j=0;j<3;j++)
{
	for(i=0;i<3;i++)
	{
	NMatrix[j][i]=NMatrix[j][i]/unitDivider[j];
	}

}
////////////////////////////////////////////////		
render->_Normal[0][X]=NMatrix[0][0]*normal[0][X]+NMatrix[0][1]*normal[0][Y]+NMatrix[0][2]*normal[0][Z]+NMatrix[0][3]*1;
render->_Normal[0][Y]=NMatrix[1][0]*normal[0][X]+NMatrix[1][1]*normal[0][Y]+NMatrix[1][2]*normal[0][Z]+NMatrix[1][3]*1;
render->_Normal[0][Z]=NMatrix[2][0]*normal[0][X]+NMatrix[2][1]*normal[0][Y]+NMatrix[2][2]*normal[0][Z]+NMatrix[2][3]*1;
_w[0]=NMatrix[3][0]*normal[0][X]+NMatrix[3][1]*normal[0][Y]+NMatrix[3][2]*normal[0][Z]+NMatrix[3][3]*1;

render->_Normal[1][X]=NMatrix[0][0]*normal[1][X]+NMatrix[0][1]*normal[1][Y]+NMatrix[0][2]*normal[1][Z]+NMatrix[0][3]*1;
render->_Normal[1][Y]=NMatrix[1][0]*normal[1][X]+NMatrix[1][1]*normal[1][Y]+NMatrix[1][2]*normal[1][Z]+NMatrix[1][3]*1;
render->_Normal[1][Z]=NMatrix[2][0]*normal[1][X]+NMatrix[2][1]*normal[1][Y]+NMatrix[2][2]*normal[1][Z]+NMatrix[2][3]*1;
_w[1]=NMatrix[3][0]*normal[1][X]+NMatrix[3][1]*normal[1][Y]+NMatrix[3][2]*normal[1][Z]+NMatrix[3][3]*1;

render->_Normal[2][X]=NMatrix[0][0]*normal[2][X]+NMatrix[0][1]*normal[2][Y]+NMatrix[0][2]*normal[2][Z]+NMatrix[0][3]*1;
render->_Normal[2][Y]=NMatrix[1][0]*normal[2][X]+NMatrix[1][1]*normal[2][Y]+NMatrix[1][2]*normal[2][Z]+NMatrix[1][3]*1;
render->_Normal[2][Z]=NMatrix[2][0]*normal[2][X]+NMatrix[2][1]*normal[2][Y]+NMatrix[2][2]*normal[2][Z]+NMatrix[2][3]*1;
_w[2]=NMatrix[3][0]*normal[2][X]+NMatrix[3][1]*normal[2][Y]+NMatrix[3][2]*normal[2][Z]+NMatrix[3][3]*1;
			

//////////////////final norx,nory,norz
render->_Normal[0][X]=render->_Normal[0][X]/_w[0];
render->_Normal[0][Y]=render->_Normal[0][Y]/_w[0];
render->_Normal[0][Z]=render->_Normal[0][Z]/_w[0];

render->_Normal[1][X]=render->_Normal[1][X]/_w[1];
render->_Normal[1][Y]=render->_Normal[1][Y]/_w[1];
render->_Normal[1][Z]=render->_Normal[1][Z]/_w[1];

render->_Normal[2][X]=render->_Normal[2][X]/_w[2];
render->_Normal[2][Y]=render->_Normal[2][Y]/_w[2];
render->_Normal[2][Z]=render->_Normal[2][Z]/_w[2];
////////////////////////////////////////
//GzColor finalcolor;

//shade_equation(render,render->_Normal[0],&finalcolor);/* flat shade based on the norm of vert0 */
///////////////////////////////////////////	

///////////////////////////////////////////	
//  Transform uv->UV to fix perspective correction

float v_z[3];
float zmax=INT_MAX;
float UV[3][2];


v_z[0]=vertex[0][Z]/(zmax-vertex[0][Z]);
v_z[1]=vertex[1][Z]/(zmax-vertex[1][Z]);
v_z[2]=vertex[2][Z]/(zmax-vertex[2][Z]);


UV[0][U]=uv[0][U]/(v_z[0]+1);
UV[0][V]=uv[0][V]/(v_z[0]+1);

UV[1][U]=uv[1][U]/(v_z[1]+1);
UV[1][V]=uv[1][V]/(v_z[1]+1);

UV[2][U]=uv[2][U]/(v_z[2]+1);
UV[2][V]=uv[2][V]/(v_z[2]+1);




////////////////////////////////////////////
		maxVertex=FindMax(vertex);

		// define first, second, third
		if(maxVertex==0)
		{	
			if(vertex[1][1]==vertex[2][1])
			{
				if(vertex[1][X]>vertex[2][X]){secondVertex=1; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=1;}
			
			}
			else if(vertex[1][1]>vertex[2][1]){secondVertex=1; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=1;}
		}
		else if(maxVertex==1)
		{
			if(vertex[0][1]==vertex[2][1])
			{
				if(vertex[0][X]>vertex[2][X]){secondVertex=0; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[2][1]){secondVertex=0; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=0;}
		}
		else if(maxVertex==2)
		{
			if(vertex[0][1]==vertex[1][1])
			{
			if(vertex[0][X]>vertex[1][X]){secondVertex=0; thirdVertex=1;}
			else{secondVertex=1; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[1][1]){secondVertex=0; thirdVertex=1;}
			else {secondVertex=1; thirdVertex=0;}
		
		}

		
render->Ka[R]=render->Kd[R]=render->Ks[R]=1;
render->Ka[G]=render->Kd[G]=render->Ks[G]=1;
render->Ka[B]=render->Kd[B]=render->Ks[B]=1;
	

shade_equation(render, render->_Normal[maxVertex], &render->gouraudcolor[maxVertex]);
shade_equation(render, render->_Normal[secondVertex], &render->gouraudcolor[secondVertex]);
shade_equation(render, render->_Normal[thirdVertex], &render->gouraudcolor[thirdVertex]);
/////////////////////////////////////////////////////////////set middle color
GzColor onethreecolor;

if(vertex[maxVertex][Y]==vertex[secondVertex][Y])
{
	onethreecolor[R]=render->gouraudcolor[maxVertex][R];
	onethreecolor[G]=render->gouraudcolor[maxVertex][G];
	onethreecolor[B]=render->gouraudcolor[maxVertex][B];
}
else if(vertex[thirdVertex][Y]==vertex[secondVertex][Y])
{
	onethreecolor[R]=render->gouraudcolor[secondVertex][R];
	onethreecolor[G]=render->gouraudcolor[secondVertex][G];
	onethreecolor[B]=render->gouraudcolor[secondVertex][B];
}
else
{
	float f=(vertex[secondVertex][Y]-vertex[thirdVertex][Y])/(vertex[maxVertex][Y]-vertex[thirdVertex][Y]);
	
	onethreecolor[R]=render->gouraudcolor[thirdVertex][R]*(1-f)+render->gouraudcolor[maxVertex][R]*f;
	onethreecolor[G]=render->gouraudcolor[thirdVertex][G]*(1-f)+render->gouraudcolor[maxVertex][G]*f;
	onethreecolor[B]=render->gouraudcolor[thirdVertex][B]*(1-f)+render->gouraudcolor[maxVertex][B]*f;


}
///////////////////////////////////////////////////////////// set middle UV



float onethreeUV[2];

if(vertex[maxVertex][Y]==vertex[secondVertex][Y])
{
	onethreeUV[U]=UV[maxVertex][U];
	onethreeUV[V]=UV[maxVertex][V];

}
else if(vertex[thirdVertex][Y]==vertex[secondVertex][Y])
{
	onethreeUV[U]=UV[thirdVertex][U];
	onethreeUV[V]=UV[thirdVertex][V];
}
else
{
	float f=(vertex[secondVertex][Y]-vertex[thirdVertex][Y])/(vertex[maxVertex][Y]-vertex[thirdVertex][Y]);
	
	onethreeUV[U]=UV[thirdVertex][U]*(1-f)+UV[maxVertex][U]*f;
	onethreeUV[V]=UV[thirdVertex][V]*(1-f)+UV[maxVertex][V]*f;

}



//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
		ED12.start[0]=vertex[thirdVertex][0];
		ED12.start[1]=vertex[thirdVertex][1];
		ED12.start[2]=vertex[thirdVertex][2];
		ED12.end[0]=vertex[secondVertex][0];
		ED12.end[1]=vertex[secondVertex][1];
		ED12.end[2]=vertex[secondVertex][2];
		ED12.current[0]=vertex[thirdVertex][0];
		ED12.current[1]=vertex[thirdVertex][1];
		ED12.current[2]=vertex[thirdVertex][2];

		ED12.slopex=(vertex[secondVertex][0]-vertex[thirdVertex][0])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
		ED12.slopez=(vertex[secondVertex][2]-vertex[thirdVertex][2])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
////////////////////////////////////////////////////////////
		ED13.start[0]=vertex[thirdVertex][0];
		ED13.start[1]=vertex[thirdVertex][1];
		ED13.start[2]=vertex[thirdVertex][2];
		ED13.end[0]=vertex[maxVertex][0];
		ED13.end[1]=vertex[maxVertex][1];
		ED13.end[2]=vertex[maxVertex][2];
		ED13.current[0]=vertex[thirdVertex][0];
		ED13.current[1]=vertex[thirdVertex][1];
		ED13.current[2]=vertex[thirdVertex][2];

		ED13.slopex=(vertex[maxVertex][0]-vertex[thirdVertex][0])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
		ED13.slopez=(vertex[maxVertex][2]-vertex[thirdVertex][2])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
/////////////////////////////////////////////////////////////

		ED23.start[0]=vertex[secondVertex][0];
		ED23.start[1]=vertex[secondVertex][1];
		ED23.start[2]=vertex[secondVertex][2];
		ED23.end[0]=vertex[maxVertex][0];
		ED23.end[1]=vertex[maxVertex][1];
		ED23.end[2]=vertex[maxVertex][2];
		ED23.current[0]=vertex[secondVertex][0];
		ED23.current[1]=vertex[secondVertex][1];
		ED23.current[2]=vertex[secondVertex][2];

		ED23.slopex=(vertex[maxVertex][0]-vertex[secondVertex][0])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
		ED23.slopez=(vertex[maxVertex][2]-vertex[secondVertex][2])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
////////////////////////////////////////////////////////////////////




		if(ED12.slopex<ED13.slopex)////////Left
		{	
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;
			int bughappen=0;
			int bughappen2=0;
	
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];
				
		


			if((DeltaY==0)&&(ED12.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs


				while(((ED12.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{ 

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED12.current[0];
			spanline.start[2]=ED12.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED12.current[0];
			spanline.current[2]=ED12.current[2];
			spanline.slopez=(ED13.current[2]-ED12.current[2])/(ED13.current[0]-ED12.current[0]);


////////////////////////////////////////////////////////////////////
	float f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);


	ED12.color[R]=render->gouraudcolor[thirdVertex][R]*(1-f)+render->gouraudcolor[secondVertex][R]*f;
	ED12.color[G]=render->gouraudcolor[thirdVertex][G]*(1-f)+render->gouraudcolor[secondVertex][G]*f;
	ED12.color[B]=render->gouraudcolor[thirdVertex][B]*(1-f)+render->gouraudcolor[secondVertex][B]*f;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.color[R]=render->gouraudcolor[thirdVertex][R]*(1-f)+onethreecolor[R]*f;
	ED13.color[G]=render->gouraudcolor[thirdVertex][G]*(1-f)+onethreecolor[G]*f;
	ED13.color[B]=render->gouraudcolor[thirdVertex][B]*(1-f)+onethreecolor[B]*f;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED12.UV[U]=UV[thirdVertex][U]*(1-f)+UV[secondVertex][U]*f;
	ED12.UV[V]=UV[thirdVertex][V]*(1-f)+UV[secondVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.UV[U]=UV[thirdVertex][U]*(1-f)+onethreeUV[U]*f;
	ED13.UV[V]=UV[thirdVertex][V]*(1-f)+onethreeUV[V]*f;

//___________________________________________________________//



			/*span start */
		

		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
		//	deltaX=floor(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY

			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;

/////////////////////////////////////////////////////////////////////////////
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.color[R]=ED12.color[R]*(1-f)+ED13.color[R]*f;
spanline.color[G]=ED12.color[G]*(1-f)+ED13.color[G]*f;
spanline.color[B]=ED12.color[B]*(1-f)+ED13.color[B]*f;

/////////////////////////////////////////////////////////////////////////////
//___________________________________________________________// set span UV

spanline.UV[U]=ED12.UV[U]*(1-f)+ED13.UV[U]*f;
spanline.UV[V]=ED12.UV[V]*(1-f)+ED13.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED12.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
/////////////////////////////////////////////////////////////////////////// C*KT

spanline.color[R]=spanline.color[R]*texturecolor[R];
spanline.color[G]=spanline.color[G]*texturecolor[G];
spanline.color[B]=spanline.color[B]*texturecolor[B];

/////////////////////////////////////////////////////////////////////////////
			GzPutDisplay(render->display,spanline.current[0],ED12.current[1],spanline.color[R],spanline.color[G],spanline.color[B],0,spanline.current[2]);
			}
			spanXtimes++;
			deltaX=1;

		
			
														}//spanwhile
			spanYtimes++;
			DeltaY=1;
		}//uperwhile 
				
				
				if((ED12.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs
			
				DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
				if(DeltaY==0){DeltaY=1;}

			while((ED13.current[1]+DeltaY<=vertex[maxVertex][1])&&(bughappen==0)&&(bughappen2==0)){

				if(First1223)
				{
				  ED23.current[1]=ED12.current[1];
				  DeltaY=ceil(ED23.current[1])-ED23.current[1];
			    	if(DeltaY==0){DeltaY=1;}
				}
				else{	DeltaY=1;}
					
				First1223=false;
				
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
			//		else{DeltaY=1;}
				
			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED23.current[0];
			spanline.start[2]=ED23.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED23.current[0];
			spanline.current[2]=ED23.current[2];
			spanline.slopez=(ED13.current[2]-ED23.current[2])/(ED13.current[0]-ED23.current[0]);

////////////////////////////////////////////////////////////////////
	float f=(ED23.current[Y]-vertex[secondVertex][Y])/(vertex[maxVertex][Y]-vertex[secondVertex][Y]);

	ED23.color[R]=render->gouraudcolor[secondVertex][R]*(1-f)+render->gouraudcolor[maxVertex][R]*f;
	ED23.color[G]=render->gouraudcolor[secondVertex][G]*(1-f)+render->gouraudcolor[maxVertex][G]*f;
	ED23.color[B]=render->gouraudcolor[secondVertex][B]*(1-f)+render->gouraudcolor[maxVertex][B]*f;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.color[R]=onethreecolor[R]*(1-f)+render->gouraudcolor[maxVertex][R]*f;
	ED13.color[G]=onethreecolor[G]*(1-f)+render->gouraudcolor[maxVertex][G]*f;
	ED13.color[B]=onethreecolor[B]*(1-f)+render->gouraudcolor[maxVertex][B]*f;

////////////////////////////////////////////////////////////////////

//___________________________________________________________//count uv ED.uv

	ED23.UV[U]=UV[secondVertex][U]*(1-f)+UV[maxVertex][U]*f;
	ED23.UV[V]=UV[secondVertex][V]*(1-f)+UV[maxVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.UV[U]=onethreeUV[U]*(1-f)+UV[maxVertex][U]*(f);
	ED13.UV[V]=onethreeUV[V]*(1-f)+UV[maxVertex][V]*(f);

//___________________________________________________________//

			/*span start*/
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
	//		deltaX=floor(spanline.start[0])-spanline.start[0];
			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;

		
/////////////////////////////////////////////////////////////////////////////
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.color[R]=ED23.color[R]*(1-f)+ED13.color[R]*f;
spanline.color[G]=ED23.color[G]*(1-f)+ED13.color[G]*f;
spanline.color[B]=ED23.color[B]*(1-f)+ED13.color[B]*f;

/////////////////////////////////////////////////////////////////////////////
//___________________________________________________________// set span UV

spanline.UV[U]=ED23.UV[U]*(1-f)+ED13.UV[U]*f;
spanline.UV[V]=ED23.UV[V]*(1-f)+ED13.UV[V]*f;
//___________________________________________________________//

			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED23.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
/////////////////////////////////////////////////////////////////////////// C*KT

spanline.color[R]=spanline.color[R]*texturecolor[R];
spanline.color[G]=spanline.color[G]*texturecolor[G];
spanline.color[B]=spanline.color[B]*texturecolor[B];

/////////////////////////////////////////////////////////////////////////////
			//GzPutDisplay(render->display,spanline.current[0],ED23.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED23.current[1],spanline.color[R],spanline.color[G],spanline.color[B],0,spanline.current[2]);
	
			}///if
			spanXtimes++;
			deltaX=1;

		
			
														}
			
					}
		
		}
		else /// Right
		{
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;

			int bughappen=0;
			int bughappen2=0;
		
		
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];


			if((DeltaY==0)&&(ED13.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs
				
				while(((ED13.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{
				

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);

			
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED12.current[0];
			spanline.end[2]=ED12.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED12.current[2]-ED13.current[2])/(ED12.current[0]-ED13.current[0]);
////////////////////////////////////////////////////////////////////
	float f=(ED13.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);


	ED13.color[R]=render->gouraudcolor[thirdVertex][R]*(1-f)+onethreecolor[R]*f;
	ED13.color[G]=render->gouraudcolor[thirdVertex][G]*(1-f)+onethreecolor[G]*f;
	ED13.color[B]=render->gouraudcolor[thirdVertex][B]*(1-f)+onethreecolor[B]*f;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED12.color[R]=render->gouraudcolor[thirdVertex][R]*(1-f)+render->gouraudcolor[secondVertex][R]*f;
	ED12.color[G]=render->gouraudcolor[thirdVertex][G]*(1-f)+render->gouraudcolor[secondVertex][G]*f;
	ED12.color[B]=render->gouraudcolor[thirdVertex][B]*(1-f)+render->gouraudcolor[secondVertex][B]*f;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED13.UV[U]=UV[thirdVertex][U]*(1-f)+onethreeUV[U]*f;
	ED13.UV[V]=UV[thirdVertex][V]*(1-f)+onethreeUV[V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED12.UV[U]=UV[thirdVertex][U]*(1-f)+UV[secondVertex][U]*f;
	ED12.UV[V]=UV[thirdVertex][V]*(1-f)+UV[secondVertex][V]*f;
//___________________________________________________________//
/////////////////////////////////////////////////////////////////////

			/////span start 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];


			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;

/////////////////////////////////////////////////////////////////////////////
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.color[R]=ED13.color[R]*(1-f)+ED12.color[R]*f;
spanline.color[G]=ED13.color[G]*(1-f)+ED12.color[G]*f;
spanline.color[B]=ED13.color[B]*(1-f)+ED12.color[B]*f;

/////////////////////////////////////////////////////////////////////////////
//___________________________________________________________// set span UV

spanline.UV[U]=ED13.UV[U]*(1-f)+ED12.UV[U]*f;
spanline.UV[V]=ED13.UV[V]*(1-f)+ED12.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
/////////////////////////////////////////////////////////////////////////// C*KT

spanline.color[R]=spanline.color[R]*texturecolor[R];
spanline.color[G]=spanline.color[G]*texturecolor[G];
spanline.color[B]=spanline.color[B]*texturecolor[B];

/////////////////////////////////////////////////////////////////////////////
			//GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],spanline.color[R],spanline.color[G],spanline.color[B],0,spanline.current[2]);
			
			}
			spanXtimes++;
			deltaX=1;
			
														}//spanwhile
			spanYtimes++;
			DeltaY=1;
			}//upperwhile

					if((ED13.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs

						DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
						
						if(DeltaY==0){DeltaY=1;}

				while(ED13.current[1]+DeltaY<=vertex[maxVertex][1]&&(bughappen==0)&&(bughappen2==0))
					{

					
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
				//	else{DeltaY=1;}

				if(First1223)
				{
					ED23.current[1]=ED12.current[1];


					DeltaY=ceil(ED23.current[1])-ED23.current[1];
					if(DeltaY==0){DeltaY=1;}
				}
				else{	DeltaY=1;}
					
				First1223=false;
					
				

			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED23.current[0];
			spanline.end[2]=ED23.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED23.current[2]-ED13.current[2])/(ED23.current[0]-ED13.current[0]);

////////////////////////////////////////////////////////////////////
	float f=(ED13.current[Y]-vertex[secondVertex][Y])/(vertex[maxVertex][Y]-vertex[secondVertex][Y]);

	ED13.color[R]=onethreecolor[R]*(1-f)+render->gouraudcolor[maxVertex][R]*f;
	ED13.color[G]=onethreecolor[G]*(1-f)+render->gouraudcolor[maxVertex][G]*f;
	ED13.color[B]=onethreecolor[B]*(1-f)+render->gouraudcolor[maxVertex][B]*f;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED23.color[R]=render->gouraudcolor[secondVertex][R]*(1-f)+render->gouraudcolor[maxVertex][R]*f;
	ED23.color[G]=render->gouraudcolor[secondVertex][G]*(1-f)+render->gouraudcolor[maxVertex][G]*f;
	ED23.color[B]=render->gouraudcolor[secondVertex][B]*(1-f)+render->gouraudcolor[maxVertex][B]*f;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED13.UV[U]=onethreeUV[U]*(1-f)+UV[maxVertex][U]*f;
	ED13.UV[V]=onethreeUV[V]*(1-f)+UV[maxVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED23.UV[U]=UV[secondVertex][U]*(1-f)+UV[maxVertex][U]*(f);
	ED23.UV[V]=UV[secondVertex][V]*(1-f)+UV[maxVertex][V]*(f);

//___________________________________________________________//
			/*span start*/ 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;

/////////////////////////////////////////////////////////////////////////////
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.color[R]=ED13.color[R]*(1-f)+ED23.color[R]*f;
spanline.color[G]=ED13.color[G]*(1-f)+ED23.color[G]*f;
spanline.color[B]=ED13.color[B]*(1-f)+ED23.color[B]*f;

/////////////////////////////////////////////////////////////////////////////
//___________________________________________________________// set span UV

spanline.UV[U]=ED13.UV[U]*(1-f)+ED23.UV[U]*f;
spanline.UV[V]=ED13.UV[V]*(1-f)+ED23.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
/////////////////////////////////////////////////////////////////////////// C*KT

spanline.color[R]=spanline.color[R]*texturecolor[R];
spanline.color[G]=spanline.color[G]*texturecolor[G];
spanline.color[B]=spanline.color[B]*texturecolor[B];

/////////////////////////////////////////////////////////////////////////////
			//GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],spanline.color[R],spanline.color[G],spanline.color[B],0,spanline.current[2]);
			}
			spanXtimes++;
			deltaX=1;
			
														}//second while
						
	
		
					}//firstwhile



		}//else   



		
		
}//  else if Gz_Color
else if(render->interp_mode==GZ_NORMAL)
{
	
	int NumberOfParts=numParts;
	int i,j;
//	int tokencounter;
//	while(NumberOfParts>0){

//		for(tokencounter=0;tokencounter<NumberOfParts;tokencounter++){

		//GzToken Decider=nameList[NumberOfParts-1];



		
		GzToken Decider=nameList[0];
		GzToken Decider2=nameList[1];
		
		GzCoord *tmp=static_cast<GzCoord*>(valueList[0]);
		 GzCoord *normal=static_cast<GzCoord*>(valueList[1]);
		 GzTextureIndex *uv=static_cast<GzTextureIndex*>(valueList[2]);

	

		if(Decider==GZ_POSITION){}

		if(Decider2==GZ_NORMAL){}

   //float current[2],end[2],start[2],slope;
   float vertex[3][3];
   float w[3];
   int maxVertex,secondVertex,thirdVertex;
   DDA ED12,ED23,ED13;



		GzMatrix FMatrix;

		if(ComputeStack(render, render->matlevel, &FMatrix,0));// 0 stands for Vertexstack computation 

//////////////////////// ScreenMatrix
//float ScreenMatrix[4];


vertex[0][X]=FMatrix[0][0]*tmp[0][X]+FMatrix[0][1]*tmp[0][Y]+FMatrix[0][2]*tmp[0][Z]+FMatrix[0][3]*1;
vertex[0][Y]=FMatrix[1][0]*tmp[0][X]+FMatrix[1][1]*tmp[0][Y]+FMatrix[1][2]*tmp[0][Z]+FMatrix[1][3]*1;
vertex[0][Z]=FMatrix[2][0]*tmp[0][X]+FMatrix[2][1]*tmp[0][Y]+FMatrix[2][2]*tmp[0][Z]+FMatrix[2][3]*1;
w[0]=FMatrix[3][0]*tmp[0][X]+FMatrix[3][1]*tmp[0][Y]+FMatrix[3][2]*tmp[0][Z]+FMatrix[3][3]*1;

vertex[1][X]=FMatrix[0][0]*tmp[1][X]+FMatrix[0][1]*tmp[1][Y]+FMatrix[0][2]*tmp[1][Z]+FMatrix[0][3]*1;
vertex[1][Y]=FMatrix[1][0]*tmp[1][X]+FMatrix[1][1]*tmp[1][Y]+FMatrix[1][2]*tmp[1][Z]+FMatrix[1][3]*1;
vertex[1][Z]=FMatrix[2][0]*tmp[1][X]+FMatrix[2][1]*tmp[1][Y]+FMatrix[2][2]*tmp[1][Z]+FMatrix[2][3]*1;
w[1]=FMatrix[3][0]*tmp[1][X]+FMatrix[3][1]*tmp[1][Y]+FMatrix[3][2]*tmp[1][Z]+FMatrix[3][3]*1;

vertex[2][X]=FMatrix[0][0]*tmp[2][X]+FMatrix[0][1]*tmp[2][Y]+FMatrix[0][2]*tmp[2][Z]+FMatrix[0][3]*1;
vertex[2][Y]=FMatrix[1][0]*tmp[2][X]+FMatrix[1][1]*tmp[2][Y]+FMatrix[1][2]*tmp[2][Z]+FMatrix[1][3]*1;
vertex[2][Z]=FMatrix[2][0]*tmp[2][X]+FMatrix[2][1]*tmp[2][Y]+FMatrix[2][2]*tmp[2][Z]+FMatrix[2][3]*1;
w[2]=FMatrix[3][0]*tmp[2][X]+FMatrix[3][1]*tmp[2][Y]+FMatrix[3][2]*tmp[2][Z]+FMatrix[3][3]*1;
			

//////////////////final x,y,z
vertex[0][X]=vertex[0][X]/w[0];
vertex[0][Y]=vertex[0][Y]/w[0];
vertex[0][Z]=vertex[0][Z]/w[0];

vertex[1][X]=vertex[1][X]/w[1];
vertex[1][Y]=vertex[1][Y]/w[1];
vertex[1][Z]=vertex[1][Z]/w[1];

vertex[2][X]=vertex[2][X]/w[2];
vertex[2][Y]=vertex[2][Y]/w[2];
vertex[2][Z]=vertex[2][Z]/w[2];

/////////////  shift dx,dy for antialising
vertex[0][X]=vertex[0][X]-render->dx;
vertex[1][X]=vertex[1][X]-render->dx;
vertex[2][X]=vertex[2][X]-render->dx;

vertex[0][Y]=vertex[0][Y]-render->dy;
vertex[1][Y]=vertex[1][Y]-render->dy;
vertex[2][Y]=vertex[2][Y]-render->dy;


////////////////////////////////////////////////// bring Normal from mode space to image


		//float vertex[3][3];// set the normalized vertex
		float _w[3];
	
	

		
		GzMatrix NMatrix;
		float unitDivider[3];

		if(ComputeStack(render, render->matlevel, &NMatrix,1)); // 1 stands for normalstack computation 

////////////////////////////////////////////////Normalize the NMatrix

		NMatrix[0][3]=0;// remove Translation
		NMatrix[1][3]=0;
		NMatrix[2][3]=0;
					
unitDivider[0]=sqrt((pow(NMatrix[0][0],2)+pow(NMatrix[0][1],2)+pow(NMatrix[0][2],2)));
unitDivider[1]=sqrt((pow(NMatrix[1][0],2)+pow(NMatrix[1][1],2)+pow(NMatrix[1][2],2)));
unitDivider[2]=sqrt((pow(NMatrix[2][0],2)+pow(NMatrix[2][1],2)+pow(NMatrix[2][2],2)));


for(j=0;j<3;j++)
{
	for(i=0;i<3;i++)
	{
	NMatrix[j][i]=NMatrix[j][i]/unitDivider[j];
	}

}
////////////////////////////////////////////////		
render->_Normal[0][X]=NMatrix[0][0]*normal[0][X]+NMatrix[0][1]*normal[0][Y]+NMatrix[0][2]*normal[0][Z]+NMatrix[0][3]*1;
render->_Normal[0][Y]=NMatrix[1][0]*normal[0][X]+NMatrix[1][1]*normal[0][Y]+NMatrix[1][2]*normal[0][Z]+NMatrix[1][3]*1;
render->_Normal[0][Z]=NMatrix[2][0]*normal[0][X]+NMatrix[2][1]*normal[0][Y]+NMatrix[2][2]*normal[0][Z]+NMatrix[2][3]*1;
_w[0]=NMatrix[3][0]*normal[0][X]+NMatrix[3][1]*normal[0][Y]+NMatrix[3][2]*normal[0][Z]+NMatrix[3][3]*1;

render->_Normal[1][X]=NMatrix[0][0]*normal[1][X]+NMatrix[0][1]*normal[1][Y]+NMatrix[0][2]*normal[1][Z]+NMatrix[0][3]*1;
render->_Normal[1][Y]=NMatrix[1][0]*normal[1][X]+NMatrix[1][1]*normal[1][Y]+NMatrix[1][2]*normal[1][Z]+NMatrix[1][3]*1;
render->_Normal[1][Z]=NMatrix[2][0]*normal[1][X]+NMatrix[2][1]*normal[1][Y]+NMatrix[2][2]*normal[1][Z]+NMatrix[2][3]*1;
_w[1]=NMatrix[3][0]*normal[1][X]+NMatrix[3][1]*normal[1][Y]+NMatrix[3][2]*normal[1][Z]+NMatrix[3][3]*1;

render->_Normal[2][X]=NMatrix[0][0]*normal[2][X]+NMatrix[0][1]*normal[2][Y]+NMatrix[0][2]*normal[2][Z]+NMatrix[0][3]*1;
render->_Normal[2][Y]=NMatrix[1][0]*normal[2][X]+NMatrix[1][1]*normal[2][Y]+NMatrix[1][2]*normal[2][Z]+NMatrix[1][3]*1;
render->_Normal[2][Z]=NMatrix[2][0]*normal[2][X]+NMatrix[2][1]*normal[2][Y]+NMatrix[2][2]*normal[2][Z]+NMatrix[2][3]*1;
_w[2]=NMatrix[3][0]*normal[2][X]+NMatrix[3][1]*normal[2][Y]+NMatrix[3][2]*normal[2][Z]+NMatrix[3][3]*1;
			

//////////////////final norx,nory,norz
render->_Normal[0][X]=render->_Normal[0][X]/_w[0];
render->_Normal[0][Y]=render->_Normal[0][Y]/_w[0];
render->_Normal[0][Z]=render->_Normal[0][Z]/_w[0];

render->_Normal[1][X]=render->_Normal[1][X]/_w[1];
render->_Normal[1][Y]=render->_Normal[1][Y]/_w[1];
render->_Normal[1][Z]=render->_Normal[1][Z]/_w[1];

render->_Normal[2][X]=render->_Normal[2][X]/_w[2];
render->_Normal[2][Y]=render->_Normal[2][Y]/_w[2];
render->_Normal[2][Z]=render->_Normal[2][Z]/_w[2];
////////////////////////////////////////
//GzColor finalcolor;

//shade_equation(render,render->_Normal[0],&finalcolor);/* flat shade based on the norm of vert0 */
///////////////////////////////////////////	
//  Transform uv->UV to fix perspective correction

float v_z[3];
float zmax=INT_MAX;
float UV[3][2];


v_z[0]=vertex[0][Z]/(zmax-vertex[0][Z]);
v_z[1]=vertex[1][Z]/(zmax-vertex[1][Z]);
v_z[2]=vertex[2][Z]/(zmax-vertex[2][Z]);


UV[0][U]=uv[0][U]/(v_z[0]+1);
UV[0][V]=uv[0][V]/(v_z[0]+1);

UV[1][U]=uv[1][U]/(v_z[1]+1);
UV[1][V]=uv[1][V]/(v_z[1]+1);

UV[2][U]=uv[2][U]/(v_z[2]+1);
UV[2][V]=uv[2][V]/(v_z[2]+1);




////////////////////////////////////////////
		maxVertex=FindMax(vertex);

		// define first, second, third
		if(maxVertex==0)
		{	
			if(vertex[1][1]==vertex[2][1])
			{
				if(vertex[1][X]>vertex[2][X]){secondVertex=1; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=1;}
			
			}
			else if(vertex[1][1]>vertex[2][1]){secondVertex=1; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=1;}
		}
		else if(maxVertex==1)
		{
			if(vertex[0][1]==vertex[2][1])
			{
				if(vertex[0][X]>vertex[2][X]){secondVertex=0; thirdVertex=2;}
				else{secondVertex=2; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[2][1]){secondVertex=0; thirdVertex=2;}
			else {secondVertex=2; thirdVertex=0;}
		}
		else if(maxVertex==2)
		{
			if(vertex[0][1]==vertex[1][1])
			{
			if(vertex[0][X]>vertex[1][X]){secondVertex=0; thirdVertex=1;}
			else{secondVertex=1; thirdVertex=0;}
			}
			else if(vertex[0][1]>vertex[1][1]){secondVertex=0; thirdVertex=1;}
			else {secondVertex=1; thirdVertex=0;}
		
		}
/////////////////////////////////////////////////////////////set middle color
float onethreenormal[3];

if(vertex[maxVertex][Y]==vertex[secondVertex][Y])
{
	onethreenormal[X]=render->_Normal[maxVertex][X];
	onethreenormal[Y]=render->_Normal[maxVertex][Y];
	onethreenormal[Z]=render->_Normal[maxVertex][Z];
}
else if(vertex[thirdVertex][Y]==vertex[secondVertex][Y])
{
	onethreenormal[X]=render->_Normal[secondVertex][X];
	onethreenormal[Y]=render->_Normal[secondVertex][Y];
	onethreenormal[Z]=render->_Normal[secondVertex][Z];
}
else
{
	float f=(vertex[secondVertex][Y]-vertex[thirdVertex][Y])/(vertex[maxVertex][Y]-vertex[thirdVertex][Y]);
	
	onethreenormal[X]=render->_Normal[thirdVertex][X]*(1-f)+render->_Normal[maxVertex][X]*f;
	onethreenormal[Y]=render->_Normal[thirdVertex][Y]*(1-f)+render->_Normal[maxVertex][Y]*f;
	onethreenormal[Z]=render->_Normal[thirdVertex][Z]*(1-f)+render->_Normal[maxVertex][Z]*f;


}

///////////////////////////////////////////////////////////// set middle UV



float onethreeUV[2];

if(vertex[maxVertex][Y]==vertex[secondVertex][Y])
{
	onethreeUV[U]=UV[maxVertex][U];
	onethreeUV[V]=UV[maxVertex][V];

}
else if(vertex[thirdVertex][Y]==vertex[secondVertex][Y])
{
	onethreeUV[U]=UV[thirdVertex][U];
	onethreeUV[V]=UV[thirdVertex][V];
}
else
{
	float f=(vertex[secondVertex][Y]-vertex[thirdVertex][Y])/(vertex[maxVertex][Y]-vertex[thirdVertex][Y]);
	
	onethreeUV[U]=UV[thirdVertex][U]*(1-f)+UV[maxVertex][U]*f;
	onethreeUV[V]=UV[thirdVertex][V]*(1-f)+UV[maxVertex][V]*f;

}



//////////////////////////////////////////////////////////////
		
		ED12.start[0]=vertex[thirdVertex][0];
		ED12.start[1]=vertex[thirdVertex][1];
		ED12.start[2]=vertex[thirdVertex][2];
		ED12.end[0]=vertex[secondVertex][0];
		ED12.end[1]=vertex[secondVertex][1];
		ED12.end[2]=vertex[secondVertex][2];
		ED12.current[0]=vertex[thirdVertex][0];
		ED12.current[1]=vertex[thirdVertex][1];
		ED12.current[2]=vertex[thirdVertex][2];

		ED12.slopex=(vertex[secondVertex][0]-vertex[thirdVertex][0])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
		ED12.slopez=(vertex[secondVertex][2]-vertex[thirdVertex][2])/(vertex[secondVertex][1]-vertex[thirdVertex][1]);
////////////////////////////////////////////////////////////
		ED13.start[0]=vertex[thirdVertex][0];
		ED13.start[1]=vertex[thirdVertex][1];
		ED13.start[2]=vertex[thirdVertex][2];
		ED13.end[0]=vertex[maxVertex][0];
		ED13.end[1]=vertex[maxVertex][1];
		ED13.end[2]=vertex[maxVertex][2];
		ED13.current[0]=vertex[thirdVertex][0];
		ED13.current[1]=vertex[thirdVertex][1];
		ED13.current[2]=vertex[thirdVertex][2];

		ED13.slopex=(vertex[maxVertex][0]-vertex[thirdVertex][0])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
		ED13.slopez=(vertex[maxVertex][2]-vertex[thirdVertex][2])/(vertex[maxVertex][1]-vertex[thirdVertex][1]);
/////////////////////////////////////////////////////////////

		ED23.start[0]=vertex[secondVertex][0];
		ED23.start[1]=vertex[secondVertex][1];
		ED23.start[2]=vertex[secondVertex][2];
		ED23.end[0]=vertex[maxVertex][0];
		ED23.end[1]=vertex[maxVertex][1];
		ED23.end[2]=vertex[maxVertex][2];
		ED23.current[0]=vertex[secondVertex][0];
		ED23.current[1]=vertex[secondVertex][1];
		ED23.current[2]=vertex[secondVertex][2];

		ED23.slopex=(vertex[maxVertex][0]-vertex[secondVertex][0])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
		ED23.slopez=(vertex[maxVertex][2]-vertex[secondVertex][2])/(vertex[maxVertex][1]-vertex[secondVertex][1]);
////////////////////////////////////////////////////////////////////




		if(ED12.slopex<ED13.slopex)////////Left
		{	
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;
			int bughappen=0;
			int bughappen2=0;
	
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];
				
		


			if((DeltaY==0)&&(ED12.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs


				while(((ED12.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{ 

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED12.current[0];
			spanline.start[2]=ED12.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED12.current[0];
			spanline.current[2]=ED12.current[2];
			spanline.slopez=(ED13.current[2]-ED12.current[2])/(ED13.current[0]-ED12.current[0]);
////////////////////////////////////////////////////////////////////
	float f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);


	ED12.normal[X]=render->_Normal[thirdVertex][X]*(1-f)+render->_Normal[secondVertex][X]*f;
	ED12.normal[Y]=render->_Normal[thirdVertex][Y]*(1-f)+render->_Normal[secondVertex][Y]*f;
	ED12.normal[Z]=render->_Normal[thirdVertex][Z]*(1-f)+render->_Normal[secondVertex][Z]*f;

		float unnifiter;
unnifiter=unify(ED12.normal[X],ED12.normal[Y],ED12.normal[Z]);

ED12.normal[X]=ED12.normal[X]/unnifiter;
ED12.normal[Y]=ED12.normal[Y]/unnifiter;
ED12.normal[Z]=ED12.normal[Z]/unnifiter;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.normal[X]=render->_Normal[thirdVertex][X]*(1-f)+onethreenormal[X]*f;
	ED13.normal[Y]=render->_Normal[thirdVertex][Y]*(1-f)+onethreenormal[Y]*f;
	ED13.normal[Z]=render->_Normal[thirdVertex][Z]*(1-f)+onethreenormal[Z]*f;

	
unnifiter=unify(ED13.normal[X],ED13.normal[Y],ED13.normal[Z]);

ED13.normal[X]=ED13.normal[X]/unnifiter;
ED13.normal[Y]=ED13.normal[Y]/unnifiter;
ED13.normal[Z]=ED13.normal[Z]/unnifiter;

////////////////////////////////////////////////////////////////////

//___________________________________________________________//count uv ED.uv

	ED12.UV[U]=UV[thirdVertex][U]*(1-f)+UV[secondVertex][U]*f;
	ED12.UV[V]=UV[thirdVertex][V]*(1-f)+UV[secondVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.UV[U]=UV[thirdVertex][U]*(1-f)+onethreeUV[U]*f;
	ED13.UV[V]=UV[thirdVertex][V]*(1-f)+onethreeUV[V]*f;

//___________________________________________________________//


			/*span start*/ 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
		//	deltaX=floor(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;

///////////////////////////////////////////////////////////////////////////// set normal
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.normal[X]=ED12.normal[X]*(1-f)+ED13.normal[X]*f;
spanline.normal[Y]=ED12.normal[Y]*(1-f)+ED13.normal[Y]*f;
spanline.normal[Z]=ED12.normal[Z]*(1-f)+ED13.normal[Z]*f;

float unnifiter;
unnifiter=unify(spanline.normal[X],spanline.normal[Y],spanline.normal[Z]);

spanline.normal[X]=spanline.normal[X]/unnifiter;
spanline.normal[Y]=spanline.normal[Y]/unnifiter;
spanline.normal[Z]=spanline.normal[Z]/unnifiter;

//___________________________________________________________// set span UV

spanline.UV[U]=ED12.UV[U]*(1-f)+ED13.UV[U]*f;
spanline.UV[V]=ED12.UV[V]*(1-f)+ED13.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED12.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				GzColor phongcolor;

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
				
/////////////////////////////////////////////////////////////////////////// N->C


render->Ka[R]=texturecolor[R];
render->Ka[G]=texturecolor[G];
render->Ka[B]=texturecolor[B];

render->Kd[R]=texturecolor[R];
render->Kd[G]=texturecolor[G];
render->Kd[B]=texturecolor[B];


shade_equation(render, spanline.normal, &phongcolor);

/////////////////////////////////////////////////////////////////////////////
				
			

//			GzPutDisplay(render->display,spanline.current[0],ED12.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
//			GzPutDisplay(render->display,spanline.current[0],ED12.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED12.current[1],phongcolor[R],phongcolor[G],phongcolor[B],0,spanline.current[2]);
	
			}
			spanXtimes++;
			deltaX=1;

		
			
														}
			spanYtimes++;
			DeltaY=1;
		}//upperwhile 
				
				
				if((ED12.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs
			
				DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
				if(DeltaY==0){DeltaY=1;}

			while((ED13.current[1]+DeltaY<=vertex[maxVertex][1])&&(bughappen==0)&&(bughappen2==0)){

				if(First1223)
				{
				  ED23.current[1]=ED12.current[1];
				  DeltaY=ceil(ED23.current[1])-ED23.current[1];
			    	if(DeltaY==0){DeltaY=1;}
				}
				else{	DeltaY=1;}
					
				First1223=false;
				
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
			//		else{DeltaY=1;}
				
			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED23.current[0];
			spanline.start[2]=ED23.current[2];
			spanline.end[0]=ED13.current[0];
			spanline.end[2]=ED13.current[2];
			spanline.current[0]=ED23.current[0];
			spanline.current[2]=ED23.current[2];
			spanline.slopez=(ED13.current[2]-ED23.current[2])/(ED13.current[0]-ED23.current[0]);

////////////////////////////////////////////////////////////////////
	float f=(ED23.current[Y]-vertex[secondVertex][Y])/(vertex[maxVertex][Y]-vertex[secondVertex][Y]);

	ED23.normal[X]=render->_Normal[secondVertex][X]*(1-f)+render->_Normal[maxVertex][X]*f;
	ED23.normal[Y]=render->_Normal[secondVertex][Y]*(1-f)+render->_Normal[maxVertex][Y]*f;
	ED23.normal[Z]=render->_Normal[secondVertex][Z]*(1-f)+render->_Normal[maxVertex][Z]*f;

		float unnifiter;
unnifiter=unify(ED23.normal[X],ED23.normal[Y],ED23.normal[Z]);

ED23.normal[X]=ED23.normal[X]/unnifiter;
ED23.normal[Y]=ED23.normal[Y]/unnifiter;
ED23.normal[Z]=ED23.normal[Z]/unnifiter;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.normal[X]=onethreenormal[X]*(1-f)+render->_Normal[maxVertex][X]*f;
	ED13.normal[Y]=onethreenormal[Y]*(1-f)+render->_Normal[maxVertex][Y]*f;
	ED13.normal[Z]=onethreenormal[Z]*(1-f)+render->_Normal[maxVertex][Z]*f;

	
unnifiter=unify(ED13.normal[X],ED13.normal[Y],ED13.normal[Z]);

ED13.normal[X]=ED13.normal[X]/unnifiter;
ED13.normal[Y]=ED13.normal[Y]/unnifiter;
ED13.normal[Z]=ED13.normal[Z]/unnifiter;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED23.UV[U]=UV[secondVertex][U]*(1-f)+UV[maxVertex][U]*f;
	ED23.UV[V]=UV[secondVertex][V]*(1-f)+UV[maxVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED13.UV[U]=onethreeUV[U]*(1-f)+UV[maxVertex][U]*(f);
	ED13.UV[V]=onethreeUV[V]*(1-f)+UV[maxVertex][V]*(f);

//___________________________________________________________//

			/*span start*/ 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
	//		deltaX=floor(spanline.start[0])-spanline.start[0];
			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
///////////////////////////////////////////////////////////////////////////// set normal
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.normal[X]=ED23.normal[X]*(1-f)+ED13.normal[X]*f;
spanline.normal[Y]=ED23.normal[Y]*(1-f)+ED13.normal[Y]*f;
spanline.normal[Z]=ED23.normal[Z]*(1-f)+ED13.normal[Z]*f;

float unnifiter;
unnifiter=unify(spanline.normal[X],spanline.normal[Y],spanline.normal[Z]);

spanline.normal[X]=spanline.normal[X]/unnifiter;
spanline.normal[Y]=spanline.normal[Y]/unnifiter;
spanline.normal[Z]=spanline.normal[Z]/unnifiter;
//___________________________________________________________// set span UV

spanline.UV[U]=ED23.UV[U]*(1-f)+ED13.UV[U]*f;
spanline.UV[V]=ED23.UV[V]*(1-f)+ED13.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED23.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			
				GzColor phongcolor;

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
				
/////////////////////////////////////////////////////////////////////////// N->C


render->Ka[R]=texturecolor[R];
render->Ka[G]=texturecolor[G];
render->Ka[B]=texturecolor[B];

render->Kd[R]=texturecolor[R];
render->Kd[G]=texturecolor[G];
render->Kd[B]=texturecolor[B];


shade_equation(render, spanline.normal, &phongcolor);

/////////////////////////////////////////////////////////////////////////////


		//	GzPutDisplay(render->display,spanline.current[0],ED23.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
		//	GzPutDisplay(render->display,spanline.current[0],ED23.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED23.current[1],phongcolor[R],phongcolor[G],phongcolor[B],0,spanline.current[2]);
		

			}///if
			spanXtimes++;
			deltaX=1;

		
			
														}
			
					}
		
		}
		else /// Right
		{
			float DeltaY; SPAN spanline;
			int spanYtimes=1;
			int spanXtimes=1;
			float deltaX;
			GzIntensity tmpr,tmpg,tmpb,tmpa;
			GzDepth tmpz;
			bool First1223=true;

			int bughappen=0;
			int bughappen2=0;
		
		
			DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];


			if((DeltaY==0)&&(ED13.current[1]==vertex[secondVertex][1])){bughappen=1;}  // fix bugs
				
				while(((ED13.current[1]+DeltaY)<=vertex[secondVertex][1])&&(bughappen==0))
			{
				

					if(spanYtimes==1){DeltaY=ceil(vertex[thirdVertex][1])-vertex[thirdVertex][1];}
					else{DeltaY=1;}
			ED12.current[0]=(ED12.current[0]+ED12.slopex*DeltaY);
			ED12.current[1]=(ED12.current[1]+DeltaY);
			ED12.current[2]=(ED12.current[2]+ED12.slopez*DeltaY);

			
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED12.current[0];
			spanline.end[2]=ED12.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED12.current[2]-ED13.current[2])/(ED12.current[0]-ED13.current[0]);
////////////////////////////////////////////////////////////////////
	float f=(ED13.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);


	ED13.normal[X]=render->_Normal[thirdVertex][X]*(1-f)+onethreenormal[X]*f;
	ED13.normal[Y]=render->_Normal[thirdVertex][Y]*(1-f)+onethreenormal[Y]*f;
	ED13.normal[Z]=render->_Normal[thirdVertex][Z]*(1-f)+onethreenormal[Z]*f;

		float unnifiter;
unnifiter=unify(ED13.normal[X],ED13.normal[Y],ED13.normal[Z]);

ED13.normal[X]=ED13.normal[X]/unnifiter;
ED13.normal[Y]=ED13.normal[Y]/unnifiter;
ED13.normal[Z]=ED13.normal[Z]/unnifiter;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED12.normal[X]=render->_Normal[thirdVertex][X]*(1-f)+render->_Normal[secondVertex][X]*f;
	ED12.normal[Y]=render->_Normal[thirdVertex][Y]*(1-f)+render->_Normal[secondVertex][Y]*f;
	ED12.normal[Z]=render->_Normal[thirdVertex][Z]*(1-f)+render->_Normal[secondVertex][Z]*f;

	
unnifiter=unify(ED12.normal[X],ED12.normal[Y],ED12.normal[Z]);

ED12.normal[X]=ED12.normal[X]/unnifiter;
ED12.normal[Y]=ED12.normal[Y]/unnifiter;
ED12.normal[Z]=ED12.normal[Z]/unnifiter;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED13.UV[U]=UV[thirdVertex][U]*(1-f)+onethreeUV[U]*f;
	ED13.UV[V]=UV[thirdVertex][V]*(1-f)+onethreeUV[V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED12.UV[U]=UV[thirdVertex][U]*(1-f)+UV[secondVertex][U]*f;
	ED12.UV[V]=UV[thirdVertex][V]*(1-f)+UV[secondVertex][V]*f;
//___________________________________________________________//
/////////////////////////////////////////////////////////////////////

			/* span start */
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];


			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0]){/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
///////////////////////////////////////////////////////////////////////////// set normal
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.normal[X]=ED13.normal[X]*(1-f)+ED12.normal[X]*f;
spanline.normal[Y]=ED13.normal[Y]*(1-f)+ED12.normal[Y]*f;
spanline.normal[Z]=ED13.normal[Z]*(1-f)+ED12.normal[Z]*f;

float unnifiter;
unnifiter=unify(spanline.normal[X],spanline.normal[Y],spanline.normal[Z]);

spanline.normal[X]=spanline.normal[X]/unnifiter;
spanline.normal[Y]=spanline.normal[Y]/unnifiter;
spanline.normal[Z]=spanline.normal[Z]/unnifiter;

//___________________________________________________________// set span UV

spanline.UV[U]=ED13.UV[U]*(1-f)+ED12.UV[U]*f;
spanline.UV[V]=ED13.UV[V]*(1-f)+ED12.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv
				

				GzColor phongcolor;

			render->tex_fun(finaluv[U],finaluv[V], texturecolor);
				
			/////////////////////////////////////////////////////////////////////////// N->C


render->Ka[R]=texturecolor[R];
render->Ka[G]=texturecolor[G];
render->Ka[B]=texturecolor[B];

render->Kd[R]=texturecolor[R];
render->Kd[G]=texturecolor[G];
render->Kd[B]=texturecolor[B];


shade_equation(render, spanline.normal, &phongcolor);

/////////////////////////////////////////////////////////////////////////////
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],phongcolor[R],phongcolor[G],phongcolor[B],0,spanline.current[2]);
		
			}
			spanXtimes++;
			deltaX=1;
			
														}//secondwhile
			spanYtimes++;
			DeltaY=1;
			}//upperwhile

					if((ED13.current[1]==vertex[maxVertex][1])){bughappen2=1;}  // fix bugs

						DeltaY=ceil(ED13.current[1])-ED13.current[1];  // fix the second entry
						
						if(DeltaY==0){DeltaY=1;}

				while(ED13.current[1]+DeltaY<=vertex[maxVertex][1]&&(bughappen==0)&&(bughappen2==0))
					{

					
				//	if(spanYsecond==1){DeltaY=ceil(vertex[secondVertex][1])-vertex[secondVertex][1];}
				//	else{DeltaY=1;}

				if(First1223)
				{
					ED23.current[1]=ED12.current[1];


					DeltaY=ceil(ED23.current[1])-ED23.current[1];
					if(DeltaY==0){DeltaY=1;}
				}
				else{	DeltaY=1;}
					
				First1223=false;
					
				

			ED23.current[0]=(ED23.current[0]+ED23.slopex*DeltaY);
			ED23.current[1]=(ED23.current[1]+DeltaY);
			ED23.current[2]=(ED23.current[2]+ED23.slopez*DeltaY);
			////////////////////////////////////////////////////////
			ED13.current[0]=(ED13.current[0]+ED13.slopex*DeltaY);
			ED13.current[1]=(ED13.current[1]+DeltaY);
			ED13.current[2]=(ED13.current[2]+ED13.slopez*DeltaY);
			////set up spanline default
		    spanline.start[0]=ED13.current[0];
			spanline.start[2]=ED13.current[2];
			spanline.end[0]=ED23.current[0];
			spanline.end[2]=ED23.current[2];
			spanline.current[0]=ED13.current[0];
			spanline.current[2]=ED13.current[2];
			spanline.slopez=(ED23.current[2]-ED13.current[2])/(ED23.current[0]-ED13.current[0]);
////////////////////////////////////////////////////////////////////
	float f=(ED13.current[Y]-vertex[secondVertex][Y])/(vertex[maxVertex][Y]-vertex[secondVertex][Y]);

	ED13.normal[X]=onethreenormal[X]*(1-f)+render->_Normal[maxVertex][X]*f;
	ED13.normal[Y]=onethreenormal[Y]*(1-f)+render->_Normal[maxVertex][Y]*f;
	ED13.normal[Z]=onethreenormal[Z]*(1-f)+render->_Normal[maxVertex][Z]*f;

		float unnifiter;
unnifiter=unify(ED13.normal[X],ED13.normal[Y],ED13.normal[Z]);

ED13.normal[X]=ED13.normal[X]/unnifiter;
ED13.normal[Y]=ED13.normal[Y]/unnifiter;
ED13.normal[Z]=ED13.normal[Z]/unnifiter;
////////////////////////////////////////////////////////////////////
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED23.normal[X]=render->_Normal[secondVertex][X]*(1-f)+render->_Normal[maxVertex][X]*f;
	ED23.normal[Y]=render->_Normal[secondVertex][Y]*(1-f)+render->_Normal[maxVertex][Y]*f;
	ED23.normal[Z]=render->_Normal[secondVertex][Z]*(1-f)+render->_Normal[maxVertex][Z]*f;

	
unnifiter=unify(ED23.normal[X],ED23.normal[Y],ED23.normal[Z]);

ED23.normal[X]=ED23.normal[X]/unnifiter;
ED23.normal[Y]=ED23.normal[Y]/unnifiter;
ED23.normal[Z]=ED23.normal[Z]/unnifiter;

////////////////////////////////////////////////////////////////////
//___________________________________________________________//count uv ED.uv

	ED13.UV[U]=onethreeUV[U]*(1-f)+UV[maxVertex][U]*f;
	ED13.UV[V]=onethreeUV[V]*(1-f)+UV[maxVertex][V]*f;
//___________________________________________________________//
//	f=(ED12.current[Y]-vertex[thirdVertex][Y])/(vertex[secondVertex][Y]-vertex[thirdVertex][Y]);

	ED23.UV[U]=UV[secondVertex][U]*(1-f)+UV[maxVertex][U]*(f);
	ED23.UV[V]=UV[secondVertex][V]*(1-f)+UV[maxVertex][V]*(f);

//___________________________________________________________//
			/*  span start  */ 
		
		
			spanXtimes=1;
			deltaX=ceil(spanline.start[0])-spanline.start[0];
			spanline.end[0]=ceil(spanline.end[0]);// do simuntanelously with deltaY
		//	deltaX=floor(spanline.start[0])-spanline.start[0];

			while(spanline.current[0]+deltaX<=spanline.end[0])
														{/////do draw if end==int
			
			if(spanXtimes==1){deltaX=ceil(spanline.start[0])-spanline.start[0];}
			else{deltaX=1;}

			spanline.current[0]=spanline.current[0]+deltaX;
			spanline.current[2]=spanline.current[2]+deltaX*spanline.slopez;
///////////////////////////////////////////////////////////////////////////// set normal
f= (spanline.current[X]-spanline.start[X])/(spanline.end[X]-spanline.start[X]);

spanline.normal[X]=ED13.normal[X]*(1-f)+ED23.normal[X]*f;
spanline.normal[Y]=ED13.normal[Y]*(1-f)+ED23.normal[Y]*f;
spanline.normal[Z]=ED13.normal[Z]*(1-f)+ED23.normal[Z]*f;

float unnifiter;
unnifiter=unify(spanline.normal[X],spanline.normal[Y],spanline.normal[Z]);

spanline.normal[X]=spanline.normal[X]/unnifiter;
spanline.normal[Y]=spanline.normal[Y]/unnifiter;
spanline.normal[Z]=spanline.normal[Z]/unnifiter;

//___________________________________________________________// set span UV

spanline.UV[U]=ED13.UV[U]*(1-f)+ED23.UV[U]*f;
spanline.UV[V]=ED13.UV[V]*(1-f)+ED23.UV[V]*f;
//___________________________________________________________//
			//draw pixel
			GzGetDisplay(render->display, spanline.current[0], ED13.current[1],&tmpr,&tmpg,&tmpb,&tmpa,&tmpz);
			if(spanline.current[2]<tmpz)
			{
				float finaluv[2];
				float vz;
				GzColor texturecolor;

				vz=spanline.current[2]/(zmax-spanline.current[2]);

				finaluv[U]=spanline.UV[U]*(vz+1);  // transform UV-> uv
				finaluv[V]=spanline.UV[V]*(vz+1);  // transform UV-> uv

			

				GzColor phongcolor;

				render->tex_fun(finaluv[U],finaluv[V], texturecolor);
				
/////////////////////////////////////////////////////////////////////////// N->C


render->Ka[R]=texturecolor[R];
render->Ka[G]=texturecolor[G];
render->Ka[B]=texturecolor[B];

render->Kd[R]=texturecolor[R];
render->Kd[G]=texturecolor[G];
render->Kd[B]=texturecolor[B];


shade_equation(render, spanline.normal, &phongcolor);

/////////////////////////////////////////////////////////////////////////////
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],render->flatcolor[0],render->flatcolor[1],render->flatcolor[2],0,spanline.current[2]);
		//	GzPutDisplay(render->display,spanline.current[0],ED13.current[1],finalcolor[R],finalcolor[G],finalcolor[B],0,spanline.current[2]);
			GzPutDisplay(render->display,spanline.current[0],ED13.current[1],phongcolor[R],phongcolor[G],phongcolor[B],0,spanline.current[2]);

			}
			spanXtimes++;
			deltaX=1;
			
														}//second while
						
	
		
					}//firstwhile



		}//else   



}// else if Gz_normal
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */
int ComputeStack(GzRender	*render, int stacklevel, GzMatrix *EndMatrix, int type)
{
	
	int bottom;

	if(type==0){bottom=0;}
	else if(type==1){bottom=2;}

	if(render->matlevel==NULL){return GZ_FAILURE;}


	///////////////////// compute the Stack
			GzMatrix FMatrix,tmpMatrix;
			int countx,county;


			for(county=0;county<4;county++){

			for(countx=0;countx<4;countx++)
			{
			tmpMatrix[countx][county]=0;
			}
											}

			tmpMatrix[0][0]=1;
			tmpMatrix[1][1]=1;
			tmpMatrix[2][2]=1;
			tmpMatrix[3][3]=1;
		
			
	 short matlevelcounter=stacklevel;
		
				while(matlevelcounter>bottom)      {//while	

					matlevelcounter--;

		for(county=0;county<4;county++){  //multify push matrix

		for(countx=0;countx<4;countx++)
	{
			FMatrix[county][countx]=render->Ximage[matlevelcounter][county][0]*tmpMatrix[0][countx]+render->Ximage[matlevelcounter][county][1]*tmpMatrix[1][countx]+render->Ximage[matlevelcounter][county][2]*tmpMatrix[2][countx]+render->Ximage[matlevelcounter][county][3]*tmpMatrix[3][countx];

		//	FMatrix[county][countx]=tmpMatrix[county][0]*render->Ximage[render->matlevel][0][countx]+tmpMatrix[county][1]*render->Ximage[render->matlevel][1][countx]+tmpMatrix[county][2]*render->Ximage[render->matlevel][2][countx]+tmpMatrix[county][3]*render->Ximage[render->matlevel][3][countx];
	}		
		
										}
////////////////////////////////////////////////
		for(county=0;county<4;county++){   /// assign F to tmp

			for(countx=0;countx<4;countx++)
			{
			tmpMatrix[county][countx]=FMatrix[county][countx];
			}
											}

						
												}//while

/////////////////////////////////////////////////
	for(county=0;county<4;county++){   /// assign F to EndMatrix

			for(countx=0;countx<4;countx++)
			{
			(*EndMatrix)[county][countx]=FMatrix[county][countx];
			}
											}



return GZ_SUCCESS;

}

void shade_equation(GzRender *render,GzCoord norm, GzColor *finalcolor)
{
  int i;
  float speccolor[3]={0};
  float diffcolor[3]={0};
  float ambcolor[3]={0};


  GzCoord Rvertex;

  float NL;
  float NE;
  
///////////////////////////////////// set speccolor
	  for(i=0;i<render->numlights;i++)
{

	  NL=(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z]);
	  NE=(norm[X]*0+norm[Y]*0+norm[Z]*(-1));

	  if(NL>0&&NE>0){


speccolor[R]+=render->lights[i].color[R]*pow(((2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[X]-(render->lights[i].direction[X]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Y]-(render->lights[i].direction[Y]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Z]-(render->lights[i].direction[Z]))*(-1)),render->spec);
speccolor[G]+=render->lights[i].color[G]*pow(((2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[X]-(render->lights[i].direction[X]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Y]-(render->lights[i].direction[Y]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Z]-(render->lights[i].direction[Z]))*(-1)),render->spec);
speccolor[B]+=render->lights[i].color[B]*pow(((2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[X]-(render->lights[i].direction[X]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Y]-(render->lights[i].direction[Y]))*0+(2*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z])*norm[Z]-(render->lights[i].direction[Z]))*(-1)),render->spec);
	
	  }
	  else if(NL<0&&NE<0)
	  {

speccolor[R]+=render->lights[i].color[R]*pow(((2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[X])-(render->lights[i].direction[X]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Y])-(render->lights[i].direction[Y]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Z])-(render->lights[i].direction[Z]))*(-1)),render->spec);
speccolor[G]+=render->lights[i].color[G]*pow(((2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[X])-(render->lights[i].direction[X]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Y])-(render->lights[i].direction[Y]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Z])-(render->lights[i].direction[Z]))*(-1)),render->spec);
speccolor[B]+=render->lights[i].color[B]*pow(((2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[X])-(render->lights[i].direction[X]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Y])-(render->lights[i].direction[Y]))*0+(2*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z])*(-norm[Z])-(render->lights[i].direction[Z]))*(-1)),render->spec);
	  
	  
	  }
	  
}

speccolor[R]*=render->Ks[R];
speccolor[G]*=render->Ks[G];
speccolor[B]*=render->Ks[B];



///////////////////////////////  set diffcolor
  for(i=0;i<render->numlights;i++)
  {
	  NL=(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z]);
	  NE=(norm[X]*0+norm[Y]*0+norm[Z]*(-1));

	  if(NL>0&&NE>0){

diffcolor[R]+=render->lights[i].color[R]*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z]);
diffcolor[G]+=render->lights[i].color[G]*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z]);
diffcolor[B]+=render->lights[i].color[B]*(norm[X]*render->lights[i].direction[X]+norm[Y]*render->lights[i].direction[Y]+norm[Z]*render->lights[i].direction[Z]);
					}
	  else if(NL<0&&NE<0)
	  {
	  
diffcolor[R]+=render->lights[i].color[R]*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z]);
diffcolor[G]+=render->lights[i].color[G]*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z]);
diffcolor[B]+=render->lights[i].color[B]*((-norm[X])*render->lights[i].direction[X]+(-norm[Y])*render->lights[i].direction[Y]+(-norm[Z])*render->lights[i].direction[Z]);
	  
	  
	  }

  }
diffcolor[R]*=render->Kd[R];
diffcolor[G]*=render->Kd[G];
diffcolor[B]*=render->Kd[B];
///////////////////////////////set ambientcolor


  ambcolor[R]=render->ambientlight.color[R]*render->Ka[R];
  ambcolor[G]=render->ambientlight.color[G]*render->Ka[G];
  ambcolor[B]=render->ambientlight.color[B]*render->Ka[B];


	//	render->flatcolor[R]=ctoi(speccolor[R]+diffcolor[R]+ambcolor[R]);
	//	render->flatcolor[G]=ctoi(speccolor[G]+diffcolor[G]+ambcolor[G]);
	//    render->flatcolor[B]=ctoi(speccolor[B]+diffcolor[B]+ambcolor[B]);
		
	(*finalcolor)[R]=ctoi(speccolor[R]+diffcolor[R]+ambcolor[R]);	
	(*finalcolor)[G]=ctoi(speccolor[G]+diffcolor[G]+ambcolor[G]);
	(*finalcolor)[B]=ctoi(speccolor[B]+diffcolor[B]+ambcolor[B]);

}
