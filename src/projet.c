#include "projet.h"

int main(int argc, char **argv) 
{  
	// Initialisation  des paramètres de GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Définition et création de la fenêtre graphique
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Projet d'informatique Graphique");

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
		
		if(manualMode)
		{
				objectsL[16].pos[0] = posX;				
				objectsL[16].pos[1] = 16;
				objectsL[16].pos[2] = posZ;
				objectsL[16].angle[0] = direction;
				if(persoAnim == Running)
				{
					posX += sin(direction*PI/180)*PERSO_RUN_SPEED;
					posZ += cos(direction*PI/180)*PERSO_RUN_SPEED;
				}
				else if(persoAnim == Walking)
				{
					posX -= sin(direction*PI/180)*PERSO_WALK_SPEED;
					posZ -= cos(direction*PI/180)*PERSO_WALK_SPEED;
				}
				checkPersoLimits();
				if(moveDirection > 0)
					moveDirection--;
				else
					persoAnim = Idle;
				
		}
		else // Gestion des déplacements automatiques
		{
			switch(persoAnim)
			{
				case Running: // Le perso cours
					posAuto += delta;
					if(posAuto >= 2*3*PI)
						posAuto = posAuto - 2*3*PI;
					objectsL[16].pos[0] = cos(posAuto/3)*20;
					objectsL[16].pos[2] = sin(posAuto/3)*20;
					objectsL[16].angle[0] = posAuto*180/PI/-3;
					
					if(goToChair
					&& posAuto >= 3.0/2.0*PI-delta
					&& posAuto <= 3.0/2.0*PI+delta)
					{
						posAuto = 0;
						deltaAnim = 0;
						persoAnim = Turn90_180;
					}
					break;
					
				case Walking: // Le perso marche vers la chaise
				case WalkingReverse: // Le perso marche à l'opposé de la chaise
					objectsL[16].pos[2] = 20 - posAuto;
					objectsL[16].pos[0] = 0;
					if(persoAnim == Walking)
					{
						posAuto += delta;
						if(posAuto > 15)
						{
							persoAnim = Turn180_0;
							posAuto = 0;
							deltaAnim = 0;
						}
					}
					else
					{
						posAuto -= delta;
						if(posAuto < 0)
						{
							persoAnim = Turn0_90;
							posAuto = 0;
							deltaAnim = 0;
						}
					}
					break;
				
				case GoingToSit: // Le perso s'assoit
				case GettingUp: // Le perso se lève
					break;
					
				case Sitting: // Le perso est assit
					objectsL[16].pos[2] = 0;
					objectsL[16].pos[0] = 0;
					if(!goToChair)
					{
						posAuto = 0;
						deltaAnim = 10;
						persoAnim = GettingUp;
					}
					break;
					
				case Turn90_180: // Le perso se tourne de -90° à 180°
				case Turn0_90:  // Le perso se tourne de 0° à -90°
					objectsL[16].pos[2] = 20;
					objectsL[16].pos[0] = 0;
					break;
				case Turn180_0:  // Le perso se tourne de 180° à 0°
					objectsL[16].pos[2] = 5;
					objectsL[16].pos[0] = 0;
					break;
				
				default: // Le perso est immobile
					break;
			}
		}
		
		// Réinitialisation des animations
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
		
		// Running -> Turn90_180 -> Walking -> Turn180_0 -> GoingToSit -> Sitting
		// Sitting -> WalkingReverse -> Turn180_90
		// Gestion des animations
		switch(persoAnim)
		{
			case Running: // Le perso cours
				deltaAnim += delta;
				objectsL[debutBras+1].angle[0] = sin(deltaAnim)*40;
				objectsL[debutBras+3].angle[0] = -90;
				objectsL[debutBras+6].angle[0] = sin(deltaAnim+PI)*40;
				objectsL[debutBras+8].angle[0] = -90;
				objectsL[debutBras+10].angle[0] = sin(deltaAnim)*40+80;
				objectsL[debutBras+12].angle[0] = sin(deltaAnim)*40+40;
				objectsL[debutBras+14].angle[0] = sin(deltaAnim+PI)*40+80;
				objectsL[debutBras+16].angle[0] = sin(deltaAnim+PI)*40+40;
				break;
				
			case Walking: // Le perso marche vers la chaise
			case WalkingReverse: // Le perso marche à l'opposé de la chaise
				deltaAnim += delta;
				objectsL[debutBras+1].angle[0] = sin(deltaAnim)*20;
				objectsL[debutBras+6].angle[0] = sin(deltaAnim+PI)*20;
				objectsL[debutBras+10].angle[0] = sin(deltaAnim)*10+90;
				objectsL[debutBras+12].angle[0] = sin(deltaAnim)*10+10;
				objectsL[debutBras+14].angle[0] = sin(deltaAnim+PI)*10+90;
				objectsL[debutBras+16].angle[0] = sin(deltaAnim+PI)*10+10;
				break;
			
			case GoingToSit: // Le perso va s'assoir
			case GettingUp: // Le perso se lève
				objectsL[16].pos[2] = 5 - deltaAnim/2;
				objectsL[16].pos[1] = 16 - deltaAnim/3.33;
				objectsL[debutBras+10].angle[0] = 90 + sin(deltaAnim*PI/10/2)*-80;
				objectsL[debutBras+12].angle[0] = sin(deltaAnim*PI/10/2)*80;
				objectsL[debutBras+14].angle[0] = 90 + sin(deltaAnim*PI/10/2)*-80;
				objectsL[debutBras+16].angle[0] = sin(deltaAnim*PI/10/2)*80;
				if(persoAnim == GoingToSit)
				{
					deltaAnim += delta;
					if(deltaAnim > 10 + delta)
						persoAnim = Sitting;
				}
				else
				{
					deltaAnim -= delta;
					if(deltaAnim < 0)
					{
						persoAnim = WalkingReverse;
						posAuto = 15;
					}
				}
				break;
			
			case Turn90_180: // Le perso se tourne de -90° à 180°
			case Turn180_0:  // Le perso se tourne de 180° à 0°
			case Turn0_90:  // Le perso se tourne de 0° à -90°
				deltaAnim += delta;
				if(persoAnim == Turn90_180)
				{
					objectsL[16].angle[0] = -90 - deltaAnim*10;
					if(deltaAnim >= 90/10)
					{
						persoAnim = Walking;
						deltaAnim = 0;
					}
				}
				else if(persoAnim == Turn180_0)
				{
					objectsL[16].angle[0] = 180 - deltaAnim*10;
					if(deltaAnim >= 180/10)
					{
						persoAnim = GoingToSit;
						deltaAnim = 0;
					}
				}
				else
				{
					objectsL[16].angle[0] = -1*deltaAnim*10;
					if(deltaAnim >= 90/10)
					{
						persoAnim = Running;
						deltaAnim = 0;
						posAuto = PI*1.5;
					}
				}
			
			case Sitting: // Le perso est assit
			default: // Le perso est immobile
				if(persoAnim == Sitting)
				{
					objectsL[debutBras+10].angle[0] = 10;
					objectsL[debutBras+12].angle[0] = 80;
					objectsL[debutBras+14].angle[0] = 10;
					objectsL[debutBras+16].angle[0] = 80;
				}
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
	wall=1;
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
				posAuto = 0;
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
		case 't': // Activer/désactiver le mode manuel
			if(manualMode == 0)
			{
				manualMode = 1;
				persoAnim = Idle;
				posX = 0;
				posZ = 20;
				direction = 0;
			}
			else
			{
				manualMode = 0;
				persoAnim = Running;
			}
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
		case 'm': // Tourner à droite
			if(manualMode)
				direction -= PERSO_TURN_SPEED;
			break;
		case 'l': // Déplacement vers l'arrière
			if(manualMode)
			{
					persoAnim = Walking;
					moveDirection = PERSO_WALK_SPEED*10;
			}
			break;		
		case 'k': // Tourner à gauche
			if(manualMode)
				direction += PERSO_TURN_SPEED;
			break;
		case 'o': // Deplacement vers l'avant
			if(manualMode)
			{
					persoAnim = Running;
					moveDirection = PERSO_RUN_SPEED*10;
			}
			break;
	}
}

void checkPersoLimits(){
	if(-1.0*posX < cameraLimits[0])
		posX=-1*cameraLimits[0];
	else if(posX<0 && posX < -1*cameraLimits[1])
		posX=-1*cameraLimits[1];
		
	if(-1*posZ < cameraLimits[4])
		posZ=-1*cameraLimits[4];
	else if(posZ<0 && posZ < -1*cameraLimits[5])
		posZ=-1*cameraLimits[5];
		
	if(posZ>chaiseLimits[0]+PERSO_RUN_SPEED&&posZ<chaiseLimits[1]-PERSO_RUN_SPEED){
		if(posX>0&&posX<chaiseLimits[1])
			posX=chaiseLimits[1];
		else if(posX<0&&posX>chaiseLimits[0])
			posX=chaiseLimits[0];
	}
	if(posX>chaiseLimits[0]+PERSO_RUN_SPEED&&posX<chaiseLimits[1]-PERSO_RUN_SPEED){
		if(posZ>0&&posZ<chaiseLimits[1])
			posZ=chaiseLimits[1];
		else if(posZ<0&&posZ>chaiseLimits[0])
			posZ=chaiseLimits[0];
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
