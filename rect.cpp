#include "rect.h"

using namespace std;

Rect::Rect(void)
{
    obj_vertices.clear();
    obj_texcoords.clear();
    obj_elements.clear();

    obj_vertices.push_back(glm::vec4(-1, -1, -1, 1));
    obj_vertices.push_back(glm::vec4(1, -1, -1, 1));
    obj_vertices.push_back(glm::vec4(1, 1, -1, 1));
    obj_vertices.push_back(glm::vec4(-1, 1, -1, 1));

    obj_texcoords.push_back(glm::vec2(0,0));
    obj_texcoords.push_back(glm::vec2(1,0));
    obj_texcoords.push_back(glm::vec2(1,1));
    obj_texcoords.push_back(glm::vec2(0,0));

    obj_elements.push_back(0);
    obj_elements.push_back(1);
    obj_elements.push_back(2);

    obj_elements.push_back(2);
    obj_elements.push_back(3);
    obj_elements.push_back(0);
}

Rect::~Rect(void)
{
}

void Rect::upload()
{
    if (obj_vertices.size() > 0)
    {
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER,
            obj_vertices.size() * sizeof(obj_vertices[0]),
            obj_vertices.data(), GL_STATIC_DRAW);
    }

    if (obj_elements.size() > 0)
    {
        glGenBuffers(1, &ibo_elements);
        glBindBuffer(GL_ARRAY_BUFFER, ibo_elements);
        glBufferData(GL_ARRAY_BUFFER,
            obj_elements.size() * sizeof(obj_elements[0]),
            obj_elements.data(), GL_STATIC_DRAW);
    }

}

void Rect::draw(Shader * shader)
{
    int v_coord_attrib = shader->attribute_v_coord;
    glEnableVertexAttribArray(v_coord_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(
        v_coord_attrib,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(v_coord_attrib);
}