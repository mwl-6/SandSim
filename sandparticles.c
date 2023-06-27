#include "sandsim.h"


int fHeight(char ***arr, int j, int k){
	int rVal = WORLD_H-1;
	
	while(rVal > 0 && arr[rVal][j][k] == 4){
		
		rVal--;
	}
	
	return WORLD_H-rVal;
}


int rayBounds(char ***arr, int i, int j, int k, Vector3 cameraPos, float size){
	
	//Convert camera position to grid values
	Vector3 cameraBlockPos = (Vector3){(cameraPos.x/size)-1, WORLD_H-(cameraPos.y/size), round(cameraPos.z/size)};
	//Vector3 unnormalizedDir = Vector3Subtract((Vector3){cameraBlockPos.x, cameraBlockPos.y, cameraBlockPos.z}, (Vector3){j, i, k});
	//cameraBlockPos = Vector3Add(cameraBlockPos, Vector3Scale(unnormalizedDir, 20));
	//printf("%f%s%f%s%f\n", cameraBlockPos.x, ",", cameraBlockPos.y, ",", cameraBlockPos.z);
	int monitor = 0;
	
	Vector3 dir = Vector3Normalize(Vector3Subtract((Vector3){cameraBlockPos.x, cameraBlockPos.y, cameraBlockPos.z}, (Vector3){j, i, k}));
	Vector3 nxt = Vector3Add((Vector3){j, i, k}, dir);
	i = (int)round(nxt.y);
	j = (int)round(nxt.x);
	k = (int)round(nxt.z);
	int blockCounts = 0;

	

	while(i >= 0 && i < WORLD_H && j >= 0 && j < WORLD_W && k >= 0 && k <= WORLD_Z){
		
		if(arr[i][j][k] == 2){
			blockCounts++;
		}
	    nxt = Vector3Add((Vector3){nxt.x, nxt.y, nxt.z}, dir);
		i = (int)round(nxt.y);
		j = (int)round(nxt.x);
		k = (int)round(nxt.z);
		
		//printf("%d%s%d%s%d\n", i, ",", j, ",", k);
		if(blockCounts > 2){
			return 0;
		}
		
	}
	//printf("\n");
	
	return 1;
}


int optimizedEvalParticles3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, Matrix *m, int *mCounts, int density, Vector3 cameraPos, int *updateQueue, int updateLength){

	int mIs = 0;
		
	Vector3 axis = (Vector3){ 0, 0, 0 };
	float angle = 0;
	Matrix rotation = MatrixRotate(axis, angle);

	for(int v = 0; v < updateLength; v+=3){
		if(updateQueue[v] == -1){
			continue;
		}
		int i = updateQueue[v];
		int j = updateQueue[v+1];
		int k = updateQueue[v+2];

		if(!(j >= cX && j <= cX+cXW && k >= cZ && k <= cZ+cZW)){
			continue;
		}

		int withinBounds = (i-1 >= 0 && i+1 < WORLD_H && j-1 >= cX && j+1 < cX+cXW && k-1 > cZ && k+1 < cZ+cZW);
					
		if(rayBounds(arr, i, j, k, cameraPos, size) == 1 && !(withinBounds && arr[i-1][j][k] == 2 && arr[i+1][j][k] == 2 && arr[i][j-1][k] == 2 && arr[i][j+1][k] == 2 && arr[i][j][k-1] == 2 && arr[i][j][k+1] == 2)){
			Matrix translation = MatrixTranslate(j*size, -i*size + WORLD_H*size, k*size);
			m[mCounts[0]+mIs] = MatrixMultiply(rotation, translation);
			mIs++;
		}

	}
	mCounts[0]+=mIs;


}
