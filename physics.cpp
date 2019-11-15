#include "physics.h"

Physics::Physics()
{

}

Physics::~Physics()
{

}

void Physics::newtonSecondLaw()
{

    calcForce(mMass).normalize();

    mAcceleration = ((mNormal + mForce)*(1/mMass));
}

float Physics::calcHeight()
{

}

void Physics::setRadius()
{

}

void Physics::calcAplha()
{
    mAlpha = std::acos(static_cast<double>(gsl::Vector3D::dot(mNormal,gsl::Vector3D().y)));
}

gsl::Vector3D Physics::calcForce(float mMass)
{
    float mPtr = mMass * gsl::GRAVITY;
    mForce = gsl::Vector3D(0, mPtr, 0);
    return mForce;
}

gsl::Vector3D Physics::calcNormal(gsl::Vector3D p1, gsl::Vector3D p2, gsl::Vector3D p3 )
{
    gsl::Vector3D v1 = p2 - p1;
    gsl::Vector3D v2 = p3 - p1;

    gsl::Vector3D mPoints = gsl::Vector3D::cross(v1,v2);

    mPoints.normalize();
    mNormal = mPoints;
    return mNormal;
}

gsl::Vector3D Physics::calcNormalForce()
{
    mNormalForce = (mNormal* mForce.y) * static_cast<float>(std::cos(mAlpha) );
    return mNormalForce;
}


void Physics::setTriangles(std::vector<Vertex> verteces, std::vector<gsl::Vector3D> mNeighbours )
{

}

void Physics::checkBaricentricCoord()
{
    gsl::Vector3D barcentricCoords;
    gsl::Vector3D p1;
    gsl::Vector3D p2;
    gsl::Vector3D p3;

    gsl::Vector2D mTarget(mTargetCoord.x, mTargetCoord.z);
    int mStartPositionTriangle = 0;
    int mTriangle = 0;
}

