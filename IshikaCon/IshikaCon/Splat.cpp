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
	const float alpha = 0.03;

	void Splat::init(GLfloat splatSize, GLfloat pos_x, GLfloat pos_y, GLint color, GLushort bias_x, GLushort bias_y, GLushort age, GLubyte roughness, GLubyte flow_pct, GLubyte opacity){
			//copy location co-ordinates			
			x[0] = pos_x+splatSize/2;
			y[0] = pos_y;
    
			x[1] = pos_x+splatSize/3;
			y[1] = pos_y-splatSize/3;
    
			x[2] = pos_x;
			y[2] = pos_y-splatSize/2;
    
			x[3] = pos_x-splatSize/3;
			y[3] = pos_y-splatSize/3;
    
			x[4] = pos_x-splatSize/2;
			y[4] = pos_y;

			x[5] = pos_x-splatSize/3;
			y[5] = pos_y+splatSize/3;
    
			x[6] = pos_x;
			y[6] = pos_y+splatSize/2;
    
			x[7] = pos_x+splatSize/3;
			y[7] = pos_y+splatSize/3;
			//color
			this->color = color;
			//params
			this->bx = bias_x;
			this->by = bias_y;
			this->a = age;
			this->r = roughness;
			this->f = flow_pct;
			this->o = opacity;
		}

	void Splat::advect(const GLushort wetmap[][HEIGHT]){
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