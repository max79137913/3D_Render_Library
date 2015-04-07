/*
OBJParser.h
Header file for OBJParser.cpp
Copyright 2006 Andrew Goodney
This file is freely distributable and useable for any purpose as long as this
copyright statement is included and credit is given to the author.

If you make changes or improvements, please consider releasing those so that others
may benefit.
*/

struct Vertex {
       
       float x;
       float y;
       float z;

};

struct VertexNormal {

       float x;
       float y;
       float z;

};

struct VertexTexture {

       float s;
       float t;
};

struct Face {
     
      char *mtl;
      int numVertices;
      int *faceVList;
      int *faceVNList;
      int *faceVTList;

};

class OBJObject {
       
       char *mtlfile;
       int numVertices;
       int numVNormals;
       int numVTextures;
       int numFaces;
       int dBug;
       Vertex *vList;
       VertexNormal *vnList;
       VertexTexture *vtList;
       Face *faceList;
       
public:
       OBJObject();
       int parse(char *);
       
       Vertex getVertex(int);
       Face getFace(int);
       VertexNormal getVertexNormal(int);
       VertexTexture getVertexTexture(int);
       
       int getNumOfVertices(); 
	   int getNumOfNormals(); 
	   int getNumOfTextures();
	   int getNumOfFaces();
	   int debug();
	   void debugOn();
	   void debugOff();
       ~OBJObject();
       
};







  
        
