#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include <assert.h>

enum block_t {
    BLOCK_AIR = 0u,
    BLOCK_GRASS,

    BLOCK_LEN,
};

namespace World {
    constexpr static int WIDTH = 1000;
    constexpr static int HEIGHT = 200;
    constexpr static int AREA = WIDTH * WIDTH;
    constexpr static int VOL = AREA * HEIGHT;

    extern block_t world[VOL];

    static inline block_t get(const Vector3& p) {
        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);
        int z = static_cast<int>(p.z);

        if (x < 0 || x >= WIDTH)
            return BLOCK_AIR;

        if (z < 0 || z >= WIDTH)
            return BLOCK_AIR;

        if (y < 0 || y >= HEIGHT)
            return BLOCK_AIR;

        return world[x + z * WIDTH + y * AREA];
    }

    static inline void set(const Vector3& p, block_t block) {
        int x = static_cast<int>(p.x);
        int y = static_cast<int>(p.y);
        int z = static_cast<int>(p.z);

        assert(x >= 0 && x < WIDTH);
        assert(z >= 0 && z < WIDTH);
        assert(y >= 0 && y < HEIGHT);

        world[x + z * WIDTH + y * AREA] = block;
    }
}

#endif