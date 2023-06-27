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

	Vector3 ul = (Vector3){j1, fHeight(arr, j1, k1), k1};
	Vector3 ml = (Vector3){j1, fHeight(arr, j1, k), k};
	Vector3 ll = (Vector3){j1, fHeight(arr, j1, k2), k2};
	Vector3 ur = (Vector3){j2, fHeight(arr, j2, k1), k1};
	Vector3 mr = (Vector3){j2, fHeight(arr, j2, k), k};
	Vector3 lr = (Vector3){j2, fHeight(arr, j2, k2), k2};
	Vector3 c = (Vector3){j, fHeight(arr, j, k), k};
	Vector3 cu = (Vector3){j, fHeight(arr, j, k1), k1};
	Vector3 cl = (Vector3){j, fHeight(arr, j, k2), k2};

	
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

				//Vector3 n = quickCalcNormals(ll, lr, ul, j, k);
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
				mesh.vertices[a+2] = (k+1) * size;
				n = calcNormals(j, k+1, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 1 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1)*size;
				n = calcNormals(j+1, k+1, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0.5f + shiftX;
    			mesh.texcoords[b+1] = 1.0f + shiftY;
				b+=2;
				a+=3;
					
				//1, 0
				i = fHeight(arr, j+1, k);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				n = calcNormals(j+1, k, arr);
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
				n = calcNormals(j, k, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			mesh.texcoords[b] = 0 + shiftX;
    			mesh.texcoords[b+1] = 0.5 + shiftY;
				b+=2;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1) * size;
				n = calcNormals(j+1, k+1, arr);
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

void recalculateMesh3D(float size, char ***arr, int cX, int cZ, int cXW, int cZW, int density, Mesh mesh){
		
		int j, k = 0;
		int aInit = (cX-1) * CHUNK_SIZE * 3 * 6 + cZ+cX%density * 3 * 6;
		//int a = 0;
		int b = 0;
		int i = 0;

		if(cX < 0){
			cX = 0;
		}
		if(cZ < 0){
			cZ = 0;
		}
		
		for(j = cX;  j < cX+cXW; j+=1*density){
			for(k = cZ+j%density; k < cZ+cZW; k+=1*density){
				int a = (j-1) * CHUNK_SIZE * 3 * 6 + k * 3 * 6;
				int sA = 0;
				int sB = 0;
				int ll = fHeight(arr, j, k);
				int lr = fHeight(arr, j, k+1);
				int ul = fHeight(arr, j+1, k);
				int ur = fHeight(arr, j+1, k+1);

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
				//0, 0
				
				
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
				
				//0, 1
				
				mesh.vertices[a] = j * size;
				mesh.vertices[a+1] = lr * size;
				mesh.vertices[a+2] = (k+1) * size;
				n = calcNormals(j, k+1, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			//mesh.texcoords[b] = 0 + shiftX;
    			//mesh.texcoords[b+1] = 1 + shiftY;
				//b+=2;
				a+=3;

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ur*size;
				mesh.vertices[a+2] = (k+1)*size;
				n = calcNormals(j+1, k+1, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			//mesh.texcoords[b] = 0.5f + shiftX;
    			//mesh.texcoords[b+1] = 1.0f + shiftY;
				//b+=2;
				a+=3;
					
				//1, 0
				i = fHeight(arr, j+1, k);
				mesh.vertices[a] = (j+1)*size;
				mesh.vertices[a+1] = ul*size;
				mesh.vertices[a+2] = k*size;
				n = calcNormals(j+1, k, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			//mesh.texcoords[b] = 0.5f + shiftX;
    			//mesh.texcoords[b+1] = 0.5f + shiftY;
				//b+=2;
				a+=3;

				

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

				//1, 1
				i = fHeight(arr, j+1, k+1);
				mesh.vertices[a] = (j+1) * size;
				mesh.vertices[a+1] = ur * size;
				mesh.vertices[a+2] = (k+1) * size;
				n = calcNormals(j+1, k+1, arr);
				mesh.normals[a] = n.x;
    			mesh.normals[a+1] = n.y;
    			mesh.normals[a+2] = n.z;
    			//mesh.texcoords[b] = 0.5f + shiftX;
    			//mesh.texcoords[b+1] = 1.0f + shiftY;
				//b+=2;
				
				a+=3;
				
					
			}
		}
		UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);
		UpdateMeshBuffer(mesh, 2, mesh.normals, sizeof(float) * mesh.vertexCount * 3, 0);
		
	
}
