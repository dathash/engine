


#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include <string>
#include <vector>
#include <memory>






class Program;

class Shape
{

public:
	//stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
	void loadMesh(const std::string &meshName, std::string *mtlName = NULL, unsigned char *(loadimage)(char const *, int *, int *, int *, int) = NULL);
	void init();
	void resize();
    void CalculateMinsAndMaxes();
    void GetNormalized();
    void GetBoundingVertices();
    void RecalculateWithNewBoundingBox();
    void UpdateBoundingVertex(int, vec3);
	void draw(const std::shared_ptr<Program> prog, bool use_extern_texures) const;
	unsigned int *textureIDs = NULL;

    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    vec3 a, b, c, d, e, f, g, h;

	int obj_count = 0;
	std::vector<unsigned int> *eleBuf = NULL;
	std::vector<float> *posBuf = NULL;
	std::vector<float> *relposBuf = NULL;
	std::vector<float> *norBuf = NULL;
	std::vector<float> *texBuf = NULL;
	unsigned int *materialIDs = NULL;


	unsigned int *eleBufID = 0;
	unsigned int *posBufID = 0;
	unsigned int *norBufID = 0;
	unsigned int *texBufID = 0;
	unsigned int *vaoID = 0;

};

#endif // LAB471_SHAPE_H_INCLUDED
