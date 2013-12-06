#include "IshikaStdInc.h"
#include "Util.h"
#include "Splat.h"
#include "Stroke.h"
#include "C:\\Install\SOIL\SOIL.h"

using namespace std;

GLfloat strokePt[STROKE_PTS][DIM];
GLint strokeCol[STROKE_PTS];
GLint strokeEndIdx[STROKE_CNT];
int strokeEITop = 1;
int strokePtIdx = 0;
int lastStampedPt = -1;

GLfloat stampPt[STAMPS][DIM];
GLint stampColr[STAMPS];
int stampTopIdx = 0;

std::vector<unsigned char> rgbdata (4*WIDTH*HEIGHT);


void colorPicker(int s=2){
	glBegin(GL_TRIANGLES);//Denotes the beginning of a group of vertices that define one or more primitives.
	glColor3f(1,1,1); 	glVertex3f( s, s, .9);//7
	glColor3f(1,0,0); 	glVertex3f( s, 0, .9);//4
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(1,1,0); 	glVertex3f( s,-s, .9);//6
	glColor3f(1,0,1); 	glVertex3f( 0,-s, .9);//5
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(0,0,0); 	glVertex3f(-s,-s, .9);//0
	glColor3f(0,0,1); 	glVertex3f(-s, 0, .9);//1
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(0,1,1); 	glVertex3f(-s, s, .9);//3
	glColor3f(0,1,0); 	glVertex3f( 0, s, .9);//2
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C


	glColor3f(1,0,0); 	glVertex3f( s, 0, .9);//4
	glColor3f(1,1,0); 	glVertex3f( s,-s, .9);//6
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(1,0,1); 	glVertex3f( 0,-s, .9);//5
	glColor3f(0,0,0); 	glVertex3f(-s,-s, .9);//0
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(0,0,1); 	glVertex3f(-s, 0, .9);//1
	glColor3f(0,1,1); 	glVertex3f(-s, s, .9);//3
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C

	glColor3f(0,1,0); 	glVertex3f( 0, s, .9);//2
	glColor3f(1,1,1); 	glVertex3f( s, s, .9);//7
	glColor3f(1,1,1); 	glVertex3f( 0, 0, .9);//C


	glEnd();
}


namespace ishika{ 

	namespace Current{
		GLint BrushPx = 5;
		ishika::BrushType BrushType = BrushType::Simple;
		GLint Color = 0x0077cc;
		int StrokePointCount = 0;
	}

	GLushort WetMap[WIDTH][HEIGHT];

	vector<Splat> Splats;
	list<Stroke> Strokes;
	queue<Stamp> Stamps;
	GLint splatTopIndex = 0;
}
using namespace ishika;


void storeScreen(){
	glReadPixels(0, 0, WIDTH, HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE, &rgbdata[0]); 
	int save_result = SOIL_save_image(
		"image_patch1.tga",
		SOIL_SAVE_TYPE_TGA,
		WIDTH, HEIGHT, 4,
		rgbdata.data()
		);

}

void stamp2splat(int i){
	GLfloat x = stampPt[i][X];
	GLfloat y = stampPt[i][Y];	

	int c = stampColr[i];

	ishika::Splat thisSplat;
	thisSplat.init(15, x, y, c, 0, 0, 5, 50, 100, 50);
	Splats.push_back(thisSplat);

	int ix = x*RATIO+xmid;
	int iy = ymid-y*RATIO;

	int kx = -16, ky = -16;
	if(ix+kx<0) kx = 0-ix;
	if(iy+ky<0) ky = 0-iy;
	while( kx< 4 && (ix+kx)<WIDTH){
		while( ky< 4 && (iy+ky)<HEIGHT){
			WetMap[ix+kx][iy+ky]=90;
			ky++;
		}kx++;
	}
}


void CommitStrokeToStamps(int C){
	if(C<0 || Strokes.empty()) return;

	Stamp firstStamp;
	Stroke sk1 = Strokes.front();
	Strokes.pop_front();
	firstStamp.copyStroke(sk1);
	Stamps.push(firstStamp);

	int i=0;

	while (i<C && !Strokes.empty())
	{
		//next pt
		Stroke sk2 = Strokes.front();
		//calculate point delta
		GLfloat delta = ptDistance(sk1.x, sk1.y,sk2.x, sk2.y);

		if(delta <=StampDelta){
			int j = 1;//jump
			//advance a min distance
			GLfloat halfway = StampDelta*.65;
			while(delta<halfway && i<C && !Strokes.empty()){
				j++;
				Strokes.pop_front();
				sk2 = Strokes.front();
				delta = ptDistance(sk1.x, sk1.y,sk2.x, sk2.y);
			}
			i=i+j;
			sk1 = sk2;
			Stamp newStamp; newStamp.copyStroke(sk1);
			Stamps.push(newStamp);
			Strokes.pop_front();
		}
		if(delta>StampDelta){
			GLfloat x,y, dist = 0.0,
				x1 = sk1.x,
				y1 = sk1.y,
				x2 = sk2.x,
				y2 = sk2.y;
			GLint col = strokeCol[i];
			x=x1;y=y1;
			int steps = ceil(delta/StampDelta);
			int step = 0;
			while(step++<steps){
				x = x1 + step * StampDelta * (x2-x1) / delta;
				y = y1 + step * StampDelta * (y2-y1) / delta;
				Stamp newStamp; newStamp.copyStroke(sk1);
				newStamp.x = x; newStamp.y = y;
				Stamps.push(newStamp);
				//dist+=StampDelta;
			}
			Stamp newStamp; newStamp.copyStroke(sk2);
			sk1=sk2;
			Strokes.pop_front();
			newStamp.x = x; newStamp.y = y;
			Stamps.push(newStamp);
		}
		i++;
	}//while done

	while (!Stamps.empty())
	{
		ishika::Splat thisSplat;
		Stamp smp = Stamps.front();
		//thisSplat.init(15, x, y, c, 0, 0, 5, 50, 100, 50);
		thisSplat.init(smp.strokePx,smp.x, smp.y, smp.color, 0, 0, 50, smp.strokePx,100,50);
		Stamps.pop();
		Splats.push_back(thisSplat);
	}
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
				x0 = strokePt[i-1][X],
				y0 = strokePt[i-1][Y],
				x1 = strokePt[i][X],
				y1 = strokePt[i][Y],
				x2 = strokePt[i+1][X],
				y2 = strokePt[i+1][Y],
				x3 = strokePt[i+2][X],
				y3 = strokePt[i+2][Y];
			GLint col = strokeCol[i];
			x=x1;y=y1;
			int steps = ceil(delta/StampDelta);
			int step = 0;
			//a0 = y0/ (x0 - x1)(x0 - x2);
			//a1 = y1 / (x1 - x0)(x1 - x2);
			//a2 = y2/ (x2 - x0)(x2 - x1);
			while(step++<steps){
				k++;
				stampPt[k][X] = x = x1 + step * StampDelta * (x2-x1) / delta;
				stampPt[k][Y] = y = y1 + step * StampDelta * (y2-y1) / delta;
				/*= y0 *((x- x1)*(x- x2)) / ((x0 - x1)*(x0 - x2))
				+ y1 *((x - x0)*(x - x2))/ ((x1 - x0)*(x1 - x2)) 
				+ y2 *((x- x0)*(x- x1))/ ((x2 - x0)*(x2 - x1));*/
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


void canvas()
{
	glBegin(GL_QUADS);//Denotes the beginning of a group of vertices that define one or more primitives.
	glColor3f(1,1,1);
	glVertex3f(Xmin,Ymin, .0);
	glVertex3f(Xmin,Ymax, .0);
	glVertex3f(Xmax,Ymax, .0);
	glVertex3f(Xmax,Ymin, .0);
	glEnd();	//Terminates a list of vertices that specify a primitive initiated by glBegin.

}

void regStrokePoint(int x, int y){
	//std::cout<<x<<","<<y<<std::endl;
	strokePt[strokePtIdx][0]=(float)(x-xmid)/RATIO;
	strokePt[strokePtIdx][1]=(float)(ymid-y)/RATIO;
	strokeCol[strokePtIdx] = Current::Color;
	strokePtIdx=(strokePtIdx+1)%STROKE_PTS;

	//ishika 
	Stroke newStroke = Stroke( 
		(float)(x-xmid)/RATIO,
		(float)(ymid-y)/RATIO,
		Current::Color,
		Current::BrushPx,
		Current::BrushType);
	Strokes.push_back(newStroke);
	Current::StrokePointCount++;
}

void regStrokeEndIdx(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		strokeEndIdx[strokeEITop++] = strokePtIdx;
		strokeEndIdx[strokeEITop] = STROKE_CNT+1;

		//strokeToStamps();
		CommitStrokeToStamps(Current::StrokePointCount);
		Current::StrokePointCount = 0;
		//
		strokeEITop = 1; strokePtIdx=0;
		lastStampedPt = strokePtIdx; 
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		regStrokePoint(x,y);
	}
}

void drawPoint(GLfloat x, GLfloat y, GLfloat red=0.5f, GLfloat green=0.5f, GLfloat blue=0.5f, GLfloat pointSize=1.0f, GLfloat z=.1){
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glColor3f(red, green, blue);
	glVertex3f(x,y,z);
	glEnd();
}

void drawPoints(){
	for(int i=0;i<strokePtIdx;i++){
		drawPoint(strokePt[i][0],strokePt[i][1]);
	}
}

void WetMapUpdate(){
	for(int x=0;x<WIDTH;x++){
		for(int y=0;y<HEIGHT;y++){
			int wet = WetMap[x][y];
			if(wet>0){
				WetMap[x][y]=wet-1;
				/*drawPoint(
				(float)(x-xmid)/RATIO,
				(float)(ymid-y)/RATIO,
				0.9,0.9, 0.9, 
				1.0,0);*/
			}
			if(x>0 && y>0 && x+1<WIDTH && y+1<HEIGHT){
				if(WetMap[x][y]<7){
					int wetNbor = 0;
					if(WetMap[x][y+1]>7) wetNbor++;
					if(WetMap[x+1][y]>7) wetNbor++;
					if(WetMap[x][y-1]>7) wetNbor++;
					if(WetMap[x-1][y]>7) wetNbor++;
					if (wetNbor>1){
						WetMap[x][y] += WetMap[x][y+1]/10;
						WetMap[x][y] += WetMap[x+1][y]/10;
						WetMap[x][y] += WetMap[x][y-1]/10;
						WetMap[x][y] += WetMap[x-1][y]/10; 
					}
				}
			}
		}
	}
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
	try{
		for(int i=lastStampedPt+2; i+2<strokePtIdx ;i+=3){
			gluBeginCurve(cbz);
			drawSpline(i);
			gluEndCurve(cbz);
		}
	}catch(exception e){}
}

void drawStamps(){
	for(int i=0;i<stampTopIdx;i++){
		drawStamp(i);
	}
}

//void drawSplats(){
//	for(int i=0;i<stampTopIdx;i++){
//		//for(int i=stampTopIdx-1;i>=0;i--){
//		//drawSplat(i);
//		Splats[i].draw(i);
//		Splats[i].advect(WetMap);
//	}
//}

void drawSplats(){
	int i=0;
	for(vector<Splat>::iterator splatIt = Splats.begin(); splatIt!=Splats.end(); ++splatIt, i++){
		(*splatIt).draw(i);
		(*splatIt).advect(WetMap);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	canvas();

	drawSplines();
	//drawStamps();
	drawSplats();
	WetMapUpdate();
	//colorPicker();
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
	case '+':
		Current::BrushPx++;
		break;
	case '-':
		Current::BrushPx--;
		break;
	case '0':
		strokeEITop = 1; strokePtIdx=0; stampTopIdx = 0;
		ishika::Splats.clear();
		ishika::Strokes.clear();
		break;

	case '1': storeScreen();
		//colors
	case 'w': Current::Color = 0xffffff; break;//white
	case 'k': Current::Color = 0x000000; break;//black
	case 'a': Current::Color = 0x808080; break;//ash
	case 'A': Current::Color = 0xb0b0b0; break;//ash

	case 'e': Current::Color  = 0xf0f0d0; break;//eggshell
	case 'E': Current::Color  = 0xf0f0c0; break;//eggshell
	case 'F': Current::Color  = 0xE5AA70; break;//fawns

	case 'r': Current::Color  = 0xff0000; break;//red
	case 'R': Current::Color  = 0x8b0000; break;//dark red
	case 'p': Current::Color  = 0xFF8080; break;
	case 'P': Current::Color  = 0xFFc0c0; break;

	case 'g': Current::Color  = 0x00ff00; break;//green
	case 'G': Current::Color  = 0x008000; break;//dark gr

	case 'b': Current::Color  = 0x0000ff; break;//blue
	case 'B': Current::Color  = 0x000080; break;//dark bl
	case 's': Current::Color  = 0x80c0f0; break;//sky bl
	case 'S': Current::Color  = 0xb0e0ff; break;//light sky bl


	case 'v': Current::Color  = 0xee82ee; break;//violet
	case 'm': Current::Color  = 0xff00ff; break;//magenta
	case 'z': Current::Color  = 0x8000ff; break;//z-blue

	case 'y': Current::Color  = 0xffFF00; break;//yellow
	case 'o': Current::Color  = 0xff8c00; break;//orange
	case 'O': Current::Color  = 0xff4500; break;//OrangeRed
	case 'l': Current::Color  = 0x80ff00; break;//yellow green


	case 'c': Current::Color  = 0x00ffFF; break;//cyan
	case 'f': Current::Color  = 0x00FF80; break;//floruscent green
	case 't': Current::Color  = 0x0080ff; break;//teal

	case 'd': Current::Color  = 0x561010; break;//dusk red
	case 'D': Current::Color  = 0x210808; break;//dusk red


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