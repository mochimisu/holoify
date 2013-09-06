#include "main.h"
#include <vector>

#ifndef __SHADER_H_
#define __SHADER_H_

class Shader {
public:
	Shader(std::string shader_name=""); // Default constructor
    ~Shader(); // Deconstructor for cleaning up
    
	void initShader(unsigned int type, const char * file); // Add a shader
	void initProgram(); // Init program with shaders intialized so far
    
    void bind(); // Bind our GLSL shader program
    void unbind(); // Unbind our GLSL shader program
    
    unsigned int id(); // Get the identifier for our program

	GLint attribute_v_coord;
	GLint attribute_v_normal;

	std::string name;
    
private:
	std::vector<unsigned int> shaders; // Vector of shaders added
    unsigned int shader_id; // The shader program identifier
};

#endif