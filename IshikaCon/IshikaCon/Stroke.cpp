#include "IshikaStdInc.h"
#include "Util.h"
#include "Stroke.h"

using namespace std;

namespace ishika{

	
	Stroke::Stroke(GLfloat x, GLfloat y, GLint color, GLuint strokePx, BrushType brush){
		init(x,y,color, strokePx, brush);
	}

	void Stroke::init(GLfloat x, GLfloat y, GLint color, GLuint strokePx, BrushType brush){
		this->x = x;
		this->y = y;
		this->color = color;
		this->strokePx = strokePx;
		this->brushType = brush;
	}

}