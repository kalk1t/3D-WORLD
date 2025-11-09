#include "raylib.h"

int main(void) {
    InitWindow(1280, 720, "3D sanity");
    SetTargetFPS(60);

    Camera3D cam = { 0 };
    cam.position = (Vector3){ 8, 6, 8 };
    cam.target = (Vector3){ 0, 1, 0 };
    cam.up = (Vector3){ 0, 1, 0 };
    cam.fovy = 60;
    cam.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode3D(cam);
        DrawGrid(20, 1.0f);
        DrawCube((Vector3) { 0, 1, 0 }, 1, 1, 1, RED);
        DrawCubeWires((Vector3) { 0, 1, 0 }, 1, 1, 1, BLACK);
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
