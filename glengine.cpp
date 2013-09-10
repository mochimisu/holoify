#include "glengine.h"
#include <cstdio>

GLEngine::GLEngine(void)
{
    inited = false;
    rot_orientation = glm::mat4(1.f);
    mouse_pos = glm::vec2(0.);
    mouse_state = GLUT_LEFT_BUTTON;
    scene_scale = 2.;
    scene_translate = glm::vec3(0.f,-2.f,0.f);
    cur_shader = 0;
    alpha = 1;
    final_frame = false;
}

GLEngine::~GLEngine(void)
{
    if (inited)
    {
        delete(shader);
    }
}

void GLEngine::setupScene(void)
{
    glClearColor(0.f, 0.f, 0.f, 0.f);

    Shader * avg_tes = new Shader("Averaging Tess + Normal Extrusion");
    avg_tes->initShader(GL_VERTEX_SHADER, "shaders/vert.glsl");
    avg_tes->initShader(GL_TESS_CONTROL_SHADER, "shaders/tess.control.glsl");
    avg_tes->initShader(GL_TESS_EVALUATION_SHADER, "shaders/tess.eval.glsl");
    avg_tes->initShader(GL_GEOMETRY_SHADER, "shaders/geometry.glsl");
    avg_tes->initShader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
    avg_tes->initProgram();
    shaders.push_back(avg_tes);

    Shader * bloom = new Shader("Bloom");
    bloom->initShader(GL_VERTEX_SHADER, "shaders/bloom/vert.glsl");
    bloom->initShader(GL_FRAGMENT_SHADER, "shaders/bloom/frag.glsl");
    bloom->initProgram();
    shaders.push_back(bloom);

    Shader * comp = new Shader("Comp");
    comp->initShader(GL_VERTEX_SHADER, "shaders/comp/vert.glsl");
    comp->initShader(GL_FRAGMENT_SHADER, "shaders/comp/frag.glsl");
    comp->initProgram();
    shaders.push_back(comp);

    Shader * final = new Shader("Final");
    final->initShader(GL_VERTEX_SHADER, "shaders/final/vert.glsl");
    final->initShader(GL_FRAGMENT_SHADER, "shaders/final/frag.glsl");
    final->initProgram();
    shaders.push_back(final);

    shader = avg_tes;

    obj_filenames.push_back("data/bunny2.obj");
    obj_filenames.push_back("data/dragon.obj");
    obj.loadObj("data/bunny2.obj");
    obj.upload();

    projection_matrix = glm::perspective(60.0f, (float)window_width / (float)window_height, 0.1f, 100.f);  // Create our perspective matrix

    tess_level = 2;
    cur_time = 0;
    inited = true;

    setupFbos();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(2);
}

void GLEngine::setupFbos()
{
    glGenFramebuffers(1, &bloom_source_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_source_fbo);

    glGenTextures(1, &bloom_source_texture);
    glBindTexture(GL_TEXTURE_2D, bloom_source_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        bloom_source_texture, 0);

    // bloom 1d
    glGenFramebuffers(1, &bloom_pass1_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_pass1_fbo);

    glGenTextures(1, &bloom_pass1_texture);
    glBindTexture(GL_TEXTURE_2D, bloom_pass1_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        bloom_pass1_texture, 0);

    // final bloom
    glGenFramebuffers(1, &bloom_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo);

    glGenTextures(1, &bloom_texture);
    glBindTexture(GL_TEXTURE_2D, bloom_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        bloom_texture, 0);

    // previous frame (ghosting)
    glGenFramebuffers(1, &ghost_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ghost_fbo);

    glGenTextures(1, &ghost_texture);
    glBindTexture(GL_TEXTURE_2D, ghost_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        ghost_texture, 0);

    // final frame
    glGenFramebuffers(1, &final_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, final_fbo);

    glGenTextures(1, &final_texture);
    glBindTexture(GL_TEXTURE_2D, final_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        final_texture, 0);

    //GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1, draw_buffers);
}

void GLEngine::reshape(int w, int h)
{
    window_width = w; // Set the window width
    window_height = h; // Set the window height

    projection_matrix = glm::perspective(60.0f, (float)window_width / (float)window_height, 0.1f, 100.f);  // Create our perspective matrix
    glViewport(0,0,w,h);
    setupFbos();
}

void GLEngine::mouseFunc(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouse_state = button;
    }
}

void GLEngine::activeMotion(int x, int y)
{
    glm::vec2 new_mouse = glm::vec2((double) x / window_width,
        (double) y / window_height);
    glm::vec2 diff = new_mouse - mouse_pos;

    float len = glm::dot(diff, diff);

    if ( len > 0.00001)
    {
        if (mouse_state == GLUT_LEFT_BUTTON)
        {
            glm::vec3 axis(diff[1]/len, diff[0]/len, 0.f);
            axis /= axis.length();
            rot_orientation = glm::rotate(rot_orientation, 20*180*len, axis);
        } else if (mouse_state == GLUT_RIGHT_BUTTON)
        {
            if (scene_scale + diff.x > 0)
            {
                scene_scale += diff.x;
            }
        } else if (mouse_state == GLUT_MIDDLE_BUTTON)
        {
            scene_translate.x += diff.x*4;
            scene_translate.y -= diff.y*4;
        }
    }

    mouse_pos = new_mouse;

    glutPostRedisplay();
}

void GLEngine::passiveMotion(int x, int y)
{
    glm::vec2 new_mouse = glm::vec2((double) x / window_width,
        (double) y / window_height);
    mouse_pos = new_mouse;

    glutPostRedisplay();
}

void GLEngine::keyboard(unsigned char key, int x, int y)
{
    int delta = 1;
    float fdelta = 1;
    switch(key)
    {
     case 27:
     exit(0);
     break;
     case 'q':
     delta *= -1;
     case 'w':
         if ((tess_level + delta) > 0) {
             tess_level += delta;
         }
     break;
     case 'A':
         fdelta *= -1;
     case 'a':
         fdelta *= 0.1;
         alpha += fdelta;
         alpha = (alpha > 1.) ? 1 : alpha;
         alpha = (alpha < 0.) ? 0 : alpha;
         std::cout << "alpha: " << alpha << std::endl;
         break;
     case 'O':
         delta += obj_filenames.size()-2;
     case 'o':
         cur_obj = (cur_obj+delta)%obj_filenames.size();
         obj.loadObj(obj_filenames[cur_obj]);
         obj.upload();
         std::cout << obj_filenames[cur_obj].c_str() << std::endl;
         break;
     break;
 }
 glutPostRedisplay();
}


void GLEngine::display(void)
{
    //switch between final frame and ghost frame
    final_frame = !final_frame;

    GLuint cur_final_texture;
    GLuint cur_final_fbo;
    GLuint cur_ghost_texture;
    GLuint cur_ghost_fbo;

    if (final_frame)
    {
        cur_final_texture = final_texture;
        cur_final_fbo = final_fbo;
        cur_ghost_texture = ghost_texture;
        cur_ghost_fbo = ghost_fbo;
    } else {
        cur_final_texture = ghost_texture;
        cur_final_fbo = ghost_fbo;
        cur_ghost_texture = final_texture;
        cur_ghost_fbo = final_fbo;
    }

    cur_time += 0.001;

    glBindFramebuffer(GL_FRAMEBUFFER, bloom_source_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers

    view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f)); // Create our view matrix
    glm::vec3 axis(0.f, 1.f, 0.f);
    rot_orientation = glm::rotate(rot_orientation, 0.1f, axis);
    model_matrix = glm::translate(glm::mat4(1.0f), scene_translate) * glm::scale(glm::mat4(1.0f), glm::vec3(scene_scale)) * rot_orientation;  // Create our model matrix
    normal_matrix = glm::transpose(glm::inverse(view_matrix*model_matrix));

    Shader * cur_shader = shader;

    cur_shader->bind(); // Bind our cur_shader

    int projectionMatrixLocation = glGetUniformLocation(cur_shader->id(), "projectionMatrix"); // Get the location of our projection matrix in the cur_shader
    int viewMatrixLocation = glGetUniformLocation(cur_shader->id(), "viewMatrix"); // Get the location of our view matrix in the cur_shader
    int modelMatrixLocation = glGetUniformLocation(cur_shader->id(), "modelMatrix"); // Get the location of our model matrix in the cur_shader
    int normalMatrixLocation = glGetUniformLocation(cur_shader->id(), "normalMatrix"); // Get the location of our normal matrix in the cur_shader
    int tl_loc = glGetUniformLocation(cur_shader->id(), "tessLevel");
    int time_loc = glGetUniformLocation(cur_shader->id(), "time");
    int camera_loc = glGetUniformLocation(cur_shader->id(), "camera");
    int alpha_loc = glGetUniformLocation(cur_shader->id(), "alpha");

    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection_matrix[0][0]); // Send our projection matrix to the cur_shader
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view_matrix[0][0]); // Send our view matrix to the cur_shader
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model_matrix[0][0]); // Send our model matrix to the cur_shader
    glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, &normal_matrix[0][0]); // Send our model matrix to the cur_shader
    glUniform3f(camera_loc, 0,0,0);
    glUniform1f(tl_loc, tess_level);
    glUniform1f(time_loc, cur_time);
    glUniform1f(alpha_loc, alpha);


    glPatchParameteri(GL_PATCH_VERTICES, 3);

    obj.draw(cur_shader);

    cur_shader->unbind(); // Unbind our cur_shader

    // blur in x
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_pass1_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
    glViewport(0,0,window_width,window_height);

    cur_shader = shaders[1];
    cur_shader->bind();
    int scale_loc = glGetUniformLocation(cur_shader->id(), "scale");
    int texture_source_loc = glGetUniformLocation(cur_shader->id(), "texture_source");
    time_loc = glGetUniformLocation(cur_shader->id(), "time");
    glUniform2f(scale_loc, 1.f/window_width, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloom_source_texture);
    glUniform1i(texture_source_loc, 0);
    glUniform1f(time_loc, cur_time);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2d(1,0); glVertex3f(1, -1, -1);
    glTexCoord2d(1,1); glVertex3f(1, 1, -1);
    glTexCoord2d(0,0); glVertex3f(-1, 1, -1);
    glEnd();

    // blur y
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
    glViewport(0,0,window_width,window_height);

    glUniform2f(scale_loc, 0, 1.f/window_height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloom_pass1_texture);
    glUniform1i(texture_source_loc, 0);

    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2d(1,0); glVertex3f(1, -1, -1);
    glTexCoord2d(1,1); glVertex3f(1, 1, -1);
    glTexCoord2d(0,0); glVertex3f(-1, 1, -1);
    glEnd();
    cur_shader->unbind();


    //comp
    cur_shader = shaders[2];
    cur_shader->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, cur_final_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
    glViewport(0,0,window_width,window_height);

    int texture_plain_loc = glGetUniformLocation(cur_shader->id(), "texture_plain");
    int texture_bloom_loc = glGetUniformLocation(cur_shader->id(), "texture_bloom");
    int texture_ghost_loc = glGetUniformLocation(cur_shader->id(), "texture_ghost");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloom_source_texture);
    glUniform1i(texture_plain_loc, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloom_texture);
    glUniform1i(texture_bloom_loc, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cur_ghost_texture);
    glUniform1i(texture_ghost_loc, 2);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2d(1,0); glVertex3f(1, -1, -1);
    glTexCoord2d(1,1); glVertex3f(1, 1, -1);
    glTexCoord2d(0,0); glVertex3f(-1, 1, -1);
    glEnd();

    cur_shader->unbind();

    //ghosting final display
    cur_shader = shaders[3];
    cur_shader->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
    glViewport(0,0,window_width,window_height);

    int texture_final_loc = glGetUniformLocation(cur_shader->id(), "texture_final");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cur_final_texture);
    glUniform1i(texture_final_loc, 0);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-1, -1, -1);
    glTexCoord2d(1,0); glVertex3f(1, -1, -1);
    glTexCoord2d(1,1); glVertex3f(1, 1, -1);
    glTexCoord2d(0,0); glVertex3f(-1, 1, -1);
    glEnd();

    cur_shader->unbind();

    // display

    glutSwapBuffers(); // Swap buffers so we can see our rendering
}
