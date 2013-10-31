#include <glut.h>
#include <math.h>

GLfloat theta = 0.0, trans=0.0, Xmin = -3.0f, Xmax = 3.0f, Ymin = -3.0f, Ymax = 3.0f;
GLboolean bRotate = false;
void triangle()
{
		glBegin(GL_TRIANGLES);//Denotes the beginning of a group of vertices that define one or more primitives.
		glColor3f(0.0,0.5,0.0);
		glVertex2f(-2.0,-2.0);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(2.0,-2.0);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(2.0,2.0);
	glEnd();	//Terminates a list of vertices that specify a primitive initiated by glBegin.

}
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//the glClear() command now consists of the bitwise 
			//OR of all the buffers to be cleared.
	glMatrixMode( GL_MODELVIEW );
		//This informs OpenGL that we are going to change 
		//the modelview matrix 
	 glMatrixMode( GL_PROJECTION );
		//This informs OpenGL that we are going to change the projection matrix 
	glLoadIdentity();
		//replaces the current transformation matrix with 
		//the identity matrix. This essentially resets 
		//the coordinate system to eye coordinates.
	
	
	//glColor3f(0.0,0.0,1.0);
	glPushMatrix();
		//This function pushes the current matrix onto the current matrix stack. 
		//This function is most often used to save the current transformation matrix 
		//so that it can be restored later with a call to glPopMatrix();
	
		glViewport( 0, 0,600 ,400 );
		//This function sets the region within a window that is used 
		//for mapping the clipping volume coordinates to physical 
		//window coordinates.

		//glScalef(-1,1,1);//Multiplies the current matrix by a scaling matrix.
		
		glTranslatef(trans,0,0);//Multiplies the current matrix by a translation matrix.

		glRotatef(theta, 0, 0, 1);//Rotates the current matrix by a rotation matrix.
		
		triangle();
	glPopMatrix();//Pops the current matrix off the matrix stack.

	
	/*glPushMatrix();
		glViewport( 0,0, 300, 200 );
		glTranslatef(trans,0,0);
		glRotatef(theta, 0.0, 0.0, 1.0);
		triangle();
	glPopMatrix();*/
	glFlush();
		//glFlush() ensures that the drawing commands 
		//are actually executed rather than stored in a buffer 
		//waiting additional OpenGL commands.
	glutSwapBuffers();
	      //Performs a buffer swap on the layer in use for the current
	      //window. Specifically, glutSwapBuffers promotes the contents 
	      //of the BACK BUFFER of the layer in use of the current window
	      //to become the contents of the FRONT BUFFER. The contents of 
	      //the back buffer then become undefined. The update typically
	      //takes place during the vertical retrace of the monitor, rather 
	      //than immediately after glutSwapBuffers is called.
}
void initRendering()
{
	//glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );
		//Sets the default shading to flat or smooth.
		//OpenGL primitives are always shaded, but the shading model
		//can be flat (GL_FLAT) or smooth (GL_SMOOTH). 
    glEnable( GL_DEPTH_TEST );
		//glEnable enables an OpenGL drawing feature.
}
void resizeWindow(int w, int h)
{
	double scale, center;
	double windowXmin, windowXmax, windowYmin, windowYmax;


	//glViewport( 0, 0, w, h );
		// Define the portion of the window used for OpenGL rendering.
		// View port uses whole window

		// Set up the projection view matrix: orthographic projection
		// Determine the min and max values for x and y that should appear in the window.
		// The complication is that the aspect ratio of the window may not match the
		//		aspect ratio of the scene we want to view.
	w = (w==0) ? 1 : w;
	h = (h==0) ? 1 : h;
	if ( (Xmax-Xmin)/w < (Ymax-Ymin)/h ) {
		scale = ((Ymax-Ymin)/h)/((Xmax-Xmin)/w);
		center = (Xmax+Xmin)/2;
		windowXmin = center - (center-Xmin)*scale;
		windowXmax = center + (Xmax-center)*scale;
		windowYmin = Ymin;
		windowYmax = Ymax;
	}
	else {
		scale = ((Xmax-Xmin)/w)/((Ymax-Ymin)/h);
		center = (Ymax+Ymin)/2;
		windowYmin = center - (center-Ymin)*scale;
		windowYmax = center + (Ymax-center)*scale;
		windowXmin = Xmin;
		windowXmax = Xmax;
	}
	
	// Now that we know the max & min values for x & y 
	//		that should be visible in the window,
	//		we set up the orthographic projection.
   
    
    gluOrtho2D(windowXmin, windowXmax, windowYmin, windowYmax);
		//This function defines a 2D orthographic projection matrix.
}
void myKeyboardFunc( unsigned char key, int x, int y )
{
	switch ( key ) 
	{
		case 'r':
			bRotate = !bRotate;	
			break;
		case 'l':
			trans +=0.2;
			break;
		case 'j':
			trans -=0.2;
			break;
		case 27:	// Escape key
			exit(1);
	}
}

void animate()
{
	if (bRotate == true)
	{
		theta += 0.2;
		if(theta > 360.0)
			theta -= 360.0*floor(theta/360.0);
	}	
	glutPostRedisplay();
		//This function informs the GLUT library that the 
		//current window needs to be refreshed. 
		//Multiple calls to this function before the next refresh 
		//result in only one repainting of the window.

	
}
int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	//This initializes the GLUT library, passing command line parameters 
	//(this has an effect mostly on Linux/Unix) 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//Initial parameters for the display. In this case, we are specifying 
	//a RGB display (GLUT_RGB) along with double-buffering (GLUT_DOUBLE), 
	//so the screen won't flicker when we redraw it. 
	//GLUT_DEPTH Specifies a 32-bit depth buffer
 
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,400);
	//Initial window size - width, height 
	glutCreateWindow("Traingle-Demo");
	//Creates and sets the window title 
	initRendering();

	glutKeyboardFunc(myKeyboardFunc);
		//Sets the keyboard callback function for the current window.

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(drawScene);
		//This function tells GLUT which function to call 
		//whenever the windows contents must be drawn. 
		//This can occur when the window is resized or uncovered or 
		//when GLUT is specifically asked to refresh with a call to 
		//the glutPostRedisplay function.
	glutIdleFunc(animate);
	//This is particularly useful for continuous
    //animation or other background processing.
	
	glutMainLoop();
		//This function begins the main GLUT event-handling loop. 
		//The event loop is the place where all keyboard, mouse, 
		//timer, redraw, and other window messages are handled. 
		//This function does not return until program termination.
	
	return (0);
}