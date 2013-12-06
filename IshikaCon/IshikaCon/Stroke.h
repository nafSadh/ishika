#pragma once
#ifndef STROKE_H
#define STROKE_H

#include "IshikaStdInc.h"
#include "Util.h"

namespace ishika {
	class Stroke
	{
	public:
		GLfloat x;
		GLfloat y;
		GLint color;
		BrushType brushType;
		GLushort strokePx;
		Stroke(GLfloat x=0, GLfloat y=0, GLint color=0x0077CC, GLuint strokePx=5, BrushType brush=BrushType::Simple);

		void init(GLfloat x, GLfloat y, GLint color, GLuint strokePx=5, BrushType brush=BrushType::Simple);
	};

	class  Stamp : public Stroke
	{
		public:
			void copyStroke(Stroke stroke);
		private:

	};
}
#endif STROKE_H