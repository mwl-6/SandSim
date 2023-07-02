#include "sandsim.h"


int testAdj(int i, int j, int k, int w, int h, int d, char ***arr, int moves, int type, int *updateQueue, int *updateLength, int x){
	int options = moves;
	int dir = randRange(options-1);
	int placed = 0;
	int sDir = -1;
	while(placed != 1){
		//Fail condition: OOB or all options have been exhausted
		if(dir == sDir || i+1 >= h){
			placed = 1;
			updateQueue[x-2] = -1;
			updateQueue[x-1] = -1;
			updateQueue[x] = -1;
			return 0;
			break;
		}//Attempt right
		if(dir == 0){
			//Element into air
			if(j-1 >= 0 && arr[i+1][j-1][k] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j-1][k] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x-1]--;
				return 1;
			} // Sand into water
			else if(j-1 >= 0 && arr[i+1][j-1][k] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j-1][k] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x-1]--;
				return 1;
			}
			else if(j-1 < 0){
				updateQueue[x-2] = -1;
				updateQueue[x-1] = -1;
				updateQueue[x] = -1;
				placed = 1;
			}
		}//Attempt Left
		if(dir == 1){
			//Element into air
			if(j+1 < w && arr[i+1][j+1][k] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j+1][k] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x-1]++;
				return 1;
			} //Sand into water
			else if(j+1 < w && arr[i+1][j+1][k] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j+1][k] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x-1]++;
				return 1;
			}
			else if(j+1 >= w){
				placed = 1;
				updateQueue[x-2] = -1;
				updateQueue[x-1] = -1;
				updateQueue[x] = -1;
			}
		}
		//Attempt backwards
		if(dir == 2){
			//Element into air
			if(k-1 >= 0 && arr[i+1][j][k-1] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j][k-1] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x]--;
				return 1;
			} // Sand into water
			else if(k-1 >= 0 && arr[i+1][j][k-1] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j][k-1] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x]--;
				return 1;
			}
			else if(k-1 < 0){
				placed = 1;
				updateQueue[x-2] = -1;
				updateQueue[x-1] = -1;
				updateQueue[x] = -1;
			}
		}
		//Attempt forwards
		if(dir == 3){
			//Element into air
			if(k+1 < d && arr[i+1][j][k+1] == 0){
				arr[i][j][k] = 0;
				arr[i+1][j][k+1] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x]++;
				return 1;
			} //Sand into water
			else if(k+1 < d && arr[i+1][j][k+1] == 3 && type == 2){
				arr[i][j][k] = 3;
				arr[i+1][j][k+1] = type;
				placed = 1;
				updateQueue[x-2]++;
				updateQueue[x]++;
				return 1;
			}
			else if(k+1 >= d){
				placed = 1;
				updateQueue[x-2] = -1;
				updateQueue[x-1] = -1;
				updateQueue[x] = -1;
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

void updateGrid(int *updateQueue, int *updateLength, char ***arr, float blockSize, Model **meshArr){

	int v;
	int emptyQueue = 1;
	int emptyValues = 0;

	//printf("%d\n", *updateLength);



	for(int x = *updateLength-1; x >= 0; x-=3){
		int i = updateQueue[x-2];
		int j = updateQueue[x-1];
		int k = updateQueue[x];
		//printf("%s%d\n", "i: ", i);
		int updated = 0;

		if(i == -1 && j == -1 && k == -1){
			emptyValues+=3;
			continue;
		}
		else {
			emptyQueue = 0;
		}

		

		if(arr[i][j][k] == 2){
			//Downward into air
			if(i+1 < WORLD_H && arr[i+1][j][k] == 0){
				
				arr[i][j][k] = 0;
				arr[i+1][j][k] = 2;
				updated = 1;
				updateQueue[x-2]++;
				

			} //Downward into water
			else if(i+1 < WORLD_H && arr[i+1][j][k] == 3){
				arr[i][j][k]=3;
				arr[i+1][j][k] = 2;
				updated = 1;
				updateQueue[x-2]++;
				
			} //Diagonal search
			else{
				v = testAdj(i, j, k, WORLD_W, WORLD_H, WORLD_Z, arr, 4, 2, updateQueue, updateLength, x);
				if(v == 1){
					updated = 1;
				}
				else {
					arr[i][j][k] = 4;
					recalculateMesh3D(blockSize, arr, j-1, k-1, 2, 2, 1, meshArr[j/CHUNK_SIZE][k/CHUNK_SIZE].meshes[0]);
					updateQueue[x] = -1;
					updateQueue[x-1] = -1;
					updateQueue[x-2] = -1;
					updated = 0;
					if(x == *updateLength-1){
						*updateLength-=3;
						//printf("Shrank Update Buffer\n");
					}
				}
				
			}

			
		}//Water 

		
	}

	if(emptyQueue == 1){
		*updateLength = 0;
		//printf("%s\n", "Update Queue Cleared");
	}
	//printf("%s%d\n", "Empty values: ", emptyValues);

}

