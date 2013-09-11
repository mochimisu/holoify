#include "main.h"
#include <vector>
#include "shader.h"

#ifndef __RECT_H_
#define __RECT_H_

class Rect {
public:
    Rect(void);
    ~Rect(void);
    void draw(Shader* shader);
    void upload();

private:
    std::vector<glm::vec4> obj_vertices;
    std::vector<glm::vec2> obj_texcoords;
    std::vector<GLushort> obj_elements;

    GLuint vbo_vertices, ibo_elements, vbo_phases;
};

#endif