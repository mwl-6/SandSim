#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
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
#define WORLD_W 1300
#define WORLD_Z 1300
#define CHUNK_SIZE 100

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif



int randRange(int range){
	return (rand() % (range + 1));
}
double dist(double x1, double y1, double x2, double y2){
	return sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1));
}



int calcMesh3D(float *size, char ***arr, int cX, int cY, int cZ, int cXW, int cYW, int cZW, Mesh cube, Material matDefault, Matrix **m, int *mCounts, int *divs, int divCnt){
		
		int mIs[7] = {0};
		
		//m = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(Matrix));
		
		Vector3 axis = (Vector3){ 0, 0, 0 };
		float angle = 0;
		Matrix rotation = MatrixRotate(axis, angle);

		#pragma omp parallel shared (arr)
		{
			int localmIs[7] = {0};
			int skip = 1;
			Matrix **localM = RL_CALLOC(7, sizeof(Matrix*));
			for(int i = 0; i < 7; i++){
				localM[i] = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 7, sizeof(Matrix));
				if(localM[i] == NULL){
					printf("Failed memory allocation\n");
				}
			}
			//Matrix *localM = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 7, sizeof(Matrix));
			
			
			int tid = omp_get_thread_num();
			int num_threads = omp_get_num_threads();
			int localI;
			
			

			int i, j, k;
			for(i = cY+cYW; i >= cY; i-=1){
			
				if(i == WORLD_H)
					continue;
				/*DIV1 LOCATIONS*/
				int localDivisor = 1;
				for(j = cX; j < divs[0] && j < cX+cXW; j++){
					for(k = tid+cZ; k < divs[0] && k < cZW; k+=num_threads){
						
						char curr = 0;
						curr = arr[i][j][k];

						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], k*size[0]);
								localM[0][localmIs[0]] = MatrixMultiply(rotation, translation);
								localmIs[0]++;
								
							}
						}
						
					}
				}
				
				/*divs[1] LOCATIONS*/
				//for(int l = 0; l < divCnt; l++){
					localDivisor = (int)(size[1]/size[0]);
					for(j = cX + divs[0]; j < divs[1] && j < cX+cXW; j+=localDivisor){
						for(k = tid*2+cZ; k < divs[1] && k < cZW; k+=num_threads*localDivisor){
							char curr = 0;
							curr = arr[i][j][k];
							if(curr == 2){
								if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
									Matrix translation = MatrixTranslate(j*size[1]/localDivisor, -i*size[1]/localDivisor + WORLD_H*size[1]/localDivisor, k*size[1]/localDivisor);
									localM[1][localmIs[1]] = MatrixMultiply(rotation, translation);
									localmIs[1]++;
									
								}
							}
							
						}
					}
					for(j = cX; j < divs[0] && j < cX+cXW; j+=localDivisor){
						for(k = tid*2+cZ+divs[0]; k < divs[1] && k < cZW; k+=num_threads*localDivisor){
							char curr = 0;
							curr = arr[i][j][k];
							if(curr == 2){
								if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
									Matrix translation = MatrixTranslate(j*size[1]/localDivisor, -i*size[1]/localDivisor + WORLD_H*size[1]/localDivisor, k*size[1]/localDivisor);
									localM[1][localmIs[1]] = MatrixMultiply(rotation, translation);
									localmIs[1]++;
									
								}
							}
							
						}
					}
				//}
				
				//divs[2] LOCATIONS
				
				localDivisor = (int)(size[2]/size[0]);
				
				for(j = cX + divs[1]; j < divs[2] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ; k < divs[2] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[2]/localDivisor, -i*size[2]/localDivisor + WORLD_H*size[2]/localDivisor, k*size[2]/localDivisor);
								localM[2][localmIs[2]] = MatrixMultiply(rotation, translation);
								localmIs[2]++;
								
							}
						}
						
					}
				}
				for(j = cX; j < divs[1] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ+divs[1]; k < divs[2] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[2]/localDivisor, -i*size[2]/localDivisor + WORLD_H*size[2]/localDivisor, k*size[2]/localDivisor);
								localM[2][localmIs[2]] = MatrixMultiply(rotation, translation);
								localmIs[2]++;
								
							}
						}
						
					}
				}
				
				
				//divs[3] LOCATIONS
				localDivisor = (int)(size[3]/size[0]);
				for(j = cX + divs[2]; j < divs[3] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ; k < divs[3] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[3]/localDivisor, -i*size[3]/localDivisor + WORLD_H*size[3]/localDivisor, k*size[3]/localDivisor);
								localM[3][localmIs[3]] = MatrixMultiply(rotation, translation);
								localmIs[3]++;
								
							}
						}
						
					}
				}
				for(j = cX; j < divs[2] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ+divs[2]; k < divs[3] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[3]/localDivisor, -i*size[3]/localDivisor + WORLD_H*size[3]/localDivisor, k*size[3]/localDivisor);
								localM[3][localmIs[3]] = MatrixMultiply(rotation, translation);
								localmIs[3]++;
								
							}
						}
						
					}
				}

				//divs[4] LOCATIONS
				localDivisor = (int)(size[4]/size[0]);
				for(j = cX + divs[3]; j < divs[4] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ; k < divs[4] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[4]/localDivisor, -i*size[4]/localDivisor + WORLD_H*size[4]/localDivisor, k*size[4]/localDivisor);
								localM[4][localmIs[4]] = MatrixMultiply(rotation, translation);
								
								localmIs[4]++;
								
							}
						}
						
					}
				}
				for(j = cX; j < divs[3] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ+divs[3]; k < divs[4] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[4]/localDivisor, -i*size[4]/localDivisor + WORLD_H*size[4]/localDivisor, k*size[4]/localDivisor);
								localM[4][localmIs[4]] = MatrixMultiply(rotation, translation);
								localmIs[4]++;
								
							}
						}
						
					}
				}
				
				//divs[5] LOCATIONS
				localDivisor = (int)(size[5]/size[0]);
				for(j = cX + divs[4]; j < divs[5] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ; k < divs[5] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[5]/localDivisor, -i*size[5]/localDivisor + WORLD_H*size[5]/localDivisor, k*size[5]/localDivisor);
								localM[5][localmIs[5]] = MatrixMultiply(rotation, translation);
								localmIs[5]++;
								
							}
						}
						
					}
				}
				for(j = cX; j < divs[4] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ+divs[4]; k < divs[5] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[5]/localDivisor, -i*size[5]/localDivisor + WORLD_H*size[5]/localDivisor, k*size[5]/localDivisor);
								localM[5][localmIs[5]] = MatrixMultiply(rotation, translation);
								localmIs[5]++;
								
							}
						}
						
					}
				}

				/*FINAL ONE*/
				localDivisor = (int)(size[6]/size[0]);
				for(j = cX + divs[5]; j < divs[6] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ; k < divs[6] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[6]/localDivisor, -i*size[6]/localDivisor + WORLD_H*size[6]/localDivisor, k*size[6]/localDivisor);
								localM[6][localmIs[6]] = MatrixMultiply(rotation, translation);
								localmIs[6]++;
								
							}
						}
						
					}
				}
				for(j = cX; j < divs[5] && j < cX+cXW; j+=localDivisor){
					for(k = tid*localDivisor+cZ+divs[5]; k < divs[6] && k < cZW; k+=num_threads*localDivisor){
						char curr = 0;
						curr = arr[i][j][k];
						if(curr == 2){
							if(!(i-1 >= 0 && arr[i-1][j][k] == 2)){
								Matrix translation = MatrixTranslate(j*size[6]/localDivisor, -i*size[6]/localDivisor + WORLD_H*size[6]/localDivisor, k*size[6]/localDivisor);
								localM[6][localmIs[6]] = MatrixMultiply(rotation, translation);
								localmIs[6]++;
								
							}
						}
						
					}
				}
				
				
			}
			
			//#pragma omp barrier
			#pragma omp critical 
			{
				
				for(int j = 0; j < 7; j++){
					for(int i = 0; i < localmIs[j]; i++){
						m[j][mIs[j]] = localM[j][i];
						mIs[j]++;
					}
				}
				
			}
			
			#pragma omp barrier
			{
				for(int i = 0; i < 7; i++){
					RL_FREE(localM[i]);
				}
			
				RL_FREE(localM);
			}
			

		}
		//printf("%d\n", mI);
		//DrawMeshInstanced(cube, matDefault, m, mI);
		for(int i = 0; i < 7; i++){
			mCounts[i] = mIs[i];
		}
		//return mI;
		//RL_FREE(m);
		//free(m);
	
}


int testAdj(int i, int j, int k, int w, int h, int d, char ***arr, int moves, int type){
	int options = moves;
	int dir = randRange(options-1);
	int placed = 0;
	int sDir = -1;
	while(placed != 1){
		//Fail condition: OOB or all options have been exhausted
		if(dir == sDir || i+1 >= h){
			placed = 1;
			return 0;
			break;
		}//Attempt right
		if(dir == 0){
			//Element into air
			if(j-1 >= 0 && arr[i+1][j-1][k] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j-1][k] = type;
				placed = 1;
				return 1;
			} // Sand into water
			else if(j-1 >= 0 && arr[i+1][j-1][k] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j-1][k] = type;
				placed = 1;
				return 1;
			}
			else if(j-1 < 0){
				placed = 1;
			}
		}//Attempt Left
		if(dir == 1){
			//Element into air
			if(j+1 < w && arr[i+1][j+1][k] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j+1][k] = type;
				placed = 1;
				return 1;
			} //Sand into water
			else if(j+1 < w && arr[i+1][j+1][k] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j+1][k] = type;
				placed = 1;
				return 1;
			}
			else if(j+1 >= w){
				placed = 1;
			}
		}
		//Attempt backwards
		if(dir == 2){
			//Element into air
			if(k-1 >= 0 && arr[i+1][j][k-1] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j][k-1] = type;
				placed = 1;
				return 1;
			} // Sand into water
			else if(k-1 >= 0 && arr[i+1][j][k-1] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j][k-1] = type;
				placed = 1;
				return 1;
			}
			else if(k-1 < 0){
				placed = 1;
			}
		}
		//Attempt forwards
		if(dir == 3){
			//Element into air
			if(k+1 < d && arr[i+1][j][k+1] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j][k+1] = type;
				placed = 1;
				return 1;
			} //Sand into water
			else if(k+1 < d && arr[i+1][j][k+1] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j][k+1] = type;
				placed = 1;
				return 1;
			}
			else if(k+1 >= d){
				placed = 1;
			}
		}
		
		//If current direction fails attempt next direction
		if(sDir == -1)
			sDir = dir;
		dir++;
		if(dir >= options){
			dir = 0;
		}
		
	}
	return 0;
}

void updateGrid(int cX, int cY, int cZ, int w, int h, int d, char ***arr, char ***chunks){
	int i, j, k, v;
	int updated = 0;
	
	for(i = cY*CHUNK_SIZE+CHUNK_SIZE-1; i >= cY*CHUNK_SIZE; i--){
		for(j = cX*CHUNK_SIZE; j < cX*CHUNK_SIZE+CHUNK_SIZE; j++){
			for(k = cZ*CHUNK_SIZE; k < cZ*CHUNK_SIZE+CHUNK_SIZE; k++){
				//Sand
				if(arr[i][j][k] == 2){
					//Downward into air
					if(i+1 < h && arr[i+1][j][k] == 0){
						arr[i][j][k] = 0;
						arr[i+1][j][k] = 2;
						updated = 1;
						

					} //Downward into water
					else if(i+1 < h && arr[i+1][j][k] == 3){
						arr[i][j][k]=3;
						arr[i+1][j][k] = 2;
						updated = 1;
						
					} //Diagonal search
					else{
						v = testAdj(i, j, k, w, h, d, arr, 4, 2);
						if(v == 1){
							updated = 1;
							
						}
						
					}
				}//Water 

				/*
				else if(arr[i][j] == 3){
					//Downward into air
					if(i+1 < h && arr[i+1][j] == 0){
						arr[i][j] = 0;
						arr[i+1][j] = 3;
						updated = 1;
					}
					else {
						//Attempt diagonal motion
						v = testAdj(i, j, w, h, arr, 2, 3);
						if(v == 0){
							//If diagonal fails attempt left/right motion
							if(j+1 < w && arr[i][j+1] == 0){
								arr[i][j] = 0;
								arr[i][j+1] = 3;
								updated = 1;
							}
							else if(j-1 >= 0 && arr[i][j-1] == 0){
								arr[i][j] = 0;
								arr[i][j-1] = 3;
								updated = 1;
							}
						}
						else {
							updated = 1;
						}
					}
				}
				*/


			}
		}
	}

	if(updated == 1){
		//printf("updating\n");
		
		for(i = cY-1; i <= cY+1; i++){
			for(j = cX-1; j <= cX+1; j++){
				for(k = cZ-1; k <= cZ+1; k++){
					//printf("%d\n", WORLD_W/CHUNK_SIZE);
					if(i >= 0 && i < WORLD_H/CHUNK_SIZE && j >= 0 && j < WORLD_W/CHUNK_SIZE && k>=0 && k < WORLD_Z/CHUNK_SIZE){
						//if(chunks[i][j] != 1){	
						chunks[i][j][k] = 1;
						//}
					}	
				}
			}
		}
		
			
	}
	else {
		
		chunks[cY][cX][cZ] = 0;
		
	}
}


//Random brush
void rainBrush(char ***grid, char ***chunks){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				r = randRange(155);
				if(r == 1){
					grid[y][x][z] = 2;
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
				}
				else {
					grid[y][x][z] = 0;
				}
			}
		}
	}
}

//Random brush
void dustBrush(char ***grid, char ***chunks){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				
				r = randRange(2);
				if(r == 1 && y > WORLD_H-16){
					grid[y][x][z] = 2;
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
				}
				else {
					grid[y][x][z] = 0;
				}
			}
		}
	}
}



int main(void){
	int brushMode = 1;
	const int screenWidth = 1280;
	const int screenHeight = 720;
	int offsetX = 100;
	int offsetY = 20;
	float blockSizes[7];
	/*
	for(int i = 0; i < 6; i++){
		blockSizes[i] = 0.025f * (2*(i+1));
	}
	*/
	blockSizes[0] = 0.025f;
	blockSizes[1] = 0.05f;
	blockSizes[2] = 0.1f;
	blockSizes[3] = 0.2f;
	blockSizes[4] = 0.4f;
	blockSizes[5] = 1.0f;
	blockSizes[6] = 100.0f;
	int chunkSize = 50;

	//char grid[WORLD_H][WORLD_W][WORLD_Z] = {0};
	
	char ***grid = calloc(WORLD_H, sizeof(char**));
	for (int i = 0; i < WORLD_H; i++) {
		
    	grid[i] = calloc(WORLD_W , sizeof(char*));
	}
	for (int i = 0; i < WORLD_H; i++) {
		for(int j = 0; j < WORLD_W; j++){
    		grid[i][j] = calloc(WORLD_Z , sizeof(char));
			if(grid[i][j] == NULL){
				printf("Failed memory allocation (grid)\n");
			}
		}
	}

	//char chunks[WORLD_H/CHUNK_SIZE][WORLD_W/CHUNK_SIZE][WORLD_Z/CHUNK_SIZE] = {0};
	char ***chunks = calloc(WORLD_H/CHUNK_SIZE , sizeof(char**));
	for (int i = 0; i < WORLD_H/CHUNK_SIZE; i++) {
		
    	chunks[i] = calloc(WORLD_W/CHUNK_SIZE , sizeof(char*));
	}
	for (int i = 0; i < WORLD_H/CHUNK_SIZE; i++) {
		for(int j = 0; j < WORLD_W/CHUNK_SIZE; j++){
    		chunks[i][j] = calloc(WORLD_Z/CHUNK_SIZE , sizeof(char));
			if(chunks[i][j] == NULL){
				printf("Failed memory allocation (chunks)\n");
			}
		}
	}
	
	//Mesh data array
	Matrix **sandMeshData = RL_CALLOC(6, sizeof(Matrix*));
	//Matrix *sandMeshData = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z, sizeof(Matrix));
	for(int i = 0; i < 6; i++){
		sandMeshData[i] = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 400, sizeof(Matrix));
		if(sandMeshData[i] == NULL){
			printf("Failed memory allocation (mesh)\n");
			printf("%d\n", i);
		}
	}

	int mCounts[7] = {0};

	int divCnt = 7;
	int divs[7] = {300, 400, 500, 600, 700, 1300, 0};
	
	char f[4];
	int k, l;
	int init_update = 1;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f }; // Camera position
    camera.target = (Vector3){ 10.0f, 2.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	//Mesh cube = GenMeshPlane(blockSize, blockSize, 1, 1);
	Mesh cube = GenMeshCube(blockSizes[0], blockSizes[0], blockSizes[0]);
	Mesh cube2 = GenMeshCube(blockSizes[1], blockSizes[0], blockSizes[1]);
	Mesh cube3 = GenMeshCube(blockSizes[2], blockSizes[0], blockSizes[2]);
	Mesh cube4 = GenMeshCube(blockSizes[3], blockSizes[0], blockSizes[3]);
	Mesh cube5 = GenMeshCube(blockSizes[4], blockSizes[0], blockSizes[4]);
	Mesh cube6 = GenMeshCube(blockSizes[5], blockSizes[0], blockSizes[5]);
	Mesh cube7 = GenMeshCube(blockSizes[6], blockSizes[0], blockSizes[6]);

	// Load lighting shader
    Shader shader = LoadShader(TextFormat("resources/lighting_instancing.vs", GLSL_VERSION),
                               TextFormat("resources/lighting.fs", GLSL_VERSION));
    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create one light
    CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 50.0f, 50.0f, 0.0f }, Vector3Zero(), WHITE, shader);
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 0.0f, 50.0f, 50.0f }, Vector3Zero(), WHITE, shader);


	
    Texture texture = LoadTexture("resources/sandtexture2.png");
    

    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader;
	matDefault.maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    //matDefault.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;


	//Material matDefault = LoadMaterialDefault();
	//matDefault.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;


    

	dustBrush(grid, chunks);
	//grid[0][0][0] = 2;
	//grid[0][100][0] = 2;

	calcMesh3D(blockSizes, grid, 0, 0, 0, WORLD_W, WORLD_H, WORLD_Z, cube, matDefault, sandMeshData, mCounts, divs, divCnt);	

	while(!WindowShouldClose()){

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		
		BeginDrawing();
		
		if(IsKeyDown(49)){
			brushMode = 1;
		}
		if(IsKeyDown(50)){
			brushMode = 2;
		}
		if(IsKeyDown(51)){
			brushMode = 3;
		}
		
		ClearBackground(WHITE);
		

		//3D Drawing Chunks
		BeginMode3D(camera);

            DrawCube((Vector3){0, 0, 0}, 2.0f, 2.0f, 2.0f, RED);
			DrawCube((Vector3){3, 0, 0}, 1.0f, 1.0f, 0.5f, BLUE);
            DrawGrid(50, 1.0f);
			//DrawMeshInstanced(cube, matDefault, transforms, 1000);

		#pragma omp parallel
		{
			int t = omp_get_thread_num();
			int tcnt = omp_get_num_threads();
			for(int u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(int v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					for(int w = t; w < WORLD_Z/CHUNK_SIZE; w+=tcnt){
						if(chunks[u][v][w] == 1){
							//DrawCube((Vector3){v * CHUNK_SIZE * 0.2 + (CHUNK_SIZE * 0.2 * 0.5), u  * chunkSize * 0.4 + (CHUNK_SIZE * 0.2 * 0.5), w*chunkSize*0.2 + (CHUNK_SIZE * 0.2 * 0.5)}, CHUNK_SIZE * 0.2, CHUNK_SIZE * 0.2, CHUNK_SIZE * 0.2, (Color){255, 0, 0, 30});
							updateGrid(v, u, w, WORLD_W, WORLD_H, WORLD_Z, grid, chunks);
							
							init_update = 1;
							
						}
							
					}
							
				}
			}
			
				
		}
		if(init_update == 1){
			calcMesh3D(blockSizes, grid, 0, 0, 0, WORLD_W, WORLD_H, WORLD_Z, cube, matDefault, sandMeshData, mCounts, divs, divCnt);
			init_update = 0;
		}
		DrawMeshInstanced(cube, matDefault, sandMeshData[0], mCounts[0]);
		DrawMeshInstanced(cube2, matDefault, sandMeshData[1], mCounts[1]);
		DrawMeshInstanced(cube3, matDefault, sandMeshData[2], mCounts[2]);
		DrawMeshInstanced(cube4, matDefault, sandMeshData[3], mCounts[3]);
		DrawMeshInstanced(cube5, matDefault, sandMeshData[4], mCounts[4]);
		DrawMeshInstanced(cube6, matDefault, sandMeshData[5], mCounts[5]);
		DrawMeshInstanced(cube6, matDefault, sandMeshData[6], mCounts[6]);
		
		//DrawMeshInstanced(cube, matDefault, sandMeshData[1], 1);
        EndMode3D();

		//printf("%d\n", grid[0][0][0]);
		
		if(brushMode == 1){
			DrawText("Block Brush", 10, 10, 10, RED);
		}
		if(brushMode == 2){
			DrawText("Sand Brush", 10, 10, 10, RED);
		}
		if(brushMode == 3){
			DrawText("Water Brush", 10, 10, 10, RED);
		}
		

		//Display FPS
		sprintf(f, "%d", GetFPS());
		DrawText(f, 10, 20, 10, RED);
		
		EndDrawing();
	}
	RL_FREE(sandMeshData);

	printf("%s%d%s%d%s%d%s%d%s%d%s%d\n", "0:", mCounts[0], "1:", mCounts[1], "2:", mCounts[2], "3:", mCounts[3], "4:", mCounts[4], "5:", mCounts[5]);

	return 0;
}
