#pragma once

namespace video {
    extern int WIDTH;
    extern int HEIGHT;
    extern float INV_AR;
    extern float FOV_D;
    extern float FOV;
    extern float FOV_TAN;
    extern float NEAR;
    extern float VIEWPORT_WIDTH;
    extern float VIEWPORT_HEIGHT;
}

#ifdef VIDEO_IMPL
int video::WIDTH = 1280;
int video::HEIGHT = 720;
float video::INV_AR = static_cast<float>(video::HEIGHT) / static_cast<float>(video::WIDTH);
float video::FOV_D = 90.f;
float video::FOV = PI / 360.f * video::FOV_D;
float video::FOV_TAN = std::tanf(video::FOV);
float video::NEAR = 0.1f;
float video::VIEWPORT_WIDTH = 2.f * video::FOV_TAN * video::NEAR;
float video::VIEWPORT_HEIGHT = video::VIEWPORT_WIDTH * video::INV_AR;
#endif