#include "sandsim.h"





void dumbBrush(char ***grid){
	int x, y, z, r;
	for(y = WORLD_H-1; y > WORLD_H-2; y--){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				r = randRange(5);
				if(r == 1){
					grid[y][x][z] = 4;
					
				}
				else {
					grid[y][x][z] = 0;
				}
			}
		}
	}
}

void noiseBrush(char ***grid){
	int x, y, z;
	int r;
	
	for(x = 0; x < WORLD_W; x++){
		for(z = 0; z < WORLD_Z; z++){
			r = WORLD_H - 1 - (int)(floor(perlin2d(x, z, 0.01, 20) * 50));
			
			grid[r][x][z] = 4;
			while(r < WORLD_H-1){
				r++;
				grid[r][x][z]=4;
				
			}
				
		}
	}
	
}

//Random brush
void dustBrush(char ***grid, int *updateQueue, int *updateLength){
	int x, y, z, r;
	for(y = 0; y < WORLD_H; y++){
		for(x = 0; x < WORLD_W; x++){
			for(z = 0; z < WORLD_Z; z++){
				
				r = randRange(1);
				if(r == 0 && y > WORLD_H-26 && y < WORLD_H){
					grid[y][x][z] = 2;
					//printf("%d%s%d%s%d\n", y, ",", x, ",", *updateLength-1);
					*updateLength+=3;
					updateQueue[*updateLength-1] = z;
					updateQueue[*updateLength-2] = x;
					updateQueue[*updateLength-3] = y;
					//printf("%d\n", updateQueue[*updateLength-1]);
					
				}
				else {
					grid[y][x][z] = 0;
				}
			}
		}
	}
}


