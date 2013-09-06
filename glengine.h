#include "main.h"
#include "shader.h"
#include "objloader.h"
#include <vector>

#ifndef __GLENGINE_H_
#define __GLENGINE_H_

class GLEngine {
public:
	GLEngine(void); 
	~GLEngine(void); 
	void setupScene(void); // Initialization
	void reshape(int w, int h); // Resizing window
	void display(void); // Show the scene
	void keyboard(unsigned char key, int x, int y); // Keyboard interaction
	void mouseFunc(int button, int state, int x, int y);
	void activeMotion(int x, int y);
	void passiveMotion(int x, int y);

private:
	int window_width; // Store the width of our window
	int window_height; // Store the height of our window

	std::vector<Shader *> shaders;
	Shader *shader; // Our GLSL shader
	std::vector<std::string> obj_filenames;
	ObjLoader obj;

	glm::mat4 projection_matrix; // Store the projection matrix
	glm::mat4 view_matrix; // Store the view matrix
	glm::mat4 model_matrix; // Store the model matrix
	glm::mat4 normal_matrix;

	glm::mat4 rot_orientation;

	glm::vec2 mouse_pos;

	unsigned int vao_id[1]; // Our Vertex Array Object
	unsigned int vbo_id[1]; // Our Vertex Buffer Object

	float tess_level;

	bool inited;
	int mouse_state;
	float scene_scale;
	glm::vec3 scene_translate;

	unsigned int cur_shader;
	unsigned int cur_obj;
	float alpha;
};

#endif