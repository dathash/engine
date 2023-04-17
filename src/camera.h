// Author: Alex Hartford
// Program: Experience
// File: Camera
// Date: March 2023

#ifndef CAMERA_H
#define CAMERA_H

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct Camera
{
    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 world_up = vec3(0.0f, 1.0f, 0.0f);

    // NOTE: This defaults to looking down (1, 0, 0).
    float yaw   = 0.0f;
    float pitch = 0.0f;

    // basis
    vec3 front;
    vec3 up;
    vec3 right;

    mat4 projection;
    mat4 view;

    // tuning variables
    float sensitivity = DEFAULT_MOUSE_SENSITIVITY;
    float zoom        = DEFAULT_MOUSE_ZOOM;
    float speed       = DEFAULT_MOVEMENT_SPEED;

    Camera() {
        UpdateBasis();
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = speed * deltaTime;
        switch(GlobalMode)
        {
            case EDITOR:
            {
                if(direction == FORWARD)
                    position += front * velocity;
                if(direction == BACKWARD)
                    position -= front * velocity;
                if(direction == LEFT)
                    position -= right * velocity;
                if(direction == RIGHT)
                    position += right * velocity;
            } break;
            case GAME:
            {
                if(direction == FORWARD)
                    position += cross(world_up, right) * velocity;
                if(direction == BACKWARD)
                    position -= cross(world_up, right) * velocity;
                if(direction == LEFT)
                    position += cross(world_up, front) * velocity;
                if(direction == RIGHT)
                    position -= cross(world_up, front) * velocity;    
            } break;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

        UpdateBasis();
    }

    void ProcessMouseScroll(float yoffset)
    {
        zoom -= (float)yoffset;
        if(zoom < 1.0f)
            zoom = 1.0f;
        if(zoom > 45.0f)
            zoom = 45.0f;
    }

    mat4 GetProjectionMatrix() {
        return perspective(radians(zoom),
               (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
               0.1f, 1000.0f);
    }

    mat4 GetViewMatrix() {
        return lookAt(position, position + front, up);
    }

    // Returns the inverted view matrix.
    mat4 GetInvertedViewMatrix(float distance) {
        vec3 invert_pos = position + vec3(0.0f, -distance, 0.0f);
        
        float new_pitch = -pitch;
        return lookAt(invert_pos, invert_pos + GetFront(new_pitch, yaw), up);
    }

    // This mathematics gibberish is a polar coordinate transform.
    // It determines what our basis vectors should be, given a pitch and a yaw.
    vec3 GetFront(float pitch_in, float yaw_in)
    {
        vec3 result;
        result.x = cos(radians(yaw_in)) * cos(radians(pitch_in));
        result.y = sin(radians(pitch_in));
        result.z = sin(radians(yaw_in)) * cos(radians(pitch_in));
        return result;
    }

    void UpdateBasis()
    {
        vec3 new_front = GetFront(pitch, yaw);

        front = normalize(new_front); 
        right = normalize(cross(front, world_up));
        up = normalize(cross(right, front));
    }

    // Clamps the camera to a specified x, y range.
    void ClampPosition(vec2 low, vec2 high)
    {
        cout << "XXX: " << low.x << " " << high.x << "\n";
        if(position.x < low.x)
            position.x = low.x;
        if(position.x > high.x)
            position.x = high.x;
        if(position.z < low.y)
            position.z = low.y;
        if(position.z > high.y)
            position.z = high.y;
    }

    void PrintCameraState()
    {
        cout << "CAMERA\n"
             << "POS:   " << position << "\n";
             //<< "FRONT: " << front << "\n";
    }
};

#endif
