#include "innpch.h"
#include "material.h"

Material::Material()
{

}

void Material::setColor(const gsl::Vector3D &color)
{
    mObjectColor = color;
}


void Material::setShader(Shader *shader)
{
    mShader = shader;
}
