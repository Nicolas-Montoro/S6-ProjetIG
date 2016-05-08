/***********************************************************************
* Projet d'informatique graphique
* Par Vincent Fabioux et Nicolas Montoro
* Polytech Paris Sud 2016 - S6 INFO
* 
* Commande de compilation:
* gcc -o projet projet.c -lGL -lGLU -lglut -lm
* Vérifier que OpenGL est installé:
* glxinfo | grep OpenGL
* Installer les librairies "freeglut-dev" et "libtiff-dev".
***********************************************************************/

#include <GL/gl.h>           
#include <GL/glu.h>         
#include <GL/glut.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <math.h>
#include <tiffio.h>

int writetiff(char *filename, char *description,
	      int x, int y, int width, int height, int compression);

#define windowWidth		800
#define windowHeight	800

#define RED   0
#define GREEN 0.5
#define BLUE  1
#define ALPHA 1

#define true  1
#define false 0

#define KEY_ESC 27

#define PI 3.1415926535898

#define MAP_SIZE	100

/* Variables de gestion du temps */
int Step = 0;
int latence = 4;
float delta = 1.0;
int IdleRunning = 1;

/* Variables de la caméra */
float cameraPos[3] = {0, -15, -20};
int cameraSpeed = 1;
int cameraLimits[6] = {-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20),
	-35, -15,
	-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20)};


// Enumération des formes possibles
enum ShapeList
{
	Cube = 1,
	Sphere,
	Cylindre,
	Disque
};

// Structure d'un type d'objet (id, forme, taille[3])
typedef struct
{
	int id;
	int shape;
	float size[3];
	float color[3];
} ObjectType;

// Structure d'un objet (type, position[3], angle[3], matrice)
typedef struct
{
	int type;
	float pos[3];
	float angle[4];
	int matrix;
} Object;



/* Enumération des noms d'objets */
enum ObjectName
{
	Sol = 1,
	Herbe,
	Soleil,
	Mur,
	ChaiseAssise,
	ChaisePied,
	ChaiseBarre,
	ChaiseDossier
};

/* Forme et taille de chaque type d'objet:
 * {Nom, Forme, {tailleX, tailleY, tailleZ}, {R, G, B}}
 */
ObjectType objectsType[] = {
	{Sol, Cube, {MAP_SIZE, 3, MAP_SIZE}, {0.7, 0.7, 0.7}},
	{Herbe, Disque, {0, 1000, 0}, {0, 0.7, 0}},
	{Soleil, Sphere, {7000, 1, 1}, {1, 1, 0}},
	{Mur, Cube, {MAP_SIZE+1.5, 30, 3}, {0.3, 0.3, 0.3}},
	{ChaiseAssise, Cube, {7, 1, 7}, {0.6, 0.3, 0}},
	{ChaisePied, Cube, {1, 5, 1}, {0.6, 0.3, 0}},
	{ChaiseBarre, Cube, {1, 4, 1}, {0.6, 0.3, 0}},
	{ChaiseDossier, Cube, {7, 4, 1}, {0.6, 0.3, 0}},
	
	{0, 0, {0, 0, 0}}
};

/* Type, position et angle de chaque objet:
 * {Type, {posX, posY, posZ}, {angle, x, y, z}, +-matrix}
 */
Object objectsL[] = {
	{Sol, {0, -1.5, 0}, {0, 0, 0, 0}, 1},
	{Herbe, {0, 0, 0}, {90, 1, 0, 0}, 0},
	{Soleil, {500, 0, -100000}, {0, 0, 0, 0}, -1},
	{Mur, {0, 15, MAP_SIZE/2}, {0, 0, 0, 0}, 1},
		{Mur, {0, 0, -1*MAP_SIZE}, {0, 0, 0, 0}, 0},
		{Mur, {MAP_SIZE/2, 0, MAP_SIZE/2}, {90, 0, 1, 0}, 0},
	{Mur, {0, 0, -1*MAP_SIZE}, {0, 0, 0, 0}, -1},
	// Chaise
	{ChaiseAssise, {0, 6.5, -20}, {0, 0, 1, 0}, 1},
		{ChaisePied, {-3, -3, -3}, {0, 0, 0, 0}, 1},
			{ChaisePied, {6, 0, 0}, {0, 0, 0, 0}, 0},
			{ChaisePied, {0, 0, 6}, {0, 0, 0, 0}, 0},
		{ChaisePied, {-6, 0, 0}, {0, 0, 0, 0}, -1},
		{ChaiseBarre, {-3, 2.5, -3}, {0, 0, 0, 0}, 1},
		{ChaiseBarre, {6, 0, 0}, {0, 0, 0, 0}, -1},
	{ChaiseDossier, {0, 6.5, -3}, {0, 0, 0, 0}, -1},
	//Bonhomme
	
	
	
	{0, {0, 0, 0}, {0, 0, 0}, 0}
};
	


enum Sides {Left, Right};


static GLfloat mat_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
static GLfloat mat_ambientanddiffuse[] = { 0.4, 0.4 , 0.0 , 1.0 };
static GLfloat mat_shininess[] = { 20.0};

static GLfloat light_position0[] = { 5.0 , 15.0 , 15.0 , 0.0 };
static GLfloat light_position1[] = { 15.0 , 15.0 , -15.0 , 0.0 };

static GLfloat ambient_light0[] = { 0.0 , 0.0 , 0.0 , 0.0 };
static GLfloat diffuse_light0[] = { 0.7 , 0.7 , 0.7 , 1.0 };
static GLfloat specular_light0[] = { 0.1 , 0.1 , 0.1 , 0.1 };

static GLfloat ambient_light1[] = { 0.50 , 0.50 , 0.50 , 1.0 };
static GLfloat diffuse_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };
static GLfloat specular_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };


/* Variables de gestion de la souris */
int angle_x = 0, angle_y = 0;
int mouse_pos_x = 0, mouse_pos_y = 0;
int mouse_down_is_left = 0;


void render_scene();
GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height); 
GLvoid window_key(unsigned char key, int x, int y); 
GLvoid window_timer();
GLvoid window_mouseFunc(int button, int state, int x, int y);
GLvoid window_motionFunc(int x, int y);
void Faire_Composantes();
void checkCameraLimits();

void init_cube(ObjectType object);
void init_cylinder(ObjectType object, GLUquadricObj* qobj);
void init_sphere(ObjectType object);
void init_disk(ObjectType object, GLUquadricObj* qobj);


int main(int argc, char **argv) 
{  
	// Initialisation  des paramètres de GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Définition et création de la fenêtre graphique
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Projet d'informatique systeme");

	// Initialisation de OpenGL et de la scène
	initGL(); 
	Faire_Composantes(); 

	// Choix du callback pour le tracé graphique...
	glutDisplayFunc(&window_display);
	// .. pour le redimensionnement de la fenêtre
	glutReshapeFunc(&window_reshape);
	// ... pour la gestion des événements clavier
	glutKeyboardFunc(&window_key);
	// .. pour le timer
	glutTimerFunc(latence,&window_timer,Step);
	// ... pour les clicks souris
	glutMouseFunc(&window_mouseFunc);
	// ... pour les déplacement souris
	glutMotionFunc(&window_motionFunc);

	// Boucle principale de gestion des évenements
	glutMainLoop();  

	return 1;
}


/* Gestion du timer */
GLvoid window_timer() 
{
	if(IdleRunning)
	{
		objectsL[7].angle[0] += delta;
	}
	glutTimerFunc(latence,&window_timer,++Step);
	glutPostRedisplay();
}


// Initialisation d'OpenGL
GLvoid initGL() 
{
  // Initialisation de l´éclairement
	glEnable(GL_COLOR_MATERIAL);

  // Définition de deux source lumineuses
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  // Activation de l´éclairage
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  // Propriétés matérielles des objets
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambientanddiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glShadeModel( GL_SMOOTH );
  glEnable(GL_COLOR_MATERIAL);

  // Initialisation du fond
  glClearColor(RED, GREEN, BLUE, ALPHA);        
  // Activation du z-buffer
  glEnable(GL_DEPTH_TEST);
}


// Affichage de la fenêtre
GLvoid window_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  render_scene();

  // Tracé la scène grapnique qui vient juste d'être défini
  glFlush();
}


/* Gestion du redimensionnement de la fenêtre */
GLvoid window_reshape(GLsizei width, GLsizei height)
{  
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho(-20, 20, -20, 20, -1000, 1000);
	glFrustum(-10, 10, -10, 10, 10, 1000000);
	// glScalef(10, 10, 10);

	// Toutes les transformations suivantes s´appliquent au modèle de vue 
	glMatrixMode(GL_MODELVIEW);
}


/* Gestion des évenements clavier */
GLvoid window_key(unsigned char key, int x, int y) 
{  
	switch (key) {
		case KEY_ESC:
			exit(1);
			break;
		case 'p': // Pause
			if (IdleRunning)
				IdleRunning = 0;
			else
				IdleRunning = 1;
			break;
		case '+':
			delta *= 1.01;
			break; 
		case '-':
			delta /= 1.01;
			break;
			
		case 'z':
			cameraPos[0] -= sin(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] += cos(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 's':
			cameraPos[0] += sin(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] -= cos(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'q':
			cameraPos[0] += cos(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] += sin(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'd':
			cameraPos[0] -= cos(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] -= sin(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'a':
			cameraPos[1] -= cameraSpeed;
			if(cameraPos[1] < cameraLimits[2])
				cameraPos[1] = cameraLimits[2];
			break;
		case 'e':
			cameraPos[1] += cameraSpeed;
			if(cameraPos[1] > cameraLimits[3])
				cameraPos[1] = cameraLimits[3];
			break;
	}
}


void checkCameraLimits()
{
	if(cameraPos[0] < cameraLimits[0])
		cameraPos[0] = cameraLimits[0];
	if(cameraPos[0] > cameraLimits[1])
		cameraPos[0] = cameraLimits[1];
	if(cameraPos[2] < cameraLimits[4])
		cameraPos[2] = cameraLimits[4];
	if(cameraPos[2] > cameraLimits[5])
		cameraPos[2] = cameraLimits[5];
}



/* Gestion des clicks de la souris */
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


/* Gestion des déplacements de la souris */
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


/* Initialisation des objets */
void Faire_Composantes() {
	
	// Allocation d´une description de quadrique
	GLUquadricObj* GLAPIENTRY qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL); 
	gluQuadricNormals(qobj, GLU_SMOOTH);

	// Génération des emplacements de display list
	int i;
	for(i = 0; objectsType[i].id; i++);
	int listStart = (int) glGenLists(i);
	for(i = 0; objectsType[i].id; i++)
		objectsType[i].id += listStart;
	for(i = 0; objectsL[i].type; i++)
		objectsL[i].type += listStart;

	// Compilation des display lists
	for(i = 0; objectsType[i].id; i++)
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
			case Disque:
				init_disk(objectsType[i], qobj);
				break;
		}
	}
}


/* Display list de cube */
void init_cube(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glPushMatrix();
			glColor3f(object.color[0], object.color[1], object.color[2]);
			glScalef(object.size[0], object.size[1], object.size[2]);
			glutSolidCube(1.0);
		glPopMatrix();
	glEndList();
}


/* Display list de cylindre */
void init_cylinder(ObjectType object, GLUquadricObj* qobj)
{
	glNewList(object.id, GL_COMPILE);
		glColor3f(object.color[0], object.color[1], object.color[2]);
		gluCylinder(qobj, object.size[0], object.size[1], object.size[2], 20, 20);
	glEndList();
}


/* Display list de sphere */
void init_sphere(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glColor3f(object.color[0], object.color[1], object.color[2]);
		glScalef(1, object.size[1], object.size[2]);
		glutSolidSphere(object.size[0], 20, 20);
	glEndList();
}


void init_disk(ObjectType object, GLUquadricObj* qobj)
{
	glNewList(object.id, GL_COMPILE);
		glColor3f(object.color[0], object.color[1], object.color[2]);
		gluDisk(qobj, object.size[0], object.size[1], 20, 20);
	glEndList();
}

/* Rendu de la scène */
void render_scene()
{
	// Rotation de la souris
	glRotatef(angle_x, 1, 0, 0);
	glRotatef(angle_y, 0, 1, 0);
	
	// Coordonnée de la caméra
	glTranslatef(cameraPos[0], cameraPos[1], cameraPos[2]);
	
	int i, j;
	
	// Affichage de tout les objets
	for(i = 0; objectsL[i].type; i++)
	{
		j = objectsL[i].matrix;
		while(j > 0)
		{
			glPushMatrix();
			j--;
		}
		glTranslatef(objectsL[i].pos[0], objectsL[i].pos[1], objectsL[i].pos[2]);
		glRotatef(objectsL[i].angle[0], objectsL[i].angle[1], objectsL[i].angle[2], objectsL[i].angle[3]);
		glCallList(objectsL[i].type);
		while(j < 0)
		{
			glPopMatrix();
			j++;
		}
	}

	// Permutation des buffers lorsque le tracé est achevé
	glutSwapBuffers();
}
