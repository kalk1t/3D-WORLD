#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdlib.h>
#include <time.h>

#include "city.c"

// Controls:
//  - WASD / Space / LeftCtrl: move
//  - Mouse: look
//  - Shift: sprint
//  - F: toggle wireframe
//  - R: reset camera
//  - Esc: quit

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Small City - raylib");

    // Camera setup (FPS style)
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 12.0f, 5.0f, 12.0f };
    camera.target = (Vector3){ 13.0f, 5.0f, 13.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(120);
    DisableCursor();   // FPS look
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);

    // City settings (you can tweak these)
    CityConfig cfg = {
        .seed = 1337,
        .blocksX = 6,         // number of blocks on X
        .blocksZ = 6,         // number of blocks on Z
        .blockSize = 20.0f,   // meters per block
        .roadWidth = 4.0f,    // meters
        .minBuilding = (Vector3){ 3.0f, 6.0f, 3.0f },
        .maxBuilding = (Vector3){ 8.0f, 20.0f, 8.0f },
        .parkChance = 0.15f   // chance that a lot is a park/green
    };

    City city = { 0 };
    GenerateCity(&city, cfg);

    bool wireframe = false;
    Vector3 spawn = (Vector3){ (cfg.blocksX * cfg.blockSize) * 0.5f, 3.0f, (cfg.blocksZ * cfg.blockSize) * 0.5f };

    while (!WindowShouldClose())
    {
        // Toggle wireframe
        if (IsKeyPressed(KEY_F)) wireframe = !wireframe;

        // Reset camera
        if (IsKeyPressed(KEY_R)) camera.position = (Vector3){ spawn.x, 5.0f, spawn.z };

        // FPS movement
        float dt = GetFrameTime();
        float speed = IsKeyDown(KEY_LEFT_SHIFT) ? 10.0f : 5.0f;
        Vector2 mouseDelta = GetMouseDelta();
        UpdateCameraPro(&camera,
            (Vector3) {
            (IsKeyDown(KEY_W) - IsKeyDown(KEY_S))* speed* dt,
                (IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_CONTROL))* speed* dt,
                (IsKeyDown(KEY_A) - IsKeyDown(KEY_D))* speed* dt
        },
            (Vector3) {
            mouseDelta.x * -0.1f, mouseDelta.y * -0.1f, 0.0f},0.0f);

        BeginDrawing();
        ClearBackground((Color) { 145, 193, 241, 255 }); // pleasant sky

        BeginMode3D(camera);

        // Ground plane (big)
        DrawPlane((Vector3) { cfg.blocksX* cfg.blockSize * 0.5f, 0.0f, cfg.blocksZ* cfg.blockSize * 0.5f },
            (Vector2) {
            cfg.blocksX* cfg.blockSize + 100.0f, cfg.blocksZ* cfg.blockSize + 100.0f
        },
            (Color) {
            205, 231, 255, 255
        }); // subtle tint

        // Draw roads
        for (int i = 0; i < city.roadCount; i++) {
            Road r = city.roads[i];
            Color asphalt = (Color){ 35, 35, 38, 255 };
            DrawCube(r.pos, r.size.x, r.size.y, r.size.z, asphalt);
        }

        // Road markings (simple dashed lines)
        for (int i = 0; i < city.markCount; i++) {
            Mark m = city.marks[i];
            DrawCube(m.pos, m.size.x, m.size.y, m.size.z, YELLOW);
        }

        // Sidewalks/curbs (light gray)
        for (int i = 0; i < city.sidewalkCount; i++) {
            Sidewalk s = city.sidewalks[i];
            DrawCube(s.pos, s.size.x, s.size.y, s.size.z, (Color) { 185, 185, 185, 255 });
        }

        // Parks (green quads)
        for (int i = 0; i < city.parkCount; i++) {
            Park p = city.parks[i];
            DrawCube(p.pos, p.size.x, p.size.y, p.size.z, (Color) { 81, 145, 86, 255 });
        }

        if (wireframe) rlEnableWireMode();

        // Buildings
        for (int i = 0; i < city.buildingCount; i++) {
            Building b = city.buildings[i];
            // subtle color variety
            Color base = (Color){ (unsigned char)(180 + (b.colorVar % 50)),
                                  (unsigned char)(180 + (b.colorVar % 40)),
                                  (unsigned char)(180 + (b.colorVar % 30)), 255 };
            DrawCube(b.pos, b.size.x, b.size.y, b.size.z, base);

            // simple darker top to simulate roof
            DrawCube((Vector3) { b.pos.x, b.pos.y + b.size.y * 0.5f + 0.05f, b.pos.z },
                b.size.x, 0.1f, b.size.z, (Color) { 60, 60, 60, 255 });
        }

        if (wireframe) rlDisableWireMode();

        // Draw axes at origin
        DrawGrid(40, 1.0f);

        EndMode3D();

        // UI
        DrawRectangle(10, 10, 360, 94, Fade(BLACK, 0.5f));
        DrawText("Small City (raylib)", 20, 20, 20, RAYWHITE);
        DrawText("- WASD/Space/Ctrl to move, Mouse to look", 20, 46, 18, RAYWHITE);
        DrawText("- Shift sprint  |  F wireframe  |  R reset  |  Esc quit", 20, 68, 18, RAYWHITE);

        DrawFPS(GetScreenWidth() - 90, 10);

        EndDrawing();
    }

    UnloadCity(&city);
    CloseWindow();
    return 0;
}
