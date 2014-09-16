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
void saveScreenshot (char *filename)
{
    int i, j;
    Pic *in = NULL;
    
    if (filename == NULL)
        return;
    
    /* Allocate a picture buffer */
    in = pic_alloc(640, 480, 3, NULL);
    
    printf("File to save to: %s\n", filename);
    
    for (i=479; i>=0; i--) {
        glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                     &in->pix[i*in->nx*in->bpp]);
    }
    
    if (jpeg_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");
    
    pic_free(in);
}

void myinit()
{
    /* setup gl view here */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //double aspectRatio = (double)glutGet( GLUT_WINDOW_WIDTH )/glutGet( GLUT_WINDOW_HEIGHT );
    //glOrtho(-aspectRatio, aspectRatio, -1.0, 1.0, -1.0, 1.0); // prevents square in starter code from stretching out
    //glFrustum(0.0, 1.0, 0.0, 1.0, 0.01, 1000.0);
    gluPerspective(60.0, 640.0/480.0, 0.01, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-200.0, -200.0, 500.0, 0.0, 0.0, 100.0, 0.0, 0.0, 1.0); // adjust camera position so that object appears in center of window
    //glPointSize(2.0);
    //glEnable(GL_POINT_SMOOTH);
    glEnable(GL_DEPTH_TEST);            // enable depth buffering
}

void display()
{
    /* draw 1x1 cube about origin */
    /* replace this code with your height field implementation */
    /* you may also want to precede it with your
     rotation/translation/scaling */
    /*if (g_ControlState == ROTATE) {
        glRotatef(1.0, g_vLandRotate[0], g_vLandRotate[1], g_vLandRotate[2]);
    }*/
    //glTranslated(0.0, 0.0, -1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (g_RenderState == POINTS)
    {
        glBegin(GL_POINTS);
    }
    else if (g_RenderState == LINES)
    {
        glBegin(GL_LINES);
    }
    else if (g_RenderState == TRIANGLES)
    {
        glBegin(GL_TRIANGLE_STRIP);
    }
    for (int i = 0; i < g_pHeightData->nx; ++i)
    {
        for (int j = 0; j < g_pHeightData->ny; ++j)
        {
            int ch = PIC_PIXEL(g_pHeightData, i, j, 0) - 256; // z-axis value must be negative to fit in viewing volume
            glColor3f(0.5, (float)i/(float)g_pHeightData->nx, (float)j/(float)g_pHeightData->ny);
            glVertex3i(i, j, ch); // for each pixel in the image, create a vertex with a z-axis value specified by its grayscale level
        }
    }
    glEnd();
    /*glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.5, -0.5, 0.0);*/
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
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandTranslate[2] += vMouseDelta[1]*0.01;
            }
            glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
            break;
        case ROTATE:
            if (g_iLeftMouseButton)
            {
                g_vLandRotate[0] += vMouseDelta[1];
                g_vLandRotate[1] += vMouseDelta[0];
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandRotate[2] += vMouseDelta[1];
            }
            glRotatef(1.0, g_vLandRotate[0], g_vLandRotate[1], g_vLandRotate[2]);
            break;
        case SCALE:
            if (g_iLeftMouseButton)
            {
                g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
                g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
            }
            glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);
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
            glTranslatef(0.0, 0.0, -100.0);
            break;
        case 27:
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
