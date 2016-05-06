/*****************************************************************************
File: TD3-avatar-skel.c

Informatique Graphique IG1
IFIPS
Christian Jacquemin, Université Paris 11

Copyright (C) 2007 University Paris 11 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include <GL/gl.h>           
#include <GL/glu.h>         
#include <GL/glut.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <math.h>
#include <tiffio.h>     /* Sam Leffler's libtiff library. */
int writetiff(char *filename, char *description,
	      int x, int y, int width, int height, int compression);

#define    windowWidth 800
#define    windowHeight 800

#define RED   0
#define GREEN 0
#define BLUE  0
#define ALPHA 1

#define true  1
#define false 0

#define KEY_ESC 27

#define PI 3.1415926535898

#define position_Ini                   -30.0

float t = 0.f;
float delta = 0.1f;
float k = 0.001f;
float K = 0.002f;
int IdleRunning = 1;
int TraceEcran = 0;
int RangFichierStockage = 0;
float position = position_Ini;


// Enumération des formes possibles
enum ShapeList
{
	Cube,
	Sphere,
	Cylindre
};


// Structure d'un type d'objet (id, forme, taille[3])
typedef struct
{
	int id;
	int shape;
	float size[3];
} ObjectType;


// Structure d'un objet (type, position[3], matrice)
typedef struct
{
	int type;
	float pos[3];
	int matrix;
} Object;


// Enumération des noms d'objets
#define OBJECTS_TYPE_COUNT	2
enum ObjectName
{
	ChaisePied,
	ChaiseAssise
};


// Forme et taille de chaque type d'objet
ObjectType objectsType[OBJECTS_TYPE_COUNT] = {
	{ChaisePied, Cube, {1, 5, 1}},
	{ChaiseAssise, Cube, {7, 1, 7}}
};

#define OBJECTS_COUNT	5
Object objects[OBJECTS_COUNT] = {
	// Chaise
	{ChaisePied, {-3, 0, -3}, 1},
	{ChaisePied, {6, 0, 0}, 0},
	{ChaisePied, {0, 0, 6}, 0},
	{ChaisePied, {-6, 0, 0}, -1},
	{ChaiseAssise, {0, 3, 0}, 0}
};
	


enum Sides {Left, Right};


static GLfloat mat_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
static GLfloat mat_ambientanddiffuse[] = { 0.4, 0.4 , 0.0 , 1.0 };
static GLfloat mat_shininess[] = { 20.0};

static GLfloat light_position0[] = { 15.0 , 15.0 , 15.0 , 0.0 };
static GLfloat light_position1[] = { 15.0 , 15.0 , -15.0 , 0.0 };

static GLfloat ambient_light0[] = { 0.0 , 0.0 , 0.0 , 0.0 };
static GLfloat diffuse_light0[] = { 0.7 , 0.7 , 0.7 , 1.0 };
static GLfloat specular_light0[] = { 0.1 , 0.1 , 0.1 , 0.1 };

static GLfloat ambient_light1[] = { 0.50 , 0.50 , 0.50 , 1.0 };
static GLfloat diffuse_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };
static GLfloat specular_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };

int angle_x = 0, angle_y = 0;
int mouse_pos_x = 0, mouse_pos_y = 0;
int mouse_down_is_left = 0;

int Step = 0;
int latence = 4;

void init_scene();
void render_scene();
void init_angles();
GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height); 
GLvoid window_key(unsigned char key, int x, int y); 
GLvoid window_timer();
GLvoid window_mouseFunc(int button, int state, int x, int y);
GLvoid window_motionFunc(int x, int y);
void Faire_Composantes();

void init_cube(ObjectType object);
void init_cylinder(ObjectType object, GLUquadricObj* qobj);
void init_sphere(ObjectType object);


int main(int argc, char **argv) 
{  
  // initialisation  des paramètres de GLUT en fonction
  // des arguments sur la ligne de commande
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // définition et création de la fenêtre graphique
  glutInitWindowSize(windowWidth,windowHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Projet d'informatique systeme");

  // initialisation de OpenGL et de la scène
  initGL();  
  init_scene();

  // choix des procédures de callback pour 
  // le tracé graphique
  glutDisplayFunc(&window_display);
  // le redimensionnement de la fenêtre
  glutReshapeFunc(&window_reshape);
  // la gestion des événements clavier
  glutKeyboardFunc(&window_key);
  // fonction appelée régulièrement entre deux gestions d´événements
  glutTimerFunc(latence,&window_timer,Step);
  // la gestion des clicks souris
  glutMouseFunc(&window_mouseFunc);
  // la gestion des déplacement souris
  glutMotionFunc(&window_motionFunc);

  // la boucle prinicipale de gestion des événements utilisateur
  glutMainLoop();  

  return 1;
}

// initialisation du fond de la fenêtre graphique : noir opaque

GLvoid initGL() 
{
  // initialisation de l´éclairement
	glEnable(GL_COLOR_MATERIAL);

  // définition de deux source lumineuses
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  // activation de l´éclairement
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  // propriétés matérielles des objets
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambientanddiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glShadeModel( GL_SMOOTH );
  glEnable(GL_COLOR_MATERIAL);

  // initialisation du fond
  glClearColor(RED, GREEN, BLUE, ALPHA);        
  // z-buffer
  glEnable(GL_DEPTH_TEST);
}

void init_scene()
{
  // initialise des display lists des composantes cylindriques du corps
  Faire_Composantes();
}

// fonction de call-back pour l´affichage dans la fenêtre

GLvoid window_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  render_scene();

  // Tracé la scène grapnique qui vient juste d'être définie
  glFlush();
}

// fonction de call-back pour le redimensionnement de la fenêtre

GLvoid window_reshape(GLsizei width, GLsizei height)
{  
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20, 20, -20, 20, -1000, 1000);
	// glFrustum(-20, 20, -20, 20, 10, 1000);
	// glScalef(10, 10, 10);

	// toutes les transformations suivantes s´appliquent au modèle de vue 
	glMatrixMode(GL_MODELVIEW);
}

// fonction de call-back pour la gestion des événements clavier

GLvoid window_key(unsigned char key, int x, int y) 
{  
	switch (key) {    
		case KEY_ESC:  
			exit(1);                    
			break; 
		case 's':  
			if (IdleRunning) {
				glutTimerFunc(latence,NULL,Step);
				IdleRunning = 0;
			} 
			else {
				glutTimerFunc(latence,&window_timer,Step);
				IdleRunning = 1;
			}
			break; 
		case '+':  
			delta *= 1.01;
			break; 
		case '-':  
			delta /= 1.01;
			break; 
		default:
			printf ("La touche %d n´est pas active.\n", key);
			break;
	}
}

// fonction de call-back appelée régulièrement

GLvoid window_timer() 
{
	glutTimerFunc(latence,&window_timer,++Step);
	glutPostRedisplay();
}

// fonction de call-back de gestion des clicks souris

GLvoid window_mouseFunc(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouse_pos_x = x;
		mouse_pos_y = y;
		mouse_down_is_left = 1;
	}
	else
	{
		mouse_down_is_left = 0;
	}
}

// fonction de call-back de gestion des déplacement de la souris

GLvoid window_motionFunc(int x, int y)
{
	if( !mouse_down_is_left )
		return;

	angle_x += y - mouse_pos_y;
	angle_y += x - mouse_pos_x;

	mouse_pos_x = x;
	mouse_pos_y = y;

	glutPostRedisplay();
}


// Initialisation des objets
void Faire_Composantes() {
	
	// Allocation d´une description de quadrique
	GLUquadricObj* GLAPIENTRY qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL); 
	gluQuadricNormals(qobj, GLU_SMOOTH);

	int listStart = (int) glGenLists(OBJECTS_TYPE_COUNT), i;
	for(i = 0; i < OBJECTS_TYPE_COUNT; i++)
		objectsType[i].id += listStart;
	for(i = 0; i < OBJECTS_COUNT; i++)
		objects[i].type += listStart;

	// Compilation des display lists
	for(i = 0; i < OBJECTS_TYPE_COUNT; i++)
	{
		switch(objectsType[i].shape)
		{
			case Cube:
				init_cube(objectsType[i]);
				break;
			case Cylindre:
				init_cylinder(objectsType[i], qobj);
				break;
			case Sphere:
				init_sphere(objectsType[i]);
				break;
		}
	}
}


void init_cube(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glPushMatrix();
			glScalef(object.size[0], object.size[1], object.size[2]);
			glutSolidCube(1.0);
		glPopMatrix();
	glEndList();
}


void init_cylinder(ObjectType object, GLUquadricObj* qobj)
{
	glNewList(object.id, GL_COMPILE);
		gluCylinder(qobj, object.size[0], object.size[1], object.size[2], 20, 20);
	glEndList();
}


void init_sphere(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glScalef(1, object.size[1], object.size[2]);
		glutSolidSphere(object.size[0], 20, 20);
	glEndList();
}


void render_scene()
{
	// Rotation de la souris
	glRotatef(angle_x, 1, 0, 0);
	glRotatef(angle_y, 0, 1, 0);

	glColor3f(1.0, 1.0, 1.0);
	
	int i, j;
	
	// Affichage de tout les objets
	for(i = 0; i < OBJECTS_COUNT; i++)
	{
		j = objects[i].matrix;
		while(j > 0)
		{
			glPushMatrix();
			j--;
		}
		glTranslatef(objects[i].pos[0], objects[i].pos[1], objects[i].pos[2]);
		glCallList(objects[i].type);
		while(j < 0)
		{
			glPopMatrix();
			j++;
		}
	}

	// Permutation des buffers lorsque le tracé est achevé
	glutSwapBuffers();
}
