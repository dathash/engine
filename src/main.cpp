// Author: Alex Hartford
// Program: Engine
// File: Main
// Date: March 2023

// =============================== Outside Includes ============================
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <stack>

// gl
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

using namespace std;
using namespace glm;

// ======================================== Globals ============================
#include "constants.h"

static GLFWwindow *window = nullptr;

enum Program_Mode {
    GAME,
    EDITOR,
};
static Program_Mode GlobalMode = GAME;

static bool GlobalDepthBufferDebug = false;

// ============================ Project Includes ===============================
#include "utils.h"
#include "audio.h" // NOTE: Includes Global Audio engine and Sound groups, as well as GlobalMusic and GlobalSfx.
#include "camera.h"
Camera camera;
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool  firstMouse = true;


enum EditorMode
{
    MODE_DEFAULT,
    MODE_LOOK,
};

struct EditorState
{
    EditorMode mode = MODE_DEFAULT;
    int selected    = -1;
    vec2 mouse_pos;
};
static EditorState GlobalEditorState;
#include "input.h"
#include "init.h"
#include "shader.h"
#include "fbo.h"
#include "interp.h"
#include "curve.h"
#include "model.h"
#include "object.h"
#include "terrain.h"
#include "water.h"
#include "shadow.h"
#include "skybox.h"
#include "fog.h"
#include "serial.h"
#include "level.h"
//#include "light.h"

#include "render.h"
#include "command.h"
#include "debug.h"
#include "editor.h"

// ======================================= The Main Function ===================
int main(void)
{
    srand(time(NULL));
    Init();

    // ============================= High-level constructs =====================
    EditorCommands editor_commands;

    RenderContext context =
    {
        .shader_index       = 6,
        .light_direction    = normalize(vec3(0.0f, -1.0f, 0.0f)),
        .default_color      = vec4(1.0f, 0.0f, 0.0f, 1.0f),
        .background_color   = vec4(0.61f, 0.86f, 0.93f, 1.0f),
        .brightness         = 1.0f,
        .opacity            = 1.0f,
        .cel_threshold_high = 0.7f,
        .cel_threshold_mid  = 0.3f,
        .cel_threshold_low  = 0.0f,

        .skybox             = false,
        .processing         = false,
        .water              = true,
        .shadow             = false,
    };
    Framebuffer framebuffer;

    Shaders shaders =
    {
        .list = 
        {
            Shader("Basic",     "basic.vs", "basic.fs"),
            Shader("Normals",   "normals.vs", "normals.fs"),
            Shader("Height",    "normals.vs", "height.fs"),
            Shader("Material",  "normals.vs", "mat.fs"),
            Shader("Texture",   "tex.vs", "tex.fs"),
            Shader("Cel",       "tex.vs", "cel.fs"),
            Shader("Main",      "tex.vs", "main.fs"),
        },
        .quad_shader = Shader("Quad", "quad.vs", "quad.fs"),
        .processing_shader = Shader("Processing", "quad.vs", "post.fs"),
        .cube_shader = Shader("Cube", "cube.vs", "cube.fs"),
        .terrain_shader = Shader("Terrain", "terrain.vs", "terrain.fs"),
        .water_shader = Shader("Water", "water.vs", "water.fs"),
        .depth_shader = Shader("Depth", "depth.vs", "depth.fs"),
        .line_shader = Shader("Line", "line.vs", "line.fs"),
    };


    // ============================== Geometry-level stuff =====================
    vector<Model> models =
        {
            Model("porygon/porygon.obj"),
            Model("sphere.obj"),
            Model("brain.obj"),
            Model("skull/skull.obj"),
            Model("fox/fox.fbx"),
            Model("andre/andre.fbx"),
            Model("hornet/FA-18E_SuperHornet.obj"),
        };

    vector<Object> palette = 
        {
            Object("Porygon", &models[0]),
            Object("Sphere",  &models[1]),
            Object("Brain",   &models[2]),
            Object("Skull",   &models[3]),
            Object("Fox",     &models[4]),
            Object("Andre",   &models[5]),
            Object("Hornet",  &models[6]),
        };

    vector<Terrain> terrains =
        {
            Terrain("Dunes", "dunes.png", "dirt.png",  "redgrass.png"),
            Terrain("Snow",  "lake.png", "white.png", "redgrass.png"),
            Terrain("Lake",  "lake.png",  "grass.png", "dirt.png"),
        };

    vector<Skybox> skyboxes =
        {
            Skybox("Desert Sky", "desert/"),
            Skybox("Night Sky",  "night/"),
            Skybox("Cloudy Sky", "sun/"),
            Skybox("White Sky", "white/"),
        };

    vector<Fog> fogs =
        {
            {"Light Fog", 200, 50, vec3(1.0f, 1.0f, 1.0f)},
            {"Heavy Fog", 80, 20, vec3(0.5f, 0.5f, 0.5f)},
            {"Standard Fog", 100, 30, vec3(1.0f, 1.0f, 1.0f)},
            {"Black Fog", 100, 30, vec3(0.0f, 0.0f, 0.0f)},
        };

    //vector<Light> lights;

    Level level = LoadLevel("level.txt", palette, terrains, skyboxes, fogs);
    Water water = Water(2.0f);
    Shadow shadow = Shadow();

    Line line = {{
                  vec3(0.0f,   30.0,   2.0f),
                  vec3(10.0f,  30.0f, -2.0f),
                  vec3(20.0f,  30.0f,  2.0f),
                  vec3(30.0f,  30.0f, -2.0f),
                  vec3(40.0f,  30.0f,  2.0f),
                  vec3(50.0f,  30.0f, -2.0f),
                  vec3(60.0f,  30.0f,  2.0f),
                  vec3(70.0f,  30.0f, -2.0f),
                  vec3(80.0f,  30.0f,  2.0f),
                  vec3(90.0f,  30.0f, -2.0f),
                  vec3(0.0f,   30.0f, -2.0f),
                }};

    vector<vec3> splinepoints;
    spline(splinepoints, line.points, 10, 1.0);
    Line smooth(splinepoints);

    GlobalMusic.sounds =
    {
        new Sound("bach.mp3", MUSIC),
        new Sound("wasteland.mp3", MUSIC),
        new Sound("wind.wav", MUSIC),
        new Sound("bird.wav", MUSIC),
    };

    GlobalSfx.sounds =
    {
        new Sound("fire.mp3", SFX),
        new Sound("cricket.wav", SFX),
        new Sound("fog.wav", SFX),
    };

    StartMusic("wind.wav");
    StartMusic("bird.wav");
    StartMusic("wasteland.mp3");

    // =============================== FBO STUFF ===============================
    float lastFrame  = 0.0f;
    float delta_time;
    float time;
    while (!glfwWindowShouldClose(window))
    {
        time = Time();
        delta_time = time - lastFrame;
        lastFrame = time;

        // Input Resolution
        glfwPollEvents();
        processInput(delta_time);

        if(GlobalMode == GAME)
        {
            float player_height = 4.0f;
            camera.ClampPosition(vec2(-(level.terrain.height * 0.5f) + 2, -(level.terrain.width * 0.5f) + 2), 
                                 vec2(level.terrain.height * 0.5f - 2, level.terrain.width * 0.5f - 2));
            camera.position.y = level.terrain.HeightAt(camera.position.x, camera.position.z) + player_height;
        }

        // Simulation
        for(Object &object : level.objects)
            object.Update(delta_time, smooth);

        // Render
        Render(context, shaders, level, water, shadow, framebuffer, smooth);

        // Editor
        if(GlobalMode == EDITOR) {
            editor_commands.UpdateCommands(&level);
            editor_commands.HandleInput();
            Editor(&editor_commands, &context, palette, 
                   terrains, skyboxes, fogs, shaders, 
                   &level, &shadow);
        }

        // Present
        glfwSwapBuffers(window);
        //CheckOpenGLErrors();
    }

    for(Sound *sound : GlobalMusic.sounds)
        delete sound;
    for(Sound *sound : GlobalSfx.sounds)
        delete sound;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
