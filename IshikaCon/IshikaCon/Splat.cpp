#include "Splat.h"

namespace ishika{
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

	void Splat::advect(const GLushort** wetmap){
		a--;
		if(a<=0) return;

		float dx = (1-alpha)*bx;
		float dy = (1-alpha)*by;
		unsigned int u = getrand(1,r+1);
		float U = u;

		for(int j=0;j<N;j++){
			dx = (1-alpha)*bx + alpha * (1/U) * v[j][X];
			dy = (1-alpha)*by + alpha * (1/U) * v[j][Y];

			u = getrand(-r,r);

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

	void Splat::draw(int i){
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

}