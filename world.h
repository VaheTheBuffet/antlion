#pragma once

#include <raylib.h>
#include <assert.h>
#include "block.h"

namespace World {
    constexpr inline int WIDTH = 1000;
    constexpr inline int HEIGHT = 200;
    constexpr inline int AREA = (WIDTH * WIDTH);
    constexpr inline int VOL = (AREA * HEIGHT);
    constexpr inline int WIDTH_H = (WIDTH / 2);
    constexpr inline int HEIGHT_H = (HEIGHT / 2);

    extern block_t world[VOL];

    inline block_t get_unchecked(int x, int y, int z) 
    {
        return world[x + z * WIDTH + y * AREA];
    }

    inline block_t get(int x, int y, int z) 
    {
        if (x < 0 || x >= WIDTH)               
            return B_NULL;

        if (z < 0 || z >= WIDTH)
            return B_NULL;

        if (y < 0 || y >= HEIGHT)
            return B_NULL;                 

        return get_unchecked(x, y, z);
    }

    inline block_t get(const Vector3& p)
    {
        if (p.x < 0.f || p.x >= WIDTH)
            return B_NULL;

        if (p.z < 0.f || p.z >= WIDTH)
            return B_NULL;

        if (p.y < 0.f || p.y >= HEIGHT)
            return B_NULL;

        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);
        int z = static_cast<int>(p.z);

        return get_unchecked(x, y, z);
    }

    inline void set(int x, int y, int z, block_t b)
    {
        assert(x >= 0 && x < WIDTH);
        assert(z >= 0 && z < WIDTH);
        assert(y >= 0 && y < HEIGHT);

        world[x + z * WIDTH + y * AREA] = b;
    }

    inline void set(const Vector3& p, block_t b) 
    {
        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);
        int z = static_cast<int>(p.z);

        return set(x, y, z, b);
    }

    void init_world();
}

#ifdef WORLD_IMPL
block_t World::world[World::VOL];

void World::init_world() 
{
    std::fill(world, world + VOL, B_AIR);

    for (int y = HEIGHT_H; y < HEIGHT_H + 100; ++y) {
        for (int x = WIDTH_H; x < WIDTH_H + 100; ++x) {
            World::set(x, y, WIDTH_H + 300, B_GRASS);
        }
    }
}
#endif