/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"

#include "Shape.h"
#include "line.h"
shared_ptr<Shape> shape;
shared_ptr<Shape> plane;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;
static int GlobalSelectedPoint = 0;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, psky, pplane;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

	//texture data
	GLuint Texture;
	GLuint Texture2;

	//line
	Line linerender;
	Line smoothrender;
	vector<vec3> line;
	

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
            GlobalSelectedPoint += 1;
            if(GlobalSelectedPoint > 7)
                GlobalSelectedPoint = 0;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
            GlobalSelectedPoint -= 1;
            if(GlobalSelectedPoint < 0)
                GlobalSelectedPoint = 7;
		}
        float offset_inc = 0.1f;
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(0, offset_inc, 0));
            plane->RecalculateWithNewBoundingBox();
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(0, -offset_inc, 0));
            plane->RecalculateWithNewBoundingBox();
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(-offset_inc, 0, 0));
            plane->RecalculateWithNewBoundingBox();
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(offset_inc, 0, 0));
            plane->RecalculateWithNewBoundingBox();
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(0, 0, offset_inc));
            plane->RecalculateWithNewBoundingBox();
		}
		if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
            plane->UpdateBoundingVertex(GlobalSelectedPoint, vec3(0, 0, -offset_inc));
            plane->RecalculateWithNewBoundingBox();
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom(const std::string& resourceDirectory)
	{
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		plane = make_shared<Shape>();
		plane->loadMesh(resourceDirectory + "/t800.obj");

        plane->CalculateMinsAndMaxes();
        //printf("x: %f to %f.\ny: %f to %f.\nz: %f to %f.\n", plane->minX, plane->maxX, plane->minY, plane->maxY, plane->minZ, plane->maxZ);

		plane->resize();

        plane->CalculateMinsAndMaxes();
        //printf("x: %f to %f.\ny: %f to %f.\nz: %f to %f.\n", plane->minX, plane->maxX, plane->minY, plane->maxY, plane->minZ, plane->maxZ);

        plane->GetBoundingVertices();

        plane->GetNormalized();
        // NOTE: Not that many of them...
        for (int i = 0; i < plane->obj_count; i++)
        {
            for(size_t v = 0; v < plane->relposBuf[i].size() / 3; v++)
            {
                printf("%f %f %f\n", plane->relposBuf[i][3 * v + 0], plane->relposBuf[i][3 * v + 1], plane->relposBuf[i][3 * v + 2]);
            }
        }

		plane->init();

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,//LD
			1.0, -1.0,  1.0,//RD
			1.0,  1.0,  1.0,//RU
			-1.0,  1.0,  1.0,//LU
		};
		//make it a bit smaller
		for (int i = 0; i < 12; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &VertexNormDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),

		};
		glGenBuffers(1, &VertexTexBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		
		glBindVertexArray(0);

	

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/explosion.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/sky.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);

		smoothrender.init();
		linerender.init();
		line.push_back(vec3(0,0,-3));
		line.push_back(vec3(5,-1,-3));
		line.push_back(vec3(5,3,-8));
		line.push_back(vec3(2,2,-10));
		line.push_back(vec3(-3,-2,-15));
		line.push_back(vec3(0,1.5,-20));
		linerender.re_init_line(line);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("campos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		pplane = std::make_shared<Program>();
		pplane->setVerbose(true);
		pplane->setShaderNames(resourceDirectory + "/plane_vertex.glsl", resourceDirectory + "/plane_frag.glsl");
		if (!pplane->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
			}
		pplane->addUniform("P");
		pplane->addUniform("V");
		pplane->addUniform("M");
		pplane->addUniform("campos");
		pplane->addAttribute("vertPos");
		pplane->addAttribute("vertNor");
		pplane->addAttribute("vertTex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * RotateXSky * SSky;

		// Draw the sky using GLSL.
		psky->bind();		
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, false);			//render!!!!!!!
		glEnable(GL_DEPTH_TEST);	
		psky->unbind();

        // Draw Sphere
        pplane->bind();
        {
            glUniformMatrix4fv(pplane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(pplane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
            glUniform3fv(pplane->getUniform("campos"), 1, &mycam.pos[0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture2);

            // 0, 0, 0
            mat4 Trans = glm::translate(glm::mat4(1.0f), plane->a + vec3(0, 0, -3));
            float scale_factor = 0.1f;
            if(GlobalSelectedPoint == 0)
                scale_factor = 0.2f;
            mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 0, 0, 1
            Trans = glm::translate(glm::mat4(1.0f), plane->b + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 1)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 0, 1, 0
            Trans = glm::translate(glm::mat4(1.0f), plane->c + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 2)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 1, 0, 0
            Trans = glm::translate(glm::mat4(1.0f), plane->d + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 3)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 0, 1, 1
            Trans = glm::translate(glm::mat4(1.0f), plane->e + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 4)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 1, 0, 1
            Trans = glm::translate(glm::mat4(1.0f), plane->f + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 5)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 1, 1, 0
            Trans = glm::translate(glm::mat4(1.0f), plane->g + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 6)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!

            // 1, 1, 1
            Trans = glm::translate(glm::mat4(1.0f), plane->h + vec3(0, 0, -3));
            scale_factor = 0.1f;
            if(GlobalSelectedPoint == 7)
                scale_factor = 0.2f;
            Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            
            M = Trans*Scale;

            glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            shape->draw(pplane, false);			//render!!!!!!!
        }
        pplane->unbind();

		// Draw the <T-800 MODEL, NOT A PLANE> using GLSL.
		glm::mat4 Trans = glm::translate(glm::mat4(1.0f), vec3(0,0,-3));
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		sangle = -3.1415926 / 2.;
		glm::mat4 RotX = glm::rotate(glm::mat4(1.0f), sangle, vec3(1,0,0));
		
		M = Trans*RotX*Scale;

		pplane->bind();
		glUniformMatrix4fv(pplane->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(pplane->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(pplane->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(pplane->getUniform("campos"), 1, &mycam.pos[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		plane->draw(pplane, false);			//render!!!!!!!
		pplane->unbind();


	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
