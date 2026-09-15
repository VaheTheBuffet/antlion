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

    {
        int model, view, v_width, v_height, v_viewport_width, v_viewport_height;

        model = GetShaderLocation(voxel_shader, "model");
        view = GetShaderLocation(voxel_shader, "view");
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

    while(!WindowShouldClose()) {
        PollInputEvents();
        BeginDrawing();

        ClearBackground(WHITE);
        BeginShaderMode(voxel_shader);
        DrawRectangle(0, 0, video::WIDTH, video::HEIGHT, WHITE);

        EndShaderMode();
        EndDrawing();
    }

    glDeleteTextures(1, &world_tex);
    CloseWindow();

    return 0;
}