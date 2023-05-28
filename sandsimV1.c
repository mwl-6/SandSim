#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define WORLD_H 60
#define WORLD_W 100
#define CHUNK_SIZE 10


int randRange(int range){
	return (rand() % (range + 1));
}

void drawGrid(int w, int h, int size, int offsetX, int offsetY, char arr[][w]){
	int i, j;
	for(i = 0; i < h; i++){
		for(j = 0; j < w; j++){
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
				DrawRectangle(j*size + offsetX, i*size + offsetY, size, size, SKYBLUE);
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
		if(dir == sDir || i+1 >= h){
			placed = 1;
			return 0;
			break;
		}
		if(dir == 0){
			if(j-1 >= 0 && arr[i+1][j-1] == 0){
				arr[i][j] = 0;
				arr[i+1][j-1] = type;
				placed = 1;
			}
			else if(j-1 >= 0 && arr[i+1][j-1] == 3 && type == 2){
				arr[i][j] = 3;
				arr[i+1][j-1] = type;
				placed = 1;
			}
			else if(j-1 < 0){
				placed = 1;
			}
		}
		if(dir == 1){
			if(j+1 < w && arr[i+1][j+1] == 0){
				arr[i][j] = 0;
				arr[i+1][j+1] = type;
				placed = 1;
			}
			else if(j+1 < w && arr[i+1][j+1] == 3 && type == 2){
				arr[i][j] = 3;
				arr[i+1][j+1] = type;
				placed = 1;
			}
			else if(j+1 >= w){
				placed = 1;
			}
		}
		
		
		if(sDir == -1)
			sDir = dir;
		dir++;
		if(dir >= options){
			dir = 0;
		}
		
	}
	return 1;
}

void updateGrid(int w, int h, char arr[][w]){
	int i, j, v;

	for(i = h-1; i >= 0; i--){
		for(j = 0; j < w; j++){
			if(arr[i][j] == 2){
				if(i+1 < h && arr[i+1][j] == 0){
					arr[i][j] = 0;
					arr[i+1][j] = 2;
				}
				else if(i+1 < h && arr[i+1][j] == 3){
					arr[i][j]=3;
					arr[i+1][j] = 2;
				}
				else{
					v = testAdj(i, j, w, h, arr, 2, 2);
					
				}
			}
			if(arr[i][j] == 3){
				if(i+1 < h && arr[i+1][j] == 0){
					arr[i][j] = 0;
					arr[i+1][j] = 3;
				}
				else {
					v = testAdj(i, j, w, h, arr, 2, 3);
					if(v == 0){
						if(j+1 < w && arr[i][j+1] == 0){
							arr[i][j] = 0;
							arr[i][j+1] = 3;
						}
						else if(j-1 >= 0 && arr[i][j-1] == 0){
							arr[i][j] = 0;
							arr[i][j-1] = 3;
						}
					}
				}
			}

		}
	}
}

void rBrush(int x, int y, int w, int h, int r, char t, char arr[][w]){
	int i, j;
	for(i = 0; i < r; i++){
		for(j = 0; j < r; j++){
			int k = randRange(10);
			if(k == 1){
				arr[y+i][x+j] = t;
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
	//const int h = 300;
	//const int w = 500;
	int chunkSize = 50;

	char grid[WORLD_H][WORLD_W] = {0};
	char chunks[WORLD_H/CHUNK_SIZE][WORLD_W/CHUNK_SIZE] = {0};
	char f[4];
	int k, l;
	
	grid[5][4] = 1;
	grid[8][8] = 1;
	grid[10][6] = 1;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	while(!WindowShouldClose()){
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
		if(IsMouseButtonDown(0)){
			printf("%d%s%d%s%d\n", (int)floor(GetMouseX()/CHUNK_SIZE), ",", (int)floor(GetMouseY()/CHUNK_SIZE), " :", chunks[(int)floor(GetMouseY() / CHUNK_SIZE)][(int)floor(GetMouseX()/CHUNK_SIZE)]);
			
			
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
				}
				if(brushMode == 3){
					grid[bY][bX] = 3;
					rBrush(bX, bY, WORLD_W, WORLD_H, 30, 3, grid);
				}
			}
		}
		
		ClearBackground(WHITE);
		drawGrid(WORLD_W, WORLD_H, blockSize, offsetX, offsetY, grid);
		updateGrid(WORLD_W, WORLD_H, grid);
		if(brushMode == 1){
			DrawText("Block Brush", 10, 10, 10, RED);
		}
		if(brushMode == 2){
			DrawText("Sand Brush", 10, 10, 10, RED);
		}
		if(brushMode == 3){
			DrawText("Water Brush", 10, 10, 10, RED);
		}
		sprintf(f, "%d", GetFPS());
		DrawText(f, 10, 20, 10, RED);
		//DrawCircle(100, 100, 10, RED);
		EndDrawing();
	}
	return 0;
}
