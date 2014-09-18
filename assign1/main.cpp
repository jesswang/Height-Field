/*
 CSCI 480 Computer Graphics
 Assignment 1: Height Fields
 C++ starter code
 */

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <iostream>
#include <sstream>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
typedef enum { POINTS, LINES, TRIANGLES } RENDERSTATE;

CONTROLSTATE g_ControlState = ROTATE;
RENDERSTATE g_RenderState = POINTS;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename){
    int i, j;
    
    Pic *in = NULL;
    Pic *out = NULL;
    
    if (filename == NULL)
        return;
    
    in = pic_alloc(640, 480, 3, NULL);
    out = pic_alloc(640, 480, 3, NULL);
    printf("File to save to: %s\n", filename);
    
    glReadPixels(0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, &in->pix[0]);
    
    for ( int j=0; j<480; j++ ) {
        for ( int i=0; i<640; i++ ) {
            PIC_PIXEL(out, i, j, 0) = PIC_PIXEL(in, i, 480-1-j, 0);
            PIC_PIXEL(out, i, j, 1) = PIC_PIXEL(in, i, 480-1-j, 1);
            PIC_PIXEL(out, i, j, 2) = PIC_PIXEL(in, i, 480-1-j, 2);
        } 
    }
    
    if (jpeg_write(filename, out))       
        printf("File saved Successfully\n");   
    
    else       
        printf("Error in Saving\n");
    
    pic_free(in);    
    pic_free(out);
    
}

void myinit()
{
    /* setup gl view here */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 640.0/480.0, 0.01, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-200.0, -200.0, 0.0, 0.0, 0.0, -100.0, 0.0, 0.0, 1.0); // adjust camera position so that object appears in center of window with the z-axis facing up
    //glPointSize(2.0);
    //glEnable(GL_POINT_SMOOTH);
    glEnable(GL_DEPTH_TEST);            // enable depth buffering
}

void drawPoints()
{
    glBegin(GL_POINTS);
    for (int i = 0; i < g_pHeightData->ny; ++i)
    {
        for (int j = 0; j < g_pHeightData->nx; ++j)
        {
            int z = PIC_PIXEL(g_pHeightData, j, i, 0) - 256; // z-axis value must be negative to fit in viewing volume
            glColor3f(1.0, (float)z/255.0*-1, 0.5);
            glVertex3i(j, i, z); // for each pixel in the image, create a vertex with a z-axis value specified by its grayscale level
        }
    }
    glEnd();
}

void drawTriangles()
{
    for (int i = 0; i < g_pHeightData->ny-1; ++i)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < g_pHeightData->nx; ++j)
        {
            int z1 = PIC_PIXEL(g_pHeightData, j, i, 0) - 256; // z-axis value must be negative to fit in viewing volume
            int z2 = PIC_PIXEL(g_pHeightData, j, i+1, 0) - 256;
            glColor3f(1.0, (float)z1/255.0*-1, 0.5);
            glVertex3i(j, i, z1); // for each pixel in the image, create a vertex with a z-axis value specified by its grayscale level
            glColor3f(1.0, (float)z2/255.0*-1, 0.5);
            glVertex3i(j, i+1, z2); // create vertex at position directly below create tri-strip
        }
        glEnd();
    }
}

static void takeScreenshots(int num)
{
    std::stringstream s;
    if (num < 100)
    {
        if (num < 10)
        {
            s << "screenshots/00" << num << ".jpg";
        }
        else
        {
            s << "screenshots/0" << num << ".jpg";
        }
    }
    else
    {
        s << "screenshots/" << num << ".jpg";
    }
    char* file = new char[19];
    strcpy(file, s.str().c_str());
    saveScreenshot(file);
    delete [] file;
    
    ++num;
    glutTimerFunc(500, takeScreenshots, num); // callback is triggered every 500 ms
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    if (g_RenderState == POINTS)
    {
        drawPoints();
    }
    else if (g_RenderState == LINES)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wire frame view
        drawTriangles();
    }
    else if (g_RenderState == TRIANGLES)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // solid object view
        drawTriangles();
    }
    /*glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.5, -0.5, 0.0);*/
    glPopMatrix();
    glRotatef(0.1, 0.0, 0.0, 1.0); // continuously rotate about the z-axis
    glutSwapBuffers();
}

void menufunc(int value)
{
    switch (value)
    {
        case 0:
            exit(0);
            break;
    }
}

void doIdle()
{
    /* do some stuff... */
    
    /* make the screen update */
    glutPostRedisplay();
}

/* converts mouse drags into information about
 rotation/translation/scaling */
void mousedrag(int x, int y)
{
    int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
    
    switch (g_ControlState)
    {
        case TRANSLATE:
            if (g_iLeftMouseButton)
            {
                g_vLandTranslate[0] += vMouseDelta[0]*0.01;
                g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
                glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], 0.0); // translate w/ respect to x- and y- axes
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandTranslate[2] += vMouseDelta[1]*0.01;
                glTranslatef(0.0, 0.0, g_vLandTranslate[2]); // translate w/ respect to z-axis
            }
            break;
        case ROTATE:
            if (g_iLeftMouseButton)
            {
                g_vLandRotate[0] += vMouseDelta[1]*0.01;
                g_vLandRotate[1] += vMouseDelta[0]*0.01;
                glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0); // rotate about x-axis
                glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0); // rotate about y-axis
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandRotate[2] += vMouseDelta[1]*0.01;
                glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0); // rotate about z-axis
            }
            break;
        case SCALE:
            if (g_iLeftMouseButton)
            {
                g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
                g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
                glScalef(g_vLandScale[0], g_vLandScale[1], 1.0); // scale w/ respect to x- and y- axes
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
                glScalef(1.0, 1.0, g_vLandScale[2]); // scale w/ respect to z-axis
            }
            break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{
    
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            g_iLeftMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_MIDDLE_BUTTON:
            g_iMiddleMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            g_iRightMouseButton = (state==GLUT_DOWN);
            break;
    }
    
    switch(glutGetModifiers())
    {
        case GLUT_ACTIVE_CTRL:
            g_ControlState = TRANSLATE;
            break;
        case GLUT_ACTIVE_SHIFT:
            g_ControlState = SCALE;
            break;
        default:
            g_ControlState = ROTATE;
            break;
    }
    
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void keyboard(unsigned char c, int x, int y)
{
    switch (c) { // switch type of render view based on keyboard input
        case 'p':
            g_RenderState = POINTS;
            break;
        case 'l':
            g_RenderState = LINES;
            break;
        case 't':
            g_RenderState = TRIANGLES;
            break;
        case 's':
            takeScreenshots(0);
            break;
        case 27: // ESC key
            exit(0);
            break;
    }
}

int main (int argc, char ** argv)
{
    if (argc<2)
    {
        printf ("usage: %s heightfield.jpg\n", argv[0]);
        exit(1);
    }
    
    g_pHeightData = jpeg_read(argv[1], NULL);
    if (!g_pHeightData)
    {
        printf ("error reading %s.\n", argv[1]);
        exit(1);
    }
    
    glutInit(&argc,argv);
    
    /*
     create a window here..should be double buffered and use depth testing
     
     the code past here will segfault if you don't have a window set up....
     replace the exit once you add those calls.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Height Field");
    //exit(0);
    
    /* tells glut to use a particular display function to redraw */
    glutDisplayFunc(display);
    
    /* allow the user to quit using the right mouse button menu */
    g_iMenuId = glutCreateMenu(menufunc);
    glutSetMenu(g_iMenuId);
    glutAddMenuEntry("Quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    /* replace with any animate code */
    glutIdleFunc(doIdle);
    
    /* callback for mouse drags */
    glutMotionFunc(mousedrag);
    /* callback for idle mouse movement */
    glutPassiveMotionFunc(mouseidle);
    /* callback for mouse button changes */
    glutMouseFunc(mousebutton);
    /* callback for keyboard press */
    glutKeyboardFunc(keyboard);
    
    /* do initialization */
    myinit();
    
    glutMainLoop();
    return(0);
}
