#include "vector2d.h"
#include "vector3d.h"
#include <cmath>

namespace gsl
{
    Vector2D::Vector2D(GLfloat x_in, GLfloat y_in) : x{x_in}, y{y_in}
    {

    }

    Vector2D::Vector2D(const int v) : x{static_cast<GLfloat>(v)}, y{static_cast<GLfloat>(v)}
    {

    }

    Vector2D::Vector2D(const double v) : x{static_cast<GLfloat>(v)}, y{static_cast<GLfloat>(v)}
    {

    }

    const Vector2D& Vector2D::operator=(const Vector2D &rhs)
    {
        x = rhs.getX();
        y = rhs.getY();

        return *this;
    }

    Vector2D Vector2D::operator+(const Vector2D &rhs) const
    {
        return {x + rhs.getX(), y + rhs.getY()};
    }

    Vector2D Vector2D::operator-(const Vector2D &rhs) const
    {
        return {x - rhs.getX(), y - rhs.getY()};
    }

    Vector2D& Vector2D::operator+=(const Vector2D &rhs)
    {
        x += rhs.getX();
        y += rhs.getY();

        return *this;
    }

    Vector2D& Vector2D::operator-=(const Vector2D &rhs)
    {
        x -= rhs.getX();
        y -= rhs.getY();

        return *this;
    }

    Vector2D Vector2D::operator-() const
    {
        return {-x, -y};
    }

    Vector2D Vector2D::operator*(float lhs) const
    {
        return {x * lhs, y * lhs};
    }

    GLfloat Vector2D::length() const
    {
        return std::sqrt(std::pow(x, 2.f) + std::pow(y, 2.f));
    }

    void Vector2D::normalize()
    {
        GLfloat l = length();

        if (l > 0.f)
        {
            x = (x / l);
            y = (y / l);
        }
    }

    Vector2D Vector2D::normalized()
    {
        Vector2D normalized;
        GLfloat l = length();

        if (l > 0.f)
        {
            normalized.setX(x / l);
            normalized.setY(y / l);
        }

        return normalized;
    }
Vector3D Vector2D::barycentricCoordinates(const Vector2D &p1, const Vector2D &p2, const Vector2D &p3)
{
    Vector2D p12 = p2-p1; //x1
    Vector2D p13 = p3-p1; //x2

    Vector3D n = Vector3D(0,0,cross(p12,p13)); //arealet mellom p12 og p13
    GLfloat areal_123 = n.z; //n.length();//dobbelt areal

    Vector3D baryc; //til retur. husk
    //u
    Vector2D p = p2 - *this; //u1
    Vector2D q = p3 - *this;//u2
    n = Vector3D(0,0, cross(p,q));
    baryc.x = n.z/areal_123;

    //v
    p = p3- *this; //v1
    q = p1 - *this; //v2
    n = Vector3D(0,0, cross(p,q));
    baryc.y = n.z/areal_123;

    //w
    p = p1 - *this; //w1
    q = p2 -*this; //w2
    n = Vector3D(0,0, cross(p,q));
    baryc.z = n.z/areal_123;
   // std::cout << " baryryry: " << baryc << std::endl;
    return baryc;
}
    GLfloat Vector2D::cross(const Vector2D &v1, const Vector2D &v2)
    {
        return ((v1.getX() * v2.getY()) - (v1.getY() * v2.getX()));
    }

    GLfloat Vector2D::dot(const Vector2D  &v1, const Vector2D  &v2)
    {
        return ((v1.getX() * v2.getX()) + (v1.getY() * v2.getY()));
    }

    GLfloat Vector2D::getX() const
    {
        return x;
    }

    void Vector2D::setX(const GLfloat &value)
    {
        x = value;
    }

    GLfloat Vector2D::getY() const
    {
        return y;
    }

    void Vector2D::setY(const GLfloat &value)
    {
        y = value;
    }
} //namespace
