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

		//params
		GLushort bx; //motion bias x
		GLushort by; //motion bias y
		GLushort a;//age
		GLubyte  r;//roughness [1~255px]
		GLubyte  f;//flow percentage [0-100]
		GLubyte  o;//opacity

	public:
		void advect(const GLushort** wetmap);
		void draw(int i);
	};

}

#endif SPLAT_H