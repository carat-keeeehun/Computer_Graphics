/*
 * Skeleton code for CSE471 Fall 2019
 *
 * Won-Ki Jeong, wkjeong@unist.ac.kr
 */

#include <stdio.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <assert.h>
#include "math.h"

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

//
// Definitions
//

typedef struct{
    unsigned char x, y, z, w;
} uchar4;
typedef unsigned char uchar;

// BMP loader
void LoadBMPFile(uchar4 **dst, int *width, int *height, const char *name);

#define screenSize 1024
#define	imageSize 512

// Trackball
bool Left_pressed = false;
bool Right_pressed = false;
bool Middle_pressed = false;

// Initial x,y coords in Rotation
float r_init_x = 0;
float r_init_y = 0;

float angle_x = 0;
float angle_y = 0;

// initial x coords in Zooming
float z_init_x = 0;

float zoom = 0.5;

// Initial x,y coords and moust position in Panning
float p_init_x = 0;
float p_init_y = 0;

float init_dx = 0;
float init_dy = 0;
float dx = 0;
float dy = 0;

void mouseButton(int, int, int, int);
void mouseMotion(int, int);
void keyboard(unsigned char, int, int);

int task = 1;

void cube(GLfloat);
void e_cube(GLfloat);
void tex_binding(GLuint ,GLubyte*);

//
// Variables
//

// array to store synthetic cubemap raw image
static GLubyte image1[4][4][4];
static GLubyte image2[4][4][4];
static GLubyte image3[4][4][4];
static GLubyte image4[4][4][4];
static GLubyte image5[4][4][4];
static GLubyte image6[4][4][4];

// texture object handles, FBO handles
GLuint cube_tex, color_tex, s_tex;
GLuint box_tex[6];
GLuint fb, depth_rb;

//
// Functions
//

// create synthetic data for static cubemap
void makeSyntheticImages(void)
{
	int i, j, c;
    
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c = ((((i&0x1)==0)^((j&0x1))==0))*255;
			image1[i][j][0] = (GLubyte) c;
			image1[i][j][1] = (GLubyte) c;
			image1[i][j][2] = (GLubyte) c;
			image1[i][j][3] = (GLubyte) 255;
			
			image2[i][j][0] = (GLubyte) c;
			image2[i][j][1] = (GLubyte) c;
			image2[i][j][2] = (GLubyte) 0;
			image2[i][j][3] = (GLubyte) 255;
			
			image3[i][j][0] = (GLubyte) c;
			image3[i][j][1] = (GLubyte) 0;
			image3[i][j][2] = (GLubyte) c;
			image3[i][j][3] = (GLubyte) 255;
			
			image4[i][j][0] = (GLubyte) 0;
			image4[i][j][1] = (GLubyte) c;
			image4[i][j][2] = (GLubyte) c;
			image4[i][j][3] = (GLubyte) 255;
			
			image5[i][j][0] = (GLubyte) 255;
			image5[i][j][1] = (GLubyte) c;
			image5[i][j][2] = (GLubyte) c;
			image5[i][j][3] = (GLubyte) 255;
			
			image6[i][j][0] = (GLubyte) c;
			image6[i][j][1] = (GLubyte) c;
			image6[i][j][2] = (GLubyte) 255;
			image6[i][j][3] = (GLubyte) 255;
		}
	}
}

void init(void)
{
	// You need to ues glew
	glewInit();
	
	GLfloat diffuse[4] = {1.0, 1.0, 1.0, 1.0};
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// make synthetic cubemap data
	makeSyntheticImages();
	
	//
	// Creating a 2D texture from image
	// 
	int width, height;
	uchar4 *dst;
	LoadBMPFile(&dst, &width, &height, "../mob.bmp"); // this is how to load image
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dst);

	

#define STATIC_CUBEMAP
//#define DYNAMIC_CUBEMAP

	//
	// creating cube map texture (either static or dynamic)
	//

#ifdef STATIC_CUBEMAP
	// create static cubemap from synthetic data
	glGenTextures(1, &cube_tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_tex);

	// ToDo...
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image5);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image6);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

#endif


// generate cubemap on-the-fly
#ifdef DYNAMIC_CUBEMAP
	//RGBA8 Cubemap texture, 24 bit depth texture, 256x256
	glGenTextures(1, &cube_tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_tex);
	
	// ToDo...


	//
	// creating FBO and attach cube map texture
	//

	//-------------------------
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	//Attach one of the faces of the Cubemap texture to this FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cube_tex, 0);
	//-------------------------
	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, imageSize, imageSize);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
	//-------------------------
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout<<"good"<<std::endl; break;
	default:
		assert(false); break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
#endif
}

void idle()
{
	// do something for animation here b/c this will be called when idling

	glutPostRedisplay();
}

// dynamically update cubemap ( Task 3 )
void update_cubemap()
{
	// bind FBO to render to texture
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	// render to +x face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cube_tex, 0);

	// render the entire scene here...

	// render to -x face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, cube_tex, 0);

	// render the entire scene here...

	// render to +y face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, cube_tex, 0);

	// render the entire scene here...

	// render to -y face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, cube_tex, 0);

	// render the entire scene here...

	// render to +z face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, cube_tex, 0);

	// render the entire scene here...

	// render to -z face
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, cube_tex, 0);

	// render the entire scene here...

	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void display(void)
{
	// update dynamic cubemap per frame
#ifdef DYNAMIC_CUBEMAP
	update_cubemap();
#endif

	// render something here...
	/*
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);*/

	// Task 1, 2D taxture mapping
	if (task == 1) {
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();

		// Rotating camera along x-axis, y-axis
		glRotatef(angle_x, 1.0, 0.0, 0.0);
		glRotatef(angle_y, 0.0, 1.0, 0.0);

		// Zooming in and out with right button
		glScalef(zoom, zoom, zoom);

		cube(2.0);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	// Task 2, Static mapping
	else if (task == 2) {
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);

		glGenTextures(6, &box_tex[0]);

		tex_binding(0, &image1[0][0][0]);
		tex_binding(1, &image2[0][0][0]);
		tex_binding(2, &image3[0][0][0]);
		tex_binding(3, &image4[0][0][0]);
		tex_binding(4, &image5[0][0][0]);
		tex_binding(5, &image6[0][0][0]);

		e_cube(1.2);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		glPushMatrix();

		// Rotating camera along x-axis, y-axis
		glRotatef(angle_x, 1.0, 0.0, 0.0);
		glRotatef(angle_y, 0.0, 1.0, 0.0);

		// Zooming in and out with right button
		glScalef(zoom, zoom, zoom);

		glutSolidTeapot(1.0);
		//glutSolidTorus(0.5, 1.0, 100,100);

		glPopMatrix();

		glDisable(GL_TEXTURE_CUBE_MAP);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
	}

	// Task 3, Dynamic mapping
	else if (task == 3) {

	}


	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -20.0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//glutInitWindowSize(screenSize, screenSize);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow (argv[0]);
	
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	// keyboard callback function
	glutKeyboardFunc(keyboard);

	// mouse callback function
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseButton);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Initial projection setting
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.5, 1.5, -1.5, 1.5, -1.5, 1.5);

	glutMainLoop();

	return 0;
}


void cube(GLfloat s)		// s = size of cube
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, color_tex);

	glBegin(GL_QUADS);
	// Front
	glTexCoord2f(0.34f, 0.25f); glVertex3f(-s, -s, s);
	glTexCoord2f(0.66f, 0.25f); glVertex3f(s, -s, s);
	glTexCoord2f(0.66f, 0.5f); glVertex3f(s, s, s);
	glTexCoord2f(0.34f, 0.5f); glVertex3f(-s, s, s);
	
	// Top
	glTexCoord2f(0.34f, 0.5f); glVertex3f(-s, s, s);
	glTexCoord2f(0.66f, 0.5f); glVertex3f(s, s, s);
	glTexCoord2f(0.66f, 0.75f); glVertex3f(s, s, -s);
	glTexCoord2f(0.34f, 0.75f); glVertex3f(-s, s, -s);

	// Back
	glTexCoord2f(0.34f, 0.75f); glVertex3f(-s, s, -s);
	glTexCoord2f(0.66f, 0.75f); glVertex3f(s, s, -s);
	glTexCoord2f(0.66f, 1.0f); glVertex3f(s, -s, -s);
	glTexCoord2f(0.34f, 1.0f); glVertex3f(-s, -s, -s);

	// Bottom
	glTexCoord2f(0.34f, 0.25f); glVertex3f(-s, -s, s);
	glTexCoord2f(0.66f, 0.25f); glVertex3f(s, -s, s);
	glTexCoord2f(0.66f, 0.0f); glVertex3f(s, -s, -s);
	glTexCoord2f(0.34f, 0.0f); glVertex3f(-s, -s, -s);

	// Left side
	glTexCoord2f(0.34f, 0.5f); glVertex3f(-s, s, s);
	glTexCoord2f(0.34f, 0.75f); glVertex3f(-s, s, -s);
	glTexCoord2f(0.0f, 0.75f); glVertex3f(-s, -s, -s);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-s, -s, s);

	// Right side
	glTexCoord2f(0.66f, 0.5f); glVertex3f(s, s, s);
	glTexCoord2f(0.66f, 0.75f); glVertex3f(s, s, -s);
	glTexCoord2f(1.0f, 0.75f); glVertex3f(s, -s, -s);
	glTexCoord2f(1.0f, 0.5f); glVertex3f(s, -s, s);
	glEnd();
}

void e_cube(GLfloat s) {
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	glBindTexture(GL_TEXTURE_2D, box_tex[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, s);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, s);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, -s);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, box_tex[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, s);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, -s);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, s);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, box_tex[2]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(s, s, s);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, s, s);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, s, -s);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(s, s, -s);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, box_tex[3]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(s, -s, -s);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, -s, s);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(s, -s, s);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, box_tex[5]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -0.5f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(s, -s, -s);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(s, s, -s);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-s, s, -s);
	glEnd();
}

void tex_binding(GLuint n, GLubyte* image) {

	glBindTexture(GL_TEXTURE_2D, box_tex[n]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void mouseButton(int button, int state, int x, int y) {	// Click

	if (button == GLUT_LEFT_BUTTON) switch (state)
	{	// Left Button : Rotation
	case GLUT_DOWN:	// with pushing a button
		// Start Motion
		Left_pressed = true;

		r_init_x = x - angle_y;	// Update the x coord upto angle_y.
		r_init_y = y - angle_x;	// Update the y coord upto angle_x.
		break;
	case GLUT_UP:		// without pushing a button
		// Stop Motion
		Left_pressed = false;
		break;
	}

	if (button == GLUT_RIGHT_BUTTON) switch (state)
	{	// Right Button : Zooming
	case GLUT_DOWN:
		// Start Motion
		Right_pressed = true;

		z_init_x = x;
		break;
	case GLUT_UP:
		// Stop Motion
		Right_pressed = false;
		break;
	}
}

void mouseMotion(int x, int y) {	// Drag

	if (Left_pressed) {		// Rotation
		angle_x = y - r_init_y;	// Rotate along x-axis, so angle_x. 
		angle_y = x - r_init_x;	// Rotate along y-axis, so angle_y.

		glutPostRedisplay();
	}

	if (Right_pressed) {	// Zooming
		zoom += (x - z_init_x) / 5000;
		if (zoom > 5) { zoom = 5; }
		if (zoom < 0.1) { zoom = 0.1; }

		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case '27':	//	Exit from glutMainLoop
		exit(0);

	case 'a':	// Task 1, 2D texture mapping
		task = 1;
		break;

	case 's':	// Task 2, static mapping
		task = 2;
		break;

	case 'd':	// Task 3, dynamic mapping
		task = 3;
		break;

	}
}