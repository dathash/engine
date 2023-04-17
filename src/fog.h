// Author: Alex Hartford
// Program: Experience
// File: Fog
// Date: March 2023

#ifndef FOG_H
#define FOG_H

struct Fog
{
    string name;
    float max_distance;
    float min_distance;
    vec3 color;

    size_t ID() const { return hash<string>{}(name); }
};

Fog GetFogCopyByName(const string &name, const vector<Fog> &fogs)
{
    for(const Fog &fog : fogs) {
        if(hash<string>{}(name) == fog.ID())
            return Fog(fog);
    }
    cout << "WARNING GetFogCopyByName: No fog of name " << name << "\n";
    return {};
}

#endif
