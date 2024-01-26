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
	Matrix *sandMeshData = (Matrix*)RL_CALLOC(1000 * WORLD_H * 1000 / 60, sizeof(Matrix));
	if(sandMeshData == NULL){
		printf("Failed memory allocation (mesh)\n");
	}

	/* Update Queue */
	int *updateQueue = calloc(100 * WORLD_H * 100, sizeof(int));
	int updateLength = 0;

	int mCounts[1];
	mCounts[0] = 0;

	char f[14];
	int k, l;
	int init_update = 1;
	
	InitWindow(screenWidth, screenHeight, "Sand Simulation");

	Camera camera = { 0 };
    camera.position = (Vector3){ 0.5, 4, 0.5 }; // Camera position
    camera.target = (Vector3){ WORLD_W * blockSizes / 2.0f, 1.0f, WORLD_W * blockSizes / 2.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
   	camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Mesh cube = GenMeshCube(blockSizes, blockSizes, blockSizes);
	
	//Mesh testShaderCube = GenMeshCube(1, 1, 1);
	Mesh testShaderCube = GenMeshCone(1, 1, 4);
	Mesh platformTestCube = GenMeshCube(1, 1, 1);
	
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
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 250.0f, 250.0f, 20.0f }, Vector3Zero(), WHITE, shader2);
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -250.0f, -250.0f, 20.0f }, Vector3Zero(), WHITE, shader2);
	
    Texture texture = LoadTexture("resources/textures/sandtexture3.png");
    

    Material matDefault = LoadMaterialDefault();
    matDefault.shader = shader2;
	
	matDefault.maps[MATERIAL_MAP_DIFFUSE].texture = texture;


	Material matDefault2 = LoadMaterialDefault();
    matDefault2.shader = shader;
	matDefault2.maps[MATERIAL_MAP_DIFFUSE].color = RED;

	Material matDefault3 = LoadMaterialDefault();
    matDefault3.shader = shader;
	matDefault3.maps[MATERIAL_MAP_DIFFUSE].texture = texture;



	Model m3 = LoadModel("resources/test.obj");
	m3.materials[0].shader = shader;


	Model platformDemoCube = LoadModelFromMesh(GenMeshCube(1, 1, 1));
	platformDemoCube.materials[0].shader = shader;
	noiseBrush(grid);

	int myBlockOBJsCount = 5;
	struct BlockOBJ *myBlockOBJs = calloc(myBlockOBJsCount, sizeof(struct BlockOBJ));

	initBlockOBJ(&myBlockOBJs[0], 0, 300, 140, 200, 75, 5, 75, grid);
	initBlockOBJ(&myBlockOBJs[1], 0, 400, 155, 400, 35, 5, 35, grid);
	initBlockOBJ(&myBlockOBJs[2], 0, 300, 125, 320, 15, 5, 15, grid);
	
	DisableCursor();

	int dispenserX = 230;
	int dispenserZ = 170;
	float disX = 230.0f;
	float disZ = 170.0f;
	
	while(!WindowShouldClose()){
		UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*10.0f*GetFrameTime() -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*10.0f*GetFrameTime(),    
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*10.0f*GetFrameTime() -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*10.0f*GetFrameTime(),
                0.0f                                                // Move up-down
            },
            (Vector3){
                GetMouseDelta().x*0.16f,                            // Rotation: yaw
                GetMouseDelta().y*0.15f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove()*2.0f); 
		
		
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
		Vector3 cameraTargetVector = Vector3Subtract(camera.position, camera.target);
		Vector2 normalTarget = Vector2Normalize((Vector2){cameraTargetVector.x, cameraTargetVector.z});

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


			//DrawLine3D((Vector3){50, 50, 50}, (Vector3){0, 0, 0}, RED);
			DrawCube((Vector3){3, 0, 0}, 0.2f, 0.2f, 0.2f, BLUE);
			DrawCube((Vector3){0, 0, 3}, 0.2f, 0.2f, 0.2f, GREEN);
            DrawGrid(50, 1.0f);
			Matrix translationTest = MatrixTranslate(1, 1, 1);
			
			renderBlockOBJ(&myBlockOBJs[0], blockSizes, platformDemoCube);
			renderBlockOBJ(&myBlockOBJs[1], blockSizes, platformDemoCube);
			renderBlockOBJ(&myBlockOBJs[2], blockSizes, platformDemoCube);
			
			Vector3 axis = (Vector3){ 0, 0, 0 };
		//#pragma omp parallel

		if(updateLength > 0){
			updateGrid(updateQueue, &updateLength, grid, blockSizes, meshArr, myBlockOBJs, myBlockOBJsCount);
		}

		{
			//int t = omp_get_thread_num();
			//int tcnt = omp_get_num_threads();
			
				for(int v = 0; v < WORLD_W/CHUNK_SIZE; v++){
					for(int w = 0; w < WORLD_Z/CHUNK_SIZE; w+=1){
						
							if(occupiedMesh[v][w] == 0){
								//If mesh does not exist create the mesh
								occupiedMesh[v][w] = 1;
								
								meshArr[v][w] = LoadModelFromMesh(calcMesh3D(blockSizes, grid, v*CHUNK_SIZE, w*CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 1));
								
								meshArr[v][w].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
								
								meshArr[v][w].materials[0].shader = shader;
							}
							init_update = 1;
							
							
						
							
					}
							
				}
			
			
				
		}
		
		if(init_update == 1){
			
			refreshMeshBuffer(sandMeshData, mCounts);
			optimizedEvalParticles3D(blockSizes, grid, 0, 0, 1000, 1000, sandMeshData, mCounts, 1, (Vector3){camera.position.x, camera.position.y, camera.position.z}, updateQueue, updateLength);
			
			init_update = 0;
		}
		
		
		//Only draw visible chunks
		int chunkCnt = 0;
		for(int i = 0; i < WORLD_W/CHUNK_SIZE; i++){
			for(int j = 0; j < WORLD_Z/CHUNK_SIZE; j++){
				float localChunkX = (i+0.5)*CHUNK_SIZE*blockSizes;
				float localChunkY = (j+0.5)*CHUNK_SIZE*blockSizes;

				
				Vector3 newTargetVector = Vector3Normalize(Vector3Subtract(camera.position, (Vector3){localChunkX, camera.position.y, localChunkY}));
				Vector2 newTarget = (Vector2){newTargetVector.x, newTargetVector.z};
				
				float angleBetween = acos(Vector2DotProduct(newTarget, normalTarget));

				if(angleBetween < 1.4 || isnan(angleBetween) || Vector2Distance((Vector2){localChunkX, localChunkY}, (Vector2){camera.position.x, camera.position.z}) < 5){
					chunkCnt++;
					DrawModel(meshArr[i][j], (Vector3){0 + 0.00*i, 0, 0+ 0.00*j}, 1, WHITE);
				}
				
			}
		}
		
		for(int i = 0; i < 3; i++){
			
			DrawMesh(myBlockOBJs[i].mesh, matDefault3, MatrixTranslate(0, 0, 0));
			
		}
		
		
		if(IsKeyDown(32)){
			
			for(int y = dispenserX-5; y < dispenserX+5; y++){
				for(int x = dispenserZ-5; x < dispenserZ+5; x++){
					if(randRange(37) == 0){
						grid[70][y][x] = 2;
						insertParticleIntoQueue(updateQueue, &updateLength, 70, y, x);
					}
				}
			}
			
			
		}
		DrawMeshInstanced(cube, matDefault, sandMeshData, mCounts[0]);
		if(IsKeyDown(76)){
			
			disZ+=(100.0f*GetFrameTime());
		}
		if(IsKeyDown(74)){
			disZ-=(100.0f*GetFrameTime());
		}
		if(IsKeyDown(75)){
			disX-=(100.0f*GetFrameTime());
		}
		if(IsKeyDown(73)){
			disX+=100.0f*GetFrameTime();
		}
		
		if(dispenserX >= WORLD_W){
			dispenserX = WORLD_W-1;
		}
		if(dispenserX < 0){
			dispenserX = 0;
		}
		if(dispenserZ >= WORLD_Z){
			dispenserZ = WORLD_Z-1;
		}
		if(dispenserZ < 0){
			dispenserZ = 0;
		}
		dispenserX = (int)disX;
		dispenserZ = (int)disZ;

		DrawCube((Vector3){dispenserX * blockSizes, 130 * blockSizes, dispenserZ * blockSizes}, 0.1, 0.1, 0.1, RED);
		if(IsKeyPressed(80)){
			printf("%s\n", "Called Mesh Refresh");
			clearAllMeshes(meshArr, occupiedMesh);
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
		char chunkCntText[20];
		sprintf(sandCnts, "%s%d%s%d", "Visibile Sand Particles: ", mCounts[0], "/", 100 * WORLD_H * 100 / 60);
		sprintf(liveParts, "%s%d%s%d", "Update Queue Size: ", updateLength, "/", 100 * WORLD_H * 100);
		sprintf(chunkCntText, "%s%d%s%d", "Chunks Visible: ", chunkCnt, "/", (WORLD_W/CHUNK_SIZE)*(WORLD_Z/CHUNK_SIZE));
		DrawText(f, 10, 20, 10, RED);
		DrawText(sandCnts, 10, 40, 10, RED);
		DrawText(liveParts, 10, 80, 10, RED);
		DrawText(chunkCntText, 10, 120, 10, RED);
		
		EndDrawing();
	}
	RL_FREE(sandMeshData);

	return 0;
}
