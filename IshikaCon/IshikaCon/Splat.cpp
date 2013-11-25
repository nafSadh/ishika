#include <iostream>
#include <gl/glut.h>

#define X 0
#define Y 1
#define DIM 2
#define STROKE_PTS 15000
#define STROKE_CNT 5000
#define STAMPS 15000
#define SPLATS 15000
#define WIDTH 1200
#define HEIGHT 800
#define RATIO 100
#define FACTX 1000
GLint const xmid = WIDTH /2;
GLint const ymid = HEIGHT/2;

int getrand1(int min,int max){
	if(min==max) return max;
	return(rand()%(max-min)+min);
}

#define N 8
GLfloat v[8][DIM]={ { 1, 0},//0
{ 1,-1},//1
{ 0,-1},//2
{-1,-1},//3
{-1, 0},//4
{-1, 1},//5
{ 0, 1},//6
{ 1, 1}//7
};

const float alpha = 0.33;

namespace ishika{
	class Splat{
	protected:
		GLfloat x[N];
		GLfloat y[N];
		GLint color;

		//params
		GLushort bx; //motion bias x
		GLushort by; //motion bias y
		GLushort a;//age
		GLubyte  r;//roughness [1~255px]
		GLubyte  f;//flow percentage [0-100]
		GLubyte  o;//opacity

	public:
		void advect(const GLushort** wetmap){
			a--;
			if(a<=0) return;

			float dx = (1-alpha)*bx;
			float dy = (1-alpha)*by;
			unsigned int u = getrand1(1,r+1);
			float U = u;

			for(int j=0;j<N;j++){
				dx = (1-alpha)*bx + alpha * (1/U) * v[j][X];
				dy = (1-alpha)*by + alpha * (1/U) * v[j][Y];

				u = getrand1(-r,r);

				GLfloat _x = x[j] + ((float)(f)/100)*dx + 0 + u/RATIO;
				GLfloat _y = y[j] + ((float)(f)/100)*dy + 0 + u/RATIO;

				int ix = _x*RATIO+xmid;
				int iy = ymid-_y*RATIO;

				if(wetmap[ix][iy]>0) {
					x[j] = _x;
					y[j] = _y;
					if(r>0) o--; else o++;
				}
			}
		}

		void draw(int i){
			GLfloat opacity = 0.0035* o;
			glColor4f((GLfloat)(color/0x010000)/256,
				(GLfloat)((color/0x000100)%0x100)/256,
				(GLfloat)(color%0x100)/256,
				opacity
				);
			GLfloat z = ((float)i)/SPLATS;
			//cout<<z<<endl;
			glBegin(GL_POLYGON);
			for(int j=0;j<N;j++){
				glVertex3f(x[j],y[j],.1*z);
			}
			glEnd();
		}

	};

}