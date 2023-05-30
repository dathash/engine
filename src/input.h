#ifndef INPUT_H
#define INPUT_H

struct Input
{
    bool undo;
    bool redo;

    bool copy;
    bool destroy;
};
static Input input;

void processInput(float delta_time)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        GlobalDepthBufferDebug = true;
    else
        GlobalDepthBufferDebug = false;

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, delta_time);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, delta_time);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, delta_time);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, delta_time);
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, delta_time);
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, delta_time);

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.speed = slow_movement_speed;
    else
        camera.speed = DEFAULT_MOVEMENT_SPEED;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    GlobalEditorState.mouse_pos = {xpos, ypos};

    if(
       GlobalMode == EDITOR
       && !(GlobalEditorState.mode == MODE_LOOK)
      )
    {
        return;
    }

	if(firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

	lastX = xpos;
	lastY = ypos;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if(GlobalMode != EDITOR) return;
    if(ImGui::GetIO().WantCaptureMouse) return;

    // Editor Functionality
    // Selection
    if(button == GLFW_MOUSE_BUTTON_1) {
        if(action == GLFW_PRESS) {
            unsigned int intermediary = 0;
            glReadPixels(GlobalEditorState.mouse_pos.x, 
                         SCREEN_HEIGHT - GlobalEditorState.mouse_pos.y, 
                         1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
                         &intermediary);
            if(intermediary > 100) {
                GlobalEditorState.selected = -1;
            }
            else {
                GlobalEditorState.selected = intermediary - 1;
            }
        }
    }

    // Look Mode Swapping
    if(button == GLFW_MOUSE_BUTTON_2) {
        if(action == GLFW_PRESS) {
            firstMouse = true;
            GlobalEditorState.mode = MODE_LOOK;
            camera.sensitivity = LOOK_MODE_SENSITIVITY;
        }
        else if(action == GLFW_RELEASE) {
            GlobalEditorState.mode = MODE_DEFAULT;
            camera.sensitivity = DEFAULT_MOUSE_SENSITIVITY;
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Editor Mode Keybindings
    if(GlobalMode == EDITOR)
    {
        if(mods & GLFW_MOD_SUPER) {
            if(key == GLFW_KEY_Z
            && action == GLFW_PRESS)
            {
                if(mods & GLFW_MOD_SHIFT)
                    input.redo = true;
                else
                    input.undo = true;
            }

            if(key == GLFW_KEY_C
            && action == GLFW_PRESS)
            {
                input.copy = true;
            }
            if(key == GLFW_KEY_BACKSPACE
            && action == GLFW_PRESS)
            {
                input.destroy = true;
            }
        }
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if(GlobalMode == EDITOR) GlobalMode = GAME;
        else if(GlobalMode == GAME) GlobalMode = EDITOR;
        GlobalEditorState.mode = MODE_DEFAULT;
        firstMouse = true;

        if(GlobalMode == EDITOR)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.sensitivity = DEFAULT_MOUSE_SENSITIVITY;
        }
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        GlobalEditorState.selected = -1;
    }

    // TODO: We should develop a clear separation of:
    // * Editor Keybindings
    // * Mixed Keybindings
    // * Game Keybindings
    if(ImGui::GetIO().WantCaptureKeyboard) return;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if(ImGui::GetIO().WantCaptureMouse) return;
	camera.ProcessMouseScroll(static_cast<float>(-yoffset));
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

#endif
