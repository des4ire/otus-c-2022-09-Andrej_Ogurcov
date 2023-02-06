#include<stdlib.h>
#include<GL/glut.h>


GLfloat vertex[] =    { -1, -1, -1, 	1, -1, -1, 	1, 1, -1, 	-1, 1, -1, 	-1, -1, 1, 	1, -1, 1, 	1, 1, 1, 	-1, 1, 1};
GLubyte idn[] =       {0, 3, 2, 	1, 2, 3, 	7, 6, 0, 	4, 7, 3, 	1, 2, 6, 	5, 4, 5, 	6, 7, 0, 	1, 5, 4};

GLfloat colors[] =   { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1};


static GLfloat angle[]= {0, 0, 0}; //начальный угол
static GLint axis=2;   //


void rotate()
{
	angle[axis] += 1; // шаг вращения в градусах
	if (angle[axis] > 360) // сброс вращения по достижении 360 град
	angle[axis] -= 360;
	glutPostRedisplay(); 
}

void mouse_click(int button, int state, int x __attribute__((unused)), int y __attribute__((unused))) //чтобы варнинги Wpedantic отключить
{
	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	axis=0; // врашение по X
	if (button==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN)
	axis=1; // врашение по Y
	if (button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
	axis=2; // врашение по Z 
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef (angle[0], 1, 0, 0); //угол вращения по X  
	glRotatef (angle[1], 0, 1, 0); //угол вращения по Y
	glRotatef (angle[2], 0, 0, 1); //угол вращения по Z
	glDrawElements(GL_QUADS,24,GL_UNSIGNED_BYTE,idn); // нарисовать куб
	glutSwapBuffers(); // 
}


void redraw(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(width<=height)
	glOrtho (-2, 2, -2*(GLfloat)height/(GLfloat)width, 2*(GLfloat)height / (GLfloat)width, -10, 10);
	else
	glOrtho (-2*(GLfloat)width/(GLfloat)height, 2*(GLfloat)width / (GLfloat)height, -2, 2, -10, 10);
	glMatrixMode(GL_MODELVIEW);
}

void terminate(unsigned char key,int x __attribute__((unused)), int y __attribute__((unused))) //выход при нажатии "ESC"
{
	if(key==27)
	exit(0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutCreateWindow("SpinnerCube3D");
	glutReshapeFunc(redraw); 
	glutDisplayFunc(display); 
	glutIdleFunc(rotate); // вызвать вращение куба во врема простоя
	glutMouseFunc(mouse_click); // обработчик нажатий кнопок мышки
	glEnable(GL_DEPTH_TEST); // глубина для 3d обЪекта
	glEnableClientState(GL_COLOR_ARRAY); // цвет и пропорции
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex); 
	glColorPointer(3, GL_FLOAT, 0, colors); 
	glColor3f(1, 1, 1);
	glutKeyboardFunc(terminate); //обработчик "ESC"
	glutMainLoop();
}
