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
#define WORLD_W 1001
#define WORLD_Z 1001
#define CHUNK_SIZE 100

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif


struct BlockOBJ {
    /*0 = RectPrism*/
    int type;
    int x, y, z, w, h, d;
    Mesh mesh;
};

/*UTILMATH.C*/
int randRange(int range);
double dist(double x1, double y1, double x2, double y2);

/*SANDMESH.C*/
void refreshMeshBuffer(Matrix *m, int *mCounts);
int fHeight(char ***arr, int j, int k, int rVal);
Vector3 calcNormals(int j, int k, char ***arr);
Vector3 quickCalcNormals(int a, int b, int c, int j, int k);
Mesh calcMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density);
void recalculateMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density, Mesh mesh, struct BlockOBJ *myBlockOBJs, int myBlockOBJsCount);
void updatedRecalculateMesh3D(float size, char ***arr, int cX, int cZ, Mesh mesh);
void clearAllMeshes(Model **meshArr, char **occupiedMesh);

void initBlockOBJ(struct BlockOBJ *b, int type, int x, int y, int z, int w, int h, int d, char ***grid);
void renderBlockOBJ(struct BlockOBJ *b, float blockSizes, Model mesh);

/*SANDPARTICLES.C*/
int rayBounds(char ***arr, int i, int j, int k, Vector3 cameraPos, float size);
int optimizedEvalParticles3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, Matrix *m, int *mCounts, int density, Vector3 cameraPos, int *updateQueue, int updateLength);

/*SANDGRID.C*/
int testAdj(int i, int j, int k, int w, int h, int d, char ***arr, int moves, int type, int *updateQueue, int *updateLength, int x);
void updateGrid(int *updateQueue, int *updateLength, char ***arr, float blockSize, Model **meshArr, struct BlockOBJ *myBlockOBJs, int myBlockOBJsCount);
void insertParticleIntoQueue(int *updateQueue, int *updateLength, int y, int x, int z);

/*SANDBRUSHES.C*/
void rainBrush(char ***grid);
void dumbBrush(char ***grid);
void dustBrush(char ***grid, int *updateQueue, int *updateLength);
void localBrush(char ***grid);
void noiseBrush(char ***grid);


/*PERLIN.C*/
int noise2(int x, int y);
float lin_inter(float x, float y, float s);
float smooth_inter(float x, float y, float s);
float noise2d(float x, float y);
float perlin2d(float x, float y, float freq, int depth);


#endif