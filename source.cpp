#include <iostream>
#ifdef WIN32
#include <windows.h>

#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include<stdio.h>
#include<vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
using namespace std;
struct Vertex
{
	float x, y, z;
};
struct Triangle
{
	int v1, v2, v3;
};
struct Vec {
	float elem[4];


};

/* Here you need to implement a 4-by-4 matrix */
struct Mat {
	float elem[4][4];


	/*Normalize the matrix*/
	void normalize()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					elem[i][j] = 1.0f;
				else
					elem[i][j] = 0.0f;
			}
		}
	}

	/*This function is to reload the multipliation operator for matrix products*/
	const Mat operator * (const Mat& right) const
	{
		Mat result;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.elem[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					result.elem[i][j] += elem[i][k] * right.elem[k][j];
				}
			}
		}

		return result;
	}

	/*This function is to reload the multipliation operator for matrix times a 4-by-1 vector*/
	const Vec operator * (const Vec& vec) const
	{
		Vec result;

		for (int i = 0; i < 4; i++)
		{

			result.elem[i] = 0;
			for (int k = 0; k < 4; k++)
			{
				result.elem[i] += elem[i][k] * vec.elem[k];
			}

		}

		return result;
	}

};

vector<Vertex> vt_list;
vector<Triangle> tg_list;
/* Define data */
int mouse_down = 0;
int change_mode = 0;
int shearing = 0;
int current_x = 0, current_y = 0;

/*Parameter control the rotation*/
float theta_x = .0f;
float theta_y = .0f;
float theta_z = .0f;
/*Parameter control the translation*/
float translate_x = .0f;
float translate_y = .0f;
float translate_z = .0f;
 
float shear_x = .0f;
float shear_y = .0f;
float shear_z = .0f;

int projection = 1;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;
static GLdouble ex = .0, ey = 0.0, ez = 60.0, upx = .0, upy = 1.0, upz = .0, ox = 0.0, oy = 0.0, oz = 0;

GLenum surfaceormesh = GL_TRIANGLES;


void transformTriangles()
{
	/*Initialize the rotation matrix*/
	Mat Rx, Ry, Rz;
	Mat T;
	Mat shear;

	/*Initially set all the matrix to */
	Rx.normalize();
	Ry.normalize();
	Rz.normalize();
	T.normalize();
	shear.normalize();

	/*rotation matrix about x axis */
	Rx.elem[1][1] = cos(theta_x);
	Rx.elem[1][2] = -sin(theta_x);
	Rx.elem[2][1] = sin(theta_x);
	Rx.elem[2][2] = cos(theta_x);


	/*rotation matrix about y axis */
	Ry.elem[0][0] = cos(theta_y);
	Ry.elem[0][2] = sin(theta_y);
	Ry.elem[2][0] = -sin(theta_y);
	Ry.elem[2][2] = cos(theta_y);

	/*Rotate matrix by z-axis*/
	Rz.elem[0][0] = cos(theta_z);
	Rz.elem[0][1] = -sin(theta_z);
	Rz.elem[1][0] = sin(theta_z);
	Rz.elem[1][1] = cos(theta_z);

	/* transaltion matrix*/
	T.elem[0][3] = translate_x;
	T.elem[1][3] = translate_y;

	/*shear matrix*/
	shear.elem[0][1] = shear_x;
	shear.elem[1][0] = shear_y;

	/*Final transformation matrix P*/
	Mat P =T * Rx * Ry * Rz * shear;

	/* Go through each vertex*/
	for (int i = 0; i < vt_list.size(); i++)
	{
		Vec cur_pt, updated_pt;
		cur_pt.elem[0] = vt_list[i].x;
		cur_pt.elem[1] = vt_list[i].y;
		cur_pt.elem[2] = vt_list[i].z;
		cur_pt.elem[3] = 1.0f;
		updated_pt = P * cur_pt;
		vt_list[i].x = updated_pt.elem[0] / updated_pt.elem[3];
		vt_list[i].y = updated_pt.elem[1] / updated_pt.elem[3];
		vt_list[i].z = updated_pt.elem[2] / updated_pt.elem[3];

		
	}

	/*Reset the angles and translations */
	theta_x = 0;
	theta_y = 0;
	theta_z = 0;
	
	shear_x = 0;
	shear_y = 0;
	shear_z = 0;
}

void renderAllTriangles()
{
	for (int i = 1; i < tg_list.size();i++)
	{
		glBegin(surfaceormesh); //draw triangles
		glColor3f(0.1, 1, 0);
		glVertex3d((vt_list[tg_list[i].v1 - 1].x), (vt_list[tg_list[i].v1 - 1].y), (vt_list[tg_list[i].v1 - 1].z));
		glVertex3d((vt_list[tg_list[i].v2 - 1].x), (vt_list[tg_list[i].v2 - 1].y), (vt_list[tg_list[i].v2 - 1].z));
		glVertex3d((vt_list[tg_list[i].v3 - 1].x), (vt_list[tg_list[i].v3 - 1].y), (vt_list[tg_list[i].v3 - 1].z));

		glEnd();
	}
}


//callback functions:
void onDisplay()
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();


	if (projection == 0)
	{

		glOrtho(-3, 3, -1, 1, 0, 100);


	}
	else
		gluPerspective(60, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, .1, 100.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(ex, ey, ez, ox, oy, ox, upx, upy, upz);

	glColor3f(1, 0, 0);

	
	if (mouse_down == 1) //only when the mouse is dragging, this function is called
		transformTriangles();
	
	renderAllTriangles();
	


	glutSwapBuffers();//display the buffer




}

void onMouse(int button, int state, int x, int y)
{


	GLint specialKey = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_DOWN) {
			mouse_down = 1;
			current_x = x;
			current_y = y;
			if (specialKey == GLUT_ACTIVE_SHIFT)
			{
				change_mode = 1;
			}
			else
			{
				change_mode = 0;
			}
		}
		else if (state == GLUT_UP)
		{
			mouse_down = 0;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)

			break;

	default:
		break;

	}

}

void onMouseMotion(int x, int y)
{
	if (mouse_down == 1)
	{
		if (change_mode == 0)
		{
			if (shearing == 0) {
				theta_y += (float)(x - current_x) / 100.0f;
				theta_x += (float)(y - current_y) / 100.0f;
			}
			else
			{
				theta_z += (float)(x - current_x) / 100.0f;
				theta_x += (float)(y - current_y) / 100.0f;
			}
		}
		else {
			shear_y += (float)(x - current_x) / 100.0f;
			shear_x += (float)(y - current_y) / 100.0f;
		}
		

		current_x = x;
		current_y = y;
	}
	glutPostRedisplay();
}

void onKeyboard(unsigned char key, int x, int y)
{
	//printf("Z\n");
	switch (key)
	{
	case 27:
		exit(1);
	case 'z':
		if (shearing == 0)
		{
			shearing = 1;
		}
		else
		{
			shearing = 0;
		}
			glutPostRedisplay();
			break;

	case 'm':
		if (surfaceormesh == GL_TRIANGLES)
			surfaceormesh = GL_LINE_LOOP;
		else
			surfaceormesh = GL_TRIANGLES;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}




int main(int argc, char * argv[]) {

	FILE *file = fopen("teddy.obj", "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0) {
			Vertex vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vt_list.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			Triangle t;
			fscanf(file, "%d %d %d\n", &t.v1, &t.v2, &t.v3);
			tg_list.push_back(t);
		}
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);


	glutInitWindowSize(640, 480);


	glutInitWindowPosition(200, 300);


	glutCreateWindow("teddy");
	glutDisplayFunc(onDisplay);
	glutKeyboardFunc(onKeyboard);

	glutMouseFunc(onMouse);
	glutMotionFunc(onMouseMotion);






	glutMainLoop();

	return 0;
}