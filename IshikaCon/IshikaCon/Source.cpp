#include <iostream>
#include <gl/glut.h>

using namespace std;

#define X 0
#define Y 1
#define DIM 2
#define STROKE_PTS 15000
#define STROKE_CNT 5000
#define STAMPS 15000
#define SPLATS 15000

#define StampDelta 0.01
#define SplatDelta 0.05

//window def
#define WIDTH 1200
#define HEIGHT 800
#define RATIO 100

GLfloat theta = 0.0, trans=0.0, 
	Xmin = -1.0f*(WIDTH /RATIO/2), 
	Xmax =  1.0f*(WIDTH /RATIO/2), 
	Ymin = -1.0f*(HEIGHT /RATIO/2), 
	Ymax =  1.0f*(HEIGHT /RATIO/2);
GLint const xmid = WIDTH /2;
GLint const ymid = HEIGHT/2;
GLboolean bRotate = false;


GLfloat strokePt[STROKE_PTS][DIM];
GLint strokeCol[STROKE_PTS];
GLint currentCol = 0x0077cc;
GLint strokeEndIdx[STROKE_CNT];
int strokeEITop = 1;
int strokePtIdx = 0;
int lastStampedPt = -1;

GLfloat stampPt[STAMPS][DIM];
GLint stampColr[STAMPS];
int stampTopIdx = 0;


struct SplatParamSt{
	GLfloat  b; //motion bias
	GLushort a;//age
	GLubyte  r;//roughness [1~255px]
	GLubyte  f;//flow percentage [0-100]
}SplatParam[SPLATS];
#define N 8
GLfloat SplatVector[SPLATS][N][DIM];
GLint SplactColor[SPLATS];
GLushort WetMap[WIDTH][HEIGHT];

void stamp2splat(int i){
	GLfloat x = stampPt[i][X];
	GLfloat y = stampPt[i][Y];	

	SplactColor[i] = stampColr[i];
	
	SplatVector[i][0][X] = x+SplatDelta/2;
	SplatVector[i][0][Y] = y;
	
	SplatVector[i][1][X] = x+SplatDelta/3;
	SplatVector[i][1][Y] = y-SplatDelta/3;
	
	SplatVector[i][2][X] = x;
	SplatVector[i][2][Y] = y-SplatDelta/2;
	
	SplatVector[i][3][X] = x-SplatDelta/3;
	SplatVector[i][3][Y] = y-SplatDelta/3;
	
	SplatVector[i][4][X] = x-SplatDelta/2;
	SplatVector[i][4][Y] = y;

	SplatVector[i][5][X] = x-SplatDelta/3;
	SplatVector[i][5][Y] = y+SplatDelta/3;
	
	SplatVector[i][6][X] = x;
	SplatVector[i][6][Y] = y+SplatDelta/2;
	
	SplatVector[i][7][X] = x+SplatDelta/3;
	SplatVector[i][7][Y] = y+SplatDelta/3;
}


void canvas()
{
	glBegin(GL_QUADS);//Denotes the beginning of a group of vertices that define one or more primitives.
		glColor3f(1,1,1);
		glVertex3f(Xmin,Ymin, .02);
		glVertex3f(Xmin,Ymax, .02);
		glVertex3f(Xmax,Ymax, .02);
		glVertex3f(Xmax,Ymin, .02);
	glEnd();	//Terminates a list of vertices that specify a primitive initiated by glBegin.

}

void regStrokePoint(int x, int y){
	//std::cout<<x<<","<<y<<std::endl;
	strokePt[strokePtIdx][0]=(float)(x-xmid)/100;
	strokePt[strokePtIdx][1]=(float)(ymid-y)/100;
	strokeCol[strokePtIdx] = currentCol;
	strokePtIdx=(strokePtIdx+1)%STROKE_PTS;
}

float sqr(float x){return x*x;}
float ptDistance(float x1, float y1, float x2, float y2){
	return sqrt((float)(
		sqr(x1-x2) + sqr (y1-y2)
		));
}
void strokeToStamps(){
	int i=strokeEndIdx[strokeEITop-2];
	int end = strokeEndIdx[strokeEITop-1]-1; 
	int k = stampTopIdx; 

	stampPt[k][X] = strokePt[i][X];
	stampPt[k][Y] = strokePt[i][Y];
	stampColr[k] = strokeCol[i];

	while(i<end){
		//calculate point delta
		GLfloat delta = ptDistance(strokePt[i][X], strokePt[i][Y],
			strokePt[i+1][X], strokePt[i+1][Y]);

		if(delta <=StampDelta){
			int j = 1;//jump
			//advance a min distance
			GLfloat halfway = StampDelta*.65;
			while(delta<halfway && i<end){
				j++;
				delta = ptDistance(strokePt[i][X], strokePt[i][Y],
					strokePt[i+j][X], strokePt[i+j][Y]);
			}
			i=i+j;
			k++;
			stampPt[k][X] = strokePt[i][X];
			stampPt[k][Y] = strokePt[i][Y];
			stampColr[k] = strokeCol[i];
		}
		if(delta>StampDelta){
			GLfloat x,y, dist = 0.0,
				x1 = strokePt[i][X],
				y1 = strokePt[i][Y],
				x2 = strokePt[i+1][X],
				y2 = strokePt[i+1][Y];
			GLint col = strokeCol[i];
			x=x1;y=y1;
			int steps = ceil(delta/StampDelta);
			int step = 0;
			while(step++<steps){
				k++;
				stampPt[k][X] = x = x1 + step * StampDelta * (x2-x1) / delta;
				stampPt[k][Y] = y = y1 + step * StampDelta * (y2-y1) / delta;
				stampColr[k] = col;
				//dist+=StampDelta;
			}
			k++;
			stampPt[k][X] = x2;
			stampPt[k][Y] = y2;
			stampColr[k] = col;
		}
		i++;
	}

	for(int j = stampTopIdx;j<=k;j++){
		stamp2splat(j);
	}
	stampTopIdx = k+1;
}

void regStrokeEndIdx(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		strokeEndIdx[strokeEITop++] = strokePtIdx;
		strokeEndIdx[strokeEITop] = STROKE_CNT+1;

		strokeToStamps();
		strokeEITop = 1; strokePtIdx=0;
		lastStampedPt = strokePtIdx; 
	}
	
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		regStrokePoint(x,y);
	}
}

void drawPoint(GLfloat x, GLfloat y, GLfloat red=0.5f, GLfloat green=0.5f, GLfloat blue=0.5f, GLfloat pointSize=1.0f){
	glPointSize(pointSize);
	glBegin(GL_POINTS);
		glColor3f(red, green, blue);
		glVertex3f(x,y,.1);
	glEnd();
}

void drawPoints(){
	for(int i=0;i<strokePtIdx;i++){
		drawPoint(strokePt[i][0],strokePt[i][1]);
	}
}

void drawSplat(int i){
	GLint c = SplactColor[i];
	glColor3f((GLfloat)(c/0x010000)/256,
		(GLfloat)((c/0x000100)%0x100)/256,
		(GLfloat)(c%0x100)/256
		);
	glBegin(GL_POLYGON);
	for(int j=0;j<N;j++)
		glVertex3f(SplatVector[i][j][X],SplatVector[i][j][Y],.1);
	glEnd();
}

void drawStamp(int i){
	GLint c = stampColr[i];
	drawPoint(stampPt[i][X], stampPt[i][Y],
		(GLfloat)(c/0x010000)/256,
		(GLfloat)((c/0x000100)%0x100)/256,
		(GLfloat)(c%0x100)/256,
		2.0f);
}

GLfloat knotVector [8] = {0.0,0.0,0.0,0.0, 1.0,1.0,1.0,1.0,};
GLfloat cntrlPnts [4][3];
GLUnurbsObj *cbz = gluNewNurbsRenderer();

void drawSpline(int i){	
	cntrlPnts [0][0] = strokePt[i-1][0];
	cntrlPnts [0][1] = strokePt[i-1][1];
	cntrlPnts [0][2] = 0.1;

	cntrlPnts [1][0] = strokePt[i][0];
	cntrlPnts [1][1] = strokePt[i][1];
	cntrlPnts [1][2] = 0.1;

	cntrlPnts [2][0] = strokePt[i+1][0];
	cntrlPnts [2][1] = strokePt[i+1][1];
	cntrlPnts [2][2] = 0.1;

	cntrlPnts [3][0] = strokePt[i+2][0];
	cntrlPnts [3][1] = strokePt[i+2][1];
	cntrlPnts [3][2] = 0.1;

	glPointSize(5.0f);
	GLint c = strokeCol[i];
	glColor3f(
		(float)(c/0x010000)/256,
		(float)((c/0x000100)%0x100)/256,
		(float)(c%0x100)/256
		);
	gluNurbsCurve(cbz, 8, knotVector, 3, &cntrlPnts[0][0], 4, GL_MAP1_VERTEX_3);
}


//void drawSplines(){
//	for(int m=0;m<=strokeEITop;m++){
//		for(int i=strokeEndIdx[m]+1; i+2<strokeEndIdx[m+1] && i+2<strokePtIdx ;i+=3){
//			gluBeginCurve(cbz);
//				drawSpline(i);
//			gluEndCurve(cbz);
//		}
//	}
//}

void drawSplines(){
	for(int i=lastStampedPt+2; i+2<strokePtIdx ;i+=3){
		gluBeginCurve(cbz);
			drawSpline(i);
		gluEndCurve(cbz);
	}
}

void drawStamps(){
	for(int i=0;i<stampTopIdx;i++){
		drawStamp(i);
	}
}

void drawSplats(){
	for(int i=stampTopIdx-1;i>=0;i--){
		drawSplat(i);
	}
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
	
	glViewport( 0, 0, WIDTH ,HEIGHT);
		//This function sets the region within a window that is used 
		//for mapping the clipping volume coordinates to physical 
		//window coordinates.

		//glScalef(-1,1,1);//Multiplies the current matrix by a scaling matrix.
		
		glTranslatef(trans,0,0);//Multiplies the current matrix by a translation matrix.

		glRotatef(theta, 0, 0, 1);//Rotates the current matrix by a rotation matrix.
		
		canvas();
		drawSplines();
		//drawStamps();
		drawSplats();
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
		case 27: exit(1);

		case '.':
			bRotate = !bRotate;	
			break;
		case '>':
			trans +=0.2;
			break;
		case '<':
			trans -=0.2;
			break;
		case '0':
			strokeEITop = 1; strokePtIdx=0; stampTopIdx = 0;
			break;

			//colors
		case 'w': currentCol = 0xffffff; break;
		case 'k': currentCol = 0x000000; break;
		case 'a': currentCol = 0x808080; break;
			
		case 'r': currentCol = 0xff0000; break;
		case 'R': currentCol = 0x8b0000; break;

		case 'g': currentCol = 0x00ff00; break;
		case 'G': currentCol = 0x008000; break;

		case 'b': currentCol = 0x0000ff; break;
		case 'B': currentCol = 0x000080; break;

			
		case 'v': currentCol = 0xee82ee; break;
		case 'm': currentCol = 0xff00ff; break;
		case 'z': currentCol = 0x8000ff; break;

		case 'y': currentCol = 0xffFF00; break;
		case 'o': currentCol = 0xff8c00; break;
		case 'O': currentCol = 0xff4500; break;//OrangeRed
		case 'l': currentCol = 0x80ff00; break;//yellow green

			
		case 'c': currentCol = 0x00ffFF; break;
		case 'f': currentCol = 0x00FF80; break;
		case 't': currentCol = 0x0080ff; break;


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
	/*cout<<"int"<<sizeof(int)<<endl;
	cout<<"GLint"<<sizeof(GLint)<<endl;
	cout<<"char"<<sizeof(char)<<endl;
	cout<<"short"<<sizeof(short)<<endl;
	cout<<"SplatParamSt"<<sizeof(SplatParamSt)<<endl;
	cout<<""<<sizeof(int)<<endl;
	cout<<""<<sizeof(int)<<endl;*/
	glutInit(&argc, argv);
	//This initializes the GLUT library, passing command line parameters 
	//(this has an effect mostly on Linux/Unix) 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//Initial parameters for the display. In this case, we are specifying 
	//a RGB display (GLUT_RGB) along with double-buffering (GLUT_DOUBLE), 
	//so the screen won't flicker when we redraw it. 
	//GLUT_DEPTH Specifies a 32-bit depth buffer
 
	//glutInitWindowPosition(100,100);
	glutInitWindowSize(WIDTH,HEIGHT);
	//Initial window size - width, height 
	glutCreateWindow("Ishika");
	//Creates and sets the window title 
	initRendering();

	glutKeyboardFunc(myKeyboardFunc);
		//Sets the keyboard callback function for the current window.
	glutMotionFunc(regStrokePoint);
	glutMouseFunc(regStrokeEndIdx);

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
	
	//printf("%s",glGetString( GL_VERSION ));
	
	glutMainLoop();
		//This function begins the main GLUT event-handling loop. 
		//The event loop is the place where all keyboard, mouse, 
		//timer, redraw, and other window messages are handled. 
		//This function does not return until program termination.
	
	return (0);
}