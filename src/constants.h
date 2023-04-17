// Author: Alex Hartford
// Program: Experience
// File: Constants
// Date: March 2023

#ifndef CONSTANTS_H
#define CONSTANTS_H

const float PI = 3.1415f;
const float EPS = 0.0001f;

const unsigned int FRAMERATE = 60;
const float FRAMETIME = 1.0f / (float)FRAMERATE;

const unsigned int SCREEN_WIDTH = 1000;
const unsigned int SCREEN_HEIGHT = 600;

const unsigned int PIXEL_SCREEN_WIDTH = 180;
const unsigned int PIXEL_SCREEN_HEIGHT = 100;

const string level_path  = "../data/";
const string serial_path = "../data/";

const string shader_path = "../shaders/";
const string model_path = "../assets/models/";

const string skybox_path = "../assets/skyboxes/";
const string heightmap_path = "../assets/heightmaps/";
const string texture_path = "../assets/textures";

const string music_path = "../assets/audio/music/";
const string sfx_path = "../assets/audio/sfx/";

#define DEFAULT_LEVEL "level.txt"
#define DEFAULT_TERRAIN "dunes.png"
#define DEFAULT_SKYBOX "desert/"
#define DEFAULT_TEXTURE "missing.png"

const float DEFAULT_MOVEMENT_SPEED = 50.0f;
const float SLOW_MOVEMENT_SPEED = 10.0f;
const float DEFAULT_MOUSE_SENSITIVITY = 0.10f;
const float LOOK_MODE_SENSITIVITY = 0.20f;
const float DEFAULT_MOUSE_ZOOM = 45.0f;

const float DEFAULT_MUSIC_VOLUME = 1.0f;
const float DEFAULT_AMBIENCE_VOLUME = 0.1f;
const float DEFAULT_SFX_VOLUME = 1.0f;

#define CLIP_NOTHING vec4(0.0f, 1.0f, 0.0f, 100000)


#endif
