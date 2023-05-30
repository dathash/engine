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
    bool animated = false;
    Animator animator;
    vector<Animation *> animations;

    mat4    matrix;
    vec3    position;
    quat    orientation;
    float   scale_factor;

    bool pathing = false;
    Line line = {{}};
    Line path = {{}};
    float time = 0.0f;
    float speed = 0.05f;
    float segment = 0;

    Object(string name_in,
           Model *model_in,
           bool animated_in = false,
           float scale_in = 1.0f,
           quat orientation_in = angleAxis(radians(0.0f), vec3(1, 0, 0)),
           vec3 position_in = vec3(0.0f, 0.0f, 0.0f))
    : name(name_in),
      model(model_in),
      animated(animated_in),
      position(position_in),
      orientation(orientation_in),
      scale_factor(scale_in)
    {
        UpdateMatrix();

        line = {{
                      vec3(30.0f,  30.0f,  -120.0f),
                      vec3(30.0f,  30.0f,  -100.0f),
                      vec3(30.0f,  30.0f,  -80.0f),
                    }};

        vector<vec3> splinepoints;
        spline(splinepoints, line.points, SPLINE_LOD, 1.0);
        path = Line(splinepoints);
    }

    Object() = default;

    inline size_t ID() const { return hash<string>{}(name); }

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

    void Update(float delta_time)
    {
        animator.UpdateAnimation(delta_time);

        if(pathing && path.points.size() > 0)
        {
            time += delta_time;

            float interp_factor = time / (FRAMETIME / speed);

            // Update position
            position = mix(path.points[segment], 
                           path.points[segment+1], 
                           interp_factor);

            // Update rotation
            vec3 z_basis = normalize(path.points[segment+1] - position);
            vec3 x_basis = cross(z_basis, vec3(0, 1, 0));
            vec3 y_basis = cross(z_basis, x_basis);
            mat3 basis;
            basis[0] = x_basis;
            basis[1] = y_basis;
            basis[2] = z_basis;
            quat first = quat(mat4(basis));

            z_basis = normalize(path.points[segment+2] - position);
            x_basis = cross(z_basis, vec3(0, 1, 0));
            y_basis = cross(z_basis, x_basis);
            basis[0] = x_basis;
            basis[1] = y_basis;
            basis[2] = z_basis;
            quat second = quat(mat4(basis));

            orientation = slerp(first, second, interp_factor);
            // Model fix
            orientation *= angleAxis(radians(180.f), vec3(0.f, 0.f, 1.f));

            UpdateMatrix();

            // Switch to the next segment
            if(!(time > (FRAMETIME / speed))) return;
            time = 0.0f;
            ++segment;
            if(segment + 1 == path.points.size())
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
