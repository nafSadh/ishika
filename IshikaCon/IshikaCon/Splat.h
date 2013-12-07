#ifndef SPLAT_H
#define SPLAT_H

#include "IshikaStdInc.h"
#include "Util.h"

#define N 8

namespace ishika{
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
		GLfloat area();
		void zeroout();
		void rewet(const GLushort wetmap[][HEIGHT]);

	public:
		/**
		<param></param>
		*/
		void init(GLint splatPx, GLfloat pos_x, GLfloat pos_y, GLint color, GLshort bias_x, GLshort bias_y, GLushort age, GLubyte roughness, GLubyte flow_pct, GLubyte opacity);

		void advect(const GLushort wetmap[][HEIGHT]);
		void draw(int i);
		GLshort age(){return a;}
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