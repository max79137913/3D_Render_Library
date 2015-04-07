/*
OBJParser.cpp
Parser to read .obj files into data structures
Copyright 2006 Andrew Goodney
This file is freely distributable and useable for any purpose as long as this
copyright statement is included and credit is given to the author.

If you make changes or improvement please consider releasing those so that others
may benefit.

The parsing is done "two-pass". First the lines are examined and counted.
Memory is the appropriately allocated.

If a face exists but the vertex normal or vertex texture is not parsed or is missing,
a value of -1 will be returned as an indication of missing information.

See the demo program for usage. 

I think this works. It rendered the teapot. Have fun.
*/
#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>

#include "OBJParser.h"

#define LINE_BUFFER_SIZE 1024

using namespace std;

OBJObject::OBJObject()
{
    this->numVertices = 0;
    this->numVNormals = 0;
    this->numVTextures = 0;
    this->numFaces = 0;
    this->dBug = 0;
}

OBJObject::~OBJObject()
{
    if(this->vList) delete this->vList; 
    if(this->vnList) delete this->vnList;
    if(this->vtList) delete this->vtList;
    if(this->faceList) delete this->faceList;
}

int OBJObject::parse(char *fileName)
{
    char line[1024];
    
    ifstream obj_file;
    obj_file.open(fileName, ifstream::in);
    
    if (!obj_file)
    {
        cerr << "OBJObject::parse File " << fileName << "could not be opened." << endl;
        return -1; 
    }                      
    while(!obj_file.eof())
    { 
        //implement two-pass scanning
        //count the vertices,normals,textures and faces so we can allocate arrays
        for(int i=0;i<1024;i++) line[i] = 0;
        
        obj_file.getline(line,1024);
        
        if( memcmp(line, "v ", 2) == 0 )
        {
            this->numVertices++;            
        }

        if( memcmp(line, "vn", 2) == 0 )
        {
            this->numVNormals++;
        }
        
        if( memcmp(line, "vt", 2) == 0 )
        {
            this->numVTextures++;
        }
        
        if( memcmp(line, "f ", 2) == 0 )
        {
            this->numFaces++;
        }
        
    }//end while, do second pass after this
    //Now allocate arrays- remember to delete!
    if(this->dBug)
    {
        cout << "Number of Vertices: " << numVertices << endl;
        cout << "Number of Vertex Normals " << numVNormals << endl;
        cout << "Number of Vertex Textures " << numVTextures << endl;
        cout << "Number of Faces " << numFaces << endl;
    }
    vList = new Vertex[numVertices]; 
    vnList = new  VertexNormal[numVNormals];
    vtList = new VertexTexture[numVTextures];
    faceList = new Face[numFaces]; 
    
    
    int vCount = 0;
    int vnCount = 0;
    int vtCount = 0;
    int faceCount = 0;
    
    obj_file.clear();
    obj_file.seekg(0,ios::beg);
      
    while(!obj_file.eof())
    {
       
       
       for(int i=0;i<1024;i++) line[i] = 0;
       
       if(obj_file.peek()== '#')
       {
           if(this->dBug)
           {
               cout << "Found comment." << endl;
           }
           obj_file.getline(line,1024);
           continue;
       }
       
       if( (obj_file.peek() == 'v') || (obj_file.peek() == 'f'))
       {
           line[0] = obj_file.get();
           line[1] = obj_file.get();
       }
       if( memcmp(line, "v ", 2) == 0 )
       {
           float x,y,z;
           stringstream linestream;
           for(int i=0;i<1024;i++) line[i] = 0;
           obj_file.getline(line,1024);
           
           linestream << line;
           linestream >> x >> y >> z;
           
           this->vList[vCount].x = x;
           this->vList[vCount].y = y;
           this->vList[vCount].z = z;
           
           
           vCount++;
           if(this->dBug)
           {
               cout << x << "," << y << "," << z << endl;
               cout << "Read vertex " << vCount << endl;
           }
                 
       }
       else if( memcmp(line, "vn", 2) == 0 )
       {
           float x,y,z;
           stringstream linestream;
           for(int i=0;i<1024;i++) line[i] = 0;
           obj_file.getline(line,1024);
           
           linestream << line;
           linestream >> x >> y >> z;
           
           this->vnList[vnCount].x = x;
           this->vnList[vnCount].y = y;
           this->vnList[vnCount].z = z;
           
           
           vnCount++;
           if(this->dBug)
           {
               cout << x << "," << y << "," << z << endl;
               cout << "Read vertex normal " << vnCount << endl;
           }      
       }
       else if( memcmp(line, "vt", 2) == 0 )
       {
           float s,t;
           stringstream linestream;
           for(int i=0;i<1024;i++) line[i] = 0;
           obj_file.getline(line,1024);
           
           linestream << line;
           linestream >> s >> t;
           
           this->vtList[vtCount].s = s;
           this->vtList[vtCount].t = t;
           
           vtCount++;
           if(this->dBug)
           {
               cout << s << "," << t << endl;
               cout << "Read vertex normal " << vtCount << endl;
           }     
       } 
       else if( memcmp(line, "f ", 2) == 0)
       {
           char numBuffer[32];
           char field[256];
           stringstream linestream;
           
           
           for(int i=0;i<256;i++)field[i]=0;
           
           int fieldIndex=0;
           int numIndex=0;
           int prevIndex=0;
           int val=0;
           int fields=0;
           
           for(int i=0;i<1024;i++)line[i]=0;
           obj_file.getline(line,1024);
           
           linestream << line;
           
           while(!linestream.fail())
           {
               linestream >> field;
               if(!linestream.fail())
               {
                   fields++;
               }
               
               
           }
           
           //now we know the number of fields on this line...
           this->faceList[faceCount].numVertices = fields;
           
           this->faceList[faceCount].faceVList = new int[fields];
           this->faceList[faceCount].faceVTList = new int[fields];
           this->faceList[faceCount].faceVNList = new int[fields];
           
           //set above to -1 to indicate missing value
           for(int i=0;i<fields;i++)
           {
               this->faceList[faceCount].faceVList[i] = -1;
               this->faceList[faceCount].faceVTList[i] = -1;
               this->faceList[faceCount].faceVNList[i] = -1;
           }
           if(this->dBug)
           {
               cout << "Face: " << faceCount << " has " << fields << " vertices." << endl;
           }
           linestream.clear();
           linestream << line;
           
           for(int i=0;i<fields;i++)
           {
           
               for(int z=0;z<256;z++)field[z]=0;
               linestream >> field;
               fieldIndex = 0;
               //look for vertex
               for(int z=0;z<32;z++)numBuffer[z]=0;
               numIndex=0;
               
               while( (field[fieldIndex] != '/') && (field[fieldIndex] != 0) )
               {
                   numBuffer[numIndex] = field[fieldIndex];
                   numIndex++;
                   fieldIndex++;
               }
               prevIndex = fieldIndex;
               if(this->dBug)
               {
                   cout << "Face: " << faceCount << " vertex " << i << ": " << atoi(numBuffer) << endl;
               }
               this->faceList[faceCount].faceVList[i] = atoi(numBuffer);
               
               if( field[fieldIndex] == 0 )
               {
                  //no vt or vn's
                  if(this->dBug)
                  {
                      cout << "Face: " << faceCount << " vertex" << i << ": No VT, no VN" << endl;
                  }
                  continue;
               }
               
               if( field[++fieldIndex] == '/' )
               {
                   //no VT but a VN
                   fieldIndex++;
                   for(int z=0;z<32;z++)numBuffer[z]=0;
                   numIndex = 0;
                   
                   while( field[fieldIndex] != 0)
                   {
                       numBuffer[numIndex] = field[fieldIndex];
                       numIndex++;
                       fieldIndex++;
                      
                   }
                   
                   if(this->dBug)
                   {
                       cout << "Face: " << faceCount << " vertex normal " << i << ": " << atoi(numBuffer) << endl;
                   }
                   this->faceList[faceCount].faceVNList[i] = atoi(numBuffer);
                   
               }//no VT but a VN
               else //must be a VT with a VN
               {
                   
                   for(int z=0;z<32;z++)numBuffer[z]=0;
                   numIndex = 0;
                   
                   while( field[fieldIndex] != '/')
                   {
                       numBuffer[numIndex] = field[fieldIndex];
                       numIndex++;
                       fieldIndex++;
                   
                   }
                   if(this->dBug)
                   {
                       cout << "Face: " << faceCount << " vertex texture " << i << ": " << numBuffer << endl;
                   }
                   this->faceList[faceCount].faceVTList[i] = atoi(numBuffer);
                   
                   fieldIndex++;
                   for(int z=0;z<32;z++)numBuffer[z]=0;
                   numIndex = 0;
                   
                   while( field[fieldIndex] != 0)
                   {
                       numBuffer[numIndex] = field[fieldIndex];
                       numIndex++;
                       fieldIndex++;
                      
                   }
                   if(this->dBug)
                   {
                       cout << "Face: " << faceCount << " vertex normal " << i << ": " << atoi(numBuffer) << endl;
                   }
                   this->faceList[faceCount].faceVNList[i] = atoi(numBuffer);
                   
               }
           }
           if(this->dBug)
           {
               cout << "Read face " << faceCount  << endl;
           }
           faceCount += 1;
           
       }
       else
       {
           obj_file.getline(line,1024);
       }
    }//second while loop
    
        
}//OBJObject::parse

int OBJObject::getNumOfVertices()
{
    return this->numVertices;
} 
int OBJObject::getNumOfNormals()
{
    return this->numVNormals++;
} 
int OBJObject::getNumOfTextures()
{
    return this->numVTextures;
}

int OBJObject::getNumOfFaces()
{
    return this->numFaces;
}

Face OBJObject::getFace(int faceNum)
{
    return this->faceList[faceNum];
}

Vertex OBJObject::getVertex(int vertexNum)
{
    return this->vList[vertexNum - 1];
}

VertexNormal OBJObject::getVertexNormal(int normalNum)
{
    return this->vnList[normalNum - 1];
}
VertexTexture OBJObject::getVertexTexture(int vertextextureNum)
{

    return this->vtList[vertextextureNum - 1];              
              
}

int OBJObject::debug()
{
   if(this->dBug)
   {
      return 1;
   }
   else
   {
       return 0;
   }
}
void OBJObject::debugOn()
{
    this->dBug = 1;     
}
void OBJObject::debugOff()
{
    this->dBug = 0;
}
