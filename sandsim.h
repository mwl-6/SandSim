#ifndef SANDSIM_H_
#define SANDSIM_H

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


/* 
 * 200 x 500 x 100 
 * Chunk size 50
 * */
#define WORLD_H 200
#define WORLD_W 1000
#define WORLD_Z 1000
#define CHUNK_SIZE 100

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif


/*UTILMATH.C*/
int randRange(int range);
double dist(double x1, double y1, double x2, double y2);

/*SANDMESH.C*/
void refreshMeshBuffer(Matrix *m, int *mCounts);
int fHeight(char ***arr, int j, int k);
Vector3 calcNormals(int j, int k, char ***arr);
Vector3 quickCalcNormals(int a, int b, int c, int j, int k);
Mesh calcMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density);
void recalculateMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density, Mesh mesh);

/*SANDPARTICLES.C*/
int rayBounds(char ***arr, int i, int j, int k, Vector3 cameraPos, float size);
int optimizedEvalParticles3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, Matrix *m, int *mCounts, int density, Vector3 cameraPos, int *updateQueue, int updateLength);

/*SANDGRID.C*/
int testAdj(int i, int j, int k, int w, int h, int d, char ***arr, int moves, int type, int *updateQueue, int *updateLength, int x);
void updateGrid(int *updateQueue, int *updateLength, char ***arr, char ***chunks, float blockSize, Model **meshArr);


/*SANDBRUSHES.C*/
void rainBrush(char ***grid, char ***chunks);
void dumbBrush(char ***grid, char ***chunks);
void dustBrush(char ***grid, char ***chunks, int *updateQueue, int *updateLength);
void localBrush(char ***grid, char ***chunks);




#endif