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
		GLushort bx; //motion bias x
		GLushort by; //motion bias y
		GLushort a;//age
		GLubyte  r;//roughness [1~255px]
		GLubyte  f;//flow percentage [0-100]
		GLubyte  o;//opacity

	private:
		GLfloat area();

	public:
		void init(GLfloat splatSize, GLfloat pos_x, GLfloat pos_y, GLint color, GLushort bias_x, GLushort bias_y, GLushort age, GLubyte roughness, GLubyte flow_pct, GLubyte opacity);

		void advect(const GLushort wetmap[][HEIGHT]);
		void draw(int i);
	};

}

#endif SPLAT_H