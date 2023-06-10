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
#define WORLD_W 1000
#define WORLD_Z 1000
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

void refreshMeshBuffer(Matrix *m, int *mCounts){
	
	//free(m);
	/*Try replacing this with a for loop that sets m to be empty
	
	*/
    /*
	for(int i = 0; i < mCounts[0]; i++){
		m[mCounts[0]] = 0;
	}
	*/

	/*
	m = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 60, sizeof(Matrix));
	
	if(m == NULL){
		printf("Failed memory allocation (refresh)\n");
	}
	*/
	
	mCounts[0] = 0;
	
}
int fHeight(char ***arr, int j, int k){
	int rVal = WORLD_H-1;
	
	while(rVal > 0 && arr[rVal][j][k] == 4){
		
		rVal--;
	}
	
	return WORLD_H-rVal;
}

static Mesh calcMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density){
		
		Mesh mesh = {0};
		
		mesh.triangleCount = cXW*cZW*2;
		mesh.vertexCount = mesh.triangleCount*3;
		mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
   		mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
   	 	mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float)); 

		int j, k = 0;
		int a = 0;
		int b = 0;
		int i = 0;
		
		for(j = cX;  j < cX+cXW; j+=1*density){
			for(k = cZ+j%density; k < cZ+cZW; k+=1*density){
				int sA = 0;
				int sB = 0;
				int ll = fHeight(arr, j, k);
				int lr = fHeight(arr, j, k+1);
				int ul = fHeight(arr, j+1, k);
				int ur = fHeight(arr, j+1, k+1);

				float delta = abs((ul+ur)/2 - (ll+lr)/2);
				float shiftX = 0;
				float shiftY = 0;

				if(delta == 1){
					shiftX = 0.5f;
					shiftY = -0.5f;
				}
				if(delta == 2){
					shiftX = 0.5f;
					shiftY = 0.0f;
				}
				//0, 0
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				
				mesh.vertices[a+2] = k * size;
				mesh.normals[a] = 0;
				mesh.normals[a+1] = 1;
				mesh.normals[a+2] = 0;
				mesh.texcoords[b] = 0 + shiftX;
				mesh.texcoords[b+1] = 0.5 + shiftY;
				b+=2;
				a+=3;
				
				//0, 1
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = lr * size;
				mesh.vertices[a+2] = (k+1) * size;
				mesh.normals[a] = 0;
    			mesh.normals[a+1] = 1;
    			mesh.normals[a+2] = 0;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 1 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1)*size;
				mesh.normals[a] = 0;
    			mesh.normals[a+1] = 1;
    			mesh.normals[a+2] = 0;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 1.0f + shiftY;
				b+=2;
				a+=3;
					
				//1, 0
				i = fHeight(arr, j+1, k);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				mesh.normals[a] = 0;
    			mesh.normals[a+1] = 1;
    			mesh.normals[a+2] = 0;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 0.5f + shiftY;
				b+=2;
				a+=3;

				

				//0, 0
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				mesh.vertices[a+2] = k * size;
				mesh.normals[a] = 0;
    			mesh.normals[a+1] = 1;
    			mesh.normals[a+2] = 0;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 0.5 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1) * size;
				mesh.normals[a] = 0;
    			mesh.normals[a+1] = 1;
    			mesh.normals[a+2] = 0;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 1.0f + shiftY;
				b+=2;
				a+=3;
				
					
			}
		}
		
		
		UploadMesh(&mesh, true);
		return mesh;
	
}

void recalculateMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density, Mesh mesh){
		
		int j, k = 0;
		int a = 0;
		int b = 0;
		int i = 0;
		
		for(j = cX;  j < cX+cXW; j+=1*density){
			for(k = cZ+j%density; k < cZ+cZW; k+=1*density){
				int sA = 0;
				int sB = 0;
				int ll = fHeight(arr, j, k);
				int lr = fHeight(arr, j, k+1);
				int ul = fHeight(arr, j+1, k);
				int ur = fHeight(arr, j+1, k+1);

				float delta = abs((ul+ur)/2 - (ll+lr)/2);
				float shiftX = 0;
				float shiftY = 0;

				if(delta == 1){
					shiftX = 0.5f;
					shiftY = -0.5f;
				}
				if(delta == 2){
					shiftX = 0.5f;
					shiftY = 0.0f;
				}
				//0, 0
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				mesh.vertices[a+2] = k * size;
				a+=3;
				
				//0, 1
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = lr * size;
				mesh.vertices[a+2] = (k+1) * size;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1)*size;
				a+=3;
					
				//1, 0
				i = fHeight(arr, j+1, k);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				a+=3;

				

				//0, 0
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				mesh.vertices[a+2] = k * size;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1) * size;
				
				a+=3;
				
					
			}
		}
		UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);
		
	
}

int rayBounds(char ***arr, int i, int j, int k, Vector3 cameraPos, float size){
	
	//Convert camera position to grid values
	Vector3 cameraBlockPos = (Vector3){(cameraPos.x/size)-1, WORLD_H-(cameraPos.y/size), round(cameraPos.z/size)};
	//Vector3 unnormalizedDir = Vector3Subtract((Vector3){cameraBlockPos.x, cameraBlockPos.y, cameraBlockPos.z}, (Vector3){j, i, k});
	//cameraBlockPos = Vector3Add(cameraBlockPos, Vector3Scale(unnormalizedDir, 20));
	//printf("%f%s%f%s%f\n", cameraBlockPos.x, ",", cameraBlockPos.y, ",", cameraBlockPos.z);
	int monitor = 0;
	if(i >= WORLD_H-2 && j == 0 && k == 0){
		printf("%d%s%d%s%d\n", i, ",", j, ",", k);
		monitor = 1;
	}
	Vector3 dir = Vector3Normalize(Vector3Subtract((Vector3){cameraBlockPos.x, cameraBlockPos.y, cameraBlockPos.z}, (Vector3){j, i, k}));
	Vector3 nxt = Vector3Add((Vector3){j, i, k}, dir);
	i = (int)round(nxt.y);
	j = (int)round(nxt.x);
	k = (int)round(nxt.z);
	int blockCounts = 0;

	if(monitor == 1){
		//printf("%d%s%d%s%d\n", i, ",", j, ",", k);
		//printf("\n");
	}

	while(i >= 0 && i < WORLD_H && j >= 0 && j < WORLD_W && k >= 0 && k <= WORLD_Z){
		
		if(arr[i][j][k] == 2){
			blockCounts++;
		}
	    nxt = Vector3Add((Vector3){nxt.x, nxt.y, nxt.z}, dir);
		i = (int)round(nxt.y);
		j = (int)round(nxt.x);
		k = (int)round(nxt.z);
		if(monitor == 1){
		//printf("%d%s%d%s%d\n", i, ",", j, ",", k);
		//printf("\n");
		}
		//printf("%d%s%d%s%d\n", i, ",", j, ",", k);
		if(blockCounts > 2){
			return 0;
		}
		
	}
	//printf("\n");
	
	return 1;
}

int evalParticles3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, Matrix *m, int *mCounts, int density, Vector3 cameraPos){
		
		int mIs = 0;
		
		Vector3 axis = (Vector3){ 0, 0, 0 };
		float angle = 0;
		Matrix rotation = MatrixRotate(axis, angle);
		
		//Matrix *localM = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(Matrix));
		
		int i, j, k;
		for(i = WORLD_H; i >= 0; i-=1){
		
			if(i == WORLD_H)
				continue;
			for(j = cX;  j < cX+cXW; j+=1*density){
				for(k = cZ; k < cZ+cZW; k+=1*density){
					
					char curr = 0;
					int res = 0;
					int withinBounds = (i-1 >= 0 && i+1 < WORLD_H && j-1 >= cX && j+1 < cX+cXW && k-1 > cZ && k+1 < cZ+cZW);
					
					if(arr[i][j][k] == 2 && rayBounds(arr, i, j, k, cameraPos, size) == 1 && !(withinBounds && arr[i-1][j][k] == 2 && arr[i+1][j][k] == 2 && arr[i][j-1][k] == 2 && arr[i][j+1][k] == 2 && arr[i][j][k-1] == 2 && arr[i][j][k+1] == 2)){
						Matrix translation = MatrixTranslate(j*size, -i*size + WORLD_H*size, k*size);
						m[mCounts[0]+mIs] = MatrixMultiply(rotation, translation);
						mIs++;
					}
				}
			}
			
		}
		
		
		//for(int i = 0; i < mIs; i++){
		//	m[mCounts[0]+i] = localM[i];
		//}
		
		
		
		//RL_FREE(localM);
			
		mCounts[0]+=mIs;
	
}


void evalMesh(char ***arr, int cX, int cY, int cZ, int cXW, int cYW, int cZW){
		int total = 0;
		int i, j, k;
		
		for(i = cY+cYW; i >= cY; i-=1){
		
			if(i == WORLD_H)
				continue;
			for(j = cX;  j < cX+cXW; j+=1){
				for(k = cZ; k < cZW; k+=3){
					
					char curr = 0;
					curr = arr[i][j][k];

					if(arr[i][j][k] == 2 && !(i-1 >= 0 && arr[i-1][j][k] == 2) && arr[i][j][k+1] == 2 && arr[i-1][j][k+1] != 2 && arr[i][j+1][k] == 2 && arr[i-1][j+1][k] != 2 && arr[i][j+1][k+1] == 2 && arr[i-1][j+1][k+1] != 2){
						total++;
					}
					
				}
			}
			
		}
		
		printf("%s%d\n", "Totals: ", total);
	
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
						else {
							arr[i][j][k] = 4;
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
				
				r = randRange(1);
				if(r == 0 && y > WORLD_H-26 && y < WORLD_H){
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

//Random local brush
void localBrush(char ***grid, char ***chunks){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < 100; x++){
			for(z = 0; z < 100; z++){
				
				r = randRange(2);
				if(r == 1 && y > WORLD_H-66 && y < WORLD_H-40){
					grid[y][x][z] = 2;
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
				}
				else if(grid[y][x][z] != 4){
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
	float blockSizes = 0.025f;
	int chunkSize = 50;

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

	Model **meshArr = calloc(WORLD_W/CHUNK_SIZE, sizeof(Model*));
	for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
		meshArr[i] = calloc(WORLD_Z/CHUNK_SIZE, sizeof(Model));
	}
	char **occupiedMesh = calloc(WORLD_W/CHUNK_SIZE, sizeof(char*));
	for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
		occupiedMesh[i] = calloc(WORLD_Z/CHUNK_SIZE, sizeof(char));
	}


	
	//Mesh data array
	Matrix *sandMeshData = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 60, sizeof(Matrix));
	if(sandMeshData == NULL){
		printf("Failed memory allocation (mesh)\n");
	}
	

	int mCounts[1];
	mCounts[0] = 0;

	char f[4];
	int k, l;
	int init_update = 1;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ -1.0f, 2.0f, -1.0f }; // Camera position
    camera.target = (Vector3){ 10.0f, 2.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Mesh cube = GenMeshCube(blockSizes, blockSizes, blockSizes);
	
	// Load lighting shader
    Shader shader = LoadShader(TextFormat("resources/lighting.vs"),
                               TextFormat("resources/lighting.fs"));
    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

	Shader shader2 = LoadShader(TextFormat("resources/lighting_instancing.vs"),
                               TextFormat("resources/lighting.fs"));
    // Get shader locations
    shader2.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader2, "mvp");
    shader2.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader2, "viewPos");
    shader2.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader2, "instanceTransform");

    // Set shader value: ambient light level
	int ambientLoc2 = GetShaderLocation(shader2, "ambient");
    SetShaderValue(shader2, ambientLoc2, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);


    // Create one light
	 //Light lights[MAX_LIGHTS] = { 0 };
    //lights[0] = 
	//CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 50.0f, 50.0f, 50.0f }, Vector3Zero(), WHITE, shader);
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 50.0f, 50.0f, 0.0f }, Vector3Zero(), WHITE, shader);
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 0.0f, 50.0f, 50.0f }, Vector3Zero(), WHITE, shader2);
	//CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 0.0f, 50.0f, 50.0f }, Vector3Zero(), WHITE, shader);

    Texture texture = LoadTexture("resources/sandtexture3.png");
    

    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader2;
	matDefault.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;

	Model m;
    

	dustBrush(grid, chunks);

	//calcMesh3D(blockSizes, grid, 0, 0, 0, 100, 200, 100, sandMeshData, mCounts, 2);
	int fCalc = 0;	
	
	while(!WindowShouldClose()){

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader2, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

		if(!fCalc){
			evalParticles3D(blockSizes, grid, 0, 0, 300, 300, sandMeshData, mCounts, 1, (Vector3){camera.position.x, camera.position.y, camera.position.z});
			fCalc = 1;
		}

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


			DrawLine3D((Vector3){50, 50, 50}, (Vector3){0, 0, 0}, RED);
			DrawCube((Vector3){3, 0, 0}, 0.2f, 0.2f, 0.2f, BLUE);
			DrawCube((Vector3){0, 0, 3}, 0.2f, 0.2f, 0.2f, GREEN);
            DrawGrid(50, 1.0f);
			
			Vector3 axis = (Vector3){ 0, 0, 0 };
		//#pragma omp parallel
		{
			//int t = omp_get_thread_num();
			//int tcnt = omp_get_num_threads();
			for(int u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(int v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					for(int w = 0; w < WORLD_Z/CHUNK_SIZE; w+=1){
						if(chunks[u][v][w] == 1){
							//DrawCube((Vector3){v * CHUNK_SIZE * chunkSize + (CHUNK_SIZE * chunkSize * 0.5), u  * chunkSize * 0.4 + (CHUNK_SIZE * chunkSize * 0.5), w*chunkSize*0.2 + (CHUNK_SIZE * chunkSize * 0.5)}, CHUNK_SIZE * chunkSize, CHUNK_SIZE * chunkSize, CHUNK_SIZE * chunkSize, (Color){255, 0, 0, 30});
							updateGrid(v, u, w, WORLD_W, WORLD_H, WORLD_Z, grid, chunks);
							
							if(occupiedMesh[v][w] == 0){
								//If mesh does not exist create the mesh
								//printf("called upon\n");
								occupiedMesh[v][w] = 1;
								meshArr[v][w] = LoadModelFromMesh(calcMesh3D(blockSizes, grid, v*chunkSize, w*chunkSize, CHUNK_SIZE, CHUNK_SIZE, 1));
								meshArr[v][w].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
								meshArr[v][w].materials[0].shader = shader;
							}
							else if(occupiedMesh[v][w] == 1){
								//Run a mesh update
								//printf("Mesh update: %d%s%d\n", v, ",", w);
								//recalculateMesh3D(blockSizes, grid, v*chunkSize, w*chunkSize, CHUNK_SIZE, CHUNK_SIZE, 1, meshArr[v][w].meshes[0]);
							}
							init_update = 1;
							
						}
							
					}
							
				}
			}
			
				
		}
		
		if(init_update == 1){
			refreshMeshBuffer(sandMeshData, mCounts);
			evalParticles3D(blockSizes, grid, 0, 0, 300, 300, sandMeshData, mCounts, 1, (Vector3){camera.position.x, camera.position.y, camera.position.z});
			//m = LoadModelFromMesh(calcMesh3D(blockSizes, grid, 0, 0, 800, 800, 1));
			init_update = 0;
		}
		
		
		for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
			for(int j = 0; j < WORLD_Z/CHUNK_SIZE; j++){
				//m = LoadModelFromMesh(meshArr[i][j]);
				//meshArr[i][j].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
				//meshArr[i][j].materials[0].shader = shader;
		
				DrawModel(meshArr[i][j], (Vector3){i*chunkSize*blockSizes, 0, j*chunkSize*blockSizes}, 1, WHITE);
			}
		}
		
		DrawMeshInstanced(cube, matDefault, sandMeshData, mCounts[0]);
		
		if(IsKeyPressed(32)){
			printf("%s\n", "Calling local brush");
			//localBrush(grid, chunks);
			grid[150][30][150] = 2;
			chunks[(int)floor(150 / CHUNK_SIZE)][(int)floor(30/CHUNK_SIZE)][(int)floor(150/CHUNK_SIZE)] = 1;
			//evalMesh(grid, 0, 0, 0, 300, 200, 300);	
		}
		
		
        EndMode3D();

		
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

	return 0;
}
