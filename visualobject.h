#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "vertex.h"
#include "matrix4x4.h"
#include "shader.h"
#include "material.h"

class Physics;
class RenderWindow;

class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
    VisualObject();
    virtual ~VisualObject();
    virtual void init();
    virtual void draw()=0;
    virtual void move();
    virtual void update();

    gsl::Matrix4x4 mMatrix;
    gsl::Vector3D mStartPosition;
    gsl::Vector3D mVelocity;
    Physics* phys{nullptr};
    Material mMaterial;


    std::string mName;
    RenderWindow *mRenderWindow; //Just to be able to call checkForGLerrors()
    void setShader(Shader *shader);

protected:
    std::vector<Vertex> mVertices;   //This is usually not needed after object is made
    std::vector<GLuint> mIndices;    //This is usually not needed after object is made

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

};
#endif // VISUALOBJECT_H

