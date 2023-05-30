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
//#include <Windows.h>

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
#include "animation.h"
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
#include "gen.h"
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
        .default_color      = vec4(0.5f, 0.5f, 0.5f, 1.0f),
        .background_color   = vec4(0.61f, 0.86f, 0.93f, 1.0f),
        .use_default_color  = false,
        .brightness         = 1.0f,
        .opacity            = 1.0f,
        .cel_threshold_high = 0.7f,
        .cel_threshold_mid  = 0.3f,
        .cel_threshold_low  = 0.0f,

        .draw_lines         = true,

        .skybox             = true,
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
            Shader("Main",      "anim.vs", "main.fs"),
        },
        .quad_shader = Shader("Quad", "quad.vs", "quad.fs"),
        .processing_shader = Shader("Processing", "quad.vs", "post.fs"),
        .cube_shader = Shader("Cube", "cube.vs", "cube.fs"),
        .heightmap_shader = Shader("heightmap", "heightmap.vs", "heightmap.fs"),
        .water_shader = Shader("Water", "water.vs", "water.fs"),
        .depth_shader = Shader("Depth", "depth.vs", "depth.fs"),
        .line_shader = Shader("Line", "line.vs", "line.fs"),
    };

    // ============================== Geometry-level stuff =====================
    vector<Model> models =
        {
    /*00*/  Model("trees/t1.dae", "normal.png"),
    /*01*/  Model("trees/t1.dae", "cold.png"),
    /*02*/  Model("trees/t2.dae", "normal.png"),
    /*03*/  Model("trees/t2.dae", "cold.png"),
    /*04*/  Model("trees/t3.dae", "normal.png"),
    /*05*/  Model("trees/t3.dae", "cold.png"),
    /*06*/  Model("trees/t4.dae", "normal.png"),
    /*07*/  Model("trees/t4.dae", "cold.png"),
    /*08*/  Model("trees/t5.dae", "normal.png"),
    /*09*/  Model("Ankylosaurus/ank.fbx", "Textures/ankyloColor.jpg"),
    /*10*/  Model("low_poly/finch.fbx", "Nature_Texture.png"),
    /*11*/  Model("low_poly/bee.fbx", "Nature_Texture.png"),
    /*12*/  Model("low_poly/bird.fbx", "Nature_Texture.png"),
    /*13*/  Model("low_poly/butterfly.fbx", "Nature_Texture.png"),
    /*14*/  Model("low_poly/firefly.fbx", "Nature_Texture.png"),
    /*15*/  Model("low_poly/fish.fbx", "Nature_Texture.png"),
    /*16*/  Model("low_poly/frog.fbx", "Nature_Texture.png"),
    /*17*/  Model("low_poly/skimmer.fbx", "Nature_Texture.png"),
    /*18*/  Model("low_poly/turtle.fbx", "Nature_Texture.png"),
    /*19*/  Model("trees/t6.fbx", "normal.png"),
    /*20*/  Model("trees/t7.fbx", "normal.png"),
    /*21*/  Model("trees/t8.fbx", "normal.png"),
    /*22*/  Model("trees/t9.fbx", "normal.png"),
    /*23*/  Model("trees/t10.fbx", "normal.png"),
    /*24*/  Model("nature/feather.fbx", "texture.png"),
    /*25*/  Model("nature/grass.fbx", "texture.png"),
    /*26*/  Model("nature/lilypad.fbx", "texture.png"),
    /*27*/  Model("nature/lilyblossom.fbx", "texture.png"),
    /*28*/  Model("nature/mushroom.fbx", "texture.png"),
    /*29*/  Model("nature/rock5.fbx", "texture.png"),
    /*30*/  Model("flowers/f1.fbx", "texture.png"),
    /*31*/  Model("flowers/f2.fbx", "texture.png"),
    /*32*/  Model("flowers/f3.fbx", "texture.png"),
    /*33*/  Model("flowers/f4.fbx", "texture.png"),
    /*34*/  Model("flowers/f5.fbx", "texture.png"),
    /*35*/  Model("flowers/f6.fbx", "texture.png"),
    /*36*/  Model("flowers/f7.fbx", "texture.png"),
    /*37*/  Model("flowers/f8.fbx", "texture.png"),
    /*38*/  Model("flowers/f9.fbx", "texture.png"),
    /*39*/  Model("flowers/f10.fbx", "texture.png"),
    /*40*/  Model("flowers/f11.fbx", "texture.png"),
    /*41*/  Model("flowers/f12.fbx", "texture.png"),
    /*42*/  Model("flowers/f13.fbx", "texture.png"),
    /*43*/  Model("flowers/f14.fbx", "texture.png"),
    /*44*/  Model("flowers/f15.fbx", "texture.png"),
    /*45*/  Model("flowers/f16.fbx", "texture.png"),
    /*46*/  Model("flowers/f17.fbx", "texture.png"),
    /*47*/  Model("flowers/f18.fbx", "texture.png"),
    /*48*/  Model("flowers/f19.fbx", "texture.png"),
    /*49*/  Model("nest/nest.obj", "nest.png"),
    /*50*/  Model("cave/cave.obj"),
    /*51*/  Model("pond/pond.obj", "pond.png"),
    /*52*/  Model("waterfall/waterfall.obj", "waterfall.png"),
        };

    vector<Object> palette = 
        {
    /*00*/  Object("t1", &models[0], false, 5.0f),
    /*01*/  Object("t1a", &models[1], false, 5.0f),
    /*02*/  Object("t2", &models[2], false, 2.0f),
    /*03*/  Object("t2a", &models[3], false, 2.5f),
    /*04*/  Object("t3", &models[4], false, 2.0f),
    /*05*/  Object("t3a", &models[5], false, 2.5f),
    /*06*/  Object("t4", &models[6], false, 2.0f),
    /*07*/  Object("t4a", &models[7], false, 2.5f),
    /*08*/  Object("t5", &models[8], false, 2.5f),
    /*09*/  Object("Ankylosaurus", &models[9], true),
    /*10*/  Object("Finch",   &models[10], true),
    /*11*/  Object("Bee",     &models[11], true),
    /*12*/  Object("Bird",    &models[12], true),
    /*13*/  Object("Butterfly", &models[13], true),
    /*14*/  Object("Firefly", &models[14], true),
    /*15*/  Object("Fish",    &models[15], true),
    /*16*/  Object("Frog",    &models[16], true),
    /*17*/  Object("Skimmer", &models[17], true),
    /*18*/  Object("Turtle",  &models[18], true),
    /*19*/  Object("t6",  &models[19], false, 2.0f),
    /*20*/  Object("t7",  &models[20], false, 2.0f),
    /*21*/  Object("t8",  &models[21], false, 2.0f),
    /*22*/  Object("t9",  &models[22], false, 2.0f),
    /*23*/  Object("t10",  &models[23], false, 2.0f),
    /*24*/  Object("feather", &models[24]),
    /*25*/  Object("grass", &models[25]),
    /*26*/  Object("lily pad", &models[26]),
    /*27*/  Object("lily blossom", &models[27]),
    /*28*/  Object("mushroom", &models[28]),
    /*29*/  Object("rocks", &models[29]),
    /*30*/  Object("flower 1", &models[30], false, 1.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*31*/  Object("flower 2", &models[31], false, 2.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*32*/  Object("flower 3", &models[32], false, 1.5f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*33*/  Object("flower 4", &models[33], false, 1.2f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*34*/  Object("flower 5", &models[34], false, 2.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*35*/  Object("flower 6", &models[35], false, 4.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*36*/  Object("flower 7", &models[36], false, 3.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*37*/  Object("flower 8", &models[37], false, 1.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*38*/  Object("flower 9", &models[38], false, 3.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*39*/  Object("flower 10", &models[39], false, 1.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*40*/  Object("flower 11", &models[40], false, 1.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*41*/  Object("flower 12", &models[41], false, 2.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*42*/  Object("flower 13", &models[42], false, 2.5f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*43*/  Object("flower 14", &models[43], false, 1.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*44*/  Object("flower 15", &models[44], false, 0.5f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*45*/  Object("flower 16", &models[45], false, 1.5f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*46*/  Object("flower 17", &models[46], false, 0.3f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*47*/  Object("flower 18", &models[47], false, 3.0f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*48*/  Object("flower 19", &models[48], false, 0.1f, angleAxis(radians(-90.0f), vec3(1, 0, 0))),
    /*49*/  Object("Nest", &models[49]),
    /*50*/  Object("Cave", &models[50]),
    /*51*/  Object("Pond", &models[51]),
    /*52*/  Object("Waterfall", &models[52]),
        };

    vector<Animation> anims =
        {
    Animation(model_path + "low_poly/finch_curious.fbx", &models[10]),
    Animation(model_path + "low_poly/finch_peck.fbx", &models[10]),
    Animation(model_path + "low_poly/finch_flap.fbx", &models[10]),
    Animation(model_path + "low_poly/finch_jump.fbx", &models[10]),
    Animation(model_path + "low_poly/finch_peck.fbx", &models[10]),

    Animation(model_path + "low_poly/bee_flight.fbx", &models[11]),
    Animation(model_path + "low_poly/bee_idle.fbx", &models[11]),
    Animation(model_path + "low_poly/bee_landing.fbx", &models[11]),
    Animation(model_path + "low_poly/bee_nudge.fbx", &models[11]),
    Animation(model_path + "low_poly/bee_walk.fbx", &models[11]),

    Animation(model_path + "low_poly/bird_curious.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_flap.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_flight.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_hoppin.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_idle.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_landing.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_loop.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_over.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_peck.fbx", &models[12]),
    Animation(model_path + "low_poly/bird_scanning.fbx", &models[12]),

    Animation(model_path + "low_poly/butterfly_big_flap.fbx", &models[13]),
    Animation(model_path + "low_poly/butterfly_flap.fbx", &models[13]),
    Animation(model_path + "low_poly/butterfly_idle_high.fbx", &models[13]),
    Animation(model_path + "low_poly/butterfly_idle_low.fbx", &models[13]),

    Animation(model_path + "low_poly/firefly_click.fbx", &models[14]),
    Animation(model_path + "low_poly/firefly_prep.fbx", &models[14]),
    Animation(model_path + "low_poly/firefly_suspicious.fbx", &models[14]),
    Animation(model_path + "low_poly/firefly_walk.fbx", &models[14]),

    Animation(model_path + "low_poly/fish_down.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_flip.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_spin.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_stay_up.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_stop.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_swim.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_swim2.fbx", &models[15]),
    Animation(model_path + "low_poly/fish_up.fbx", &models[15]),

    Animation(model_path + "low_poly/frog_alert.fbx", &models[16]),
    Animation(model_path + "low_poly/frog_curious.fbx", &models[16]),
    Animation(model_path + "low_poly/frog_idle.fbx", &models[16]),
    Animation(model_path + "low_poly/frog_jump.fbx", &models[16]),
    Animation(model_path + "low_poly/frog_stand.fbx", &models[16]),

    Animation(model_path + "low_poly/skimmer.fbx", &models[17]),

    Animation(model_path + "low_poly/turtle_dance.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_fear.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_hide.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_idle.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_out.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_spin.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_walk.fbx", &models[18]),
    Animation(model_path + "low_poly/turtle_woah.fbx", &models[18]),

    Animation(model_path + "Ankylosaurus/ank_walk.fbx", &models[9]),
    Animation(model_path + "Ankylosaurus/ank_turn180.fbx", &models[9]),
    Animation(model_path + "Ankylosaurus/ank.fbx", &models[9]),
        };

    palette[10].animations.push_back(&anims[0]);
    palette[10].animations.push_back(&anims[1]);
    palette[10].animations.push_back(&anims[2]);
    palette[10].animations.push_back(&anims[3]);
    palette[10].animations.push_back(&anims[4]);
    palette[11].animations.push_back(&anims[5]);
    palette[11].animations.push_back(&anims[6]);
    palette[11].animations.push_back(&anims[7]);
    palette[11].animations.push_back(&anims[8]);
    palette[11].animations.push_back(&anims[9]);
    palette[12].animations.push_back(&anims[10]);
    palette[12].animations.push_back(&anims[11]);
    palette[12].animations.push_back(&anims[12]);
    palette[12].animations.push_back(&anims[13]);
    palette[12].animations.push_back(&anims[14]);
    palette[12].animations.push_back(&anims[15]);
    palette[12].animations.push_back(&anims[16]);
    palette[12].animations.push_back(&anims[17]);
    palette[12].animations.push_back(&anims[18]);
    palette[12].animations.push_back(&anims[19]);
    palette[13].animations.push_back(&anims[20]);
    palette[13].animations.push_back(&anims[21]);
    palette[13].animations.push_back(&anims[22]);
    palette[13].animations.push_back(&anims[23]);
    palette[14].animations.push_back(&anims[24]);
    palette[14].animations.push_back(&anims[25]);
    palette[14].animations.push_back(&anims[26]);
    palette[14].animations.push_back(&anims[27]);
    palette[15].animations.push_back(&anims[28]);
    palette[15].animations.push_back(&anims[29]);
    palette[15].animations.push_back(&anims[30]);
    palette[15].animations.push_back(&anims[31]);
    palette[15].animations.push_back(&anims[32]);
    palette[15].animations.push_back(&anims[33]);
    palette[15].animations.push_back(&anims[34]);
    palette[15].animations.push_back(&anims[35]);
    palette[16].animations.push_back(&anims[36]);
    palette[16].animations.push_back(&anims[37]);
    palette[16].animations.push_back(&anims[38]);
    palette[16].animations.push_back(&anims[39]);
    palette[16].animations.push_back(&anims[40]);
    palette[17].animations.push_back(&anims[41]);
    palette[18].animations.push_back(&anims[42]);
    palette[18].animations.push_back(&anims[43]);
    palette[18].animations.push_back(&anims[44]);
    palette[18].animations.push_back(&anims[45]);
    palette[18].animations.push_back(&anims[46]);
    palette[18].animations.push_back(&anims[47]);
    palette[18].animations.push_back(&anims[48]);
    palette[18].animations.push_back(&anims[49]);
    palette[9].animations.push_back(&anims[50]);
    palette[9].animations.push_back(&anims[51]);
    palette[9].animations.push_back(&anims[52]);

    // Automatically start animations
    for(Object &object : palette) {
        if(object.animations.size()) {
            object.animator.PlayAnimation(object.animations[0]);
        }
    }

    vector<Terrain> terrains =
        {
            Terrain("Lake", "lake.png", vec3(0.36f, 0.61f, 0.26f), vec3(0.60f, 0.62f, 0.31f)),
            Terrain("Wasteland", "lake.png", vec3(0.17f, 0.64f, 0.11f), vec3(0.44, 0.25, 0.0f)),
        };

    vector<Skybox> skyboxes =
        {
            Skybox("Cartoon Blue", "cartoon_blue/", true),
            Skybox("Cartoon Green", "cartoon_green/", true),
            Skybox("Cartoon Red", "cartoon_red/", true),
            Skybox("Cartoon Yellow", "cartoon_yellow/", true),
            Skybox("Night Sky",  "night/"),
            Skybox("Cartoon Night",  "cartoon_night/", true),
        };

    vector<Fog> fogs =
        {
            {"No Fog", 1000, 1000, vec3(1.0f, 1.0f, 1.0f)},
            {"Light Fog", 400, 100, vec3(1.0f, 1.0f, 1.0f)},
            {"Heavy Fog", 80, 20, vec3(0.5f, 0.5f, 0.5f)},
            {"Standard Fog", 100, 30, vec3(1.0f, 1.0f, 1.0f)},
            {"Black Fog", 100, 30, vec3(0.0f, 0.0f, 0.0f)},
        };

    //vector<Light> lights;

    Level level = LoadLevel("level.txt", palette, terrains, skyboxes, fogs);
    Water water = Water(2.0f);
    Shadow shadow = Shadow();

    // Audio
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

        rotation_degrees += delta_time;

        // Input Resolution
        glfwPollEvents();
        processInput(delta_time);

        /*
        if(GlobalMode == GAME)
        {
            float player_height = 4.0f;
            camera.ClampPosition(vec2(-(level.terrain.height * 0.5f) + 2, -(level.terrain.width * 0.5f) + 2), 
                                 vec2(level.terrain.height * 0.5f - 2, level.terrain.width * 0.5f - 2));
            camera.position.y = level.terrain.HeightAt(camera.position.x, camera.position.z) + player_height;
        }
        */

        // Simulation
        for(Object &object : level.objects)
            object.Update(delta_time);

        // Render
        Render(context, shaders, level, water, shadow, framebuffer);

        // Editor
        if(GlobalMode == EDITOR) {
            editor_commands.UpdateCommands(&level);
            editor_commands.HandleInput();
            Editor(&editor_commands, &context, palette, 
                   &terrains, skyboxes, fogs, shaders, 
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
