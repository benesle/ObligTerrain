#include "physics.h"

Physics::Physics()
{

}

Physics::~Physics()
{

}

bool Physics::update(gsl::Vector3D mTargetCoordinates)
{
    if(neighbours.size() != 0)
        checkBaricentricCoord(mTargetCoordinates);


    if(isOnTriangle == true)
    {
        newtonSecondLaw();
    }

return isOnTriangle;
}

void Physics::newtonSecondLaw()
{

    calcForce(mMass).normalize();

    mAcceleration = ((mNormal + mForce)*(1/mMass));
}

float Physics::calcHeight(gsl::Vector3D mBarycentricCoord, gsl::Vector3D p1, gsl::Vector3D p2, gsl::Vector3D p3)
{
    float height;

    height = (p1.y*mBarycentricCoord.x + p2.y *mBarycentricCoord.y + p3.y*mBarycentricCoord.z);
    return height;
}

void Physics::setRadius(float radius)
{
    mRadius = radius;
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
    for(Vertex vert : verteces)
    {
trianglePoints.push_back(vert.mXYZ);
    }

    std::cout << "Triangle Points: " << trianglePoints.size() << std::endl;
    mNeighbours = neighbours;
}

void Physics::checkBaricentricCoord(gsl::Vector3D mTargetCoordinates)
{
    gsl::Vector3D barcentricCoords;
    gsl::Vector3D p1;
    gsl::Vector3D p2;
    gsl::Vector3D p3;

    gsl::Vector2D mTarget(mTargetCoordinates.x, mTargetCoordinates.z);
    int mStartPositionTriangle = 0;
    int mTriangle = 0;

    if(trianglePoints.size() != 0)
    {
        p1 = trianglePoints.at(mStartPositionTriangle);
        p2 = trianglePoints.at(mStartPositionTriangle+1);
        p3 = trianglePoints.at(mStartPositionTriangle+2);

        barcentricCoords = mTarget.barycentricCoordinates(gsl::Vector2D(p1.x,p1.z),gsl::Vector2D(p2.x,p2.z),gsl::Vector2D(p3.x,p3.z));
        GLfloat u = barcentricCoords.x;
        GLfloat v = barcentricCoords.y;
        GLfloat w = barcentricCoords.z;

        while(mTriangleFound == false)
        {
            if(trianglePoints.size() >= 3)
            {
                if(barcentricCoords.x > 0 && barcentricCoords.y > 0 && barcentricCoords.z > 0)
                {
                    mTriangleFound = true;
                    break;
                }
                else
                {
                    gsl::Vector3D mN0(neighbours.at(mTriangle));
                    if((u <= v) && (u <= w))
                    {
                        if(static_cast<int>(mN0.x) >= 0)
                        {
                            mTriangle = mN0.x;
                        }
                        else if(static_cast<int>(mN0.y) >= 0)
                        {
                            mTriangle = mN0.y;
                        }
                        else if(static_cast<int>(mN0.z) >= 0)
                        {
                            mTriangle = mN0.z;
                        }
                        else
                        {
                            std::cout << "Something went wrong" << std::endl;
                            break;
                        }
                    }
                    else if((v <= u) &&  (v <= w))
                    {
                        if(mN0.y >= 0)
                        {
                            mTriangle = mN0.y;
                        }
                        else if(mN0.z >= 0)
                        {
                            mTriangle = mN0.z;
                        }
                        else if(mN0.x >= 0)
                        {
                            mTriangle = mN0.x;
                        }
                        else
                        {
                            std::cout << "Something went wrong" << std::endl;
                            break;
                        }
                    }
                    else if((w <= u) && (w <= v))
                    {
                        // std::cout << " inside w" << std::endl;
                        if(mN0.z >= 0)
                        {
                            mTriangle = mN0.z;
                        }
                        else if(mN0.y >= 0)
                        {
                            mTriangle = mN0.y;
                        }
                        else if(mN0.x >= 0)
                        {
                            mTriangle = mN0.x;
                        }
                        else
                        {
                            std::cout << "Something went wrong" << std::endl;
                            break;
                        }
                    }
                    else
                    {
                        isOnTriangle = false;
                        break;
                    }

                    mStartPositionTriangle = mTriangle*3;
                    p1 = trianglePoints.at(mStartPositionTriangle);
                    p2 = trianglePoints.at(mStartPositionTriangle+1);
                    p3 = trianglePoints.at(mStartPositionTriangle+2);

                    barcentricCoords = mTarget.barycentricCoordinates(gsl::Vector2D(p1.x,p1.z),gsl::Vector2D(p2.x,p2.z),gsl::Vector2D(p3.x,p3.z));
                    u = barcentricCoords.x;
                    v = barcentricCoords.y;
                    w = barcentricCoords.z;

                }
            }
        }
    }
    if(mTriangleFound == true)
    {
        mHeight = calcHeight(barcentricCoords,p1,p2,p3);
        mNormal = calcNormal(p1,p2,p3);
        calcAplha();

        isOnTriangle = true;

        mTriangleFound = false;
    }
    else
    {

        // std::cout << "Not within the area" << std::endl;
        mHeight =1;
    }
}
