#include "IshikaStdInc.h"

GLfloat theta = 0.0; 
GLfloat trans=0.0; 
GLfloat Xmin = -1.0f*(WIDTH /RATIO/2);
GLfloat     Xmax =  1.0f*(WIDTH /RATIO/2);
GLfloat     Ymin = -1.0f*(HEIGHT /RATIO/2); 
GLfloat     Ymax =  1.0f*(HEIGHT /RATIO/2);
GLint const xmid = WIDTH /2;
GLint const ymid = HEIGHT/2;
GLboolean bRotate = false;

int getrand(int min,int max){
    if(min==max) return max;
    return(rand()%(max-min)+min);
}

float sqr(float x){return x*x;}
float ptDistance(float x1, float y1, float x2, float y2){
    return sqrt((float)(
        sqr(x1-x2) + sqr (y1-y2)
        ));
}