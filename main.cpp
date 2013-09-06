#include "main.h"
#include "glengine.h"

GLEngine engine;

void keyboard(unsigned char key, int x, int y)
{
	engine.keyboard(key,x,y);
}

void display()
{
	engine.display();
}

void reshape(int w, int h)
{
	engine.reshape(w,h);
}

void mouseFunc(int button, int state, int x, int y)
{
	engine.mouseFunc(button, state, x, y);
}

void activeMotion(int x, int y)
{
	engine.activeMotion(x,y);
}

void passiveMotion(int x, int y)
{
	engine.passiveMotion(x,y);
}


int main(int argc, char** argv)
{
	const unsigned int width = 1280;
	const unsigned int height = 800;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(width,height);
	glutCreateWindow("CS284 subdivision shader");

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "GLEW Error: " << glewGetString(err) << std::endl;
	}
	
	engine.reshape(width, height);
	engine.setupScene(); // Setup our OpenGL scene

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard); 
	glutMouseFunc(mouseFunc);
	glutMotionFunc(activeMotion);
	glutPassiveMotionFunc(passiveMotion);
	glutIdleFunc(display);

	glutMainLoop();
	return 0;
}