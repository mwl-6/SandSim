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
 * 300 x 500 
 * Chunk size 50
 * */
#define WORLD_H 300
#define WORLD_W 500
#define WORLD_Z 50
#define CHUNK_SIZE 50

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif



int randRange(int range){
	return (rand() % (range + 1));
}



void drawGrid3D(float size, int offsetX, int offsetY, char ***arr, int cX, int cY, int cZ, Mesh cube, Material matDefault){

		int mI = 0;
		
		Matrix *m = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(Matrix));
		
		Vector3 axis = (Vector3){ 0, 0, 0 };
		float angle = 0;
		Matrix rotation = MatrixRotate(axis, angle);

		
		//printf("%d\n", tid);
		
		#pragma omp parallel shared (arr)
		{
			int localmI = 0;
			Matrix *localM = (Matrix*)RL_CALLOC(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 7, sizeof(Matrix));
			
			
			int tid = omp_get_thread_num();
			int num_threads = omp_get_num_threads();
			int localI;
			
			//printf("%d\n", tid);
			//printf("%d\n", num_threads);
			int i, j, k;
			for(i = cY*CHUNK_SIZE+CHUNK_SIZE; i >= cY*CHUNK_SIZE; i-=1){
			
				if(i == WORLD_H)
					continue;
				for(j = cX*CHUNK_SIZE; j < cX*CHUNK_SIZE + CHUNK_SIZE && j < WORLD_W; j++){
					for(k = cZ * CHUNK_SIZE + tid; k < cZ*CHUNK_SIZE + CHUNK_SIZE && k < WORLD_Z; k+=num_threads){
						
						char curr = 0;
						//printf("%d%s%d%s%d\n", i, ",", j, ",", k);
						
						curr = arr[i][j][k];

						if(curr == 1){
							
							//DrawCube((Vector3){j*size + offsetX, i*size + offsetY, k*size}, size, size, size, GRAY);
						}
						else if(curr == 2){
							
							Matrix translation = MatrixTranslate(j*size + offsetX, -i*size + offsetY + 300*size, k*size);
							localM[localmI] = MatrixMultiply(rotation, translation);
							localmI++;
							
							
							
							//DrawMesh(cube, matDefault, MatrixTranslate(j*size + offsetX, -i*size + offsetY + 300*size, k*size));
							//DrawCube((Vector3){j*size + offsetX, -i*size + offsetY + 300*size, k*size}, size, size, size, GREEN);
						}
						else if(curr == 3){
							
							//DrawCube((Vector3){j*size + offsetX, i*size + offsetY, k*size}, size, size, size, BLUE);
						}
						else {
							//DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, SKYBLUE);
							//DrawRectangleLines(i*size + offsetX, j*size+offsetY, size, size, BLACK);
							
						}
						
					}
				}
			}
			
			//#pragma omp barrier
			#pragma omp critical 
			{
				
				for(int i = 0; i < localmI; i++){
					m[mI] = localM[i];
					mI++;
				}
				
			}
			
			RL_FREE(localM);
			
			

		}
		//printf("%d\n", mI);
		DrawMeshInstanced(cube, matDefault, m, mI);
		
		RL_FREE(m);
		//free(m);
	
}

/*
int testAdj(int i, int j, int w, int h, char arr[][w], int moves, int type){
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
			if(j-1 >= 0 && arr[i+1][j-1] == 0){
				arr[i][j] = 0;
				arr[i+1][j-1] = type;
				placed = 1;
				return 1;
			} // Sand into water
			else if(j-1 >= 0 && arr[i+1][j-1] == 3 && type == 2){
				arr[i][j] = 3;
				arr[i+1][j-1] = type;
				placed = 1;
				return 1;
			}
			else if(j-1 < 0){
				placed = 1;
			}
		}//Attempt Left
		if(dir == 1){
			//Element into air
			if(j+1 < w && arr[i+1][j+1] == 0){
				arr[i][j] = 0;
				arr[i+1][j+1] = type;
				placed = 1;
				return 1;
			} //Sand into water
			else if(j+1 < w && arr[i+1][j+1] == 3 && type == 2){
				arr[i][j] = 3;
				arr[i+1][j+1] = type;
				placed = 1;
				return 1;
			}
			else if(j+1 >= w){
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
	return 1;
}

void updateGrid(int cX, int cY, int w, int h, char arr[][w], char chunks[][WORLD_W/CHUNK_SIZE]){
	int i, j, v;
	int updated = 0;

	for(i = cY*CHUNK_SIZE+CHUNK_SIZE-1; i >= cY*CHUNK_SIZE; i--){
		for(j = cX*CHUNK_SIZE; j < cX*CHUNK_SIZE+CHUNK_SIZE; j++){
			//Sand
			if(arr[i][j] == 2){
				//Downward into air
				if(i+1 < h && arr[i+1][j] == 0){
					arr[i][j] = 0;
					arr[i+1][j] = 2;
					updated = 1;

				} //Downward into water
				else if(i+1 < h && arr[i+1][j] == 3){
					arr[i][j]=3;
					arr[i+1][j] = 2;
					updated = 1;
				} //Diagonal search
				else{
					v = testAdj(i, j, w, h, arr, 2, 2);
					if(v == 1){
						updated = 1;
					}
					
				}
			}//Water 
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

		}
	}

	if(updated == 1){
		//printf("updating\n");
		
		for(i = cY-1; i <= cY+1; i++){
			for(j = cX-1; j <= cX+1; j++){
				//printf("%d\n", WORLD_W/CHUNK_SIZE);
				if(i >= 0 && i < WORLD_H/CHUNK_SIZE && j >= 0 && j < WORLD_W/CHUNK_SIZE){
					//if(chunks[i][j] != 1){	
					chunks[i][j] = 1;
					//}
				}	
			}
		}
		
			
	}
	else {
		chunks[cY][cX] = 0;
	}
}
*/

//Random brush
void rainBrush(char ***grid, char ***chunks){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				r = randRange(255);
				if(r == 1){
					grid[y][x][z] = 2;
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
				}
				else {
					grid[y][x][z] = 9;
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
	int blockSize = 2;
	int chunkSize = 50;

	//char grid[WORLD_H][WORLD_W][WORLD_Z] = {0};
	
	char ***grid = calloc(WORLD_H, sizeof(char**));
	for (int i = 0; i < WORLD_H; i++) {
		
    	grid[i] = calloc(WORLD_W , sizeof(char*));
	}
	for (int i = 0; i < WORLD_H; i++) {
		for(int j = 0; j < WORLD_W; j++){
    		grid[i][j] = calloc(WORLD_Z , sizeof(char));
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
		}
	}
	


	char f[4];
	int k, l;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 50.0f, 50.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Mesh cube = GenMeshCube(0.2f, 0.2f, 0.2f);


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

    // NOTE: We are assigning the intancing shader to material.shader
    // to be used on mesh drawing with DrawMeshInstanced()
    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader;
    matDefault.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;


	//Material matDefault = LoadMaterialDefault();
	//matDefault.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;


    

	rainBrush(grid, chunks);

	while(!WindowShouldClose()){

		UpdateCamera(&camera, CAMERA_THIRD_PERSON);

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

		
		{
			
			int u, v, w;
			for(u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					for(w = 0; w < WORLD_Z/CHUNK_SIZE; w++){
						if(chunks[u][v][w] == 1){
								
							//updateGrid(v, u, WORLD_W, WORLD_H, grid, chunks);
						
						}
						drawGrid3D(0.2f, -60, 0, grid, v, u, w, cube, matDefault);	
					}
							
				}
			}
			
				
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
	return 0;
}
