#ifndef INNPCH_H
#define INNPCH_H

//Qt stuff
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>

#include <QDebug>
#include <QWindow>
#include <QTreeWidget>
#include <QElapsedTimer>
#include <QImage>
#include <QBuffer>
#include <QByteArray>

#include <QKeyEvent>
#include <QTimer>
#include <QStatusBar>

//STL and standard includes
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iterator>
#include <chrono>

//Own stuff
#include "matrix4x4.h"
#include "matrix3x3.h"
#include "matrix2x2.h"
#include "vector4d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "vertex.h"

#include "gsl_math.h"
#include "math_constants.h"

//Functionality
#include "camera.h"
#include "mainwindow.h"
#include "input.h"
#include "gltypes.h"

//Object
#include "shader.h"
#include "texture.h"
#include "visualobject.h"
#include "xyz.h"
#include "octahedronball.h"
#include "trianglesurface.h"



#endif // INNPCH_H
