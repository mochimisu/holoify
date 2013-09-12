#include "main.h"
#include "shader.h"
#include "objloader.h"
#include "rect.h"
#include <vector>

#ifndef __GLENGINE_H_
#define __GLENGINE_H_

struct Fbo {
    Fbo(): fbo (), texture() {};
    Fbo(GLuint fbo, GLuint texture): fbo (fbo), texture(texture) {};
    GLuint fbo;
    GLuint texture;
};

class GLEngine {
public:
    GLEngine(void);
    ~GLEngine(void);
    void setupScene(void);
    void deleteFbos(void);
    void setupFbos(void);
    void reshape(int w, int h);
    void display(void);
    void keyboard(unsigned char key, int x, int y);
    void mouseFunc(int button, int state, int x, int y);
    void activeMotion(int x, int y);
    void passiveMotion(int x, int y);
    void cycleObj();
    void loadObjs(std::string dir);

private:
    // Window size
    int window_width;
    int window_height;

    // Shaders and framebuffers
    std::vector<Shader *> shaders;
    std::vector<Fbo *> fbos;

    // OBJ stuff
    std::vector<std::string> obj_filenames;
    ObjLoader obj;
    unsigned int cur_obj;

    // Rect (for drawing screen-aligned quads, etc);
    Rect rect;

    // MVP matrices
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    glm::mat4 normal_matrix;
    glm::mat4 rot_orientation;
    glm::vec3 scene_translate;

    // User interaction
    glm::vec2 mouse_pos;
    int mouse_state;
    float tess_level;
    float tess_scale;
    float scene_scale;
    bool rotate;
    bool change_obj;
    float time_delta;
    int point_size;

    // Stuff that's handled automatically
    float cur_time;
    float tess_level_resolution_adjustment;
    bool inited;
    bool final_frame;
};

#endif
