#pragma once
#ifndef STROKE_H
#define STROKE_H

#include "IshikaStdInc.h"
#include "Util.h"

namespace ishika {
	//represents a stroke point
	class Stroke
	{
	public:
		GLfloat x;
		GLfloat y;
		GLint color;
		BrushType brushType;
		GLushort strokePx;
		GLshort bx; //motion bias x
		GLshort by; //motion bias y
		Stroke(GLfloat x=0, GLfloat y=0, GLint color=0x0077CC, GLuint strokePx=5, BrushType brush=BrushType::Simple);

		void init(GLfloat x, GLfloat y, GLint color, GLuint strokePx=5, BrushType brush=BrushType::Simple);
	};

	//represents a stamp
	// it is basically same in structure as Stroke, but conceptually it is the stamp, 
	// stamp is computed from input stroke points to place them in uniform density in the input stroke trajectory
	// stamps are converted to a set of splats, based on brush types
	class  Stamp : public Stroke
	{
		public:
			//copies from a stroke to populated this stamp object
			void copyStroke(Stroke stroke);
		private:

	};
}
#endif STROKE_H