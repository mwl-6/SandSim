#include "sandsim.h"



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

void dumbBrush(char ***grid, char ***chunks){
	int x, y, z, r;
	for(y = WORLD_H-1; y > WORLD_H-10; y--){
		for(x = 0; x < 400; x++){
			for(z = 0; z < 400; z++){
				r = randRange(5);
				if(r == 1){
					grid[y][x][z] = 4;
					//chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
				}
				else {
					grid[y][x][z] = 0;
				}
			}
		}
	}
}

//Random brush
void dustBrush(char ***grid, char ***chunks, int *updateQueue, int *updateLength){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				
				r = randRange(1);
				if(r == 0 && y > WORLD_H-26 && y < WORLD_H){
					grid[y][x][z] = 2;
					//printf("%d%s%d%s%d\n", y, ",", x, ",", *updateLength-1);
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
					*updateLength+=3;
					updateQueue[*updateLength-1] = z;
					updateQueue[*updateLength-2] = x;
					updateQueue[*updateLength-3] = y;
					//printf("%d\n", updateQueue[*updateLength-1]);
					
				}
				else {
					grid[y][x][z] = 0;
					chunks[(int)floor(y / CHUNK_SIZE)][(int)floor(x/CHUNK_SIZE)][(int)floor(z/CHUNK_SIZE)] = 1;
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

