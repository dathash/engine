// Author: Alex Hartford
// Program: Engine
// File: Serialization
// Date: March 2023

#ifndef SERIAL_H
#define SERIAL_H

// TODO: Come at this problem in a better state of mind
static int ElementNumber;
int NextValidLine(vector<string> &lines)
{
    while(lines[ElementNumber][0] == ';')
        ++ElementNumber;

    return ElementNumber++;
}

// Returns a vector of lines.
// Removes commented lines.
vector<string> Parse(const string &in)
{
    char comment_character = ';';
    vector<string> result;
    vector<string> lines = split(in, '\n');
    for(const string &line : lines)
    {
        if(!line.empty() && line[0] != comment_character)
            result.push_back(line);
    }

    return result;
}

vector<Object> Deserialize(const string &filename, const vector<Object> &palette)
{
    vector<Object> level_objects = {};

    string raw_data = ReadBinaryFile(serial_path + filename);

    vector<string> individuals = split(raw_data, '%');
    for(const string &individual : individuals) {
        vector<string> elements = Parse(individual);
        if(elements.empty()) break;

        Object object = GetObjectCopyByName(elements[0], palette);

        object.position.x = stof(elements[1]);
        object.position.y = stof(elements[2]);
        object.position.z = stof(elements[3]);

        object.orientation = quat(stof(elements[4]),
                                  stof(elements[5]),
                                  stof(elements[6]),
                                  stof(elements[7]));

        object.scale_factor = stof(elements[8]);

        object.UpdateMatrix();

        level_objects.push_back(object);
    }

    return level_objects;
}

void Serialize(const string &filename, const vector<Object> &level_objects)
{
    ofstream fp;
    fp.open(serial_path + filename);
    assert(fp.is_open());

    for(const Object &object : level_objects) {
        fp << object.name << "\n"
           << "; position\n"
           << "    " << object.position.x << "\n"
           << "    " << object.position.y << "\n"
           << "    " << object.position.z << "\n"
           << "; orientation\n"
           << "    " << object.orientation.w << "\n"
           << "    " << object.orientation.x << "\n"
           << "    " << object.orientation.y << "\n"
           << "    " << object.orientation.z << "\n"
           << "; scale\n"
           << "    " << object.scale_factor << "\n"
           << "%\n";
    }

    fp.close();
}

#endif
