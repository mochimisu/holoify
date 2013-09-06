#include "glengine.h"
#include <cstdio>

GLEngine::GLEngine(void)
{
	inited = false;
	rot_orientation = glm::mat4(1.f);
	mouse_pos = glm::vec2(0.);
	mouse_state = GLUT_LEFT_BUTTON;
	scene_scale = 0.25;
	scene_translate = glm::vec3(0.f,-1.f,0.f);
	cur_shader = 0;
	alpha = 1;
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
	glClearColor(0.4f, 0.6f, 0.9f, 0.0f); 
	

	Shader * avg_tes = new Shader("Averaging Tessellation");
	avg_tes->initShader(GL_VERTEX_SHADER, "shaders/vert.glsl");
	avg_tes->initShader(GL_TESS_CONTROL_SHADER, "shaders/tess.control.glsl");
	avg_tes->initShader(GL_TESS_EVALUATION_SHADER, "shaders/tess.eval.glsl");
	avg_tes->initShader(GL_GEOMETRY_SHADER, "shaders/geometry.glsl");
	avg_tes->initShader(GL_FRAGMENT_SHADER, "shaders/frag.glsl");
	avg_tes->initProgram();


	shaders.push_back(avg_tes);
	shader = avg_tes;

	obj_filenames.push_back("data/bunny2.obj");
	obj_filenames.push_back("data/bunny3.obj");
	obj_filenames.push_back("data/bunny4.obj");
	obj_filenames.push_back("data/cube.obj");
	obj_filenames.push_back("data/dragon.obj");
	obj_filenames.push_back("data/teapot.obj");
	obj_filenames.push_back("data/mug6.obj");
	obj.loadObj("data/bunny2.obj");
	obj.upload();

	projection_matrix = glm::perspective(60.0f, (float)window_width / (float)window_height, 0.1f, 100.f);  // Create our perspective matrix

	tess_level = 1;
	inited = true;
	
	
	glEnable(GL_DEPTH_TEST);


}


void GLEngine::reshape(int w, int h)
{
	window_width = w; // Set the window width
	window_height = h; // Set the window height

	projection_matrix = glm::perspective(60.0f, (float)window_width / (float)window_height, 0.1f, 100.f);  // Create our perspective matrix
	glViewport(0,0,w,h);
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
	  case 'Z':
		  delta += shaders.size()-2;
	  case 'z':
		  cur_shader = (cur_shader+delta)%shaders.size();
		  shader = shaders[cur_shader];
		  std::cout << (shader->name).c_str() << std::endl;
		  break;
	}
	glutPostRedisplay();
}


void GLEngine::display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers

	view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.f)); // Create our view matrix
	model_matrix = glm::translate(glm::mat4(1.0f), scene_translate) * glm::scale(glm::mat4(1.0f), glm::vec3(scene_scale)) * rot_orientation;  // Create our model matrix
	normal_matrix = glm::transpose(glm::inverse(view_matrix*model_matrix));

	Shader * cur_shader = shader;

	cur_shader->bind(); // Bind our cur_shader

	int projectionMatrixLocation = glGetUniformLocation(cur_shader->id(), "projectionMatrix"); // Get the location of our projection matrix in the cur_shader
	int viewMatrixLocation = glGetUniformLocation(cur_shader->id(), "viewMatrix"); // Get the location of our view matrix in the cur_shader
	int modelMatrixLocation = glGetUniformLocation(cur_shader->id(), "modelMatrix"); // Get the location of our model matrix in the cur_shader
	int normalMatrixLocation = glGetUniformLocation(cur_shader->id(), "normalMatrix"); // Get the location of our normal matrix in the cur_shader
	int tl_loc = glGetUniformLocation(cur_shader->id(), "tessLevel");
	int camera_loc = glGetUniformLocation(cur_shader->id(), "camera");
	int alpha_loc = glGetUniformLocation(cur_shader->id(), "alpha");

	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection_matrix[0][0]); // Send our projection matrix to the cur_shader
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view_matrix[0][0]); // Send our view matrix to the cur_shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model_matrix[0][0]); // Send our model matrix to the cur_shader
	glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, &normal_matrix[0][0]); // Send our model matrix to the cur_shader
	glUniform3f(camera_loc, 0,0,0);
	glUniform1f(tl_loc, tess_level);
	glUniform1f(alpha_loc, alpha);


	glPatchParameteri(GL_PATCH_VERTICES, 3);

	obj.draw(cur_shader);
	
	cur_shader->unbind(); // Unbind our cur_shader

	glutSwapBuffers(); // Swap buffers so we can see our rendering
}
