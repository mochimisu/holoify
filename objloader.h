#include "main.h"
#include <vector>
#include "shader.h"

#ifndef __OBJLOADER_H_
#define __OBJLOADER_H_

class ObjLoader {
public:
    ObjLoader(void);
    ~ObjLoader(void);
    void loadObj(std::string filename);
    void draw(Shader* shader);
    void upload();
    float num_vertices;

private:
    std::vector<glm::vec4> obj_vertices;
    std::vector<glm::vec3> obj_normals;
    std::vector<glm::vec2> obj_texcoords;
    std::vector<float> obj_phases;
    std::vector<GLushort> obj_elements;


    GLuint vbo_vertices, vbo_normals, ibo_elements, vbo_phases;
};

#endif