// Author: Alex Hartford
// Program: Simp
// File: Spritesheet
// Date: April 2023

#ifndef SHEET_H
#define SHEET_H

struct Sheet
{
    int width;
    int height;

    int frame = 0;
    int frames;

    float speed = 0.1f; // frames per second
    float time = 0.0f;

    Sheet(int width_in, int height_in)
    : width(width_in),
      height(height_in)
    {
        frames = width * height;
    }

    void Update(float delta_time)
    {
        time += delta_time;
        if(!(time > (FRAMETIME / speed))) return;

        time = 0.0f;

        ++frame;
        if(frame >= frames)
            frame = 0;
    }

    vec2 GetFrameOffset(int frame_in) const
    {
        int x = frame_in % width;
        int y = frame_in / width;
        return vec2(x, y);
    }
};

#endif
