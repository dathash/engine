// Author: Alex Hartford
// Program: Engine
// File: Gen
// Date: May 2023

#ifndef GEN_H
#define GEN_H

int randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}
float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}
float randomFloat(int a, int b)
{
    if (a > b)
        return randomFloat(b, a);
    if (a == b)
        return a;
 
    return (float)randomInt(a, b) + randomFloat();
}

void Generate(int num, const Object &object, vector<Object> *objects, const Terrain &terrain)
{
    int min = (-terrain.width / 2) + 2;
    int max = (terrain.width / 2) - 2;

    int i = num;
    while (i > 0) {
        float x = randomFloat(min, max);
        float z = randomFloat(min, max);
        float y = terrain.HeightAt(x, z);

        float size_variation = 0.5f;
        float size_multiplier = 1.0f + randomFloat(-size_variation, size_variation);

        if (y > 0.0f) {
            Object new_object = Object(object);
            new_object.position = vec3(x, y, z);
            new_object.scale_factor = new_object.scale_factor * size_multiplier;
            new_object.UpdateMatrix();
            objects->push_back(new_object);
            i -= 1;
        }
    }
}

#endif
