/*************************************************
 * Contibutors ::-- Hassan Hosni Abdel_Aleem     *
 *                  Mohamed Khaled Galloul       *
 *                  Ahmed Mohamed Mohamed        *
 *                  Moamen Gamal Ashmawy         *
 *                  Youssef Mahmoud Elhely       *
**************************************************/

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#include "imageloader.h"

GLfloat body = 0, shoulder1 = 0,shoulder10 = 0, shoulder2 = 0, shoulder20 = 0, elbow1 = 0, elbow2 = 0, leg1 = 0,leg1z = 0, knee1 = 0, leg2 = 0,leg2z = 0, knee2 = 0;
GLfloat moving, startx, starty, theta = 0.0;

double eye[] = { 0 , 0, -1 };
double center[] = {0, 0, 4};
double up[] = { 0, 1, 0};
double perpendicularAxis[] = { 0, 0, 0 };
GLfloat Znear = 100.0;
int windowWidth;
int windowHeight;
int fingerBase1[5] = { 0 }, fingerUp1[5] = { 0 };
int fingerBase2[5] = { 0 }, fingerUp2[5] = { 0 };

GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
int poses[5][11]={{15,15,20,0,0,0,0,0,0,0,0},
                  {3,3,6,0,0,35,0,-45,0,0,0},
                  {0,0,0,0,0,35,5,-20,20,0,0},
                  {-15,-15,0,-20,0,15,0,-10,-15,0,-30},
                  {-3,3,0,0,-6,15,-10,0,-15,0,-10}

};
////////////// render
// RGBA
GLfloat light_ambient[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light_diffuse[] = {0.5, 0.5, 0.5, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
// x , y, z, w
GLfloat light_position[] = {0.5, 5.0, 0.0, 1.0};
GLfloat lightPos1[] = {-0.5, -5.0, -2.0, 1.0};

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image *image)
{
	GLuint textureId;
	glGenTextures(1, &textureId);			 //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,				  //Always GL_TEXTURE_2D
				 0,							  //0 for now
				 GL_RGB,					  //Format OpenGL uses for image
				 image->width, image->height, //Width and height
				 0,							  //The border of the image
				 GL_RGB,					  //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE,			  //GL_UNSIGNED_BYTE, because pixels are stored
											  //as unsigned numbers
				 image->pixels);			  //The actual pixel data
	return textureId;						  //Returns the id of the texture
}

GLuint _textureId; //The id of the texture

//Initializes 3D rendering

/////////////////

void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];

}

void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}


void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}
#define ARM1 1
#define ARM2 0

void displayfingers(int armNumber) {
    float y = -0.3;
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(armNumber? 1.0:-1.6 , (i ? 0.3 : -0.2), (i ? (y + i * 0.18) : (y + i * 0.15))); // one step forward and one step in y according to finger number
        glRotatef((GLfloat) fingerBase1[i], 0.0, 0.0, i ? -1.0 : 1.0); // rotate figerbase around -z (variable)
        glTranslatef(0.15, 0.0, 0.0);                   // step 0.15 forward
        glPushMatrix();
        glScalef(0.3, 0.1, 0.1);
        glutWireCube(1);                            // DRAW THE FINGERBASE
        glPopMatrix();

        //Draw finger flang 1 
        glTranslatef(0.15, 0.0, 0.0);                   // step 0.15 forward
        glRotatef((GLfloat) -fingerUp1[i], 0.0, 0.0, i ? -1.0 : -1.0);    // rotate upperfinger around -z (variable)
        glTranslatef(0.15, 0.0, 0.0);                   // step 0.15 forward
        glPushMatrix();
        glScalef(0.3, 0.1, 0.1);
        glutWireCube(1);                            // DRAW THE UPPERFINGER
        glPopMatrix();
        glPopMatrix();
    }
}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    //glScalef(0.1, 0.1, 0.1);
    ///llllllll

    //glClearColor(1.0, 1.0, 1.0, 1.0);

	// Clear Depth and Color buffers

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
    glScalef(5.1, 5.1, 5.1);

	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPopMatrix();

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBegin(GL_QUADS);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5, -0.5,  1);
	glTexCoord2f(3.0f, 0.0f); glVertex3f( 0.5, -0.5,  1);
	glTexCoord2f(3.0f, 3.0f); glVertex3f( .5, -.5, -1);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-.5, -.5, -1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

    //lllll
    glPushMatrix();             /* DRAWING STACK */
    //gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
    glScalef(0.2, 0.2, 0.2);
    glTranslated(0.0,0.0,5.0);
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 2.0, 1.0);
    glPushMatrix();             /* BODY STACK */
    glScalef(2.0, 3.0, 1.0);    
    glutWireCube(1.0);          /* drawing the actual body center */
    glPopMatrix();              /* BODY STACK */

    /*******************************************************
     *                        HEAD                         *
    ********************************************************/
    glPushMatrix();             /* HEAD STACK */
    glTranslatef(0.0, 2.5, 0.0);
    glutSolidSphere(1.0, 50, 50); /* drawing the actual head */
    glPopMatrix();              /* HEAD STACK */

    /*******************************************************
     *                        SHOULDER 1                   *
    ********************************************************/
    glPushMatrix();             /* ARM 1 STACK */
    glTranslatef(1.5, 1.2, 0.0);
    glRotatef((GLfloat)shoulder1, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef((GLfloat)shoulder10, 0.0, 1.0, 0.0);
    glTranslatef(1.0, 0.0, 0.0);
    glPushMatrix();             /* SHOULDER 1 STACK */
    glScalef(2.0, 0.6, 1.0);
    glutWireCube(1.0);          /* drawing the actual shoulder */
    glPopMatrix();              /* SHOULDER 1 STACK */

    /*******************************************************
     *                        ELBOW 1                      *
    ********************************************************/
    glPushMatrix();             /* ELBOW 1 STACK */
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)elbow1, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    glPushMatrix();             /* ELBOW DRAW STACK */
    glScalef(2.0, 0.6, 1.0);
    glutWireCube(1.0);          /* drawing the actual elbow */
    glPopMatrix();              /* ELBOW DRAW STACK */

    /********************************************************
     *                        FINGERS 1                     *
     ********************************************************/
    displayfingers(ARM1);
    glPopMatrix();              /* ELBOW 1 STACK */
    glPopMatrix();              /* ARM 1 STACK */

    /*******************************************************
     *                        SHOULDER 2                   *
    ********************************************************/
    glPushMatrix();             /* ARM 2 STACK */
    glTranslatef(-1.5, 1.2, 0.0);
    glRotatef((GLfloat)shoulder2, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef((GLfloat)shoulder20, 0.0, 1.0, 0.0);
    glTranslatef(-1.0, 0.0, 0.0);
    glPushMatrix();             /* SHOULDER 2 STACK */
    glScalef(2.0, 0.6, 1.0);
    glutWireCube(1.0);          /* drawing the actual shoulder */
    glPopMatrix();              /* SHOULDER 2 STACK */

    /*******************************************************
     *                        ELBOW 2                      *
    ********************************************************/
    glPushMatrix();             /* ELBOW 2 STACK */
    glTranslatef(-1.0, 0.0, 0.0);
    glRotatef((GLfloat)elbow2, 0.0, 0.0, 1.0);
    glTranslatef(-1.0, 0.0, 0.0);
    glPushMatrix();             /* ELBOW DRAW STACK */
    glScalef(2.0, 0.6, 1.0);
    glutWireCube(1.0);          /* drawing the actual elbow */
    glPopMatrix();              /* ELBOW DRAW STACK */

    /********************************************************
     *                        FINGERS 2                     *
     ********************************************************/
    displayfingers(ARM2);
    glPopMatrix();              /* ELBOW 2 STACK */
    glPopMatrix();              /* ARM 2 STACK */

    /*******************************************************
     *                        LEG 1                        *
    ********************************************************/
    glPushMatrix();             /* LEG 1 STACK */
    glTranslatef(1.0, -1.35, 0.0);
    glRotatef((GLfloat)leg1, 1.0, 0.0, 0.0);
    glRotatef((GLfloat)leg1z, 0.0, 0.0, 1.0);
    glTranslatef(-0.35, -1.2, 0.0);
    glPushMatrix();             /* THIGH 1 STACK */
    glScalef(0.7, 2.0, 1.0);
    glutWireCube(1.0);          /* drawing the actual leg */
    glPopMatrix();              /* THIGH 1 STACK */

    /*******************************************************
     *                        KNEE 1                       *
    ********************************************************/
    glPushMatrix();             /* LOWER LEG 1 STACK */
    glTranslatef(1.0, -1.35, 0.0);
    glRotatef((GLfloat)knee1, 1.0, 0.0, 0.0);
    glTranslatef(-1, -0.7, 0.0);
    glPushMatrix();             /* KNEE 1 STACK */
    glScalef(0.7, 2.0, 1.0);
    glutWireCube(1.0);          /* drawing the actual knee */
    glPopMatrix();              /* KNEE 1 STACK */
    glTranslatef(0.0, -1.3, 0.0);
    glScalef(0.8, 0.7, 2.0);
    glutSolidCube(1.0);         /* drawing the actual ankle */
    glPopMatrix();              /* LOWER LEG 1 STACK */
    glPopMatrix();              /* LEG 1 STACK */

    /*******************************************************
     *                        LEG 2                        *
    ********************************************************/
    glPushMatrix();             /* LEG 2 STACK */
    glTranslatef(1.0, -1.35, 0.0);
    glRotatef((GLfloat)leg2, 1.0, 0.0, 0.0);
    glTranslatef(-1.2, -0.0, 0.0);
    glRotatef((GLfloat)leg2z, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    glTranslatef(-1.53, -1.2, 0.0);
    glPushMatrix();             /* THIGH 2 STACK */
    glScalef(0.7, 2.0, 1.0);
    glutWireCube(1.0);          /* drawing the actual leg */
    glPopMatrix();              /* THIGH 2 STACK */

    /*******************************************************
     *                        KNEE 2                       *
    ********************************************************/
    glPushMatrix();             /* LOWER LEG 2 STACK */
    glTranslatef(1.0, -1.35, 0.0);
    glRotatef((GLfloat)knee2, 1.0, 0.0, 0.0);
    glTranslatef(-1, -0.7, 0.0);
    glPushMatrix();             /* KNEE 2 STACK */
    glScalef(0.7, 2.0, 1.0);
    glutWireCube(1.0);          /* drawing the actual knee */
    glPopMatrix();              /* KNEE 2 STACK */
    glTranslatef(0.0, -1.3, 0.0);
    glScalef(0.8, 0.7, 2.0);
    glutSolidCube(1.0);          /* drawing the actual ankle */
    glPopMatrix();              /* LOWER LEG 2 STACK */
    glPopMatrix();              /* LEG 2 STACK */

    glPopMatrix();              /* DRAWING STACK */
    ///
    
	
    
	
    ///
    glutSwapBuffers(); /* Swap buffers after creating the draw */
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    printf("\n%f", Znear);
    gluPerspective(Znear, (GLfloat)w / (GLfloat)h, 0.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}
// control the movement of fingers and shoulder and elbow joints
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        crossProduct(up, center, perpendicularAxis);
        normalize(perpendicularAxis);
        rotatePoint(perpendicularAxis, 0.05, center);
        rotatePoint(perpendicularAxis, 0.05, up);
        glutPostRedisplay();
        break;
    case 's':
        crossProduct(up, center, perpendicularAxis);
        normalize(perpendicularAxis);
        rotatePoint(perpendicularAxis, -0.05, center);
        rotatePoint(perpendicularAxis, -0.05, up);
        glutPostRedisplay();
        break;
    case 'a':
        rotatePoint(up, -0.05, center);
        glutPostRedisplay();
        break;
    case 'd':
        rotatePoint(up, 0.05, center);
        glutPostRedisplay();
        break;
    case 'q':
        if (Znear > 120) { Znear = 120; }
        else if (Znear <= 60) { Znear = 60; }
        else 
            Znear -= 1.0;

        reshape(windowWidth, windowHeight);
        glutPostRedisplay();
        break;
    case 'Q':
        if (Znear >= 120) { Znear = 120; }
        else if (Znear < 60) { Znear = 60; }
        else
            Znear += 1.0;
        reshape(windowWidth, windowHeight);
        glutPostRedisplay();
        break;
    case '7':
        if (shoulder10 > 150){ shoulder10 = 150; }
        else
        {
            shoulder10 = (shoulder10 + 5);
            glutPostRedisplay();
        }
        break;
    case '8':
        if (shoulder10 <= -90)
        {
            shoulder10 = -90;
        }
        else
        {
            shoulder10 = (shoulder10 - 5);
            glutPostRedisplay();
        }
        break;
        ////////
    case 'p':
        if (shoulder1 > 45){ shoulder1 = 45; }
        else
        {
            shoulder1 = (shoulder1 + 5);
            glutPostRedisplay();
        }
        break;
    case 'P':
        if (shoulder1 <= -90)
        {
            shoulder1 = -90;
        }
        else
        {
            shoulder1 = (shoulder1 - 5);
            glutPostRedisplay();
        }
        break;
        ///////////
    case 'o':
        if (shoulder2 <= -45)
        {
            shoulder2 = -45;
        }
        else
        {
            shoulder2 = (shoulder2 - 5);
            glutPostRedisplay();
        }
        break;
    case 'O':
        if (shoulder2 > 90)
        {
            shoulder2 = 90;
        }
        else
        {
            shoulder2 = (shoulder2 + 5);
            glutPostRedisplay();
        }
        break;
        ////
    case '9':
        if (shoulder20 <= -150)
        {
            shoulder20 = -150;
        }
        else
        {
            shoulder20 = (shoulder20 - 5);
            glutPostRedisplay();
        }
        break;
    case '6':
        if (shoulder20 > 90)
        {
            shoulder20 = 90;
        }
        else
        {
            shoulder20 = (shoulder20 + 5);
            glutPostRedisplay();
        }
        break;
        ////

    case 'i':
        if (elbow1 >= 150)
        {
            elbow1 = 150;
        }
        else
        {
            elbow1 = (elbow1 + 5);
            glutPostRedisplay();
        }
        break;
    case 'I':
        if (elbow1 <= 0)
        {
            elbow1 = 0;
        }
        else
        {
            elbow1 = (elbow1 - 5);
            glutPostRedisplay();
        }
        break;
    case 'u':
        if (elbow2 >= 150)
        {
            elbow2 = 150;
        }
        else
        {
            elbow2 = (elbow2 + 5);
            glutPostRedisplay();
        }
        break;
    case 'U':
        if (elbow2 <= 0)
        {
            elbow2 = 0;
        }
        else
        {
            elbow2 = (elbow2 - 5);
            glutPostRedisplay();
        }
        break;
    case 'y':
        if (leg1 >= 90)
        {
            leg1 = 90;
        }
        else
        {
            leg1 = (leg1 + 5);
            glutPostRedisplay();
        }
        break;
    case 'Y':
        if (leg1 <= -90)
        {
            leg1 = -90;
        }
        else
        {
            leg1 = (leg1 - 5);
            glutPostRedisplay();
        }
        break;
        //
        case 'z':
        if (leg1z >= 90)
        {
            leg1z = 90;
        }
        else
        {
            leg1z = (leg1z + 5);
            glutPostRedisplay();
        }
        break;
    case 'Z':
        if (leg1z <= -90)
        {
            leg1z = -90;
        }
        else
        {
            leg1z = (leg1z - 5);
            glutPostRedisplay();
        }
        break;
        //
    
    case 't':
        if (knee1 <= -90)
        {
            knee1 = -90;
        }
        else
        {
            knee1 = (knee1 - 5);
            glutPostRedisplay();
        }
        break;
    case 'T':
        if (knee1 >= 0)
        {
            knee1 = 0;
        }
        else
        {
            knee1 = (knee1 + 5);
            glutPostRedisplay();
        }
        break;
    case 'r':
        if (leg2 >= 90)
        {
            leg2 = 90;
        }
        else
        {
            leg2 = (leg2 + 5);
            glutPostRedisplay();
        }
        break;
    case 'R':
        if (leg2 <= -90)
        {
            leg2 = -90;
        }
        else
        {
            leg2 = (leg2 - 5);
            glutPostRedisplay();
        }
        break;
    case '2':
        if (leg2z >= 90)
        {
            leg2z = 90;
        }
        else
        {
            leg2z = (leg2z + 5);
            glutPostRedisplay();
        }
        break;
    case '3':
        if (leg2z <= -90)
        {
            leg2z = -90;
        }
        else
        {
            leg2z = (leg2z - 5);
            glutPostRedisplay();
        }
        break;
        //
    case 'e':
        if (knee2 <= -90)
        {
            knee2 = -90;
        }
        else
        {
            knee2 = (knee2 - 5);
            glutPostRedisplay();
        }
        break;
    case 'E':
        if (knee2 >= 0)
        {
            knee2 = 0;
        }
        else
        {
            knee2 = (knee2 + 5);
            glutPostRedisplay();
        }
        break;
    case 'l':
        if (fingerBase1[0] < 90)
            fingerBase1[0] = (fingerBase1[0] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'L':
        if (fingerBase1[0] > 0)
            fingerBase1[0] = (fingerBase1[0] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'k':
        if (fingerUp1[0] < 90)
            fingerUp1[0] = (fingerUp1[0] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'K':
        if (fingerUp1[0] > 0)
            fingerUp1[0] = (fingerUp1[0] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'j':
        if (fingerBase1[1] < 90)
            fingerBase1[1] = (fingerBase1[1] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'J':
        if (fingerBase1[1] > 0)
            fingerBase1[1] = (fingerBase1[1] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'h':
        if (fingerUp1[1] < 90)
            fingerUp1[1] = (fingerUp1[1] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'H':
        if (fingerUp1[1] > 0)
            fingerUp1[1] = (fingerUp1[1] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'g':
        if (fingerBase1[2] < 90)
            fingerBase1[2] = (fingerBase1[2] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'G':
        if (fingerBase1[2] > 0)
            fingerBase1[2] = (fingerBase1[2] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'f':
        if (fingerUp1[2] < 90)
            fingerUp1[2] = (fingerUp1[2] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'F':
        if (fingerUp1[2] > 0)
            fingerUp1[2] = (fingerUp1[2] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'm':
        if (fingerBase1[3] < 90)
            fingerBase1[3] = (fingerBase1[3] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'M':
        if (fingerBase1[3] > 0)
            fingerBase1[3] = (fingerBase1[3] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'n':
        if (fingerUp1[3] < 90)
            fingerUp1[3] = (fingerUp1[3] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'N':
        if (fingerUp1[3] > 0)
            fingerUp1[3] = (fingerUp1[3] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'b':
        if (fingerBase1[4] < 90)
            fingerBase1[4] = (fingerBase1[4] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'B':
        if (fingerBase1[4] > 0)
            fingerBase1[4] = (fingerBase1[4] - 5) % 360;
        glutPostRedisplay();
        break;
    case 'v':
        if (fingerUp1[4] < 90)
            fingerUp1[4] = (fingerUp1[4] + 5) % 360;
        glutPostRedisplay();
        break;
    case 'V':
        if (fingerUp1[4] > 0)
            fingerUp1[4] = (fingerUp1[4] - 5) % 360;
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}


void motion(int x, int y)
{
    if (moving) {
        /*
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);*/
        int dx = x - startx;
        int dy = y - starty;
        rotatePoint(up, -(dx % 360) * 0.01 , center);
        crossProduct(up, center, perpendicularAxis);
        normalize(perpendicularAxis);
        rotatePoint(perpendicularAxis, -dy * 0.01, center);
        rotatePoint(perpendicularAxis, -dy * 0.01, up);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}
void setPoses( int frameNum)
{
    shoulder1 = poses[frameNum][0];
    shoulder2= poses[frameNum][1];
    elbow1 = poses[frameNum][2];
    elbow2 = poses [frameNum][3];
    //fingerBase1 =poses [frameNum][4];
    // fingerBase2=poses [frameNum][4];
    // fingerUp1=poses [frameNum][4];
    // fingerUp2 = poses [frameNum][4];
    leg1 = poses [frameNum][5];
    //leg1z = poses [frameNum][6];
    knee1 = poses [frameNum][7];
    leg2 = poses [frameNum][8];
    //leg2z = poses [frameNum][9];
    knee2 = poses [frameNum][10] ;

}
static int f =0 ;
void  timer(int value ){
f= f%5;
setPoses(f);
f++;
glutPostRedisplay();
glutTimerFunc(150, timer ,0);

}
void reset(){
    double e[]={0.0,0.0,1.0};
    double c[]={0.0,0.0,0.0};
    double u []= {0.0,1.0,0.0};
    for (int i =0 ; i< 3;i++)
    {
        eye [i]=e[i] ;
        center [i]=c[i] ;
        up [i] = u[i] ;
    }

}
GLfloat m=0.0; 
void screan_menu( int m)
{ 
    switch (m)
    {
    case '1':
        m=0.0 ;
    case '2':
        m=1.0 ;
    case '3':
        m=9.0 ;
        break;    

    default:
        break;
    };
    //reset();
    glutPostRedisplay();

}
void attachMenu(){
    glutCreateMenu (screan_menu);
    glutAddMenuEntry ("model",0);
    glutAddMenuEntry ("",0);
    glutAddMenuEntry ("wood",3);
    glutAddMenuEntry ("buz",2);
    glutAddMenuEntry ("floor",1);
    glutAttachMenu (GLUT_RIGHT_BUTTON);

}

void initRendering()
{   

    if (m ==1){

        Image *image = loadBMP("images/floor.bmp");
        _textureId = loadTexture(image);
        delete image;}
        else if (m==9)
        {
            Image *image = loadBMP("images/buz.bmp");
            _textureId = loadTexture(image);
            delete image;
        }
          
        else {
            Image *image = loadBMP("images/wood.bmp");
            _textureId = loadTexture(image);
            delete image;
        }
	
	
	// Turn on the power
	glEnable(GL_LIGHTING);
	// Flip light switch
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	// assign light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	// Material Properties
	GLfloat lightColor1[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
	glEnable(GL_NORMALIZE);
	//Enable smooth shading
	glShadeModel(GL_SMOOTH);
	// Enable Depth buffer
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(5000, 5000);
    glutInitWindowPosition(10, 10);
    glutCreateWindow(argv[0]);
    init();
    initRendering();
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60, 1.0, 0.1, 10);
    attachMenu();
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,timer,0);
    glutMainLoop();
    return 0;
}
