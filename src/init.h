// Author: Alex Hartford
// Program: Engine
// File: Init
// Date: March 2023

#ifndef INIT_H
#define INIT_H

void Init(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
                              "engine", NULL, NULL);
    assert(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLAD
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontFromFileTTF("../assets/verdanab.ttf", 10.0f);

    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // MINIAUDIO
    assert(ma_engine_init(NULL, &GlobalAudioEngine) == MA_SUCCESS);

    ma_sound_group_init(&GlobalAudioEngine, 0, nullptr, &(GlobalMusicGroup));
    ma_sound_group_init(&GlobalAudioEngine, 0, nullptr, &(GlobalSfxGroup));
    SetMusicVolume(DEFAULT_MUSIC_VOLUME);
    SetSfxVolume(DEFAULT_SFX_VOLUME);

    // Manage OpenGL State
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_STENCIL_TEST); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Line specifics
    glEnable(GL_LINE_SMOOTH);

    // Clipping Planes for water reflection/refraction split
    glEnable(GL_CLIP_DISTANCE0);
}


#endif
