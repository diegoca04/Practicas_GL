#define PROYECTO "Practica 2"

#include <iostream>	
#include <cmath>
#include <codebase.h>

GLuint anilloMenor, anilloMayor;

static const GLfloat vertices[18] = { 1,0,0, 0,1,0, 0,0,1, -1,0,0, 0,-1,0, 0,0,-1 };

static const GLfloat colores[18] = { 1,0,0, 0,1,0, 0,0,1, 1,1,0, 0,1,1, 1,0,1 };

static const GLuint indices[24] = { 1,2,0, 1,0,5, 1,5,3, 1,3,2, 4,2,0, 4,0,5, 4,5,3, 4,3,2 };

GLuint anillo(float diamExt, float diamInt, float grosor, float res = 20) 
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	glBegin(GL_POLYGON);
	glVertex3f(0.5 * cos(0 * 2 * 3.1415926 / 5), 0.5 * sin(0 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(1 * 2 * 3.1415926 / 5), 0.5 * sin(1 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(2 * 2 * 3.1415926 / 5), 0.5 * sin(2 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(3 * 2 * 3.1415926 / 5), 0.5 * sin(3 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(4 * 2 * 3.1415926 / 5), 0.5 * sin(4 * 2 * 3.1415926 / 5), 0.0);
	glEnd();
	glEndList();
	return id;
}

/*std::vector<cb::Vec3> puntosCircunferencia(int numeroPuntos, float radio, float fase)
{

}*/

void init()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);
}

void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, indices);
	//glCallList(anilloMenor);
	//glCallList(anilloMayor);
	
	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, indices);
	//glCallList(anilloMenor);
	//glCallList(anilloMayor);

	glFlush();
}

void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1000, 100);
	glutCreateWindow(PROYECTO);
	init();
	std::cout << PROYECTO << " running" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}