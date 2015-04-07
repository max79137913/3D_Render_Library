/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include <limits.h>


int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value

	/* Initialize the mat with value 0*/
	for (int i = 0; i < 4; i++)
		memset(mat[i], 0, sizeof(float)* 4);

	mat[0][0] = 1;
	mat[1][1] = cos((degree*3.14159265) / 180); mat[1][2] = -sin((degree*3.14159265) / 180);
	mat[2][2] = cos((degree*3.14159265) / 180); mat[2][1] = sin((degree*3.14159265) / 180);
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

	/* Initialize the mat with value 0*/
	for (int i = 0; i < 4; i++)
		memset(mat[i], 0, sizeof(float)* 4);

	mat[0][0] = cos((degree*3.14159265) / 180); mat[0][2] = sin((degree*3.14159265) / 180);
	mat[1][1] = 1;
	mat[2][0] = -sin((degree*3.14159265) / 180); mat[2][2] = cos((degree*3.14159265) / 180);
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

	/* Initialize the mat with value 0*/
	for (int i = 0; i < 4; i++)
		memset(mat[i], 0, sizeof(float)* 4);

	mat[0][0] = cos((degree*3.14159265) / 180); mat[0][1] = -sin((degree*3.14159265) / 180);
	mat[1][0] = sin((degree*3.14159265) / 180); mat[1][1] = cos((degree*3.14159265) / 180);
	mat[2][2] = 1;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value

	/* Initialize the mat with value 0*/
	for (int i = 0; i < 4; i++)
	{
		memset(mat[i], 0, sizeof(float)* 4);
		mat[i][i] = 1;
	}

	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value

	/* Initialize the mat with value 0*/
	for (int i = 0; i < 4; i++)
		memset(mat[i], 0, sizeof(float)* 4);

	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1;
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
	GzRender *r = (GzRender*)malloc(sizeof(GzRender));

	r->open = 0;
	r->display = display;
	r->renderClass = GZ_Z_BUFFER_RENDER;

	/* Setup Xsp */
	int xs = display->xres;
	int ys = display->yres;
	
	for (int i = 0; i < 4; i++)
		memset(r->Xsp[i], 0, sizeof(float)*4);

	r->Xsp[0][0] = xs / 2; r->Xsp[0][3] = xs / 2;
	r->Xsp[1][1] = -ys / 2; r->Xsp[1][3] = ys / 2;
	r->Xsp[2][2] = 10000 * tan((r->camera.FOV/2) * 3.14159265 / 180);
	r->Xsp[3][3] = 1;

	/* Init default camera */
	r->camera.position[X] = 0;
	r->camera.position[Y] = 0;
	r->camera.position[Z] = 0;

	r->camera.lookat[X] = 0;
	r->camera.lookat[Y] = 0;
	r->camera.lookat[Z] = 1;

	r->camera.worldup[X] = 0;
	r->camera.worldup[Y] = 1;
	r->camera.worldup[Z] = 0;

	r->camera.FOV = 90;

	//r->open = 1;
	r->open = 1;
	
	*render = r;

	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	free(render);
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
	

	/* Compoute Xiw */
	GzCoord CamX, CamY, CamZ, C;

	// Calculate CamZ
	for (int i = 0; i < 3; i++)
	{
		C[i] = render->camera.position[i];
		//CamY[i] = render->camera.worldup[i];
		CamZ[i] = render->camera.lookat[i] - render->camera.position[i];
	}
	float lenZ = length(CamZ);
	for (int i = 0; i < 3; i++)
		CamZ[i] = CamZ[i] / lenZ;

	// Calculate CamY
	float dotUpZ = dotProduct(render->camera.worldup, CamZ);
	for (int i = 0; i < 3; i++)
	{
		CamY[i] = render->camera.worldup[i] - dotUpZ * CamZ[i];
	}
	float lenY = length(CamY);
	for (int i = 0; i < 3; i++)
		CamY[i] = CamY[i] / lenY;

	// Calculate CamX
	float *ptr = crossProduct(CamY, CamZ);
	for (int i = 0; i < 3; i++)
		CamX[i] = ptr[i];
	float lenX = length(CamX);
	for (int i = 0; i < 3; i++)
		CamX[i] = CamX[i] / lenX;

	for (int i = 0; i < 3; i++)
	{
		render->camera.Xiw[0][i] = CamX[i];
		render->camera.Xiw[1][i] = CamY[i];
		render->camera.Xiw[2][i] = CamZ[i];
		render->camera.Xiw[3][i] = 0;
	}
	render->camera.Xiw[0][3] = -(CamX[0] * C[0] + CamX[1] * C[1] + CamX[2] * C[2]);
	render->camera.Xiw[1][3] = -(CamY[0] * C[0] + CamY[1] * C[1] + CamY[2] * C[2]);
	render->camera.Xiw[2][3] = -(CamZ[0] * C[0] + CamZ[1] * C[1] + CamZ[2] * C[2]);
	render->camera.Xiw[3][3] = 1;

	/* Compute Xpi */
	for (int i = 0; i < 4; i++)
		memset(render->camera.Xpi[i], 0, sizeof(float)*4);
	for (int i = 0; i < 4; i++)
		render->camera.Xpi[i][i] = 1;
	render->camera.Xpi[3][2] = tan((render->camera.FOV/2)*3.14159265/180);

	/* Init Ximage */
	render->matlevel = 0; // initial position of the stack
	GzPushMatrix(render, render->Xsp);			// push Xsp
	GzPushMatrix(render, render->camera.Xpi);	// push Xpi
	GzPushMatrix(render, render->camera.Xiw);	// push Xiw

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	render->camera = *camera;
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (render->matlevel == MATLEVELS - 1)
		return GZ_FAILURE;
	if (render->matlevel == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				render->Ximage[render->matlevel][i][j] = matrix[i][j];
		}
	}
	else
	{
		matrixMulti(render->Ximage[render->matlevel - 1], matrix, render->Ximage[render->matlevel]);
	}
	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render->matlevel == 0)
		return GZ_FAILURE;
	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++)
	{
		switch (nameList[i]) {
		case GZ_RGB_COLOR:
			//GzColor* color = (GzColor*) valueList;
			float* color = (float*)valueList[0];
			render->flatcolor[0] = color[0];
			render->flatcolor[1] = color[1];
			render->flatcolor[2] = color[2];
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
	GzCoord *vertices;
	for (int i = 0; i < numParts; i++)
	{
		switch (nameList[i])
		{
		case GZ_POSITION:
		{
							GzCoord *vl;
							vl = (GzCoord*)valueList[0];

							// Apply the matrices to the vertex
							for (int k = 0; k < 3; k++)
							{
								transform(render, vl[k]);
							}
							vertices = vl;
							scanConverter(render, vl);
							break;
		}

		case GZ_NORMAL:
		{
						  break;
		}
		default:
			break;
		}
	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

/*
This function returns the result of a corss product of two vectors
*/
float* crossProduct(float* v1, float* v2)
{
	float result[3];
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return result;
}

/*
This function calcualtes the dot product of two vectors
*/
float dotProduct(GzCoord x, GzCoord y)
{
	return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

/*
This function calculates the length of a vector
*/
float length(GzCoord v)
{
	return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}

/*
Do transformation on one vertex
*/
void transform(GzRender *render, GzCoord vl)
{
	float *homoCoord;
	homoCoord = (float*)malloc(sizeof(float)* 4);
	for (int j = 0; j < 3; j++)
		homoCoord[j] = vl[j];
	homoCoord[3] = 1;
	applyMatrix(homoCoord, render->Ximage[render->matlevel - 1]);

	// Return transformation values back to vl
	for (int j = 0; j < 3; j++)
	{
		vl[j] = homoCoord[j] / homoCoord[3];
	}
}

/*
This is a self-defined function which is not included in the API
provided. This is a scan converter.
*/

void scanConverter(GzRender *render, GzCoord *vertexList)
{
	GzCoord top, mid, btm;
	GzCoord vtx1, vtx2, vtx3;

	/* Find a bounding box of the tri, and sort 3 verts */
	/* Find max and min x values */
	float minx, miny, maxx, maxy;
	if (vertexList[0][0] >= vertexList[1][0])
	{
		maxx = vertexList[0][0];
		minx = vertexList[1][0];
	}
	else
	{
		maxx = vertexList[1][0];
		minx = vertexList[0][0];
	}
	if (vertexList[2][0] > maxx) maxx = vertexList[2][0];
	if (vertexList[2][0] < minx) minx = vertexList[2][0];

	/* Find max and min y values*/
	if (vertexList[0][1] >= vertexList[1][1])
	{
		top[0] = vertexList[0][0]; top[1] = vertexList[0][1]; top[2] = vertexList[0][2];
		btm[0] = vertexList[1][0]; btm[1] = vertexList[1][1]; btm[2] = vertexList[1][2];
	}
	else
	{
		top[0] = vertexList[1][0]; top[1] = vertexList[1][1]; top[2] = vertexList[1][2];
		btm[0] = vertexList[0][0]; btm[1] = vertexList[0][1]; btm[2] = vertexList[0][2];
	}
	if (vertexList[2][1] > top[1])
	{
		mid[0] = top[0]; mid[1] = top[1]; mid[2] = top[2];
		top[0] = vertexList[2][0]; top[1] = vertexList[2][1]; top[2] = vertexList[2][2];
	}
	else if (vertexList[2][1] < btm[1])
	{
		mid[0] = btm[0]; mid[1] = btm[1]; mid[2] = btm[2];
		btm[0] = vertexList[2][0]; btm[1] = vertexList[2][1]; btm[2] = vertexList[2][2];
	}
	else
	{
		mid[0] = vertexList[2][0]; mid[1] = vertexList[2][1]; mid[2] = vertexList[2][2];
	}

	maxx = ceil(maxx);
	maxy = ceil(top[1]);
	minx = floor(minx);
	miny = floor(btm[1]);

	if (top[1] == mid[1]) // Top edge exits
	{
		if (top[0] < mid[0])
		{
			vtx1[0] = top[0]; vtx1[1] = top[1]; vtx1[2] = top[2];
			vtx2[0] = mid[0]; vtx2[1] = mid[1]; vtx2[2] = mid[2];
		}
		else
		{
			vtx2[0] = top[0]; vtx2[1] = top[1]; vtx2[2] = top[2];
			vtx1[0] = mid[0]; vtx1[1] = mid[1]; vtx1[2] = mid[2];
		}
		vtx3[0] = top[0]; vtx1[1] = top[1]; vtx1[2] = top[2];
	}
	else if (btm[1] == mid[1]) // Bottom edge exits
	{
		if (btm[0] < mid[0])
		{
			vtx3[0] = btm[0]; vtx3[1] = btm[1]; vtx3[2] = btm[2];
			vtx2[0] = mid[0]; vtx2[1] = mid[1]; vtx2[2] = mid[2];
		}
		else
		{
			vtx2[0] = btm[0]; vtx2[1] = btm[1]; vtx2[2] = btm[2];
			vtx3[0] = mid[0]; vtx3[1] = mid[1]; vtx3[2] = mid[2];
		}
		vtx1[0] = top[0]; vtx1[1] = top[1]; vtx1[2] = top[2];
	}
	else
	{
		float deltax = top[0] - btm[0];
		float deltay = top[1] - btm[1];
		float slope = deltax / deltay;
		float x = mid[1] * slope - top[1] * slope + top[0];
		vtx1[0] = top[0]; vtx1[1] = top[1]; vtx1[2] = top[2];
		if (x < mid[0])
		{
			vtx2[0] = mid[0]; vtx2[1] = mid[1]; vtx2[2] = mid[2];
			vtx3[0] = btm[0]; vtx3[1] = btm[1]; vtx3[2] = btm[2];
		}
		else
		{
			vtx2[0] = btm[0]; vtx2[1] = btm[1]; vtx2[2] = btm[2];
			vtx3[0] = mid[0]; vtx3[1] = mid[1]; vtx3[2] = mid[2];
		}
	}

	for (int i = (int)minx; i < (int)maxx; i++)
	{
		for (int j = (int)miny; j < (int)maxy; j++)
		{
			GzCoord point;
			point[0] = i;
			point[1] = j;
			point[2] = 0;
			if (LEE(vtx1, vtx2, point) >= 0 && LEE(vtx2, vtx3, point) >= 0 && LEE(vtx3, vtx1, point) >= 0)
			{
				/* Interpolate Z */
				float vector1[3], vector2[3];
				float *result = (float*)malloc(sizeof(float)* 3);
				vector1[0] = vtx1[0] - vtx2[0]; vector1[1] = vtx1[1] - vtx2[1]; vector1[2] = vtx1[2] - vtx2[2];
				vector2[0] = vtx3[0] - vtx2[0]; vector2[1] = vtx3[1] - vtx2[1]; vector2[2] = vtx3[2] - vtx2[2];
				result = crossProduct(vector1, vector2);
				if (result[0] * vtx2[0] + result[1] * vtx2[1] + result[2] * vtx2[2] == result[0] * vtx3[0] + result[1] * vtx3[1] + result[2] * vtx3[2])
					int a = 3;
				point[2] = (result[0] * vtx1[0] + result[1] * vtx1[1] + result[2] * vtx1[2] - result[0] * i - result[1] * j) / result[2];
				
				///* Do the transformation */
				//transform(render, point);
				
				Rasterize(render->display, point[0], point[1],
					ctoi(render->flatcolor[0]),
					ctoi(render->flatcolor[1]),
					ctoi(render->flatcolor[2]),
					1, point[2]);
			}
		}
	}
}

/*
Linear expresion evaluation for one point
*/
float LEE(GzCoord tail, GzCoord head, GzCoord point)
{
	float deltaX = head[0] - tail[0];
	float deltaY = head[1] - tail[1];
	float x = point[0];
	float y = point[1];

	float decision = deltaY*x - deltaX*y + deltaX*tail[1] - deltaY*tail[0];
	return decision;
}

/*
Put color into a pixel
*/
int Rasterize(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, float z)
{
	/* write pixel values into the display */
	int x = display->xres;
	int y = display->yres;
	if (i >= 0 && i <= x - 1 && j >= 0 && j <= y - 1)
	{
		GzPixel *pixel = display->fbuf + i + j*x;
		if (z < pixel->z)
		{
			pixel->blue = b;
			pixel->green = g;
			pixel->red = r;
			pixel->alpha = a;
			pixel->z = z;
		}
		return GZ_SUCCESS;
	}
}

/* Apply a matrix to the vector */
void applyMatrix(float *v, GzMatrix mat)
{
	float *result = (float*)malloc(sizeof(float) * 4);
	memset(result, 0, sizeof(float)*4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			result[i] += v[j] * mat[i][j];
	}
	for (int i = 0; i < 4; i++)
		v[i] = result[i];
}

/* 
Multiply two matrices 
*/
void matrixMulti(GzMatrix A, GzMatrix B, GzMatrix C)
{
	//calcul of C=A*B 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float sum = 0;
			for (int k = 0; k < 4; k++)
				sum += A[i][k] * B[k][j];
			C[i][j] = sum;
		}
	}
	
}

void scanConverter2(GzRender *render, GzCoord *vertexList)
{
	GzCoord ver1, ver2, ver3;
	int top, btm;

	/* Sort vertices */
	ver1[0] = vertexList[0][0]; ver1[1] = vertexList[0][1]; ver1[2] = vertexList[0][2];
	for (int i = 0; i < 3; i++)
	{
		if (vertexList[i][1] < ver1[1])
		{
			ver1[0] = vertexList[i][0];
			ver1[1] = vertexList[i][1];
			ver1[2] = vertexList[i][2];
			btm = i;
		}
	}

	ver3[0] = vertexList[0][0]; ver3[1] = vertexList[0][1]; ver3[2] = vertexList[0][2];
	for (int i = 0; i < 3; i++)
	{
		if (vertexList[i][1] > ver3[1])
		{
			ver3[0] = vertexList[i][0];
			ver3[1] = vertexList[i][1];
			ver3[2] = vertexList[i][2];
			top = 1;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (i != top && i != btm)
		{
			ver2[0] = vertexList[i][0];
			ver2[1] = vertexList[i][1];
			ver2[2] = vertexList[i][2];
		}
	}

	/* Determine L/R */
	float slopex12 = (ver1[0] - ver2[0]) / (ver1[1] - ver2[1]);
	float slopex13 = (ver1[0] - ver3[0]) / (ver1[1] - ver3[1]);
	float slopez12 = (ver2[2] - ver1[2]) / (ver2[1] - ver1[1]);
	float slopez13 = (ver3[2] - ver1[2]) / (ver3[1] - ver1[1]);
	
	float currentY = ver1[1];
	float currentX = ver1[0];
	float currentZ = ver1[2];
	float endX = ver1[0];
	float endY = ver1[1];
	float endZ = ver1[2];
	while (currentY <= ver2[1])
	{
		float slopezx = (endZ - currentZ) / (endX - currentX);
		for (int i = ceil(currentX); ; i < floor(endX))
		{
			int x = render->display->xres;
			int y = render->display->yres;
			if (i >= 0 && i <= x - 1 && currentY >= 0 && currentY <= y - 1)
			{
				GzPixel *pixel = render->display->fbuf + i + (int)currentY * x;
				int z = currentZ;
				if (z < pixel->z)
				{
					pixel->red = ctoi(render->flatcolor[0]);
					pixel->green = ctoi(render->flatcolor[1]);
					pixel->blue = ctoi(render->flatcolor[2]);
					pixel->z = z;
				}
			}
			currentZ += slopezx;
			if (slopex12 < slopex13) i++;
			else i--;
		}
		float deltaY = ceil(currentY) - currentY;
		if (deltaY == 0.00) deltaY = 1.00;
		currentY += deltaY;
		currentX += deltaY*slopex12;
		currentZ += deltaY*slopez12;
		endX += deltaY*slopex13;
		endZ += deltaY*slopez13;
	}

	float slopex23 = (ver3[0] - ver2[0]) / (ver3[1] - ver2[1]);
	float slopez23 = (ver3[2] - ver2[2]) / (ver3[1] - ver2[1]);
	while (currentY <= ver3[1])
	{
		float slopezx = (endZ - currentZ) / (endX - currentX);
		for (int i = ceil(currentX);; i < floor(endX))
		{
			int x = render->display->xres;
			int y = render->display->yres;
			if (i >= 0 && i <= x - 1 && currentY >= 0 && currentY <= y - 1)
			{
				GzPixel *pixel = render->display->fbuf + i + (int)currentY * x;
				int z = currentZ;
				if (z < pixel->z)
				{
					pixel->red = ctoi(render->flatcolor[0]);
					pixel->green = ctoi(render->flatcolor[1]);
					pixel->blue = ctoi(render->flatcolor[2]);
					pixel->z = z;
				}
			}
			currentZ += slopezx;
			if (slopex12 < slopex13) i++;
			else i--;
		}
		float deltaY = ceil(currentY) - currentY;
		if (deltaY == 0.00) deltaY = 1.00;
		currentY += deltaY;
		currentX += deltaY*slopex23;
		currentZ += deltaY*slopez23;
		endX += deltaY*slopex13;
		endZ += deltaY*slopez13;
	}
}