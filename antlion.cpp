#include <raylib.h>
#include <raymath.h>
#include <chrono>
#include <iostream>

#include "world.h"

constexpr static int WIDTH = 1280;
constexpr static int HEIGHT = 720;

static float INV_AR = static_cast<float>(HEIGHT) / static_cast<float>(WIDTH);
static float FOV_D = 90.f;
static float FOV = PI / FOV_D;
static float FOV_TAN = std::tanf(FOV);
static float NEAR = 0.1f;
static float VIEWPORT_WIDTH = FOV_TAN * NEAR;
static float VIEWPORT_HEIGHT = VIEWPORT_WIDTH * INV_AR;

static unsigned int W_BUF[WIDTH * HEIGHT];
static Texture2D W_TEX;

block_t World::world[World::VOL];

static inline unsigned int cast_ray(Ray &ray) 
{
    Vector3 sgn = {
        std::copysignf(1.f, ray.direction.x),
        std::copysignf(1.f, ray.direction.y),
        std::copysignf(1.f, ray.direction.z)};

    Ray stepx, stepy, stepz;
    float tx, ty, tz, tstepx, tstepy, tstepz;

    stepx.position = ray.position;
    stepx.direction = Vector3{
        sgn.x, 
        ray.direction.y / fabsf(ray.direction.x), 
        ray.direction.z / fabsf(ray.direction.x)};

    tx = 0; 
    tstepx = sgn.x / ray.direction.x;

    stepy.position = ray.position;
    stepy.direction = Vector3{
        ray.direction.x / fabsf(ray.direction.y), 
        sgn.y, 
        ray.direction.z / fabsf(ray.direction.y)};

    ty = 0; 
    tstepy = sgn.y / ray.direction.y;

    stepz.position = ray.position;
    stepz.direction = Vector3{
        ray.direction.x / fabsf(ray.direction.z), 
        ray.direction.y / fabsf(ray.direction.z),
        sgn.z};

    tz = 0; 
    tstepz = sgn.z / ray.direction.z;

    for (; tz < 1000.f || tx < 1000.f || ty < 1000.f;
         tx += tstepx, ty += tstepy, tz += tstepz)
    {
        stepx.position = Vector3Add(stepx.position, stepx.direction);
        stepy.position = Vector3Add(stepy.position, stepy.direction);
        stepz.position = Vector3Add(stepz.position, stepz.direction);
    }

    float t_temp = fminf(tx, ty);
    unsigned int block_temp = tx < ty ? World::get(stepx.position) : World::get(stepy.position);

    return t_temp < tz ? block_temp : World::get(stepz.position);
}

static void clear_buf() 
{
    std::fill(W_BUF, W_BUF + WIDTH * HEIGHT, 0);
}

static void draw_frame() 
{
    Ray look_ray{};

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            look_ray.position = {};
            look_ray.direction.x = (2.f * (static_cast<float>(x) / static_cast<float>(WIDTH)) - 1.f) * VIEWPORT_WIDTH;
            look_ray.direction.y = (2.f * (static_cast<float>(y) / static_cast<float>(HEIGHT)) - 1.f) * VIEWPORT_HEIGHT;
            look_ray.direction.z = NEAR;
            look_ray.direction = Vector3Normalize(look_ray.direction);

            auto start = std::chrono::high_resolution_clock::now();
            unsigned int block = cast_ray(look_ray);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << std::chrono::duration<float, std::milli>(end - start).count() << "\n";

            if (block != BLOCK_AIR) {
                W_BUF[x + y * WIDTH] = 0xFFFFFF00;
            }
        }
    }

    UpdateTexture(W_TEX, W_BUF);
    DrawTexture(W_TEX, 0, 0, WHITE);
}

int main() {
    InitWindow(WIDTH, HEIGHT, "antlion");

    Image i;
    i.data = W_BUF;
    i.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    i.width = WIDTH;
    i.height = HEIGHT;
    i.mipmaps = 1;

    W_TEX = LoadTextureFromImage(i);

    UpdateTexture(W_TEX, i.data);

    //World::set({0, 0, 10}, BLOCK_GRASS);
    //World::set({1, 0, 10}, BLOCK_GRASS);
    //World::set({2, 0, 10}, BLOCK_GRASS);
    //World::set({3, 0, 10}, BLOCK_GRASS);
    //World::set({4, 0, 10}, BLOCK_GRASS);
    //World::set({5, 0, 10}, BLOCK_GRASS);

    while(!WindowShouldClose()) {
        PollInputEvents();
        BeginDrawing();

        clear_buf();
        draw_frame();

        EndDrawing();
    }

    UnloadTexture(W_TEX);
    CloseWindow();

    return 0;
}
