#include "IshikaStdInc.h"
#include "Util.h"
#include "Splat.h"
#include "Stroke.h"
#include "C:\\Install\SOIL\SOIL.h"

using namespace std;


//std::vector<unsigned char> rgbdata (4*WIDTH*HEIGHT);
unsigned char rgbdata [4*WIDTH*HEIGHT];


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
		GLint BrushPx = 25;
		ishika::BrushType BrushType = BrushType::Simple;
		GLint Color = 0x0077cc;
		int StrokePointCount = 0;
	}

	GLushort WetMap[WIDTH][HEIGHT];

	vector<Splat> Splats;
	list<Stroke> Strokes;
	queue<Stamp> Stamps;
}
using namespace ishika;


void drawPoint(GLfloat x, GLfloat y, GLfloat red=0.5f, GLfloat green=0.5f, GLfloat blue=0.5f, GLfloat pointSize=1.0f, GLfloat z=.0001){
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glColor3f(red, green, blue);
	glVertex3f(x,y,z);
	glEnd();
}
int x_1,x_2,y_1,y_2;

void DrawWetMap(){
	for(int x=0;x<WIDTH;x++){
		for(int y=0;y<HEIGHT;y++){
			int wet = WetMap[x][y];
			if(wet>0){
				drawPoint(
					(float)(x-xmid)/RATIO,
					(float)(ymid-y)/RATIO,
					0.93,0.93, 0.95, 
					.1,1.00000001);
			}
		}
	}
}
void WetMapUpdate(){
	for(int x=0;x<WIDTH;x++){
		for(int y=0;y<HEIGHT;y++){
			int wet = WetMap[x][y];
			if(wet>0){
				WetMap[x][y]=wet-1;
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

void stamp2splat(Stamp smp){
	GLfloat x = smp.x;
	GLfloat y = smp.y;	

	int ix = x*RATIO+xmid;
	int iy = ymid-y*RATIO;

	//thisSplat.init(15, x, y, c, 0, 0, 5, 50, 100, 50);

	float WRR = -0.50;
	int wetLife = 50;

	switch (smp.brushType)
	{
	default:
		break;

	case BrushType::Simple :
		ishika::Splat thisSplat;
		thisSplat.init(smp.strokePx, smp.x, smp.y, smp.color, 
			0, 0, 50, 2,100,50);
		Splats.push_back(thisSplat);
		break;

	case BrushType::WetOnDry:
		{
			int r = 2;
			int f = 100;
			float sz = ((double)smp.strokePx)/RATIO/2;
			ishika::Splat centerSplat;
			centerSplat.init(smp.strokePx/2, smp.x, smp.y, smp.color, 0, 0, 30,  r,f,50);
			Splats.push_back(centerSplat);

			ishika::Splat xSplat[6];
			xSplat[0].init(smp.strokePx/2, smp.x	 ,	smp.y+sz*.5,	smp.color, 0, 0, 30, r,f,	50);
			xSplat[1].init(smp.strokePx/2, smp.x	 ,	smp.y-sz*.5,	smp.color, 0, 0, 30, r,f,	50);
			xSplat[2].init(smp.strokePx/2, smp.x+sz*.5,	smp.y+sz*.3,	smp.color, 0, 0, 30, r,f,	50);
			xSplat[3].init(smp.strokePx/2, smp.x-sz*.5,	smp.y-sz*.3,	smp.color, 0, 0, 30, r,f,	50);
			xSplat[4].init(smp.strokePx/2, smp.x+sz*.5,	smp.y-sz*.3,	smp.color, 0, 0, 30, r,f,	50);
			xSplat[5].init(smp.strokePx/2, smp.x-sz*.5,	smp.y+sz*.3,	smp.color, 0, 0, 30, r,f,	50);
			for(int ixs=0;ixs<6;ixs++){
				Splats.push_back(xSplat[ixs]);
			}
			WRR = -.5;
			wetLife = 35;
		}
		break;

	case BrushType::WetOnWet:
		{
			int r = 5;
			int f = 100;
			ishika::Splat centerSplat;
			centerSplat.init(smp.strokePx/2, smp.x, smp.y, smp.color, 0, 0, 19, r, f, 65);

			ishika::Splat periSplat;
			periSplat.init(3*smp.strokePx/2, smp.x,	smp.y, smp.color, 0, 0, 19, r, f, 15);
			Splats.push_back(periSplat);
			Splats.push_back(centerSplat);
			WRR = -1;
			wetLife = 25;
		}
		break;
	}

	int kx = WRR*(smp.strokePx), ky = WRR*(smp.strokePx+1);
	int kxLim = -kx, kyLim = -1*ky;

	if(ix+kx<0) kx = 0-ix;
	if(iy+ky<0) ky = 0-iy;
	while( kx<= kxLim && (ix+kx)<WIDTH){
		while( ky<= kyLim && (iy+ky)<HEIGHT){
			WetMap[ix+kx][iy+ky]=wetLife;
			ky++;
		}kx++;ky=-kyLim;
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
				if(!Strokes.empty())sk2 = Strokes.front();
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
			if(!Strokes.empty())Strokes.pop_front();
			newStamp.x = x; newStamp.y = y;
			Stamps.push(newStamp);
		}
		i++;
	}//while done

	while (!Stamps.empty())
	{
		Stamp smp = Stamps.front();
		Stamps.pop();
		stamp2splat(smp);
	}
}

void storeScreen(){
	glReadPixels(0, 0, WIDTH, HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE, &rgbdata); 
	int save_result = SOIL_save_image(
		"image_patch1.tga",
		SOIL_SAVE_TYPE_TGA,
		WIDTH, HEIGHT, 4,
		//rgbdata.data()
		rgbdata
		);
	//rgbdata[0] = SOIL_load_OGL_texture("image_patch1.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
}

GLuint texture;
bool bCnavas = true;
void dryOut(){
	//glReadPixels(0, 0, WIDTH, HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE, &rgbdata[0]); 
	storeScreen();
	//tempSplat.clear();	
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexImage2D( GL_TEXTURE_2D, 4, GL_RGBA, WIDTH, HEIGHT,0,
		GL_RGBA, GL_UNSIGNED_BYTE, &rgbdata);	
	bCnavas=false;
	cout<<"fixing: "<<Splats.size()<<endl;
	vector<Splat> tempSplat;
	swap(Splats,tempSplat);
	for(vector<Splat>::iterator splatIt = tempSplat.begin(); splatIt!=tempSplat.end(); ++splatIt){
		if((*splatIt).age()>0){
			Splats.push_back(*splatIt);
		}
	}
	cout<<"to: "<<Splats.size()<<endl;
}

void canvas(float z)
{
	if(bCnavas){
		glBegin(GL_QUADS);//Denotes the beginning of a group of vertices that define one or more primitives.
		glColor3f(1,1,1);
		glVertex3f(Xmin,Ymin, z);
		glVertex3f(Xmin,Ymax, z);
		glVertex3f(Xmax,Ymax, z);
		glVertex3f(Xmax,Ymin, z);
		glEnd();	//Terminates a list of vertices that specify a primitive initiated by glBegin.
		//glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,&rgbdata[0]);
	}
	else
	{

		//glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,&rgbdata);
		glEnable( GL_TEXTURE_2D );
		//glBindTexture( GL_TEXTURE_2D, texture );
		glBegin( GL_QUADS );
		glTexCoord2d(0.0,0.0); glVertex3d(Xmin,Ymin,z);
		glTexCoord2d(1.0,0.0); glVertex3d(Xmax,Ymin,z);
		glTexCoord2d(1.0,1.0); glVertex3d(Xmax,Ymax,z);
		glTexCoord2d(0.0,1.0); glVertex3d(Xmin,Ymax,z);
		glEnd();/**/

	}
}


GLfloat knotVector [8] = {0.0,0.0,0.0,0.0, 1.0,1.0,1.0,1.0,};
GLfloat cntrlPnts [4][3];
GLUnurbsObj *cbz = gluNewNurbsRenderer();

void regStrokePoint(int x, int y){
	//std::cout<<x<<","<<y<<std::endl;
	Stroke newStroke = Stroke( 
		(float)(x-xmid)/RATIO,
		(float)(ymid-y)/RATIO,
		Current::Color,
		Current::BrushPx,
		Current::BrushType);	

	//giving bias to prev
	if(!Strokes.empty()){
		(Strokes.back()).bx = x-x_2;
		(Strokes.back()).by = y-y_2;
	}else{ x_1 =x; y_1=y;	}

	//adding new onw
	Strokes.push_back(newStroke);
	Current::StrokePointCount++;
	//temp storing for bias
	x_2 = x_1; 	x_1 = x;
	y_2 = y_1;  y_1 = y;
}

void regStrokeEndIdx(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		CommitStrokeToStamps(Current::StrokePointCount);
		Current::StrokePointCount = 0;
		Strokes.clear();
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		regStrokePoint(x,y);
	}
}

void drawSpline(int i){	/*
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
						gluNurbsCurve(cbz, 8, knotVector, 3, &cntrlPnts[0][0], 4, GL_MAP1_VERTEX_3);*/
}

void drawSplines(){/*
				   try{
				   for(int i=lastStampedPt+2; i+2<strokePtIdx ;i+=3){
				   gluBeginCurve(cbz);
				   drawSpline(i);
				   gluEndCurve(cbz);
				   }
				   }catch(exception e){}*/
}

void drawSplats(){
	static int frewla = 1;
	int i=0;
	frewla++;
	for(vector<Splat>::iterator splatIt = Splats.begin(); splatIt!=Splats.end(); ++splatIt, i++){
		(*splatIt).draw(i);
		if(frewla%15==0){(*splatIt).advect(WetMap);}
	}
	if(frewla%15==0)WetMapUpdate();
	DrawWetMap();
	//if(frewla%500==0)dryOut();
	frewla++;
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

	glPushMatrix();
	//This function pushes the current matrix onto the current matrix stack. 
	//This function is most often used to save the current transformation matrix 
	//so that it can be restored later with a call to glPopMatrix();

	glViewport( 0, 0, WIDTH ,HEIGHT);
	//This function sets the region within a window that is used 
	//for mapping the clipping volume coordinates to physical 
	//window coordinates.

	gluLookAt(0,0,-1,
		0,0,-7,
		0,1,0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	canvas(.9);

	drawSplines();
	//drawStamps();
	drawSplats();
	//colorPicker();
	glPopMatrix();//Pops the current matrix off the matrix stack.


	/*glPushMatrix();
	glViewport( 0,0, 300, 200 );
	glTranslatef(trans,0,0);
	glRotatef(theta, 0.0, 0.0, 1.0);
	triangle();
	glPopMatrix();*/
	glFlush();

	glPushMatrix();
	//glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,&rgbdata[0]);
	glPopMatrix();

	//glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,&rgbdata[0]);
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

#define CTRL -96+
void myKeyboardFunc( unsigned char key, int x, int y )
{
	cout<<(int)key;
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
		ishika::Splats.clear();
		ishika::Strokes.clear();
		while(!ishika::Stamps.empty()) ishika::Stamps.pop();
		break;

	case '1': ishika::Current::BrushType = BrushType::Simple; break;
	case '2': ishika::Current::BrushType = BrushType::WetOnDry; break;
	case '3': ishika::Current::BrushType = BrushType::WetOnWet; break;
	case '4': ishika::Current::BrushType = BrushType::Blobby; break;
	case '5': ishika::Current::BrushType = BrushType::Crunchy; break;

	case (CTRL's'): storeScreen();//ctrl+s
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