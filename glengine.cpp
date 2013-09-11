#include "glengine.h"
#include <cstdio>
#include <dirent.h>

GLEngine::GLEngine(void)
{
    inited = false;
    rot_orientation = glm::mat4(1.f);
    mouse_pos = glm::vec2(0.);
    mouse_state = GLUT_LEFT_BUTTON;
    scene_scale = 2.;
    scene_translate = glm::vec3(0.f,-2.f,0.f);
    cur_time = 1;
    tess_level = 0;
    tess_scale = 0.2;
    final_frame = false;
    change_obj = false;
    rotate = true;
    time_delta = 0.01;
    tess_level_resolution_adjustment = 0;

}

GLEngine::~GLEngine(void)
{
    for (std::vector<Shader *>::iterator it = shaders.begin();
        it != shaders.end(); ++it)
    {
        delete(*it);
    }
    for (std::vector<Fbo *>::iterator it = fbos.begin();
        it != fbos.end(); ++it)
    {
        delete(*it);
    }
}

void GLEngine::setupScene(void)
{
    glClearColor(0.f, 0.f, 0.f, 0.f);

    std::string shaders_dir = "shaders";
    Shader * avg_tes = new Shader("Averaging Tess + Normal Extrusion");
    avg_tes->initShader(GL_VERTEX_SHADER,
       shaders_dir+"/mainpoint/vert.glsl");
    avg_tes->initShader(GL_TESS_CONTROL_SHADER,
       shaders_dir+"/mainpoint/tess.control.glsl");
    avg_tes->initShader(GL_TESS_EVALUATION_SHADER,
       shaders_dir+"/mainpoint/tess.eval.glsl");
    avg_tes->initShader(GL_GEOMETRY_SHADER,
       shaders_dir+"/mainpoint/geometry.glsl");
    avg_tes->initShader(GL_FRAGMENT_SHADER,
       shaders_dir+"/mainpoint/frag.glsl");
    avg_tes->initProgram();
    shaders.push_back(avg_tes);

    Shader * bloom = new Shader("Bloom");
    bloom->initShader(GL_VERTEX_SHADER,
       shaders_dir+"/bloom/vert.glsl");
    bloom->initShader(GL_FRAGMENT_SHADER,
       shaders_dir+"/bloom/frag.glsl");
    bloom->initProgram();
    shaders.push_back(bloom);

    Shader * comp = new Shader("Comp");
    comp->initShader(GL_VERTEX_SHADER,
       shaders_dir+"/comp/vert.glsl");
    comp->initShader(GL_FRAGMENT_SHADER,
       shaders_dir+"/comp/frag.glsl");
    comp->initProgram();
    shaders.push_back(comp);

    Shader * final = new Shader("Final");
    final->initShader(GL_VERTEX_SHADER,
       shaders_dir+"/final/vert.glsl");
    final->initShader(GL_FRAGMENT_SHADER,
       shaders_dir+"/final/frag.glsl");
    final->initProgram();
    shaders.push_back(final);

    Shader * wireframe = new Shader("Wireframe");
    wireframe->initShader(GL_VERTEX_SHADER,
       shaders_dir+"/wireframe/vert.glsl");
    wireframe->initShader(GL_TESS_CONTROL_SHADER,
       shaders_dir+"/wireframe/tess.control.glsl");
    wireframe->initShader(GL_TESS_EVALUATION_SHADER,
       shaders_dir+"/wireframe/tess.eval.glsl");
    wireframe->initShader(GL_GEOMETRY_SHADER,
       shaders_dir+"/wireframe/geometry.glsl");
    wireframe->initShader(GL_FRAGMENT_SHADER,
       shaders_dir+"/wireframe/frag.glsl");
    wireframe->initProgram();
    shaders.push_back(wireframe);

    // 5 FBOs:
    // Main
    // Bloom pass 1
    // Bloom
    // Final
    // Ghost
    for (int i = 0; i < 5; ++i)
    {
        fbos.push_back(new Fbo());
    }

    loadObjs("data/pixar");
    rect.upload();

    projection_matrix = glm::perspective(60.0f,
        (float)window_width / (float)window_height, 0.1f, 100.f);

    inited = true;

    setupFbos();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(1);
}

void GLEngine::loadObjs(std::string dir)
{
    DIR * dirp;
    struct dirent *ep;
    dirp = opendir(dir.c_str());
    if (dirp != NULL)
    {
        while((ep = readdir(dirp)))
        {
            std::string fname = ep->d_name;
            if (fname.substr(fname.find_last_of(".")+1) == "obj") {
                obj_filenames.push_back(dir+"/"+fname);
            }
        }
    }
    closedir(dirp);

    obj.loadObj(obj_filenames.front());
    obj.upload();
}

void GLEngine::deleteFbos()
{
    for (std::vector<Fbo *>::iterator it = fbos.begin();
        it != fbos.end(); ++it)
    {
        Fbo * cur_fbo_it = *it;
        glDeleteFramebuffers(1, &(cur_fbo_it->fbo));
        glDeleteTextures(1, &(cur_fbo_it->texture));
    }
}

void GLEngine::setupFbos()
{
    for (std::vector<Fbo *>::iterator it = fbos.begin();
        it != fbos.end(); ++it)
    {
        Fbo * cur_fbo_it = *it;
        glGenFramebuffers(1, &(cur_fbo_it->fbo));
        glBindFramebuffer(GL_FRAMEBUFFER, cur_fbo_it->fbo);

        glGenTextures(1, &(cur_fbo_it->texture));
        glBindTexture(GL_TEXTURE_2D, cur_fbo_it->texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            cur_fbo_it->texture, 0);
    }

}

void GLEngine::reshape(int w, int h)
{
    window_width = w; // Set the window width
    window_height = h; // Set the window height
    std::cout << "Reshaped to " << w << "x" << h << std::endl;
    int maxdim = std::max(w,h);
    tess_level_resolution_adjustment = maxdim/1600;

    projection_matrix = glm::perspective(60.0f,
        (float)window_width / (float)window_height, 0.1f, 100.f);
    glViewport(0,0,w,h);

    deleteFbos();
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
           if ((tess_level + delta) > -5) {
               tess_level += delta;
           }
           std::cout << "tess_level: " << tess_level << std::endl;
           break;
       case 'e':
           fdelta *= -1;
       case 'r':
           fdelta /= 10.f;
           if ((tess_scale + fdelta) > 0) {
               tess_scale += fdelta;
           }
           std::cout << "tess_scale: " << tess_scale << std::endl;
           break;
       case 'O':
       case 'o':
           cur_time = std::min(cur_time,1.f);
           change_obj = true;
           break;
       case 'k':
           rotate = !rotate;
           break;
       case '[':
            time_delta -= 0.003;
            if (time_delta < 0)
            {
                time_delta = 0;
            }
            break;
        case ']':
            time_delta += 0.003;
    break;
    }
    glutPostRedisplay();
}

void GLEngine::cycleObj(void)
{
   cur_obj = (cur_obj+1)%obj_filenames.size();
   obj.loadObj(obj_filenames[cur_obj]);
   obj.upload();
   std::cout << obj_filenames[cur_obj].c_str() << std::endl;
}


void GLEngine::display(void)
{
    // Pre-render stuff

    // If someone wants to change the OBJ, explode the model by going from
    // t=1 to t=0 (ish), and then change models. Also reset the orientation
    if (change_obj)
    {
        cur_time -= 0.01f;

        if (cur_time < 0.15f)
        {
            cycleObj();
            rot_orientation = glm::rotate(
                glm::mat4(1.f), -30.f, glm::vec3(0.f,1.f,0.f));
            change_obj = false;
        }
    } else {
        cur_time += time_delta;
    }


    // Automatically rotate
    if (rotate)
    {
        rot_orientation = glm::rotate(rot_orientation, 0.1f,
            glm::vec3(0.f, 1.f, 0.f));
    }

    // Grab our shaders
    Shader * main_shader = shaders[0];
    Shader * bloom_shader = shaders[1];
    Shader * comp_shader = shaders[2];
    Shader * final_shader = shaders[3];

    // Grab our FBOs
    Fbo * main_fbo = fbos[0];
    Fbo * bloom_pass1_fbo = fbos[1];
    Fbo * bloom_fbo = fbos[2];
    Fbo * final_fbo = fbos[3];
    Fbo * ghost_fbo = fbos[4];
    // "double buffer" textures for ghost frames
    final_frame = !final_frame;
    if (final_frame)
    {
        final_fbo = fbos[4];
        ghost_fbo = fbos[3];
    }


    // Now render things...

    // The "main" render where we actually deal with geometry
    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f));

    model_matrix = glm::translate(glm::mat4(1.0f), scene_translate)
     * glm::scale(glm::mat4(1.0f), glm::vec3(scene_scale)) * rot_orientation;
    normal_matrix = glm::transpose(glm::inverse(view_matrix*model_matrix));

    main_shader->bind(); // Bind

    // Set a bunch of values
    int projectionMatrixLocation = glGetUniformLocation(main_shader->id(),
        "projectionMatrix");
    int viewMatrixLocation = glGetUniformLocation(main_shader->id(),
        "viewMatrix");
    int modelMatrixLocation = glGetUniformLocation(main_shader->id(),
        "modelMatrix");
    int normalMatrixLocation = glGetUniformLocation(main_shader->id(),
        "normalMatrix");
    int tl_loc = glGetUniformLocation(main_shader->id(), "tess_level");
    int ts_loc = glGetUniformLocation(main_shader->id(), "tess_scale");
    int time_loc = glGetUniformLocation(main_shader->id(), "time");
    int camera_loc = glGetUniformLocation(main_shader->id(), "camera");

    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE,
    &projection_matrix[0][0]);
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE,
        &view_matrix[0][0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
        &model_matrix[0][0]);
    glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE,
        &normal_matrix[0][0]);
    glUniform3f(camera_loc, 0,0,0);
    // Set tessLevel depending on complexity of model and resolution
    glUniform1f(tl_loc,
        std::ceil(std::max(5.-log10(obj.num_vertices),0.))
        +tess_level+tess_level_resolution_adjustment);
    glUniform1f(ts_loc, tess_scale);
    glUniform1f(time_loc, cur_time);

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    obj.draw(main_shader);

    main_shader->unbind(); // Unbind shader


    // Bloom!!
    // blur x
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_pass1_fbo->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0,0,window_width,window_height);

    bloom_shader->bind();
    int scale_loc = glGetUniformLocation(bloom_shader->id(), "scale");
    int texture_source_loc = glGetUniformLocation(bloom_shader->id(),
        "texture_source");
    time_loc = glGetUniformLocation(bloom_shader->id(), "time");
    glUniform2f(scale_loc, 0.4f/window_width, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, main_fbo->texture);
    glUniform1i(texture_source_loc, 0);
    glUniform1f(time_loc, cur_time);
    rect.draw(bloom_shader);

    // blur y
    glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0,0,window_width,window_height);
    glUniform2f(scale_loc, 0, 0.4f/window_height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloom_pass1_fbo->texture);
    glUniform1i(texture_source_loc, 0);
    rect.draw(bloom_shader);
    bloom_shader->unbind();


    // Put stuff together
    comp_shader->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, final_fbo->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0,0,window_width,window_height);

    int texture_plain_loc = glGetUniformLocation(comp_shader->id(),
        "texture_plain");
    int texture_bloom_loc = glGetUniformLocation(comp_shader->id(),
        "texture_bloom");
    int texture_ghost_loc = glGetUniformLocation(comp_shader->id(),
        "texture_ghost");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, main_fbo->texture);
    glUniform1i(texture_plain_loc, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloom_fbo->texture);
    glUniform1i(texture_bloom_loc, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ghost_fbo->texture);
    glUniform1i(texture_ghost_loc, 2);
    rect.draw(comp_shader);
    comp_shader->unbind();

    // Display the final texture
    final_shader->bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0,0,window_width,window_height);

    int texture_final_loc = glGetUniformLocation(final_shader->id(),
        "texture_final");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, final_fbo->texture);
    glUniform1i(texture_final_loc, 0);
    rect.draw(final_shader);
    final_shader->unbind();

    glutSwapBuffers();
}