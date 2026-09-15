#include <raylib.h>
#include <iostream>
#include <raymath.h>

//we trust raylib loads all the function pointers
#include "glad.h"

#define WORLD_IMPL
#include "world.h"

#define VIDEO_IMPL
#include "video.h"

#define DEBUG_LOG

int main() 
{
    World::init_world();
    InitWindow(video::WIDTH, video::HEIGHT, "antlion");
    DisableCursor();

    Shader voxel_shader = LoadShader(NULL, "voxel.frag");

    GLuint world_tex;
    glGenTextures(1, &world_tex);
    glBindTexture(GL_TEXTURE_3D, world_tex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, World::WIDTH, World::WIDTH,
                    World::HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, World::world);

    int view, model;
    {
        int v_width, v_height, v_viewport_width, v_viewport_height;

        view = GetShaderLocation(voxel_shader, "view");
        model = GetShaderLocation(voxel_shader, "model");
        v_width = GetShaderLocation(voxel_shader, "v_width");
        v_height = GetShaderLocation(voxel_shader, "v_height");
        v_viewport_width = GetShaderLocation(voxel_shader, "v_viewport_width");
        v_viewport_height = GetShaderLocation(voxel_shader, "v_viewport_height");

        SetShaderValueMatrix(voxel_shader, model, MatrixIdentity());
        SetShaderValueMatrix(voxel_shader, view, MatrixIdentity());
        SetShaderValue(voxel_shader, v_width, &video::WIDTH, SHADER_UNIFORM_INT);
        SetShaderValue(voxel_shader, v_height, &video::HEIGHT, SHADER_UNIFORM_INT);
        SetShaderValue(voxel_shader, v_viewport_width, &video::VIEWPORT_WIDTH, SHADER_UNIFORM_FLOAT);
        SetShaderValue(voxel_shader, v_viewport_height, &video::VIEWPORT_HEIGHT, SHADER_UNIFORM_FLOAT);
    }

    Camera3D cam{
        Vector3{World::WIDTH_H, World::HEIGHT_H, World::WIDTH_H},
        Vector3{World::WIDTH_H, World::HEIGHT_H, World::WIDTH_H + 1},
        Vector3{0.f, 1.f, 0.f},
        video::FOV,
        CAMERA_PERSPECTIVE};
    
    Vector2 mouse{}, prev_mouse{};

    mouse = GetMousePosition();

    while(!WindowShouldClose()) {
        PollInputEvents();
        
        //input
        mouse = GetMousePosition();
        Vector2 m_del = (mouse - prev_mouse) * 0.5;
        prev_mouse = mouse;
        Vector3 rotation{m_del.x, m_del.y, 0};

        float forwardback = 0.f;
        float rightleft = 0.f;
        float updown = 0.0f;

        if (IsKeyDown(KEY_W)) {
            forwardback += 1.f;
        }
        if (IsKeyDown(KEY_S)) {
            forwardback -= 1.f;
        }
        if (IsKeyDown(KEY_A)) {
            rightleft += 1.f;
        }
        if (IsKeyDown(KEY_D)) {
            rightleft -= 1.f;
        }
        if (IsKeyDown(KEY_Q)) {
            updown += 1.f;
        }
        if (IsKeyDown(KEY_E)) {
            updown -= 1.f;
        }

        Vector3 forward = Vector3Normalize(cam.target - cam.position);
        Vector3 right = Vector3CrossProduct(Vector3{0.f, 1.f, 0.f}, forward);
        
        Vector3 delta = forward * forwardback + right * rightleft + Vector3{0.f, 1.f, 0.f} * updown;
        delta = Vector3Normalize(delta) * GetFrameTime() * 30.f;

        cam.position += delta;
        cam.target += delta;
        UpdateCameraPro(&cam, Vector3{}, rotation, 0.f);
        //input

        Matrix v = GetCameraMatrix(cam);
        SetShaderValueMatrix(voxel_shader, view, v);

        BeginDrawing();

        ClearBackground(WHITE);
        BeginShaderMode(voxel_shader);
        DrawRectangle(0, 0, video::WIDTH, video::HEIGHT, WHITE);

        EndShaderMode();
        EndDrawing();
    }

    glDeleteTextures(1, &world_tex);
    EnableCursor();
    CloseWindow();

    return 0;
}