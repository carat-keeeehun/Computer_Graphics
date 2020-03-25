/*
 * Skeleton code for CSE471 Intro to Computer Graphics
 *
 * Won-Ki Jeong, wkjeong@unist.ac.kr
 *
 * 2019-2nd, 20131218 Park KeeHun
 */

#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GL/glut.h>
using namespace std;

struct dim_3 {
	GLfloat x, y, z;
};

struct idx_3 {
	GLuint v1, v2, v3;
};

dim_3* vtc;		// vertices vector
dim_3* vtc_n;	// per-vertex normal vector
dim_3* face_n;	// per-face normal vector
idx_3* idx;		// per-face indices

static int vtcNum;		// total number of vertices
static int faceNum;		// total number of faces
static int zero;		// just for zero

bool Left_pressed = false;
bool Right_pressed = false;
bool Middle_pressed = false;
bool is_ortho = false;
bool is_persp = false;

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

void key_input(unsigned char, int, int);
void read_file(void);
void mouseButton(int, int, int, int);
void mouseMotion(int, int);

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void renderScene(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Rotating camera along x-axis, y-axis
	glRotatef(angle_x, 1.0, 0.0, 0.0);
	glRotatef(angle_y, 0.0, 1.0, 0.0);

	// Zooming in and out with right button
	glScalef(zoom, zoom, zoom);

	// Panning (Translation on x/y plane)
	if (is_ortho) glTranslatef(dx, dy, 0.0);					// Just translation in orthogonal
	if (is_persp) gluLookAt(-dx, -dy, 1, -dx, -dy, 0, 0, 1, 0);	// also move the camera in perspective

	// Using 2 Light source (lightPos_L, lightPos_R)
	GLfloat lightPos_L[] = { -50.0,  0.0,  0.0, 1.0 };
	GLfloat lightPos_R[] = {  50.0,  0.0,  0.0, 1.0 };

	GLfloat color_L[] = { 1.0, 0.0, 0.0, 1.0 };	// Red - Left side
	GLfloat color_R[] = { 0.0, 1.0, 1.0, 1.0 };	// Sky Blue - Right side

	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color_L);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color_L);
	glLightfv(GL_LIGHT0, GL_AMBIENT, color_L);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos_L);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color_R);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color_R);
	glLightfv(GL_LIGHT1, GL_AMBIENT, color_R);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos_R);


	// Draw something here!
	size_t vSize = vtcNum * sizeof(float) * 3;
	size_t nSize = vtcNum * sizeof(float) * 3;
	size_t iSize = faceNum * sizeof(unsigned int) * 3;

	// vertex buffer object
	GLuint vtc_buffer, idx_buffer;
	glGenBuffers(1, &vtc_buffer);
	glGenBuffers(1, &idx_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vtc_buffer);
	glBufferData(GL_ARRAY_BUFFER, vSize + nSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, &vtc[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, &vtc_n[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, &idx[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//cout << "End of making buffer" << endl;

	glBindBuffer(GL_ARRAY_BUFFER, vtc_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glNormalPointer(GL_FLOAT, 0, (void*)vSize);

	//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, faceNum * 3, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vtc_buffer);
	glDeleteBuffers(1, &idx_buffer);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	read_file();

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(700, 700);
	glutCreateWindow("CSE471 - Assignment 1");

	glewInit();

	// Initializing with orthogonal projection and smooth rendering
	is_ortho = true;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_SMOOTH);
	glShadeModel(GL_SMOOTH);

	// register callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(renderScene);

	glutKeyboardFunc(key_input);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	// enter GLUT event processing cycle
	glutMainLoop();

	delete[] vtc;
	delete[] vtc_n;
	delete[] face_n;
	delete[] idx;

	return 1;
}

void key_input(unsigned char key, int x, int y) {

	switch (key) {

	case '27':	// Exit from glutMainLoop
		exit(0);

	case 'o':	// Orthogonal projection mode

		is_ortho = true;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);

		break;

	case 'p':	// Perspective projection mode

		is_persp = true;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -1.0, 1.0, 0.5, 10.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		
		break;

	case 's':	// Smooth rendering

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_SMOOTH);
		glShadeModel(GL_SMOOTH);
		break;

	case 'w':	// Wireframe rendering

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}

	glutPostRedisplay();
}

void read_file(void) {
	
	char input_f;
	ifstream input;

	while (1) {
		cout << "b : Open <bunny.off>" << endl;
		cout << "f : Open <fandisk.off>" << endl;
		cout << "d : Open <dragon-full.off>" << endl;
		cout << "::: Enter b/f/d : ";
		cin >> input_f;

		if (input_f == 'b') {
			input.open("bunny.off");
			break;
		}
		else if (input_f == 'f') {
			input.open("fandisk.off");
			break;
		}
		else if (input_f == 'd') {
			input.open("dragon-full.off");
			break;
		}
		else {
			cout << "False input" << endl;
		}
	}

	if (input.fail()) {
		cout << "파일을 여는데 실패" << endl;
		return;
	}

	string first;
	input >> first;
	if (first != "OFF") {
		cout << "OFF 입력 실패" << endl;
		return;
	}

	input >> vtcNum;
	input >> faceNum;
	input >> zero;

	vtc = new dim_3[vtcNum];
	vtc_n = new dim_3[vtcNum];
	face_n = new dim_3[faceNum];
	idx = new idx_3[faceNum];
	GLuint* count = new GLuint[vtcNum];

	int i;
	for (i = 0; i < vtcNum; i++) {
		input >> vtc[i].x;
		input >> vtc[i].y;
		input >> vtc[i].z;

		vtc_n[i].x = 0;
		vtc_n[i].y = 0;
		vtc_n[i].z = 0;

		count[i] = 0;
	}


	// To make initial object size larger, multiple 10 to data
	if (input_f == 'b') {
		for (i = 0; i < vtcNum; i++) {
			vtc[i].x *= 10;
			vtc[i].y *= 10;
			vtc[i].z *= 10;
		}
	}
	if (input_f == 'd') {
		for (i = 0; i < vtcNum; i++) {
			vtc[i].x *= 10;
			vtc[i].y *= 10;
			vtc[i].z *= 10;
		}
	}


	float a[3], b[3], sum;
	for (i = 0; i < faceNum; i++) {
		input >> idx[i].v1;
		input >> idx[i].v1;
		input >> idx[i].v2;
		input >> idx[i].v3;

		// a[] is for v2-v1, b[] is for v3-v1
		a[0] = vtc[idx[i].v2].x - vtc[idx[i].v1].x;
		a[1] = vtc[idx[i].v2].y - vtc[idx[i].v1].y;
		a[2] = vtc[idx[i].v2].z - vtc[idx[i].v1].z;
		b[0] = vtc[idx[i].v3].x - vtc[idx[i].v1].x;
		b[1] = vtc[idx[i].v3].y - vtc[idx[i].v1].y;
		b[2] = vtc[idx[i].v3].z - vtc[idx[i].v1].z;

		// Cross product with v1 and v2 --> face normal vector
		face_n[i].x = a[1] * b[2] - a[2] * b[1];
		face_n[i].y = a[2] * b[0] - a[0] * b[2];
		face_n[i].z = a[0] * b[1] - a[1] * b[0];

		sum = pow(face_n[i].x, 2) + pow(face_n[i].y, 2) + pow(face_n[i].z, 2);
		sum = sqrt(sum);

		face_n[i].x = face_n[i].x / sum;
		face_n[i].y = face_n[i].y / sum;
		face_n[i].z = face_n[i].z / sum;

		// Calculate all vertices normal vector related to i-th face.
		vtc_n[idx[i].v1].x += face_n[i].x;
		vtc_n[idx[i].v1].y += face_n[i].y;
		vtc_n[idx[i].v1].z += face_n[i].z;
		count[idx[i].v1]++;

		vtc_n[idx[i].v2].x += face_n[i].x;
		vtc_n[idx[i].v2].y += face_n[i].y;
		vtc_n[idx[i].v2].z += face_n[i].z;
		count[idx[i].v2]++;

		vtc_n[idx[i].v3].x += face_n[i].x;
		vtc_n[idx[i].v3].y += face_n[i].y;
		vtc_n[idx[i].v3].z += face_n[i].z;
		count[idx[i].v3]++;
	}

	// Complete to calculate per-vertex normal vector (normalization)
	for (i = 0; i < vtcNum; i++) {
		vtc_n[i].x = vtc_n[i].x / count[i];
		vtc_n[i].y = vtc_n[i].y / count[i];
		vtc_n[i].z = vtc_n[i].z / count[i];
	}

	input.close();
	cout << "End of File I/O" << endl;
}

void mouseButton(int button, int state, int x, int y) {	// Click

	if (button == GLUT_LEFT_BUTTON) switch (state)
	{	// Left Button : Rotation
		case GLUT_DOWN :	// with pushing a button
			// Start Motion
			Left_pressed = true;
			
			r_init_x = x - angle_y;	// Update the x coord upto angle_y.
			r_init_y = y - angle_x;	// Update the y coord upto angle_x.
			break;
		case GLUT_UP :		// without pushing a button
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

	if (button == GLUT_MIDDLE_BUTTON) switch (state)
	{	// Middle Button : Panning (Translation on x/y plane)
		case GLUT_DOWN:
			// Start Motion
			Middle_pressed = true;

			p_init_x = x - dx;
			p_init_y = y - dy;
			init_dx = dx;
			init_dy = dy;
			break;
		case GLUT_UP:
			// Stop Motion
			Middle_pressed = false;
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
		zoom += (x - z_init_x)/5000 ;
		if (zoom > 5) { zoom = 5;}
		if (zoom < 0.1) { zoom = 0.1;}

		glutPostRedisplay();
	}

	if (Middle_pressed) {	// Panning (Translation on x/y plane)

		// To realize an immediate change of direction.
		dx = init_dx + (GLfloat)(x - p_init_x) * 0.0115f;
		dy = init_dy - (GLfloat)(y - p_init_y) * 0.0115f;

		glutPostRedisplay();
	}

}


