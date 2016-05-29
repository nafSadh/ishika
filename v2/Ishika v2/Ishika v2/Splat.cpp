#include "Splat.h"

namespace ishika{
	GLfloat V[8][DIM]={ { 1, 0},//0
	{ 1,-1},//1
	{ 0,-1},//2
	{-1,-1},//3
	{-1, 0},//4
	{-1, 1},//5
	{ 0, 1},//6
	{ 1, 1}//7
	};
	const float alpha = 0.33;

	
	void Splat::init(GLint splatPx, GLfloat pos_x, GLfloat pos_y, GLint color, GLshort bias_x, GLshort bias_y, GLushort age, GLubyte roughness, GLubyte flow_pct, GLubyte opacity){
		this->splatSize = splatPx*(1.0/RATIO);
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
		this->a = age*2;
		this->r = roughness;
		this->f = flow_pct;
		this->o = opacity;
	}

	void Splat::advect(const GLushort wetmap[][HEIGHT]){
		a--;
		if(a<-2400) return;
		if(a==0) zeroout();
		if(a<0 && a>-2400 && rand()%7==3) { rewet(wetmap);  /*std::cout<<"rw ";*/ }
		if(a<0) return;

		float dx = (1-alpha)*bx;
		float dy = (1-alpha)*by;
		int u = getRandNZ(1,r+1,x[0]*RATIO,y[0]*RATIO);
		float U;

		GLfloat area0 = area();

		for(int j=0;j<N;j++){
			//U = (float)(getRandNZ(-r,r,x[j]*RATIO,y[j]*RATIO))/RATIO;
			//u = getRandNZ(-r,r,x[j]*RATIO,y[j]*RATIO);
			u = getRandNZ(1,1+r);
			U = ((float)getRandNZ(-r,r))/RATIO;
			
			dx = (1-alpha)*(bx) + alpha * (1.0/u) * V[j][X];
			dy = (1-alpha)*(by) + alpha * (1.0/u) * V[j][Y];

			//u = getrand(-r,r,x[j]*RATIO,y[j]*RATIO);
			//U = (float)(getrand(-r,r,x[j]*RATIO,y[j]*RATIO))/RATIO;


			GLfloat _x = x[j] + ((float)(f)/100)*dx/RATIO + 0 + U/a;
			GLfloat _y = y[j] + ((float)(f)/100)*dy/RATIO + 0 + U/a;

			int ix = _x*RATIO+xmid;
			int iy = ymid-_y*RATIO;

			//smpling managements
			int prev=(j-1)%N;
			int next=(j+1)%N;
			GLfloat distPrev = ptDistance(_x,_y, x[prev], y[prev]); 
			GLfloat distNext = ptDistance(_x,_y, x[next], y[next]); 

			//other way

			
			if(0<=ix && ix<WIDTH && 0<=iy && iy<HEIGHT &&
				wetmap[ix][iy]>0
				&& distPrev<splatSize*1.05
				&& distNext<splatSize*1.05) {
				x[j] = _x;
				y[j] = _y;
				//if(r>0) o--; else o++;
			}
		}
		GLfloat area1 = 0.99*area();
		//pigment conserve 
		o = o*area0/area1;
		if(o<15) o=15;
	}

	void Splat::zeroout(){
		bx=by=0;
	}

	void Splat::rewet(const GLushort wetmap[][HEIGHT]){
		int wetCnt = 0;
		for(int j=0;j<N;j++){			
			int ix = x[j]*RATIO+xmid;
			int iy = ymid-y[j]*RATIO;
			if(wetmap[ix][iy]>50)wetCnt++;
		}
		if(wetCnt>4) {a = wetCnt*10; /*std::cout<<"rewet\n";*/}
	}

	void Splat::draw(int i){
		GLfloat opacity = 0.01 *.35* o;
		glColor4f((GLfloat)(color/0x010000)/256,
			(GLfloat)((color/0x000100)%0x100)/256,
			(GLfloat)(color%0x100)/256,
			opacity
			);
		GLfloat z = ((float)i)/SPLATS;
		//cout<<z<<endl;
		glBegin(GL_POLYGON);
		for(int j=0;j<N;j++){
			glVertex3f(x[j],y[j],1+.1*z);
		}
		glEnd();
	}
	
	GLfloat Splat::area(){
		GLfloat h1 = ptDistance(x[0],y[0],x[4],y[4]);
		GLfloat w1 = ptDistance(x[2],y[2],x[6],y[6]);
		GLfloat h2 = ptDistance(x[3],y[3],x[7],y[7]);
		GLfloat w2 = ptDistance(x[5],y[5],x[5],y[5]);

		return (h1*w1 + h2*w2 )*.5; 
	}
}