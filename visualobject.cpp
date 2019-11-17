#include "innpch.h"
#include "visualobject.h"
#include "physics.h"

VisualObject::VisualObject()
{
}

VisualObject::~VisualObject()
{
   glDeleteVertexArrays( 1, &mVAO );
   glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
}

void VisualObject::move()
{

}

void VisualObject::update()
{

}

void VisualObject::setShader(Shader *shader)
{
 mMaterial.mShader = shader;
}
