#include "raylib.h"
#include "raymath.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // Optional: more logs
    // SetTraceLogLevel(LOG_DEBUG);

    InitWindow(1280, 720, "NeoCity");

    // Load model
    Model city = LoadModel("neocity/neocity.obj");
	Texture2D cityTexture = LoadTexture("neocity/KB3D_NEC_Wood_basecolor.png");
	city.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cityTexture;
   
	Camera cam = { 0 };
	cam.position = (Vector3){ 50.0f, 50.0f, 50.0f };
	cam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	cam.fovy = 90.0f;
	cam.projection = CAMERA_PERSPECTIVE;

	Vector3 pos = { 0.0f,0.0f,0.0f };
	BoundingBox bounds = GetMeshBoundingBox(city.meshes[0]);
		SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		// Update
		//pos = Vector3Add(pos, (Vector3){ 0.0f, 0.0f, 0.1f });
		//cam.position = pos;
		// Draw
		BeginDrawing();
			ClearBackground(RAYWHITE);
			BeginMode3D(cam);
				DrawModel(city, pos, 1.0f, WHITE);
				DrawBoundingBox(bounds, RED);
			EndMode3D();
			DrawText("NeoCity Model", 10, 10, 20, DARKGRAY);
		EndDrawing();
	}
	UnloadTexture(cityTexture);
	UnloadModel(city);
	CloseWindow();
	return 0;

}
