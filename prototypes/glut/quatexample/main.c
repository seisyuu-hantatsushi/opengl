
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <GL/glut.h>

#include "quat.h"

#define WINDOW_WIDTH  512
#define WINDOW_HEIGHT 512

#define APP_QUIT (1)

static bool bScaling = false;
static int beginX, beginY;
static quat current, last;
static GLfloat scaleFactor = 1.0;

static const GLfloat whiteColor[] = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat blackColor[] = { 0.0, 0.0, 0.0, 1.0 };
static const GLfloat blueColor[]  = { 0.0, 0.0, 1.0, 1.0 };

static int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;

static const float R = 0.8;
static const float ROOT_2_INV = 0.70710678118654752440;

#if 0
void test(){
	quat q,id,i,j,k;

	q = quat_zero();
	print_quat(&q);
	id = quat_identity();
	print_quat(&id);
	q = quat_set(1.0, 1.0, 1.0, 1.0);
	print_quat(&q);
	q = quat_mul(&q, &id);
	print_quat(&q);
	q = quat_mul(&q, &q);
	print_quat(&q);

	i = quat_set(0.0, 1.0, 0.0, 0.0);
	j = quat_set(0.0, 0.0, 1.0, 0.0);
	k = quat_set(0.0, 0.0, 0.0, 1.0);

	q = quat_mul(&i, &j);
	print_quat(&q);
	q = quat_mul(&j, &i);
	print_quat(&q);
	printf("\n");
	q = quat_mul(&j, &k);
	print_quat(&q);
	q = quat_mul(&k, &j);
	print_quat(&q);
	printf("\n");
	q = quat_mul(&k, &i);
	print_quat(&q);
	q = quat_mul(&i, &k);
	print_quat(&q);
	printf("\n");

	q = quat_mul(&k, &k);
	print_quat(&q);
	q = quat_mul(&i, &i);
	print_quat(&q);
	q = quat_mul(&j, &j);
	print_quat(&q);
	printf("\n");
}
#endif

#define SQR(x) (x*x)

static GLfloat projectPlaneToSphere(GLfloat x, GLfloat y){
	const GLfloat R = 0.8;
	GLfloat z,d,d_sqr;

	d_sqr = SQR(x) + SQR(y);

	d = sqrt(d_sqr);

	if(d < R){
		z = sqrt(2.0*SQR(R) - d_sqr);
	}
	else {
		z = SQR(R)/d;
	}

	return z;
}

static quat simulateTrackball(GLfloat p1x, GLfloat p1y, GLfloat p2x, GLfloat p2y){
	quat q = quat_identity();

	if(p1x == p2x && p1y == p2y){
		// マウスの移動量がゼロならば
		q = quat_identity();
	}
	else {
		// マウスの移動量がゼロでないならば
		quat p1, p2, a, d;
		float p1z, p2z;
		float s, t;

		// ベクトルp1はトラックボール移動開始位置
		p1z = projectPlaneToSphere(p1x, p1y);
		p1 = quat_set(0.0f, p1x, p1y, p1z);

		// ベクトルp2はトラックボール移動終了位置
		p2z = projectPlaneToSphere(p2x, p2y);
		p2 = quat_set(0.0f, p2x, p2y, p2z);

		// トラックボールの回転軸を求める
		a = quat_mul(&p1, &p2);

		a.w = 0.0;
		s = quat_norm(&a);
		a = quat_div_real(s, &a);

		d = quat_sub(&p1, &p2);

		t = quat_norm(&d) / (2.0 * R * ROOT_2_INV);
		if(t > 1.0) t = 1.0;
		q = quat_set(cos(asin(t)), a.x*t, a.y*t, a.z*t);
	}

	return q;
}

static void draw_frame(void){
	const float L=0.5;
	static GLfloat vdata[8][3] = {
		{-L,-L,-L},{ L,-L,-L},{ L, L,-L},{-L, L, -L},
		{-L,-L, L},{ L,-L, L},{ L, L, L},{-L, L,  L}
	};
	static GLint tindices[6][4] = {
		{0,1,2,3},{0,4,5,1},{0,3,7,4},
		{1,5,6,2},{2,6,7,3},{5,4,7,6}
	};

	int i;

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blueColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blackColor);

	for(i=0;i<3;i++){
		glBegin(GL_QUADS);
		{
			glNormal3fv(&vdata[tindices[i][0]][0]);
			glVertex3fv(&vdata[tindices[i][0]][0]);
			glNormal3fv(&vdata[tindices[i][1]][0]);
			glVertex3fv(&vdata[tindices[i][1]][0]);
			glNormal3fv(&vdata[tindices[i][2]][0]);
			glVertex3fv(&vdata[tindices[i][2]][0]);
			glNormal3fv(&vdata[tindices[i][3]][0]);
			glVertex3fv(&vdata[tindices[i][3]][0]);
		}
		glEnd();
	}
	return;
}

static void draw_objects(void){
	const float X = 0.525731112119133606/2.0;
	const float Z = 0.850650808352039932/2.0;
	static GLfloat vdata[12][3] = {
		{  -X, 0.0,   Z}, {   X, 0.0,   Z}, {  -X, 0.0,  -Z},
		{   X, 0.0,  -Z}, { 0.0,   Z,   X}, { 0.0,   Z,  -X},
		{ 0.0,  -Z,   X}, { 0.0,  -Z,  -X}, {   Z,   X, 0.0},
		{  -Z,   X, 0.0}, {   Z,  -X, 0.0}, {  -Z,  -X, 0.0}
	};
	static GLint tindices[20][3] = {
		{  0,  4,  1}, {  0,  9,  4}, {  9,  5,  4}, {  4,  5,  8},
		{  4,  8,  1}, {  8, 10,  1}, {  8,  3, 10}, {  5,  3,  8},
		{  5,  2,  3}, {  2,  7,  3}, {  7, 10,  3}, {  7,  6, 10},
		{  7, 11,  6}, { 11,  0,  6}, {  0,  1,  6}, {  6,  1, 10},
		{  9,  0, 11}, {  9, 11,  2}, {  9,  2,  5}, {  7,  2, 11}
	};
	int i;

	glEnable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, whiteColor);
	for(i=0; i<20; i++){
		glBegin(GL_TRIANGLES);
		{
			glNormal3fv(&vdata[tindices[i][0]][0]);
			glVertex3fv(&vdata[tindices[i][0]][0]);
			glNormal3fv(&vdata[tindices[i][1]][0]);
			glVertex3fv(&vdata[tindices[i][1]][0]);
			glNormal3fv(&vdata[tindices[i][2]][0]);
			glVertex3fv(&vdata[tindices[i][2]][0]);
		}
		glEnd();
	}
	glDisable(GL_CULL_FACE);
	return;
}

static void draw(void){
	static bool bFirstCall = true;
	static GLint displayList = 0;

	if(bFirstCall){
		bFirstCall = false;
		displayList = glGenLists(1);
		glNewList(displayList, GL_COMPILE_AND_EXECUTE);
		draw_frame();
		draw_objects();
		glEndList();
	}
	else{
		glCallList(displayList);
	}
	return;
}

static void displayFunc(void){

	matrix4x4 m;

	//printf("call displayFunc\n");
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	quat_to_rotation_matrix(&current, m);
	glScalef(scaleFactor, scaleFactor, scaleFactor);
	glMultMatrixf(&m[0][0]);

	draw();

	glutSwapBuffers();
	//printf("exit displayFunc\n");
	return;
}

static void reshapeFunc(int w, int h){
	static float FOVY=20.0;

	printf("reshapeFunc(%d,%d)\n",w,h);

	glViewport(0,0,(GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (GLfloat)w/(GLfloat)h, 2.0, -2.0);
	glMatrixMode(GL_MODELVIEW);

	return;
}

static void appMenuFunc(int menu){
	switch(menu){
	case APP_QUIT:
		exit(0);
		break;
	default:
		break;
	}
	return;
}

static void mouseEventFunc(int button, int state, int x, int y){

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		beginX = x;
		beginY = y;
		//printf("begin (%d,%d)\n", beginX, beginY);
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
			bScaling = true;
		}
		else {
			bScaling = false;
		}
	}

	return;
}

static void mouseMotionFunc(int x, int y){
	static int count = 0;
	if(bScaling){
		scaleFactor =
			scaleFactor * (1.0 + (((float) (beginY - y))/height));
		beginX = x;
		beginY = y;
	}
	else {
		quat t;

		last = simulateTrackball( (2.0 * beginX - width) / width,
								  (height - 2.0 * beginY) / height,
								  (2.0 * x - width) / width,
								  (height - 2.0 * y) / height );
		beginX = x;
		beginY = y;

		t = quat_mul(&last, &current);
		current = t;

		if(++count % 97 == 0){
			GLfloat n;
			n = quat_norm(&current);
			current = quat_div_real(n, &current);
		}
	}
	glutPostRedisplay();
	return;
}

static void menuFunc(int dummy0){
	return;
}

static void initGLUTmenu(void){

	int appMenu;

	appMenu = glutCreateMenu(appMenuFunc);
	glutAddMenuEntry("Quit (Q)", APP_QUIT);

	glutCreateMenu(menuFunc);
	glutAddSubMenu("Application", appMenu);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	return;

}

static void initGLUT(int *argc, char **argv){
	glutInit(argc, argv);

	/* Request RGBA, Double Buffer, Depth Buufer */
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutInitWindowPosition(100,100);

	glutCreateWindow("quaternion");

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	glutMouseFunc(mouseEventFunc);
	glutMotionFunc(mouseMotionFunc);

	initGLUTmenu();

	return;
}

static void initGL(void){
	GLfloat light1[] = { 0.0, 0.0, 10.0, 1.0 };

	glClearColor(0.0, 0.0, 0.0, 1.0); // background color

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, blackColor);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1);
	glEnable(GL_LIGHT1);

	current = simulateTrackball(0.0,0.0,0.0,0.0);

	return;
}

int main(int argc, char **argv){

	initGLUT(&argc, argv);
	initGL();

	glutMainLoop();

	return 0;
}
