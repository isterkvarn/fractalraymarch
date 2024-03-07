// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

#define MAIN
#include <iostream>
#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>
#include "LoadTGA.h"
#include "VectorUtils4.h"
// uses framework OpenGL
// uses framework Cocoa

// Globals
// Data would normally be read from files
GLfloat vertices[] =
{
	1.0f,1.0f,0.0f,
	1.0f,-1.0f,0.0f,
	-1.0f,-1.0f,0.0f,
	1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,
	-1.0f,1.0f,0.0f
};

// vertex array object
unsigned int vertexArrayObjID;
	
GLuint program;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	// Reference to shader program

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab3-4.vert", "lab3-4.frag");
	printError("init shader");

	// Upload geometry to the GPU:

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

	// End of upload of geometry

	printError("init arrays");
}

float px, py, pz;
float lookh, lookv;

float move_speed = 0.001;
float prev_t;

void mouse_moved(int mx, int my) {
  lookh -= (100 - mx) / 1000.0;
  lookv -= (100 - my) / 1000.0;
  glutWarpPointer(100, 100);
}


void display(void)
{

  mat4 camMatrix;
  GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
  float delta = t - prev_t;
  prev_t = t;

  if (glutKeyIsDown('w')) {
    px -= delta * sin(-lookh) * cos(-lookv) * move_speed;
    py -= delta * sin(lookv) * move_speed;
    pz -= delta * cos(-lookh) * cos(-lookv) * move_speed;
  }
  if (glutKeyIsDown('a')) {
    px -= delta * cos(lookh) * move_speed;
    pz -= delta * sin(lookh) * move_speed;
  }
  if (glutKeyIsDown('s')) {
    px += delta * sin(-lookh) * cos(-lookv) * move_speed;
    py += delta * sin(lookv) * move_speed;
    pz += delta * cos(-lookh) * cos(-lookv) * move_speed;
  }
  if (glutKeyIsDown('d')) {
    px += delta * cos(lookh) * move_speed;
    pz += delta * sin(lookh) * move_speed;
  }

    vec3 p = vec3(px, py, pz);
    camMatrix = lookAtv(p, p + vec3(cos(-lookh), 0, -sin(-lookh)), vec3(0, 1, 0));
	printError("pre display");

    glUniformMatrix4fv(glGetUniformLocation(program, "worldToView"), 1, GL_TRUE,
                     camMatrix.m);
	glUniform3f(glGetUniformLocation(program, "cameraCoord"), px, py, pz);

    GLfloat rotMatrix_x[] = {1.0f,    0.0f, 0.0f, 0.0f,   0.0f,   cos(-lookv),
                           -sin(-lookv), 0.0f, 0.0f, sin(-lookv), cos(-lookv), 0.0f,
                           0.0f,    0.0f, 0.0f, 1.0f};

    GLfloat rotMatrix_y[] = {cos(-lookh),  0.0f, sin(-lookh), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                           -sin(-lookh), 0.0f, cos(-lookh), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    t = 0.5 + 0.5*sin((GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix_x"), 1, GL_TRUE,
                     rotMatrix_x);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix_y"), 1, GL_TRUE,
                     rotMatrix_y);
    glUniform1f(glGetUniformLocation(program, "osillator"),
                     t);

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw object
	glDrawArrays(GL_TRIANGLES, 3, 3);	// draw object

	printError("display");

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(1800, 1200);
	glutCreateWindow ("Raymarch fractal");
	glutDisplayFunc(display);
	glutRepeatingTimer(2);
    	glutPassiveMotionFunc(mouse_moved);
  	glutHideCursor();
	init ();
	glutMainLoop();
	return 0;
}
