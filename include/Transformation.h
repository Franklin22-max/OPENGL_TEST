#ifndef TRANSFORMATION_H_INCLUDED
#define TRANSFORMATION_H_INCLUDED

union vect2D
{
    struct{ float x, y;};
    struct{ float s, t;};
};

union vect3D
{
    struct{float x,y,z;};
    struct{float s,t,p;};
    struct{float r,g,b;};
};

union vect4D
{
    struct{float x,y,z,w;};
    struct{float s,t,p,q;};
    struct{float r,g,b,a;};
};

struct matrix3x3
{
    float vertices[3][3];
};

struct matrix4x4
{
    float vertices[4][4];
};





#endif // TRANSFORMATION_H_INCLUDED
