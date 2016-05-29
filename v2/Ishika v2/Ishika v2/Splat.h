#ifndef SPLAT_H
#define SPLAT_H

#include "IshikaStdInc.h"
#include "Util.h"

#define N 8

namespace ishika{
	//class represents a Splat
	// Splat is N-vertice polygon, used to model watercolor paint pigments
	// it has, 
	//   - its co-ordinate in the image model
	//   - bias vector
	//   - roughness
	//   - flow perfentage
	//   - color
	//   - opacity
	//   - age : age is initilized to a number of timesteps until when it can be advected
	//   -      decremented at each t.s. 
	//   -		later it is used to represent fixed and potential dried stages of life
	class Splat{
	protected:
		GLfloat x[N];
		GLfloat y[N];
		GLint color;
		GLfloat splatSize;
		//params
		GLshort bx; //motion bias x
		GLshort by; //motion bias y
		GLshort a;//age
		GLubyte  r;//roughness [1~255px]
		GLubyte  f;//flow percentage [0-100]
		GLubyte  o;//opacity

	private:
		//utility function computes area
		GLfloat area();
		//when the age is reached zero, it is no longer advected and bias vector is reset
		void zeroout();
		//it rewets the splat, besed on rewetting randomness, wet map etc
		void rewet(const GLushort wetmap[][HEIGHT]);

	public:
		//initilize
		void init(GLint splatPx, GLfloat pos_x, GLfloat pos_y, GLint color, GLshort bias_x, GLshort bias_y, GLushort age, GLubyte roughness, GLubyte flow_pct, GLubyte opacity);

		//advection is implemented here
		//it also handles the lifetime partially
		void advect(const GLushort wetmap[][HEIGHT]);
		
		// draw the splat,
		// param i: is the index of the splat, it is used to create z-index to properly render brush stroke ordering
		void draw(int i);

		//current age
		GLshort age(){return a;}
		
		//set the bias
		void bias(int x, int y){bx=x; by=y;}
	};

	/* 
	todo:
	flow pct from int to float
	opacity from int to float ??
	rANDOm!
	pixel size?
	*/
	

}

#endif SPLAT_H