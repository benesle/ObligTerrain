#ifndef PHYSICS_H
#define PHYSICS_H
#include "innpch.h"

class Physics
{
public:
    Physics();
    ~Physics();
private:
    float mHeight = 1.0f;
    float mRadius = 1.0f;
    float mMass = 1.0f;

    double mAlpha;
    bool mTriangleFound = false;

    gsl::Vector3D mAcceleration;
    gsl::Vector3D mForce;

    gsl::Vector3D mTargetCoord;

    gsl::Vector3D mNormal;
    gsl::Vector3D mNormalForce;



public:
    //To check if target is on triangle
    bool isOnTriangle = true;
    //Newton's second acceleration of an object is dependent upon two variables
    //The force acting upon the object and it's mass
    void newtonSecondLaw();


    float calcHeight();
    float getHeight(){return mHeight;}
    void setRadius();
    void calcAplha();

    gsl::Vector3D getAcceleration(){return mAcceleration;}

    gsl::Vector3D calcForce(float mMass);
    gsl::Vector3D calcNormal(gsl::Vector3D p1, gsl::Vector3D p2, gsl::Vector3D p3 );
    gsl::Vector3D calcNormalForce();

    void setTarget(gsl::Vector3D mTargetCoord){mTargetCoord;}
    void setTriangles(std::vector<Vertex> verteces, std::vector<gsl::Vector3D> mNeighbours );
    void checkBaricentricCoord();

};

#endif // PHYSICS_H
