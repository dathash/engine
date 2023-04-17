// Author: Alex Hartford
// Program: Engine
// File: Level
// Date: March 2023

#ifndef LEVEL_H
#define LEVEL_H

struct Level
{
    string name;
    string filename;
    string objects_filename;

    vector<Object> objects;
    Terrain        terrain;
    Skybox         skybox;
    Fog            fog;

    Level(const string &name_in, const string &filename_in,
          const string &objects_filename_in, const vector<Object> &palette,
          const Terrain &terrain_in, const Skybox &skybox_in,
          const Fog &fog_in)
    : name(name_in),
      filename(filename_in),
      objects_filename(objects_filename_in),
      terrain(terrain_in),
      skybox(skybox_in),
      fog(fog_in)
    {
        objects = Deserialize(objects_filename, palette);
    }
};

Level LoadLevel(const string &filename,
                const vector<Object> &palette,
                const vector<Terrain> &terrains,
                const vector<Skybox> &skyboxes,
                const vector<Fog> &fogs
               )
{
    string raw = ReadBinaryFile(level_path + filename);
    vector<string> lines = Parse(raw);
    return Level(lines[0], filename,
                 lines[1], palette,
                 GetTerrainCopyByName(lines[2], terrains),
                 GetSkyboxCopyByName(lines[3], skyboxes),
                 GetFogCopyByName(lines[4], fogs)
                );
}

void SaveLevel(const Level &level)
{
    ofstream fp;
    fp.open(level_path + level.filename);
    assert(fp.is_open());

    fp << "; name\n"
       << level.name << "\n"
       << "; objects\n"
       << level.objects_filename << "\n"
       << "; terrain\n"
       << level.terrain.name << "\n"
       << "; skybox\n"
       << level.skybox.name << "\n"
       << "; fog\n"
       << level.fog.name << "\n";

    Serialize(level.objects_filename, level.objects);

    fp.close();
}

#endif
