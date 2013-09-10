#include "shader.h"
#include <string>
#include <fstream>
#include <string.h>

using namespace std;

string textFileRead (const char * filename) {
    string str, ret = "" ;
    ifstream in ;
    in.open(filename) ;
    if (in.is_open()) {
        getline (in, str) ;
        while (in) {
            ret += str + "\n" ;
            getline (in, str) ;
        }
        return ret ;
    }
    else {
        cerr << "Unable to Open File " << filename << "\n" ;
        throw 2 ;
    }
}

static void validateShader(GLuint shader, const char* file = 0) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader
    if (length > 0) // If we have any information to display
        cout << "Shader " << shader << " (" << (file?file:"") << ") compile error: " << buffer << endl; // Output the information
}

static void validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
    if (length > 0) // If we have any information to display
        cout << "Program " << program << " link error: " << buffer << endl; // Output the information

    glValidateProgram(program); // Get OpenGL to try validating the program
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
    if (status == GL_FALSE) // If there was a problem validating
        cout << "Error validating shader " << program << endl; // Output which program had the error
}

Shader::Shader(string shader_name) {
    attribute_v_coord = -1;
    attribute_v_normal = -1;
    name = shader_name;
}

void Shader::initShader(unsigned int type, const char * file) {
    unsigned int shader = glCreateShader(type);
    unsigned int compiled;
    string str = textFileRead(file);
    const char * cstr = str.c_str();
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);
    validateShader(shader, file);
    shaders.push_back(shader);
}

void Shader::initProgram()
{
    shader_id = glCreateProgram();
    GLint linked;
    for (std::vector<unsigned int>::iterator it = shaders.begin();
        it != shaders.end(); ++it)
    {
        glAttachShader(shader_id, *it);
    }

    glLinkProgram(shader_id);
    validateProgram(shader_id);


    const char* attribute_name;

    attribute_name = "vp_loc";
    attribute_v_coord = glGetAttribLocation(shader_id, attribute_name);
    if (attribute_v_coord == - 1)
    {
        cerr << name << ": Could not bind attribute " << attribute_name << endl;
    };

    attribute_name = "vp_norm";
    attribute_v_normal = glGetAttribLocation(shader_id, attribute_name);
    if (attribute_v_normal == - 1)
    {
    cerr << name << ": Could not bind attribute " << attribute_name << endl;
    }

    attribute_name = "vp_phase";
    attribute_v_phase = glGetAttribLocation(shader_id, attribute_name);
    if (attribute_v_phase == - 1)
    {
    cerr << name << ": Could not bind attribute " << attribute_name << endl;
    }
}

Shader::~Shader() {
    for (std::vector<unsigned int>::iterator it = shaders.begin();
        it != shaders.end(); ++it)
    {
        glDetachShader(shader_id, *it);
        glDeleteShader(*it);
    }

    glDeleteProgram(shader_id);
}


unsigned int Shader::id() {
    return shader_id;
}

void Shader::bind() {
    glUseProgram(shader_id);
}

void Shader::unbind() {
    glUseProgram(0);
}
