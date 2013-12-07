#pragma once
#ifndef ISHIKA_STD_INC
#define ISHIKA_STD_INC

#include <iostream>
#include <gl/glut.h>
#include <vector>
#include <queue>
#include <list>


#define X 0
#define Y 1
#define DIM 2
#define STROKE_PTS 15000
#define STROKE_CNT 5000
#define STAMPS 15000
#define SPLATS 15000

#define STAMPDELTA 0.01
#define SplatDelta 0.05
#define SplatSize 5

//window def
#define WIDTH 1200
#define HEIGHT 800
#define RATIO 100
#define FACTX 1000

extern GLfloat theta; 
extern GLfloat trans; 
extern GLfloat Xmin;
extern GLfloat Xmax;
extern GLfloat Ymin; 
extern GLfloat Ymax;
extern GLint const xmid;
extern GLint const ymid;
extern GLboolean bRotate;

#endif ISHIKA_STD_INC