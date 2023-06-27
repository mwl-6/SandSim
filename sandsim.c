#include "sandsim.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"





int main(void){
	int brushMode = 1;
	const int screenWidth = 1280;
	const int screenHeight = 720;
	int offsetX = 100;
	int offsetY = 20;
	float blockSizes = 0.025f;

	/*Establish 3D World Grid*/

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

	/* Generate 3D Chunk Grid */
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

	/*Create 2D array of ground meshes*/

	Model **meshArr = calloc(WORLD_W/CHUNK_SIZE, sizeof(Model*));
	for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
		meshArr[i] = calloc(WORLD_Z/CHUNK_SIZE, sizeof(Model));
	}
	/*2D Array that stores 1/0 for checking if a chunk already contains a ground mesh*/
	char **occupiedMesh = calloc(WORLD_W/CHUNK_SIZE, sizeof(char*));
	for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
		occupiedMesh[i] = calloc(WORLD_Z/CHUNK_SIZE, sizeof(char));
	}


	
	/* Matrix containing transforms of particles */
	Matrix *sandMeshData = (Matrix*)RL_CALLOC(WORLD_W * WORLD_H * WORLD_Z / 60, sizeof(Matrix));
	if(sandMeshData == NULL){
		printf("Failed memory allocation (mesh)\n");
	}

	/* Update Queue */
	int *updateQueue = calloc(WORLD_W * WORLD_H * WORLD_Z, sizeof(int));
	int updateLength = 0;

	int mCounts[1];
	mCounts[0] = 0;

	char f[14];
	int k, l;
	int init_update = 1;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ -1.0f, 2.0f, -1.0f }; // Camera position
    camera.target = (Vector3){ 10.0f, 2.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Mesh cube = GenMeshCube(blockSizes, blockSizes, blockSizes);
	//Mesh testShaderCube = GenMeshCube(1, 1, 1);
	Mesh testShaderCube = GenMeshCone(1, 1, 4);
	
	// Load lighting shader
	/*
	lighting.vs
	lighting.fs
	*/
    Shader shader = LoadShader(TextFormat("resources/shaders/lighting.vs"),
                               TextFormat("resources/shaders/lighting.fs"));
    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 1.9f, 1.9f, 1.9f, 1.0f }, SHADER_UNIFORM_VEC4);

	Shader shader2 = LoadShader(TextFormat("resources/shaders/lighting_instancing.vs"),
                               TextFormat("resources/shaders/lighting.fs"));
    // Get shader locations
    shader2.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader2, "mvp");
    shader2.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader2, "viewPos");
    shader2.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader2, "instanceTransform");

    // Set shader value: ambient light level
	int ambientLoc2 = GetShaderLocation(shader2, "ambient");
    SetShaderValue(shader2, ambientLoc2, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);


    // Create one light
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 250.0f, 250.0f, 20.0f }, Vector3Zero(), WHITE, shader);
	
    Texture texture = LoadTexture("resources/textures/sandtexture3.png");
    

    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader2;
	
	matDefault.maps[MATERIAL_MAP_DIFFUSE].color = YELLOW;


	Material matDefault2 = LoadMaterialDefault();
    matDefault2.shader = shader;
	matDefault2.maps[MATERIAL_MAP_DIFFUSE].color = RED;

	Model m3 = LoadModel("resources/test.obj");
	m3.materials[0].shader = shader;

	
	dumbBrush(grid, chunks);
	

	
	while(!WindowShouldClose()){

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        //SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		SetShaderValue(shader2, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

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


			DrawLine3D((Vector3){50, 50, 50}, (Vector3){0, 0, 0}, RED);
			DrawCube((Vector3){3, 0, 0}, 0.2f, 0.2f, 0.2f, BLUE);
			DrawCube((Vector3){0, 0, 3}, 0.2f, 0.2f, 0.2f, GREEN);
            DrawGrid(50, 1.0f);
			Matrix translationTest = MatrixTranslate(1, 1, 1);
			DrawMesh(testShaderCube, matDefault2, translationTest);
			
			Vector3 axis = (Vector3){ 0, 0, 0 };
		//#pragma omp parallel

		if(updateLength > 0){
			updateGrid(updateQueue, &updateLength, grid, chunks, blockSizes, meshArr);
		}

		{
			//int t = omp_get_thread_num();
			//int tcnt = omp_get_num_threads();
			for(int u = WORLD_H / CHUNK_SIZE - 1; u >= 0; u--){
				for(int v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					for(int w = 0; w < WORLD_Z/CHUNK_SIZE; w+=1){
						if(chunks[u][v][w] == 1){
							//updateGrid(v, u, w, WORLD_W, WORLD_H, WORLD_Z, grid, chunks);
							
							if(occupiedMesh[v][w] == 0){
								//If mesh does not exist create the mesh
								printf("%d%s%d\n", v, ",", w);
								occupiedMesh[v][w] = 1;
								
								meshArr[v][w] = LoadModelFromMesh(calcMesh3D(blockSizes, grid, v*CHUNK_SIZE, w*CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 1));
								//meshArr[v][w] = calcMesh3D(blockSizes, grid, v*CHUNK_SIZE, w*CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 1);
								meshArr[v][w].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
								
								meshArr[v][w].materials[0].shader = shader;
							}
							else if(occupiedMesh[v][w] == 1){
								//Run a mesh update
								//printf("Mesh update: %d%s%d\n", v, ",", w);
								//DrawCube((Vector3){v*CHUNK_SIZE*blockSizes, 0.5, w*CHUNK_SIZE*blockSizes}, 0.03, 0.03, 0.03, RED);
								//meshArr[v][w].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
								//meshArr[v][w].materials[0].shader = shader;
								//recalculateMesh3D(blockSizes, grid, v*CHUNK_SIZE, w*CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 1, meshArr[v][w].meshes[0]);
							}
							init_update = 1;
							
							
						}
							
					}
							
				}
			}
			
				
		}
		
		if(init_update == 1){
			//printf("%s\n", "calling");
			refreshMeshBuffer(sandMeshData, mCounts);
			//evalParticles3D(blockSizes, grid, 0, 0, 300, 300, sandMeshData, mCounts, 1, (Vector3){camera.position.x, camera.position.y, camera.position.z});
			optimizedEvalParticles3D(blockSizes, grid, 0, 0, 300, 300, sandMeshData, mCounts, 1, (Vector3){camera.position.x, camera.position.y, camera.position.z}, updateQueue, updateLength);
			
			//m = LoadModelFromMesh(calcMesh3D(blockSizes, grid, 0, 0, 800, 800, 1));
			init_update = 0;
		}
		
		
		for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
			for(int j = 0; j < WORLD_Z/CHUNK_SIZE; j++){
				//m = LoadModelFromMesh(meshArr[i][j]);
				//meshArr[i][j].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
				//meshArr[i][j].materials[0].shader = shader;
				
				DrawModel(meshArr[i][j], (Vector3){0, 0, 0}, 1, WHITE);
			}
		}
		
		DrawMeshInstanced(cube, matDefault, sandMeshData, mCounts[0]);
		
		if(IsKeyDown(32)){
			//printf("%s\n", "Calling local brush");
			
			grid[150][30][150] = 2;
			updateLength+=3;
			updateQueue[updateLength-3] = 150;
			updateQueue[updateLength-2] = 30;
			updateQueue[updateLength-1] = 150;
			
			chunks[(int)floor(150 / CHUNK_SIZE)][(int)floor(30/CHUNK_SIZE)][(int)floor(150/CHUNK_SIZE)] = 1;
			
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
		sprintf(f, "%s%d", "FPS: ", GetFPS()); 
		char sandCnts[50];
		char liveParts[30];
		sprintf(sandCnts, "%s%d%s%d", "Visibile Sand Particles: ", mCounts[0], "/", WORLD_W * WORLD_H * WORLD_Z / 60);
		sprintf(liveParts, "%s%d%s%d", "Update Queue Size: ", updateLength, "/", WORLD_W * WORLD_H * WORLD_Z);
		DrawText(f, 10, 20, 10, RED);
		DrawText(sandCnts, 10, 40, 10, RED);
		DrawText(liveParts, 10, 80, 10, RED);
		
		EndDrawing();
	}
	RL_FREE(sandMeshData);

	return 0;
}
