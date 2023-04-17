// Author: Alex Hartford
// Program: Engine
// File: Material
// Date: March 2023

#ifndef MATERIAL_H
#define MATERIAL_H

struct Material
{
    vec3 ambient  = vec3(0.05f, 0.05f, 0.05f);
    vec3 diffuse  = vec3(0.5f,  0.5f,  0.5f);
    vec3 specular = vec3(0.2f,  0.2f,  0.2f);
    float shine   = 0.05f;
};

#endif
