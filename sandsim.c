#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* 
 * 300 x 500 
 * Chunk size 50
 * */
#define WORLD_H 300
#define WORLD_W 500
#define CHUNK_SIZE 50


int randRange(int range){
	return (rand() % (range + 1));
}

void drawGrid(int w, int h, int size, int offsetX, int offsetY, char arr[][w], int cX, int cY){
	int i, j;
	
	for(i = cY*CHUNK_SIZE+CHUNK_SIZE; i >= cY*CHUNK_SIZE; i--){
	//for(i = 0; i < h; i++){
		if(i == h)
			continue;
		for(j = cX*CHUNK_SIZE; j < cX*CHUNK_SIZE + CHUNK_SIZE; j++){
			if(arr[i][j] == 1){
				DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, GRAY);
			}
			else if(arr[i][j] == 2){
				DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, YELLOW);
			}
			else if(arr[i][j] == 3){
				DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, BLUE);
			}
			else {
				//DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, SKYBLUE);
				//DrawRectangleLines(i*size + offsetX, j*size+offsetY, size, size, BLACK);
				
			}
		}
	}
}

void drawGrid3D(int w, int h, float size, int offsetX, int offsetY, char arr[][w], int cX, int cY){
	int i, j;
	
	for(i = cY*CHUNK_SIZE+CHUNK_SIZE; i >= cY*CHUNK_SIZE; i--){
	//for(i = 0; i < h; i++){
		if(i == h)
			continue;
		for(j = cX*CHUNK_SIZE; j < cX*CHUNK_SIZE + CHUNK_SIZE; j++){
			if(arr[i][j] == 1){
				
				DrawCube((Vector3){j*size + offsetX, i*size + offsetY, 0.0f}, size, size, size, GRAY);
			}
			else if(arr[i][j] == 2){
				
				DrawCube((Vector3){j*size + offsetX, -i*size + offsetY + 300*size, 0.0f}, size, size, size, GREEN);
			}
			else if(arr[i][j] == 3){
				
				DrawCube((Vector3){j*size + offsetX, i*size + offsetY, 0.0f}, size, size, size, BLUE);
			}
			else {
				//DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, SKYBLUE);
				//DrawRectangleLines(i*size + offsetX, j*size+offsetY, size, size, BLACK);
				
			}
		}
	}
}

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

//Random brush
void rBrush(int x, int y, int w, int h, int r, char t, char arr[][w], char chunks[][WORLD_W/CHUNK_SIZE]){
	int i, j;
	for(i = 0; i < r; i++){
		for(j = 0; j < r; j++){
			int k = randRange(10);
			if(k == 1){
				arr[y+i][x+j] = t;
				chunks[(int)floor((y+i) / CHUNK_SIZE)][(int)floor((x+j)/CHUNK_SIZE)] = 1;
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

	char grid[WORLD_H][WORLD_W] = {0};
	char chunks[WORLD_H/CHUNK_SIZE][WORLD_W/CHUNK_SIZE] = {0};
	chunks[5][4] = 1;
	char f[4];
	int k, l;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 50.0f, 50.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


	while(!WindowShouldClose()){

		UpdateCamera(&camera);

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
		//Drawing Objects
		if(IsMouseButtonDown(0)){
			//printf("%d%s%d%s%d\n", (int)floor((GetMouseX()-offsetX)/(CHUNK_SIZE*blockSize)), ",", (int)floor((GetMouseY()-offsetY)/(CHUNK_SIZE*blockSize)), " :", chunks[(int)floor(GetMouseY() / CHUNK_SIZE)][(int)floor(GetMouseX()/CHUNK_SIZE)]);
			
			
			
			if(GetMouseX() > offsetX && GetMouseX() < offsetX+WORLD_W*blockSize && GetMouseY() > offsetY && GetMouseY() < offsetY + WORLD_H * blockSize){
				int bX = (int)floor((GetMouseX()-offsetX)/blockSize);
				int bY = (int)floor((GetMouseY()-offsetY)/blockSize);
				char sBx[4];
				char sBy[4];
				char sPrint[20];
				sprintf(sBx, "%d", bX);
				sprintf(sBy, "%d", bY);
				strcpy(sPrint, sBx);
				strcat(sPrint, ",");
				strcat(sPrint, sBy);
				DrawText(sPrint, 10, 30, 10, RED);
				if(brushMode == 1){
					grid[bY][bX] = 1;
					grid[bY][bX+1] = 1;
					grid[bY+1][bX] = 1;
					grid[bY+1][bX+1] = 1;
					//printf("%d\n", (int)floor((GetMouseX()-offsetX)/blockSize));
				}
				if(brushMode == 2){
					grid[bY][bX] = 2;
					chunks[(int)floor((GetMouseY()-offsetY) / (CHUNK_SIZE*blockSize))][(int)floor((GetMouseX()- offsetX)/(CHUNK_SIZE*blockSize))] = 1;
				}
				if(brushMode == 3){
					grid[bY][bX] = 3;
					rBrush(bX, bY, WORLD_W, WORLD_H, 30, 3, grid, chunks);
				}
			}
		}
		
		ClearBackground(WHITE);
		

		//3D Drawing Chunks
		BeginMode3D(camera);

            //DrawCube((Vector3){0, 0, 0}, 2.0f, 2.0f, 2.0f, RED);
			//DrawCube((Vector3){3, 0, 0}, 1.0f, 1.0f, 0.5f, BLUE);
            DrawGrid(50, 1.0f);

			{
			int u, v;
			for(u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					if(chunks[u][v] == 1){
								
						//updateGrid(v, u, WORLD_W, WORLD_H, grid, chunks);
						
					}
					drawGrid3D(WORLD_W, WORLD_H, 0.2f, -30, 0, grid, v, u);	
							
				}
			}
			
		}

        EndMode3D();

		//2D Drawing Chunks
		{
			int u, v;
			for(u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					if(chunks[u][v] == 1){
								
						//updateGrid(v, u, WORLD_W, WORLD_H, grid, chunks);
						
					}
					drawGrid(WORLD_W, WORLD_H, blockSize, offsetX, offsetY, grid, v, u);	
							
				}
			}
			
		}
		
		
		if(brushMode == 1){
			DrawText("Block Brush", 10, 10, 10, RED);
		}
		if(brushMode == 2){
			DrawText("Sand Brush", 10, 10, 10, RED);
		}
		if(brushMode == 3){
			DrawText("Water Brush", 10, 10, 10, RED);
		}
		//Chunk debugging (show visuals and which chunks are loaded)
		{
			int tX, tY;
			int bb = 0;
			
			for(tX = 0; tX < WORLD_H/CHUNK_SIZE; tX++){
				for(tY = 0; tY < WORLD_W/CHUNK_SIZE; tY++){
					//printf("%d\n", chunks[10][4]==1);
					
					if(chunks[tX][tY] == 1){
						DrawRectangle(tY*CHUNK_SIZE*blockSize+offsetX, tX*CHUNK_SIZE*blockSize+offsetY, CHUNK_SIZE*blockSize, CHUNK_SIZE*blockSize, (Color){ 255, 0, 0, 15 });	
						char res[10];
						sprintf(res, "%d%s%d", tY, ",", tX);
						DrawText(res, 10, 50+bb*10, 10, RED);
						bb++;
					}
				}
			}
			
			
		}
		
		//Display FPS
		sprintf(f, "%d", GetFPS());
		DrawText(f, 10, 20, 10, RED);
		
		EndDrawing();
	}
	return 0;
}
