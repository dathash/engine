// Author: Alex Hartford
// Program: Engine
// File: Object
// Date: March 2023

#ifndef OBJECT_H
#define OBJECT_H


// Helper function for arbitrarily specifying an object's model matrix.
mat4 GetMatrix(const vec3 &position,
               const quat &orientation,
               float scale_factor)
{
    mat4 pos = translate(mat4(1.0f), position);
    mat4 rot = toMat4(orientation);
    mat4 scl = scale(mat4(1.0f), scale_factor * vec3(1.0f, 1.0f, 1.0f));
    return pos * rot * scl;
}

// Below are simple functions to decompose matrices.
// Swap them out for glm funcs when you feel comfortable.
/*
   WITH SCALE
void decomposeMtx(const mat4 &m, vec3 &pos, quat &rot, vec3 &scale)
{
    pos = m[3];
    for(int i = 0; i < 3; i++)
        scale[i] = length(vec3(m[i]));
    const mat3 rotMtx(
        vec3(m[0]) / scale[0],
        vec3(m[1]) / scale[1],
        vec3(m[2]) / scale[2]);
    rot = quat_cast(rotMtx);
}
*/


/*
   WITHOUT SCALE
void decomposeMtx(const mat4 &m, vec3 &pos, quat &rot)
{
    pos = m[3];
    rot = quat_cast(m);
}
*/


struct Object
{
    string  name;

    Model * model;

    mat4    matrix;

    vec3    position;
    quat    orientation;

    float   scale_factor;

    Object(string name_in,
           Model *model_in,
           vec3 position_in = vec3(0.0f, 0.0f, 0.0f),
           quat orientation_in = quat(0.0f, 0.0f, 0.0f, 1.0f),
           float scale_in = 1.0f)
    : name(name_in),
      model(model_in),
      position(position_in),
      orientation(orientation_in),
      scale_factor(scale_in)
    {
        UpdateMatrix();
    }

    Object() = default;

    size_t ID() const { return hash<string>{}(name); }

    void Draw(const Shader &shader) const
    {
        shader.setMat4("model", matrix);
        model->Draw(shader);
    }

    void DrawRotatingAtOrigin(const Shader &shader) const
    {
        mat4 trans = mat4(1.0f);
        trans = rotate(trans, (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
        trans = rotate(trans, 0.5f, vec3(0.0f, 0.0f, 1.0f));

        shader.setMat4("model", trans);
        model->Draw(shader);
    }

    void UpdateMatrix()
    {
        mat4 pos = translate(mat4(1.0f), position);
        mat4 rot = toMat4(orientation);
        mat4 scl = scale(mat4(1.0f), scale_factor * vec3(1.0f, 1.0f, 1.0f));
        this->matrix = pos * rot * scl;
    }

    float speed = 0.16f; // frames per second
    float time = 0.0f;
    int segment = 0;
    void Update(float delta_time, const Line &line)
    {
        if(name == "Hornet")
        {
            time += delta_time;

            // Update position
            cout << segment << " " << segment+1 << "\n";
            position = mix(line.points[segment], 
                           line.points[segment+1], 
                           time / (FRAMETIME / speed));

            // Update rotation
            vec3 direction = normalize(line.points[segment+1] - position);

            // Tilt
            orientation = angleAxis(radians(-45.0f * position.z / 2), direction);

            // Face direction
            // NOTES:
            // * Direction has to be normalized
            // * Direction can't be parallel to up.
            orientation *= quatLookAt(direction, vec3(0, 1, 0));

            // Model fix
            orientation *= angleAxis(radians(-90.f), vec3(1.f, 0.f, 0.f));

            UpdateMatrix();

            // Switch to the next segment
            if(!(time > (FRAMETIME / speed))) return;
            time = 0.0f;
            ++segment;
            if(segment + 1 == line.points.size())
                segment = 0;
        }
    }
};


// =================== Helper functions related mostly to objects ==============
Object GetObjectCopyByName(const string &name, const vector<Object> &objects)
{
    for(const Object &object : objects) {
        if(hash<string>{}(name) == object.ID())
            return Object(object);
    }
    cout << "WARNING GetObjectByName: No object of name " << name << "\n";
    return {};
}


#endif
