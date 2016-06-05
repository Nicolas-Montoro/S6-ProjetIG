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

#define MAP_SIZE	110

/* Variables de gestion du temps */
int Step = 0;
int latence = 4;
float delta = 0.1;
int IdleRunning = 1;

/* Variables de la caméra */
float cameraPos[3] = {0, -15, -1*(MAP_SIZE/2-20)};
int cameraSpeed = 1;
int cameraLimits[6] = {-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20),
	-35, -15,
	-1*(MAP_SIZE/2-20), (MAP_SIZE/2-20)};
	
/* Variables du personnage */
enum PersoStates {Idle, Running, Walking, GoingToSit};
int persoAnim = Running;
float deltaAnim = 0;
float posRunning = 0;
float posWalking = 0;
int goToChair = 0;
int animDirection = 1;


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

void init_cube(ObjectType object);
void init_cylinder(ObjectType object, GLUquadricObj* qobj);
void init_unicylinder(ObjectType object, GLUquadricObj* qobj);
void init_sphere(ObjectType object);
void init_disk(ObjectType object, GLUquadricObj* qobj);
void init_torus(ObjectType object);


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


/* Gestion des animations */
GLvoid window_timer() 
{
	if(IdleRunning)
	{
		// Indicateur: le perso doit aller vers la chaise
		if(goToChair)
			objectsL[15].pos[1] = -12.4;
		else
			objectsL[15].pos[1] = -14;
		
		// Réinitialisation des angles
		objectsL[16].pos[1] = 16;
		objectsL[19].angle[0] = 0;
		objectsL[23].angle[0] = 0;
		objectsL[25].angle[0] = 0;
		objectsL[debutBras+1].angle[0] = 0;
		objectsL[debutBras+3].angle[0] = 0;
		objectsL[debutBras+6].angle[0] = 0;
		objectsL[debutBras+8].angle[0] = 0;
		objectsL[debutBras+10].angle[0] = 90;
		objectsL[debutBras+12].angle[0] = 0;
		objectsL[debutBras+14].angle[0] = 90;
		objectsL[debutBras+16].angle[0] = 0;
		
		switch(persoAnim)
		{
			case Running: // Le perso cours
				deltaAnim += delta;
				posRunning += delta;
				if(posRunning >= 2*3*PI)
					posRunning = posRunning - 2*3*PI;
					
				if(goToChair
				&& posRunning >= 3.0/2.0*PI-delta
				&& posRunning <= 3.0/2.0*PI+delta)
				{
					posWalking = 0;
					persoAnim = Walking;
					deltaAnim = 0;
				}
				
				objectsL[16].pos[0] = cos(posRunning/3)*20;
				objectsL[16].pos[2] = sin(posRunning/3)*20;
				objectsL[16].angle[0] = posRunning*180/PI/-3;
				
				objectsL[debutBras+1].angle[0] = sin(deltaAnim)*40;
				objectsL[debutBras+3].angle[0] = -90;
				objectsL[debutBras+6].angle[0] = sin(deltaAnim+PI)*40;
				objectsL[debutBras+8].angle[0] = -90;
				objectsL[debutBras+10].angle[0] = sin(deltaAnim)*40+80;
				objectsL[debutBras+12].angle[0] = sin(deltaAnim)*40+40;
				objectsL[debutBras+14].angle[0] = sin(deltaAnim+PI)*40+80;
				objectsL[debutBras+16].angle[0] = sin(deltaAnim+PI)*40+40;
				break;
				
			case Walking: // Le perso marche vers ou non vers la chaise
				deltaAnim += delta;
				if(deltaAnim < 90/10)
				{
					if(animDirection == 1)
						objectsL[16].angle[0] = -90 - deltaAnim*10;
					else
						objectsL[16].angle[0] = -1*deltaAnim*10;
				}
				else if(posWalking == -10)
				{
					persoAnim = Running;
					animDirection = 1;
				}
				else
				{
					if(animDirection == 1)
					{
						posWalking += delta;
						objectsL[16].angle[0] = 180;
					}
					else
					{
						posWalking -= delta;
						objectsL[16].angle[0] = 0;
					}
					objectsL[16].pos[2] = 20 - posWalking;
					objectsL[16].pos[0] = 0;
				
					objectsL[debutBras+1].angle[0] = sin(deltaAnim)*20;
					objectsL[debutBras+6].angle[0] = sin(deltaAnim+PI)*20;
					objectsL[debutBras+10].angle[0] = sin(deltaAnim)*10+90;
					objectsL[debutBras+12].angle[0] = sin(deltaAnim)*10+10;
					objectsL[debutBras+14].angle[0] = sin(deltaAnim+PI)*10+90;
					objectsL[debutBras+16].angle[0] = sin(deltaAnim+PI)*10+10;
					if(posWalking >= 15.0)
					{
						persoAnim = GoingToSit;
						deltaAnim = 0;
					}
					if(posWalking <= 0)
					{
						deltaAnim = 0;
						posWalking = -10;
					}
				}
				break;
			
			case GoingToSit: // Le persop s'assoit ou se lève
				if(animDirection == 1)
					deltaAnim += delta;
				else
					deltaAnim -= delta;
				if(deltaAnim < 180/10)
				{
					if(animDirection == 1)
						objectsL[16].angle[0] = 180 - deltaAnim*10;
					else
					{
						posWalking = 15;
						persoAnim = Walking;
						deltaAnim = 90/10;
					}
				}
				else if(deltaAnim <= 28)
				{
					objectsL[16].angle[0] = 0;
					objectsL[16].pos[2] = 5 - deltaAnim/2 + 18/2;
					objectsL[16].pos[1] = 16 - deltaAnim/3.33 + 18/3.33;
					objectsL[debutBras+10].angle[0] = 90 + sin((deltaAnim-18)*PI/10/2)*-80;
					objectsL[debutBras+12].angle[0] = sin((deltaAnim-18)*PI/10/2)*80;
					objectsL[debutBras+14].angle[0] = 90 + sin((deltaAnim-18)*PI/10/2)*-80;
					objectsL[debutBras+16].angle[0] = sin((deltaAnim-18)*PI/10/2)*80;
				}
				else
				{
					objectsL[debutBras+10].angle[0] = 10;
					objectsL[debutBras+12].angle[0] = 80;
					objectsL[debutBras+14].angle[0] = 10;
					objectsL[debutBras+16].angle[0] = 80;
					objectsL[16].pos[1] = 13;
					if(!goToChair)
					{
						deltaAnim = 28;
						animDirection = -1;
					}
				}
			
			default: // Le perso est immobile
				if(persoAnim != GoingToSit)
					deltaAnim += delta;
				objectsL[19].angle[0] = sin(deltaAnim/6)*40;
				objectsL[23].angle[0] = sin(deltaAnim/6)*40;
				objectsL[25].angle[0] = sin(deltaAnim/6)*40;
				objectsL[debutBras+1].angle[0] = sin(deltaAnim/6)*10;
				objectsL[debutBras+6].angle[0] = sin(deltaAnim/6+PI)*10;
				break;
		}
	}
	glutTimerFunc(latence,&window_timer,++Step);
	glutPostRedisplay();
}


// Initialisation d'OpenGL
GLvoid initGL() 
{
  // Initialisation de l´éclairement
	glEnable(GL_COLOR_MATERIAL);

  // Définition d'une source lumineuses
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
  //~ glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
  //~ glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
  //~ glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  //~ glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot0);
  //~ glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  // Activation de l´éclairage
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //~ glEnable(GL_LIGHT1);

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
		case 'r': // Activer/désactiver course
			if(persoAnim == Idle)
			{
				persoAnim = Running;
				deltaAnim = 0;
			}
			else if(persoAnim == Running)
			{
				persoAnim = Idle;
				deltaAnim = 0;
			}
			break;
		case 'c': // Aller sur la chaise
			if(goToChair == 0)
				goToChair = 1;
			else
				goToChair = 0;
			break;
		case '+': // Augmenter vitesse
			delta *= 1.01;
			break; 
		case '-': // Diminuer vitesse
			delta /= 1.01;
			break;
			
		case 'z': // Avancer
			cameraPos[0] -= sin(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] += cos(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 's': // Reculer
			cameraPos[0] += sin(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] -= cos(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'q': // Pas à gauche
			cameraPos[0] += cos(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] += sin(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'd': // Pas à droite
			cameraPos[0] -= cos(angle_y*PI/180) * cameraSpeed;
			cameraPos[2] -= sin(angle_y*PI/180) * cameraSpeed;
			checkCameraLimits();
			break;
		case 'a': // Monter
			cameraPos[1] -= cameraSpeed;
			if(cameraPos[1] < cameraLimits[2])
				cameraPos[1] = cameraLimits[2];
			break;
		case 'e': // Descendre
			cameraPos[1] += cameraSpeed;
			if(cameraPos[1] > cameraLimits[3])
				cameraPos[1] = cameraLimits[3];
			break;
	}
}


/* Vérifie que le joueur ne sort pas du terrain */
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
	if(angle_x > 90)
		angle_x = 90;
	else if(angle_x < -90)
		angle_x = -90;

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
			case CylindreUni:
				init_unicylinder(objectsType[i], qobj);
				break;
			case Sphere:
				init_sphere(objectsType[i]);
				break;
			case Disque:
				init_disk(objectsType[i], qobj);
				break;
			case Torus:
				init_torus(objectsType[i]);
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


/* Display list de cylindre uni */
void init_unicylinder(ObjectType object, GLUquadricObj* qobj)
{
	glNewList(object.id, GL_COMPILE);
		glPushMatrix();
			glColor3f(object.color[0], object.color[1], object.color[2]);
			glScalef(1, object.size[1], 1);
			gluCylinder(qobj, object.size[0], object.size[0], object.size[2], 20, 20);
		glPopMatrix();
	glEndList();
}

/* Display list de sphere */
void init_sphere(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glPushMatrix();
			glColor3f(object.color[0], object.color[1], object.color[2]);
			glScalef(1, object.size[1], object.size[2]);
			glutSolidSphere(object.size[0], 20, 20);
		glPopMatrix();
	glEndList();
}

/* Display list de disque */
void init_disk(ObjectType object, GLUquadricObj* qobj)
{
	glNewList(object.id, GL_COMPILE);
		glColor3f(object.color[0], object.color[1], object.color[2]);
		gluDisk(qobj, object.size[0], object.size[1], 20, 20);
	glEndList();
}

/* Display list de torus */
void init_torus(ObjectType object)
{
	glNewList(object.id, GL_COMPILE);
		glScalef(1, object.size[2], 1);
		glColor3f(object.color[0], object.color[1], object.color[2]);
		glutSolidTorus(object.size[0], object.size[1], 20, 20);
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
		for(j = 0; j < objectsL[i].push; j++)
			glPushMatrix();
		glTranslatef(objectsL[i].pos[0], objectsL[i].pos[1], objectsL[i].pos[2]);
		glRotatef(objectsL[i].angle[0], objectsL[i].angle[1], objectsL[i].angle[2], objectsL[i].angle[3]);
		glCallList(objectsL[i].type);
		for(j = 0; j < objectsL[i].pop; j++)
			glPopMatrix();
	}

	// Permutation des buffers lorsque le tracé est achevé
	glutSwapBuffers();
}
