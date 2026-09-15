#version 330 core

#define W_WIDTH 1000
#define W_HEIGHT 200
#define W_AREA (W_WIDTH * W_WIDTH)
#define W_VOL (W_AREA * W_HEIGHT)
#define W_WIDTH_H (W_WIDTH / 2)
#define W_HEIGHT_H (W_HEIGHT / 2)

#define NEAR 0.1f

#define B_NULL 0u
#define B_AIR 1u
#define B_GRASS 2u
#define B_LEN 3u

#define EPS 0.001f

#define block_t uint

struct Ray {
    vec3 p;
    vec3 d;
};

uniform usampler3D world;
uniform mat4 model;
uniform mat4 view;
uniform int v_width;
uniform int v_height;
uniform float v_viewport_width;
uniform float v_viewport_height;

out vec4 out_color;

#define fast_floor(f) (float(int(f - 0.001f)))
#define fast_ciel(f) (float(int(f + 1.f)))

block_t cast_ray(Ray ray)
{
    int x, y, z, dx, dy, dz;
    float tx, ty, tz, stepx, stepy, stepz;

    x = int(ray.p.x);
    y = int(ray.p.y);
    z = int(ray.p.z);

    dx = ray.d.x > 0.f ? 1 : -1;
    dy = ray.d.y > 0.f ? 1 : -1;
    dz = ray.d.z > 0.f ? 1 : -1;

    stepx = 1.f / (EPS + abs(ray.d.x));
    stepy = 1.f / (EPS + abs(ray.d.y));
    stepz = 1.f / (EPS + abs(ray.d.z));

    tx = abs((ray.p.x - (dx < 0 ? fast_floor(ray.p.x) : fast_ciel(ray.p.x))) * stepx);
    ty = abs((ray.p.y - (dy < 0 ? fast_floor(ray.p.y) : fast_ciel(ray.p.y))) * stepy);
    tz = abs((ray.p.z - (dz < 0 ? fast_floor(ray.p.z) : fast_ciel(ray.p.z))) * stepz);

    do {
        float tmin = min(tx, min(ty, tz));

        block_t b = texelFetch(world, ivec3(x, z, y), 0).r;
        b = x < 0 || x > W_WIDTH ? B_NULL : b;
        b = y < 0 || y > W_HEIGHT ? B_NULL : b;
        b = z < 0 || z > W_WIDTH ? B_NULL : b;

        if (b != B_AIR)
            return b;

        if (tmin > 400.f) 
            return B_NULL;
        
        bool is_x = tmin == tx;
        bool is_y = tmin == ty;
        bool is_z = tmin == tz;

        x += is_x ? dx : 0;
        tx += is_x ? stepx : 0.f;

        y += is_y ? dy : 0;
        ty += is_y ? stepy : 0.f;

        z += is_z ? dz : 0;
        tz += is_z ? stepz : 0.f;

    } while (true);

    return B_NULL;
}

#undef fast_floor
#undef fast_ciel

void main() 
{
    Ray look_ray;
    look_ray.p.x = W_WIDTH_H;
    look_ray.p.y = W_HEIGHT_H;
    look_ray.p.z = W_WIDTH_H;

    look_ray.d.xy = gl_FragCoord.xy;
    look_ray.d.xy /= vec2(v_width, v_height);
    look_ray.d.xy -= 0.5f;
    look_ray.d.xy *= vec2(v_viewport_width, v_viewport_height);
    look_ray.d.z = NEAR;
    //look_ray.d = (vec4(look_ray.d, 1.f) * model * view).xyz;
    look_ray.d /= length(look_ray.d);

    block_t block = cast_ray(look_ray);

    out_color = float(block) * vec4(0.5, 0.5, 0.5, 1.0);
    out_color.a = 1.0;
}