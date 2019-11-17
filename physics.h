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

    gsl::Vector3D contactTriangle;
    std::vector<gsl::Vector3D> trianglePoints;
    std::vector<gsl::Vector3D> neighbours;


public:
    //To check if target is on triangle
    bool isOnTriangle = true;
    bool update(gsl::Vector3D mTargetCoordinates);
    //Newton's second acceleration of an object is dependent upon two variables
    //The force acting upon the object and it's mass
    void newtonSecondLaw();


    float calcHeight(gsl::Vector3D mBarycentricCoord,  gsl::Vector3D p1, gsl::Vector3D p2, gsl::Vector3D p3);
    float getHeight(){return mHeight;}
    void setRadius(float radius);
    void calcAplha();

    gsl::Vector3D getAcceleration(){return mAcceleration;}

    gsl::Vector3D calcForce(float mMass);
    gsl::Vector3D calcNormal(gsl::Vector3D p1, gsl::Vector3D p2, gsl::Vector3D p3 );
    gsl::Vector3D calcNormalForce();

    void setTarget(gsl::Vector3D mTargetCoordinates){mTargetCoordinates = mTargetCoord;}
    void setTriangles(std::vector<Vertex> verteces, std::vector<gsl::Vector3D> mNeighbours );
    void checkBaricentricCoord(gsl::Vector3D mTargetCoordinates);

};

#endif // PHYSICS_H
