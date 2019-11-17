#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector3d.h"
#include "matrix4x4.h"

class Shader;
class Camera;

class Material
{
public:
    Material();

    void setShader(class Shader *shader);
    void setTextureUnit(const GLuint &textureUnit);
    void setColor(const gsl::Vector3D &color);

    //virtual void setMMatrix(gsl::Matrix4x4& mMatrix) = 0;

    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0};     //the actual texture to put into the uniform
    Shader *mShader{nullptr};
};

#endif // MATERIAL_H
