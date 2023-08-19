#include "sandsim.h"

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

Vector3 calcNormals(int j, int k, char ***arr){
	int j1, j2, j3, j4;
	int k1, k2, k3, k4;
	j1 = j-1;
	j2 = j+1;
	k1 = k-1;
	k2 = k+1;

	if(j1 < 0){
		j1 = 0;
	}
	if(j2 >= WORLD_W){
		j2 = WORLD_W-1;
	}
	if(k1 < 0){
		k1 = 0;
	}
	if(k2 >= WORLD_W){
		k2 = WORLD_W-1;
	}

	Vector3 ul = (Vector3){j1, fHeight(arr, j1, k1, WORLD_H-1), k1};
	Vector3 ml = (Vector3){j1, fHeight(arr, j1, k, WORLD_H-1), k};
	Vector3 ll = (Vector3){j1, fHeight(arr, j1, k2, WORLD_H-1), k2};
	Vector3 ur = (Vector3){j2, fHeight(arr, j2, k1, WORLD_H-1), k1};
	Vector3 mr = (Vector3){j2, fHeight(arr, j2, k, WORLD_H-1), k};
	Vector3 lr = (Vector3){j2, fHeight(arr, j2, k2, WORLD_H-1), k2};
	Vector3 c = (Vector3){j, fHeight(arr, j, k, WORLD_H-1), k};
	Vector3 cu = (Vector3){j, fHeight(arr, j, k1, WORLD_H-1), k1};
	Vector3 cl = (Vector3){j, fHeight(arr, j, k2, WORLD_H-1), k2};

	
	/*
	QUADS:
	Q1 = ul, ml, c, cu
	Q2 = ml, ll, cl, c
	Q3 = cu, c, mr, ur
	Q4 = c, cl, lr, mr
	*/

	
	Vector3 Q1 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(ul, ml),  Vector3Subtract(ul, c)));
	Vector3 Q2 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(ml, ll),  Vector3Subtract(ml, cl)));
	Vector3 Q3 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(cu, c),  Vector3Subtract(cu, mr)));
	Vector3 Q4 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(c, cl),  Vector3Subtract(c, lr)));

	Vector3 final = Vector3Normalize(Vector3Add(Vector3Add(Q1, Q2), Vector3Add(Q3, Q4)));

	return final;

}

Vector3 quickCalcNormals(int a, int b, int c, int j, int k){
	Vector3 f = (Vector3){j, a, k};
	Vector3 s = (Vector3){j, b, k+1};
	Vector3 t = (Vector3){j+1, c, k};

	return Vector3Normalize(Vector3CrossProduct(Vector3Subtract(f, s),  Vector3Subtract(f, t)));

}


Mesh calcMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density){
		
		Mesh mesh = {0};
		
		mesh.triangleCount = cXW*cZW*2;
		mesh.vertexCount = mesh.triangleCount*3;
		mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
   		mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
   	 	mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float)); 

		
		//int a = 0;
		//int b = 0;
		int i = 0;
		
		#pragma omp parallel for
		for(int j = cX;  j < cX+cXW; j+=1*density){
			
			
			//1800
			//
			
			int a = (j-cX)*CHUNK_SIZE*6*3;
			int b = (j-cX)*CHUNK_SIZE*6*2;
			
			//int dist = cZ+cZW;
			//printf("%d%s%d\n", cZ+j%density+slices*tid, ", ", limit);
			for(int k = cZ+j%density; k < cZ+cZW; k+=(1*density)){
				//int a = j * cZW * 3 * 6 + k * 3 * 6;
				
				int sA = 0;
				int sB = 0;
				int ll = fHeight(arr, j, k, WORLD_H-1);
				int lr = fHeight(arr, j, k+1*density, WORLD_H-1);
				int ul = fHeight(arr, j+1*density, k, WORLD_H-1);
				int ur = fHeight(arr, j+1*density, k+1*density, WORLD_H-1);

				//Vector3 n = quickCalcNormals(ll, lr, ul, j, k);
				Vector3 n = calcNormals(j, k, arr);
				Vector3 n2 = n;
				Vector3 n3;

				float delta = abs((ul+ur)/2 - (ll+lr)/2);
				float shiftX = 0;
				float shiftY = 0;

				/*
				if(delta == 1){
					shiftX = 0.5f;
					shiftY = -0.5f;
				}
				if(delta == 2){
					shiftX = 0.5f;
					shiftY = 0.0f;
				}
				*/
				
				//0, 0
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				
				mesh.vertices[a+2] = k * size;
				
				mesh.normals[a] = n.x;
				mesh.normals[a+1] = n.y;
				mesh.normals[a+2] = n.z;
				mesh.texcoords[b] = 0 + shiftX;
				mesh.texcoords[b+1] = 0.5 + shiftY;
				b+=2;
				a+=3;
				
				//0, 1
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = lr * size;
				mesh.vertices[a+2] = (k+1*density) * size;
				n = calcNormals(j, k+1*density, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 1 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				//i = fHeight(arr, j+1*density, k+1*density);
				mesh.vertices[a] = (j+1*density)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1*density)*size;
				n = calcNormals(j+1, k+1*density, arr);
				n3 = n;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 1.0f + shiftY;
				b+=2;
				a+=3;
					
				//1, 0
				//i = fHeight(arr, j+1*density, k);
				mesh.vertices[a] = (j+1*density)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				n = calcNormals(j+1*density, k, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 0.5f + shiftY;
				b+=2;
				a+=3;

				

				//0, 0
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				mesh.vertices[a+2] = k * size;
				n = n2;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 0.5 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				//i = fHeight(arr, j+1*density, k+1*density);
				mesh.vertices[a] = (j+1*density) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1*density) * size;
				n = n3;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 1.0f + shiftY;
				b+=2;
				a+=3;
				
					
			}
			
		}
		
		
		
		UploadMesh(&mesh, true);
		return mesh;
	
}

void updatedRecalculateMesh3D(float size, char ***arr, int cX, int cZ, Mesh mesh){
	
		//int a = 0;
		int b = 0;
		int i = 0;

		if(cX < 0){
			cX = 0;
		}
		if(cZ < 0){
			cZ = 0;
		}

		int chunkPosX = (cX+1)/CHUNK_SIZE;
		int chunkPosZ = (cZ+1)/CHUNK_SIZE;

		/*This is broken when density != 1*/
		//int a = (j-2) * CHUNK_SIZE * 3 * 6 + k * 3 * 6;

		//

		int a = (cX  % CHUNK_SIZE)*CHUNK_SIZE*6*3 + (cZ % CHUNK_SIZE) * 3 * 6 ;


		int sA = 0;
		int sB = 0;
		int ll = fHeight(arr, cX, cZ, WORLD_H-1);
		int lr = fHeight(arr, cX, cZ+1, WORLD_H-1);
		int ul = fHeight(arr, cX+1, cZ, WORLD_H-1);
		int ur = fHeight(arr, cX+1, cZ+1, WORLD_H-1);

		Vector3 n = calcNormals(cX, cZ, arr);

		//0, 0
		
		//if(a < mesh.vertexCount * 3){
			//mesh.vertices[a] = cX * size;
			mesh.vertices[a+1] = ll * size;
			//mesh.vertices[a+2] = k * size;
			
			mesh.normals[a] = n.x;
			mesh.normals[a+1] = n.y;
			mesh.normals[a+2] = n.z;
			//mesh.texcoords[b] = 0 + shiftX;
			//mesh.texcoords[b+1] = 0.5 + shiftY;
			//b+=2;
			a+=3;
		//}
				
				
				
					
			
		
		UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);
		UpdateMeshBuffer(mesh, 2, mesh.normals, sizeof(float) * mesh.vertexCount * 3, 0);
		
}

void recalculateMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density, Mesh mesh, struct BlockOBJ *myBlockOBJs, int myBlockOBJsCount){
		
		int j, k = 0;
		//int aInit = (cX-1) * CHUNK_SIZE * 3 * 6 + cZ+cX%density * 3 * 6;
		//int a = 0;
		int b = 0;
		int i = 0;

		if(cX < 0){
			cX = 0;
		}
		if(cZ < 0){
			cZ = 0;
		}

		int chunkPosX = (cX+1)/CHUNK_SIZE;
		int chunkPosZ = (cZ+1)/CHUNK_SIZE;
		int aInit = -1;
		
		for(j = cX;  j < cX+cXW; j+=1*density){
			for(k = cZ; k < cZ+cZW; k+=1){
				/*This is broken when density != 1*/
				//int a = (j-2) * CHUNK_SIZE * 3 * 6 + k * 3 * 6;
				if(!(j / CHUNK_SIZE == chunkPosX && k / CHUNK_SIZE == chunkPosZ)){
					
					continue;
				}

				int a = (j  % CHUNK_SIZE)*CHUNK_SIZE*6*3 + (k % CHUNK_SIZE) * 3 * 6 ;
				if(aInit == -1){
					aInit = a;
				}

				int sA = 0;
				int sB = 0;
				int ll = fHeight(arr, j, k, WORLD_H-1);
				int lr = fHeight(arr, j, k+1*density, WORLD_H-1);
				int ul = fHeight(arr, j+1*density, k, WORLD_H-1);
				int ur = fHeight(arr, j+1*density, k+1*density, WORLD_H-1);

				Vector3 n = calcNormals(j, k, arr);


				float delta = abs((ul+ur)/2 - (ll+lr)/2);
				float shiftX = 0;
				float shiftY = 0;

				/*
				if(delta == 1){
					shiftX = 0.5f;
					shiftY = -0.5f;
				}
				if(delta == 2){
					shiftX = 0.5f;
					shiftY = 0.0f;
				}
				*/
				{
				//0, 0
				
				//if(a < mesh.vertexCount * 3){
					mesh.vertices[a] = j * size;
					mesh.vertices[a+1] = ll * size;
					mesh.vertices[a+2] = k * size;
					
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0 + shiftX;
					//mesh.texcoords[b+1] = 0.5 + shiftY;
					//b+=2;
					a+=3;
				//}
				
				//0, 1

				//if(a < mesh.vertexCount * 3){
				
					mesh.vertices[a] = j * size;
					mesh.vertices[a+1] = lr * size;
					mesh.vertices[a+2] = (k+1*density) * size;
					n = calcNormals(j, k+1*density, arr);
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0 + shiftX;
					//mesh.texcoords[b+1] = 1 + shiftY;
					//b+=2;
					a+=3;
				//}


				//if(a < mesh.vertexCount * 3){
					//1, 1
					//i = fHeight(arr, j+1*density, k+1*density);
					
					mesh.vertices[a] = (j+1*density)*size;
					mesh.vertices[a+1] = ur*size;
					mesh.vertices[a+2] = (k+1*density)*size;
					n = calcNormals(j+1*density, k+1*density, arr);
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0.5f + shiftX;
					//mesh.texcoords[b+1] = 1.0f + shiftY;
					//b+=2;
					a+=3;
				//}
					

				//if(a < mesh.vertexCount * 3){
					//1, 0
					//i = fHeight(arr, j+1*density, k);
					mesh.vertices[a] = (j+1*density)*size;
					mesh.vertices[a+1] = ul*size;
					mesh.vertices[a+2] = k*size;
					n = calcNormals(j+1*density, k, arr);
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0.5f + shiftX;
					//mesh.texcoords[b+1] = 0.5f + shiftY;
					//b+=2;
					a+=3;
				//}
				
				
				//if(a < mesh.vertexCount * 3){
					//0, 0
					mesh.vertices[a] = j * size;
					mesh.vertices[a+1] = ll * size;
					mesh.vertices[a+2] = k * size;
					n = calcNormals(j, k, arr);
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0 + shiftX;
					//mesh.texcoords[b+1] = 0.5 + shiftY;
					//b+=2;
					a+=3;
				//}
				



				//if(a < mesh.vertexCount * 3){
					//1, 1
					//i = fHeight(arr, j+1*density, k+1*density);
					mesh.vertices[a] = (j+1*density) * size;
					mesh.vertices[a+1] = ur * size;
					mesh.vertices[a+2] = (k+1*density) * size;
					n = calcNormals(j+1*density, k+1*density, arr);
					mesh.normals[a] = n.x;
					mesh.normals[a+1] = n.y;
					mesh.normals[a+2] = n.z;
					//mesh.texcoords[b] = 0.5f + shiftX;
					//mesh.texcoords[b+1] = 1.0f + shiftY;
					//b+=2;
					
					a+=3;
				//}
				}
				
					
			}
		}
		UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);
		UpdateMeshBuffer(mesh, 2, mesh.normals, sizeof(float) * mesh.vertexCount * 3, 0);
		

		for(int c = 0; c < myBlockOBJsCount; c++){
			//Check if within bounds of helper mesh
			if(cX+cXW > myBlockOBJs[c].x && cX < myBlockOBJs[c].x+myBlockOBJs[c].w && cZ+cZW > myBlockOBJs[c].z && cZ < myBlockOBJs[c].z+myBlockOBJs[c].d){
				
				for(j = cX;  j < cX+cXW; j+=1){
					for(k = cZ; k < cZ+cZW; k+=1){
						/*This is broken when density != 1*/
						//int a = (j-2) * CHUNK_SIZE * 3 * 6 + k * 3 * 6;
						if(!(j < myBlockOBJs[c].x+myBlockOBJs[c].w && k < myBlockOBJs[c].z+myBlockOBJs[c].d && j >= myBlockOBJs[c].x && k >= myBlockOBJs[c].z)){
							
							continue;
						}

						int a = (j - myBlockOBJs[c].x)*myBlockOBJs[c].w*6*3 + (k - myBlockOBJs[c].z) * 3 * 6 ;
						if(aInit == -1){
							aInit = a;
						}

						int sA = 0;
						int sB = 0;
						int ll = fHeight(arr, j, k, myBlockOBJs[c].y-1);
						int lr = fHeight(arr, j, k+1*density, myBlockOBJs[c].y-1);
						int ul = fHeight(arr, j+1*density, k, myBlockOBJs[c].y-1);
						int ur = fHeight(arr, j+1*density, k+1*density, myBlockOBJs[c].y-1);

						Vector3 n = calcNormals(j, k, arr);


						float delta = abs((ul+ur)/2 - (ll+lr)/2);
						float shiftX = 0;
						float shiftY = 0;

						/*
						if(delta == 1){
							shiftX = 0.5f;
							shiftY = -0.5f;
						}
						if(delta == 2){
							shiftX = 0.5f;
							shiftY = 0.0f;
						}
						*/
						{
						//0, 0
						
						//if(a < mesh.vertexCount * 3){
							myBlockOBJs[c].mesh.vertices[a] = j * size;
							myBlockOBJs[c].mesh.vertices[a+1] = ll * size;
							myBlockOBJs[c].mesh.vertices[a+2] = k * size;
							
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0 + shiftX;
							//mesh.texcoords[b+1] = 0.5 + shiftY;
							//b+=2;
							a+=3;
						//}
						
						//0, 1

						//if(a < mesh.vertexCount * 3){
						
							myBlockOBJs[c].mesh.vertices[a] = j * size;
							myBlockOBJs[c].mesh.vertices[a+1] = lr * size;
							myBlockOBJs[c].mesh.vertices[a+2] = (k+1*density) * size;
							n = calcNormals(j, k+1*density, arr);
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0 + shiftX;
							//mesh.texcoords[b+1] = 1 + shiftY;
							//b+=2;
							a+=3;
						//}


						//if(a < mesh.vertexCount * 3){
							//1, 1
							//i = fHeight(arr, j+1*density, k+1*density);
							
							myBlockOBJs[c].mesh.vertices[a] = (j+1*density)*size;
							myBlockOBJs[c].mesh.vertices[a+1] = ur*size;
							myBlockOBJs[c].mesh.vertices[a+2] = (k+1*density)*size;
							n = calcNormals(j+1*density, k+1*density, arr);
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0.5f + shiftX;
							//mesh.texcoords[b+1] = 1.0f + shiftY;
							//b+=2;
							a+=3;
						//}
							

						//if(a < mesh.vertexCount * 3){
							//1, 0
							//i = fHeight(arr, j+1*density, k);
							myBlockOBJs[c].mesh.vertices[a] = (j+1*density)*size;
							myBlockOBJs[c].mesh.vertices[a+1] = ul*size;
							myBlockOBJs[c].mesh.vertices[a+2] = k*size;
							n = calcNormals(j+1*density, k, arr);
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0.5f + shiftX;
							//mesh.texcoords[b+1] = 0.5f + shiftY;
							//b+=2;
							a+=3;
						//}
						
						
						//if(a < mesh.vertexCount * 3){
							//0, 0
							myBlockOBJs[c].mesh.vertices[a] = j * size;
							myBlockOBJs[c].mesh.vertices[a+1] = ll * size;
							myBlockOBJs[c].mesh.vertices[a+2] = k * size;
							n = calcNormals(j, k, arr);
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0 + shiftX;
							//mesh.texcoords[b+1] = 0.5 + shiftY;
							//b+=2;
							a+=3;
						//}
						



						//if(a < mesh.vertexCount * 3){
							//1, 1
							//i = fHeight(arr, j+1*density, k+1*density);
							myBlockOBJs[c].mesh.vertices[a] = (j+1*density) * size;
							myBlockOBJs[c].mesh.vertices[a+1] = ur * size;
							myBlockOBJs[c].mesh.vertices[a+2] = (k+1*density) * size;
							n = calcNormals(j+1*density, k+1*density, arr);
							myBlockOBJs[c].mesh.normals[a] = n.x;
							myBlockOBJs[c].mesh.normals[a+1] = n.y;
							myBlockOBJs[c].mesh.normals[a+2] = n.z;
							//mesh.texcoords[b] = 0.5f + shiftX;
							//mesh.texcoords[b+1] = 1.0f + shiftY;
							//b+=2;
							
							a+=3;
						//}
						}
						
							
					}
				}
				UpdateMeshBuffer(myBlockOBJs[c].mesh, 0, myBlockOBJs[c].mesh.vertices, sizeof(float) * myBlockOBJs[c].mesh.vertexCount * 3, 0);
				UpdateMeshBuffer(myBlockOBJs[c].mesh, 2, myBlockOBJs[c].mesh.normals, sizeof(float) * myBlockOBJs[c].mesh.vertexCount * 3, 0);
				
				//printf("%d\n", fHeight(arr, cX, cZ, myBlockOBJs[c].y-1));
			}
		}
	
}


void clearAllMeshes(Model **meshArr, char **occupiedMesh){
	for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
		for(int j = 0; j < WORLD_Z/CHUNK_SIZE; j++){
			
			UnloadMesh(meshArr[i][j].meshes[0]);
			occupiedMesh[i][j] = 0;
		}
	}
}

void initBlockOBJ(struct BlockOBJ *b, int type, int x, int y, int z, int w, int h, int d, char ***grid){
	printf("%s\n", "Init block obj...");
	for(int i = y; i < y+h; i++){
		
		for(int j = x; j < x+w; j++){
			for(int k = z; k < z+d; k++){
				grid[i][j][k] = 1;
				//printf("%s%d%s%d%s%d\n", "y", i, "x", j, "z", k);
			}
		}
	}
	
	b->type = type;
	b->x = x;
	b->y = y;
	b->z = z;
	b->w = w;
	b->h = h;
	b->d = d;
	/*
	CREATE SAND MESH HERE
	*/
	Mesh mesh = {0};
		
	mesh.triangleCount = w*d*2;
	mesh.vertexCount = mesh.triangleCount*3;
	mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
   	mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
   	mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float)); 

	float size = 0.025f;

	int a = 0; 
	int bI = 0;
	//#pragma omp parallel for
		for(int j = x;  j < x+w; j+=1){
			
			//int a = (j-x)*CHUNK_SIZE*6*3;
			//int b = (j-x)*CHUNK_SIZE*6*2;
			
			for(int k = z; k < z+d; k+=1){
				//int a = j * cZW * 3 * 6 + k * 3 * 6;
				//printf("%s\n", "hi");
				int sA = 0;
				int sB = 0;
				int ll = 3;
				int lr = 3;
				int ul = 3;
				int ur = 3;

				//Vector3 n = quickCalcNormals(ll, lr, ul, j, k);
				//Vector3 n = calcNormals(j, k, arr);
				Vector3 n = (Vector3){0, 1, 0};
				//Vector3 n2 = n;
				//Vector3 n3;

				float delta = abs((ul+ur)/2 - (ll+lr)/2);
				float shiftX = 0;
				float shiftY = 0;

				
				//0, 0
				//printf("%d%s%d", j*size, ", ", k*size);
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				
				mesh.vertices[a+2] = k * size;
				
				mesh.normals[a] = n.x;
				mesh.normals[a+1] = n.y;
				mesh.normals[a+2] = n.z;
				mesh.texcoords[bI] = 0 + shiftX;
				mesh.texcoords[bI+1] = 0.5 + shiftY;
				bI+=2;
				a+=3;
				
				//0, 1
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = lr * size;
				mesh.vertices[a+2] = (k+1) * size;
				//n = calcNormals(j, k+1*density, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[bI] = 0 + shiftX;
    			mesh.texcoords[bI+1] = 1 + shiftY;
				bI+=2;
				a+=3;

				//1, 1
				//i = fHeight(arr, j+1*density, k+1*density);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1)*size;
				//n = calcNormals(j+1, k+1*density, arr);
				//n3 = n;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[bI] = 0.5f + shiftX;
    			mesh.texcoords[bI+1] = 1.0f + shiftY;
				bI+=2;
				a+=3;
					
				//1, 0
				//i = fHeight(arr, j+1*density, k);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				//n = calcNormals(j+1*density, k, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[bI] = 0.5f + shiftX;
    			mesh.texcoords[bI+1] = 0.5f + shiftY;
				bI+=2;
				a+=3;

				

				//0, 0
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = ll * size;
				mesh.vertices[a+2] = k * size;
				//n = n2;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[bI] = 0 + shiftX;
    			mesh.texcoords[bI+1] = 0.5 + shiftY;
				bI+=2;
				a+=3;

				//1, 1
				//i = fHeight(arr, j+1*density, k+1*density);
				mesh.vertices[a] = (j+1) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1) * size;
				//n = n3;
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[bI] = 0.5f + shiftX;
    			mesh.texcoords[bI+1] = 1.0f + shiftY;
				bI+=2;
				a+=3;
				
					
			}
			
		}
		
		//mesh = GenMeshCube(1, 1, 1);
		
		UploadMesh(&mesh, true);
		b->mesh = mesh;
		
		printf("Complete init\n");

}

void renderBlockOBJ(struct BlockOBJ *b, float blockSizes, Model mesh){
	if(b->type == 0){
		DrawModelEx(mesh, (Vector3){b->x * blockSizes + b->w*blockSizes/2.0f, WORLD_H*blockSizes - ((b->y-1.01) * blockSizes + (b->h)*blockSizes/2.0f), b->z * blockSizes + b->d * blockSizes / 2.0f}, Vector3Zero(), 0.0f, (Vector3){b->w * blockSizes, b->h * blockSizes, b->d * blockSizes}, RED);
		//DrawCube((Vector3){b->x * blockSizes + b->w*blockSizes/2.0f, WORLD_H*blockSizes - (b->y * blockSizes - b->h*blockSizes/2.0f), b->z * blockSizes + b->d * blockSizes / 2.0f}, b->w * blockSizes, b->h * blockSizes, b->d * blockSizes, RED);
	}
}