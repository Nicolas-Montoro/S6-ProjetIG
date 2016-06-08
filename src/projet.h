#ifndef PROJET_H_INCLUDED
#define PROJET_H_INCLUDED
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
	      
	
void mouvement(int indexMin,int indexMax,int angleMin,int angleMax);

void allMouvements(int angle, double speed,int pos);

#define windowWidth		800
#define windowHeight	800

#define RED   0
#define GREEN 0.5
#define BLUE  1
#define ALPHA 1

#define RIGHT 	4
#define DOWN 	5
#define LEFT	6
#define UP 		7
#define CHANGE_KEY 42

#define true  1
#define false 0

#define KEY_ESC 27

#define PI 3.1415926535898

#define MAP_SIZE	110
#define PERSO_RUN_SPEED	1
#define PERSO_WALK_SPEED	0.5
#define PERSO_TURN_SPEED	5

/* Variables de gestion du temps */
int Step = 0;
int latence = 4;
float delta = 0.1;
int IdleRunning = 1;
int wall;

/* Variables de la caméra */
float cameraPos[3] = {0, -15, -1*(MAP_SIZE/2-20)};
int cameraSpeed = 1;
int cameraLimits[6] = {-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20),
	-35, -15,
	-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20)};
	
int chaiseLimits[2]={-10,10};
	
/* Variables du personnage */
enum PersoStates {Idle, Running, Walking, WalkingReverse,
	GoingToSit, GettingUp, Sitting, Turn90_180, Turn180_0, Turn0_90};
int persoAnim = Running;
float deltaAnim = 0;
float posAuto = 0;
float posX = 0;
float posZ = 0;
float direction = 0;

int goToChair = 0;
int manualMode = 0;
int moveDirection = 0;


// Enumération des formes possibles
enum ShapeList
{
	Cube = 1,
	Sphere,
	Cylindre,
	CylindreUni,
	Disque,
	Torus
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
	int push;
	int pop;
	int type;
	float pos[3];
	float angle[4];
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
	ChaiseDossier,
	HautBuste,
	Buste,
	Bassin,
	Cou,
	Tete,
	Epaule,
	AvantBras,
	Coude,
	Bras,
	Main,
	Cuisse,
	Tibia,
	Genou,
	Pied,
	Oeil,
	Nez,
	Iris,
	Cheveux1,
	Cheveux2,
	ChaiseIndicateur
};

/* Forme et taille de chaque type d'objet:
 * {Nom, Forme, {tailleX, tailleY, tailleZ}, {R, G, B}}
 */
ObjectType objectsType[] = {
	// Décor
	{Sol, Cube, {MAP_SIZE, 3, MAP_SIZE}, {0.7, 0.7, 0.7}},
	{Herbe, Disque, {0, 1000, 0}, {0, 0.7, 0}},
	{Soleil, Sphere, {7000, 1, 1}, {1, 1, 0}},
	{Mur, Cube, {MAP_SIZE+1.5, 30, 3}, {0.3, 0.3, 0.3}},
	
	// Chaise
	{ChaiseAssise, Cube, {7, 1, 7}, {0.6, 0.3, 0}},
	{ChaisePied, Cube, {1, 5, 1}, {0.6, 0.3, 0}},
	{ChaiseBarre, Cube, {1, 4, 1}, {0.6, 0.3, 0}},
	{ChaiseDossier, Cube, {7, 4, 1}, {0.6, 0.3, 0}},
	{ChaiseIndicateur, Disque, {0, 7, 0}, {1, 1, 0}},
	
	// Bonhomme
	{Buste, CylindreUni, {3, 0.5, 6}, {1, 0.9, 0.7}},
	{Epaule, Sphere, {1, 1, 1}, {1, 0.9, 0.7}},
	{Coude, Sphere, {0.55, 1, 1}, {1, 0.9, 0.7}},
	{Main, Sphere, {0.55, 1, 1}, {0.85, 0.45, 0}},
	{AvantBras, Cylindre, {0.8, 0.5, 4}, {1, 0.9, 0.7}},
	{Bras, Cylindre, {0.5,0.25,3.75},{1, 0.9, 0.7}},
	{Bassin, Sphere , {1,1,2.9}, {0, 0, 0.3}},
	{Cuisse, Cylindre,{1.17,1,5}, {0, 0, 0.3}},
	{Genou, Sphere, {1, 1, 1}, {0, 0, 0.3}},
	{Tibia, Cylindre, {0.8, 0.6, 5}, {0, 0, 0.3}},
	{Pied, Sphere, {1.17, 0.5, 2}, {1, 0.9, 0.7}},
	{HautBuste, Sphere, {3, 0.5, 0.5}, {1, 0.9, 0.7}},
	{Cou, Cylindre, {1,1, 2}, {0.85, 0.45, 0}},
	{Tete, Sphere, {1.8,1, 1}, {0.85, 0.45, 0}},
	{Oeil, Sphere, {0.4,0.7, 1}, {1, 1, 1}},
	{Iris, Sphere, {0.15,1, 0.3}, {0, 0, 0}},
	{Nez, Sphere, {1,0.3, 0.3}, {0.85, 0.45, 0}},
	{Cheveux1, Torus, {0.5, 1.2, 0.9}, {0.1, 0.1, 0.1}},
	{Cheveux2, Sphere, {1.4, 0.9, 0.5}, {0.1, 0.1, 0.1}},
	
	{0, 0, {0, 0, 0}}
};

/* Type, position et angle de chaque objet:
 * {PushMatrix, PopMatrix, Type, {posX, posY, posZ}, {angle, x, y, z}}
 */
Object objectsL[] = {
	// Décor:		0	à	6
	{1, 0, Sol, {0, -1.5, 0}},
	{0, 0, Herbe, {0, 0, 0}, {90, 1, 0, 0}},
	{0, 1, Soleil, {500, 0, -100000}},
	
	{1, 0, Mur, {0, 15, MAP_SIZE/2}},
		{0, 0, Mur, {0, 0, -1*MAP_SIZE}},
		{0, 0, Mur, {MAP_SIZE/2, 0, MAP_SIZE/2}, {90, 0, 1, 0}},
	{0, 1, Mur, {0, 0, -1*MAP_SIZE}},
	
	
	// Chaise		7	à	15
	{1, 0, ChaiseAssise, {0, 6, 0}, {0, 0, 0, 0}},
	
		{1, 0, ChaisePied, {-3, -3, -3}},
			{0, 0, ChaisePied, {6, 0, 0}},
			{0, 0, ChaisePied, {0, 0, 6}},
		{0, 1, ChaisePied, {-6, 0, 0}},
		
		{1, 0, ChaiseBarre, {-3, 2.5, -3}},
		{0, 1, ChaiseBarre, {6, 0, 0}},
	{0, 0, ChaiseDossier, {0, 6.5, -3}},
	{0, 1, ChaiseIndicateur, {0, -12.4, 3}, {90, 1, 0, 0}},
	
	
	//Bonhomme		16	à	38
	{1, 0, HautBuste, {0, 16, -5}, {0, 0,1,0}},
		{1, 1, Buste, {0, 0, 0}, {90, 1,0,0}},
		{1, 1, Cou, {0, 2, 0}, {90, 1,0,0}},
		{1, 0, Tete, {0, 3, 0}, {0, 0, 1, 0}}, // 19
		{1, 0, Cheveux1, {0, 1.3, 0}, {90, 1, 0, 0}},
		{0, 1, Cheveux2, {0, 0, 0}, {0, 0, 0, 0}},
		{1, 1, Nez, {0, 0.2, 1.5}, {90, 0, 1, 1.3}},
		{1, 0, Oeil, {0.5, 0.5, 1.4}, {0, 0, 1, 0}}, // 23
		{0, 1, Iris, {0, 0, 0.4}},
		{1, 0, Oeil, {-0.5, 0.5, 1.4}, {0, 0, 1, 0}}, // 25
		{0, 2, Iris, {0, 0, 0.4}},
		{1, 1, Bassin, {0, -6, 0}, {90,0,1,0}}, // 20
	
		{1, 0, Epaule, {2.5, 0, 0}, {90, 1,0.2,0}}, // debutBras
			{1, 0, AvantBras, {0, 0, 0}, {0, 1, 0, 0}},
			{0, 0, Coude, {0, 0, 4}},
			{1, 0, Bras, {0, 0, 0}, {0, 1, 0, 0}},
		{0, 3, Main, {0, 0, 4}, {0, 0, 0, 0}}, // 25
			
		{1, 0, Epaule, {-2.5, 0, 0}, {90, 1, -0.2, 0}},
			{1, 0, AvantBras, {0, 0, 0}, {0, 1, 0, 0}},
			{0, 0, Coude, {0, 0, 4}},
			{1, 0, Bras, {0, 0, 0}, {0, 1, 0, 0}},
		{0, 3, Main, {0, 0, 4}, {0, 0, 0, 0}}, // 30
			
		{1, 0, Cuisse, {1.6, -5.2, 0}, {90, 1, 0, 0}},
			{0, 0, Genou, {0, 0, 5}},
			{1, 0, Tibia, {0, 0, 0}, {0, 1, 0, 0}},			
		{0, 2, Pied, {0, 0.8,5}, {90, 1,0,0}}, // 34
			
		{1, 0, Cuisse, {-1.6, -5.2, 0}, {90, 1,0,0}},
			{0, 0, Genou, {0, 0, 5}},
			{1, 0, Tibia, {0, 0, 0}, {0, 1, 0, 0}},
	{0, 4, Pied, {0, 0.8,5}, {90, 1,0,0}}, // 38
	
	{0, 0, 0, {0, 0, 0}, {0, 0, 0, 0}}
};
int debutBras = 28;



/* Variables de gestion de l'éclairage */
static GLfloat mat_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
static GLfloat mat_ambientanddiffuse[] = { 0.4, 0.4 , 0.0 , 1.0 };
static GLfloat mat_shininess[] = {20.0};

static GLfloat light_position0[] = {-500.0 , 100000.0-7000.0/2, 0.0 , 0.0};
static GLfloat ambient_light0[] = {0.6, 0.6, 0.6, 0.6};
static GLfloat diffuse_light0[] = {0.4 , 0.4 , 0.4 , 0.4};
static GLfloat specular_light0[] = {0 , 0 , 0 , 0};


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
void checkPersoLimits();

void init_cube(ObjectType object);
void init_cylinder(ObjectType object, GLUquadricObj* qobj);
void init_unicylinder(ObjectType object, GLUquadricObj* qobj);
void init_sphere(ObjectType object);
void init_disk(ObjectType object, GLUquadricObj* qobj);
void init_torus(ObjectType object);


#endif
