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
#define WORLD_W 900
#define WORLD_Z 900
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

void refreshMeshBuffer(Matrix **m, int *mCounts){
	for(int i = 0; i < 3; i++){
		free(m[i]);
		m[i] = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 10, sizeof(Matrix));
		if(m[i] == NULL){
			printf("Failed memory allocation (mesh)\n");
			printf("%d\n", i);
		}
	}

	for(int i =0; i < 3; i++){
		mCounts[i] = 0;
	}
}

int calcMesh3D(float *size, char ***arr, int cX, int cY, int cZ, int cXW, int cYW, int cZW, Matrix **m, int *mCounts, int density){
		
		int mIs[3] = {0};
		
		Vector3 axis = (Vector3){ 0, 0, 0 };
		float angle = 0;
		Matrix rotation = MatrixRotate(axis, angle);
		
		Matrix **localM = RL_CALLOC(3, sizeof(Matrix*));
		for(int i = 0; i < 3; i++){
			localM[i] = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(Matrix));
			if(localM[i] == NULL){
				printf("Failed memory allocation\n");
			}
		}
		int i, j, k;
		for(i = cY+cYW; i >= cY; i-=1){
		
			if(i == WORLD_H)
				continue;
			for(j = cX;  j < cX+cXW; j+=1*density){
				for(k = cZ+j%density; k < cZ+cZW; k+=3*density){
					
					char curr = 0;
					curr = arr[i][j][k];
					int res = 0;
					Matrix translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], k*size[0]);
					if(arr[i][j][k] == 2 && !(i-1 >= 0 && arr[i-1][j][k] == 2)){
						res += 100;
					}
					if(arr[i][j][k+1] == 2 && !(i-1 >= 0 && arr[i-1][j][k+1] == 2)){
						res+=10;
					}
					if(arr[i][j][k+2] == 2 && !(i-1 >= 0 && arr[i-1][j][k+2] == 2)){
						res+=1;
					}
					
					if(res == 111){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], (k*size[0] + (k+2)*size[0])/2.0f);
						localM[2][mIs[2]] = MatrixMultiply(rotation, translation);
						mIs[2]++;
					}
					else if(res == 110){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], (k*size[0] + (k+1)*size[0])/2.0f);
						localM[1][mIs[1]] = MatrixMultiply(rotation, translation);
						mIs[1]++;
					}
					else if(res == 11){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], ((k+1)*size[0] + (k+2)*size[0])/2.0f);
						localM[1][mIs[1]] = MatrixMultiply(rotation, translation);
						mIs[1]++;
					}
					else if(res == 101){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], k*size[0]);
						localM[0][mIs[0]] = MatrixMultiply(rotation, translation);
						mIs[0]++;
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], (k+2)*size[0]);
						localM[0][mIs[0]] = MatrixMultiply(rotation, translation);
						mIs[0]++;
					}
					else if(res == 100){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], k*size[0]);
						localM[0][mIs[0]] = MatrixMultiply(rotation, translation);
						mIs[0]++;
					}
					else if(res == 10){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], (k+1)*size[0]);
						localM[0][mIs[0]] = MatrixMultiply(rotation, translation);
						mIs[0]++;
					}
					else if(res == 1){
						translation = MatrixTranslate(j*size[0], -i*size[0] + WORLD_H*size[0], (k+2)*size[0]);
						localM[0][mIs[0]] = MatrixMultiply(rotation, translation);
						mIs[0]++;
					}
					

					
				}
			}
			
		}
		
		for(int j = 0; j < 3; j++){
			for(int i = 0; i < mIs[j]; i++){
				m[j][mCounts[j]+i] = localM[j][i];
			}
		}
		
		for(int i = 0; i < 1; i++){
			RL_FREE(localM[i]);
		}
		
			RL_FREE(localM);
			
			
		for(int i = 0; i < 3; i++){
			mCounts[i] += mIs[i];
		}
	
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
	float blockSizes[4];
	/*
	for(int i = 0; i < 6; i++){
		blockSizes[i] = 0.025f * (2*(i+1));
	}
	*/
	blockSizes[0] = 0.025f * 5;
	blockSizes[1] = 0.05f * 5;
	blockSizes[2] = 0.075f * 5;
	blockSizes[3] = 0.025f;
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
	Matrix **sandMeshData = RL_CALLOC(3, sizeof(Matrix*));
	//Matrix *sandMeshData = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z, sizeof(Matrix));
	for(int i = 0; i < 3; i++){
		sandMeshData[i] = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 10, sizeof(Matrix));
		if(sandMeshData[i] == NULL){
			printf("Failed memory allocation (mesh)\n");
			printf("%d\n", i);
		}
	}

	int mCounts[3] = {0};

	
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
	Model model = LoadModel("resources/sandv2.obj");
	Model model2 = LoadModel("resources/sand2x1.obj");
	Model model3 = LoadModel("resources/sand3x1.obj");
	
	Mesh cube = GenMeshCube(blockSizes[0], blockSizes[3], blockSizes[0]);
	//Mesh cube = GenMeshCylinder(blockSizes[0], blockSizes[3], 12);
	Mesh cube2 = GenMeshCube(blockSizes[0], blockSizes[3], blockSizes[1]);
	Mesh cube3 = GenMeshCube(blockSizes[0], blockSizes[3], blockSizes[2]);

	// Load lighting shader
    Shader shader = LoadShader(TextFormat("resources/lighting_instancing.vs", GLSL_VERSION),
                               TextFormat("resources/lighting.fs", GLSL_VERSION));
    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");


	//testModel.materials[0].shader = shader;
	
    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create one light
    CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 50.0f, 50.0f, 0.0f }, Vector3Zero(), WHITE, shader);
	//CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 0.0f, 50.0f, 50.0f }, Vector3Zero(), WHITE, shader);


	
    Texture texture = LoadTexture("resources/sandtexture2.png");
    

    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader;
	matDefault.maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    //matDefault.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;


	//Material matDefault = LoadMaterialDefault();
	//matDefault.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;


    

	dustBrush(grid, chunks);
	//grid[170][0][0] = 2;
	//grid[170][0][3] = 2;
	//grid[0][100][0] = 2;

	calcMesh3D(blockSizes, grid, 0, 0, 0, 300, 200, 300, sandMeshData, mCounts, 2);	

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

            //DrawCube((Vector3){0, 0, 0}, 2.0f, 2.0f, 2.0f, RED);
			DrawCube((Vector3){3, 0, 0}, 0.2f, 0.2f, 0.2f, BLUE);
			DrawCube((Vector3){0, 0, 3}, 0.2f, 0.2f, 0.2f, GREEN);
            DrawGrid(50, 1.0f);
			//DrawPlane((Vector3){10000.0f*blockSizes[0]/2, 0.3f, 10000.0f*blockSizes[0]/2}, (Vector2){10000.0f*blockSizes[0], 10000.0f*blockSizes[0]}, GREEN);
			//DrawMeshInstanced(cube, matDefault, transforms, 1000);
			DrawCube((Vector3){0, 0, 0}, 0.025f, 0.025f, 0.025f, (Color){0, 255, 0, 100});
			//DrawMesh(testModel.meshes[0], matDefault, MatrixTranslate(0, 0, 0));
			Vector3 axis = (Vector3){ 0, 0, 0 };
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
			refreshMeshBuffer(sandMeshData, mCounts);
			calcMesh3D(blockSizes, grid, 0, 0, 0, 100, 200, 100, sandMeshData, mCounts, 1);
			//calcMesh3D(blockSizes, grid, 100, 0, 0, 200, 200, 100, sandMeshData, mCounts, 1);
			//calcMesh3D(blockSizes, grid, 0, 0, 100, 100, 200, 200, sandMeshData, mCounts, 1);
			//calcMesh3D(blockSizes, grid, 100, 0, 100, 200, 200, 200, sandMeshData, mCounts, 1);
			//calcMesh3D(blockSizes, grid, 300, 0, 0, 200, 200, 500, sandMeshData, mCounts, 2);
			//calcMesh3D(blockSizes, grid, 0, 0, 300, 500, 200, 200, sandMeshData, mCounts, 2);
			//calcMesh3D(blockSizes, grid, 0, 0, 0, 900, 200, 900, sandMeshData, mCounts);
			init_update = 0;
		}

		
		DrawMeshInstanced(model.meshes[0], matDefault, sandMeshData[0], mCounts[0]);
		DrawMeshInstanced(model2.meshes[0], matDefault, sandMeshData[1], mCounts[1]);
		DrawMeshInstanced(model3.meshes[0], matDefault, sandMeshData[2], mCounts[2]);
		if(IsKeyPressed(32)){
			evalMesh(grid, 0, 0, 0, 300, 200, 300);	
		}
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

	printf("%s%d%s%d%s%d\n", "0:", mCounts[0], ", 1:", mCounts[1], ", 2:", mCounts[2]);
	return 0;
}
