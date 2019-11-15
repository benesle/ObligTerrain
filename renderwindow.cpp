
#include "renderwindow.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>

#include "shader.h"
#include "mainwindow.h"
#include "matrix4x4.h"
#include "gsl_math.h"

#include "xyz.h"
#include "trianglesurface.h"
#include "octahedronball.h"

#include "LAS/lasloader.h"
#include "ballsimmulation.h"
#include "physics.h"
#include "vertex.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)
{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create())
    {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow()
{
    for (int i = 0; i < 4; ++i)
    {
        if (mShaderProgram[i])
            delete mShaderProgram[i];
    }
}

/// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void RenderWindow::init()
{
    //Connect the gameloop timer to the render function:
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));

    //********************** General OpenGL stuff **********************
    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!

    if (!mContext->makeCurrent(this))
    {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info:
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want -
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);    //color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    mShaderProgram[0] = new Shader("../ObligTerrain/Shaders/plainshader.vert", "../ObligTerrain/Shaders/plainshader.frag");
    qDebug() << "Plain shader program id: " << mShaderProgram[0]->getProgram();
    mShaderProgram[1]= new Shader("../ObligTerrain/Shaders/textureshader.vert", "../ObligTerrain/Shaders/textureshader.frag");
    qDebug() << "Texture shader program id: " << mShaderProgram[1]->getProgram();
    mShaderProgram[2]= new Shader("../ObligTerrain/Shaders/phongshader.vert", "../ObligTerrain/Shaders/phongshader.frag");
    qDebug() << "Phong shader program id: " << mShaderProgram[2]->getProgram();

    setupPlainShader(0);
    setupTextureShader(1);

    //**********************  Texture stuff: **********************
    mTexture[0] = new Texture("white.bmp",0);
    mTexture[1] = new Texture("hund.bmp", 1);
    mTexture[2] = new Texture("skybox.bmp", 2);

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[0]->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture[1]->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mTexture[2]->id());


    //*****************************************************HERE*************************************

    gsl::LASLoader loader{"../ObligTerrain/1.las"};

    bool flipY = true;

    gsl::Vector3D min{};
    gsl::Vector3D max{};
    std::vector<gsl::Vector3D> terrainPoints;

    terrainPoints.reserve(loader.pointCount());
    for (auto it = loader.begin(); it != loader.end(); it = it + 10)
    {
        terrainPoints.emplace_back(it->xNorm(), it->zNorm(), it->yNorm());

        min.x = (terrainPoints.back().x < min.x) ? terrainPoints.back().x : min.x;
        min.y = (terrainPoints.back().y < min.y) ? terrainPoints.back().y : min.y;
        min.z = (terrainPoints.back().z < min.z) ? terrainPoints.back().z : min.z;

        max.x = (terrainPoints.back().x > max.x) ? terrainPoints.back().x : max.x;
        max.y = (terrainPoints.back().y > max.y) ? terrainPoints.back().y : max.y;
        max.z = (terrainPoints.back().z > max.z) ? terrainPoints.back().z : max.z;
    }

    int xGridSize{10}, zGridSize{10};
    terrainPoints = mapToGrid(terrainPoints, xGridSize, zGridSize, min, max);
    terrainPoints.shrink_to_fit();



    mTerrainVertices.reserve(terrainPoints.size());
    std::transform(terrainPoints.begin(), terrainPoints.end(), std::back_inserter(mTerrainVertices), [](const gsl::Vector3D& point)
    {
        return Vertex{(point - 0.5f) * 40.f, {0.18f, 0.33f, 0.8f}, {0, 0}};
    });

    std::cout << "Point count: " << mTerrainVertices.size() << std::endl;
    //Make triangulation

    // Create indices
    mTerrainTriangles.reserve((xGridSize - 1) * (zGridSize - 1) * 2);
    for (unsigned int z{0}, i{0}; z < zGridSize - 1; ++z, ++i)
    {
        for (unsigned int x{0}; x < xGridSize - 1; ++x, ++i)
        {
            mTerrainTriangles.push_back({{i, i + xGridSize, i + 1},
                                         {
                                             static_cast<int>(mTerrainTriangles.size()) + 1,
                                             (z != 0) ? static_cast<int>(static_cast<int>(mTerrainTriangles.size()) - (xGridSize - 1) * 2 + 1) : -1,
                                             (x != 0) ? static_cast<int>(mTerrainTriangles.size()) - 1 : -1
                                         }
                                        });

            std::cout << "Added a triangle with index: " << mTerrainTriangles.back().index[0] << ", " << mTerrainTriangles.back().index[1]
                      << ", " << mTerrainTriangles.back().index[2] << " and neighbours: " << mTerrainTriangles.back().neighbour[0]
                      << ", " << mTerrainTriangles.back().neighbour[1] << ", " << mTerrainTriangles.back().neighbour[2] << std::endl;

            mTerrainTriangles.push_back({{i + 1, i + xGridSize, i + 1 + xGridSize} ,
                                         {
                                             (z < zGridSize - 2) ? static_cast<int>(static_cast<int>(mTerrainTriangles.size()) + (zGridSize - 1) * 2 - 1) : -1,
                                             (x < xGridSize - 2) ? static_cast<int>(mTerrainTriangles.size() + 1) : -1,
                                             static_cast<int>(mTerrainTriangles.size()) - 1
                                         }
                                        });

            std::cout << "Added a triangle with index: " << mTerrainTriangles.back().index[0] << ", " << mTerrainTriangles.back().index[1]
                      << ", " << mTerrainTriangles.back().index[2] << " and neighbours: " << mTerrainTriangles.back().neighbour[0]
                      << ", " << mTerrainTriangles.back().neighbour[1] << ", " << mTerrainTriangles.back().neighbour[2] << std::endl;
        }

    }

    std::cout << "Triangle count: " << mTerrainTriangles.size() << std::endl;


    glGenVertexArrays(1, &mTerrainVAO);
    glBindVertexArray(mTerrainVAO);

    GLuint terrainVBO, terrainEBO;
    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, mTerrainVertices.size() * sizeof(Vertex), mTerrainVertices.data(), GL_STATIC_DRAW);


    unsigned int *data = new unsigned int[mTerrainTriangles.size() * 3];
    for (unsigned int i{0}; i < mTerrainTriangles.size(); ++i)
        for (unsigned int j{0}; j < 3; ++j)
            data[i * 3 + j] = mTerrainTriangles.at(i).index[j];

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTerrainTriangles.size() * 3 * sizeof(unsigned int), data, GL_STATIC_DRAW);

    delete[] data;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // glPointSize(10.f);


    //********************** Making the objects to be drawn **********************
    std::string filename;
    VisualObject * temp{nullptr};

    //Testing axis
    temp = new XYZ();
    temp->init();
    mVisualObjects.push_back(temp);

    //    temp = new OctahedronBall(2);
    //    temp->init();
    //    temp->setShader(mShaderProgram[0]);
    //    temp->mMatrix.scale(0.5f, 0.5f, 0.5f);
    //    temp->mName = "Ball";
    //    mVisualObjects.push_back(temp);
    //    mPlayer = temp;

    //testing triangle surface class
    temp = new TriangleSurface("../ObligTerrain/Assets/Meshes/box2.txt");
    temp->init();
    temp->mMatrix.rotateY(180.f);
    mVisualObjects.push_back(temp);
    mPhysics = new Physics();
    //    testing objmesh class - many of them!
    //    here we see the need for resource management!
    //    int x{0};
    //    int y{0};
    //    int numberOfObjs{100};

    //    for (int i{0}; i < numberOfObjs; i++)
    //    {
    //        temp = new ObjMesh("../INNgine2019/Assets/monkey.obj");
    //        temp->setShader(mShaderProgram[0]);
    //        temp->init();
    //        x++;
    //        temp->mMatrix.translate(0.f + x, 0.f, -2.f - y);
    //        temp->mMatrix.scale(0.5f);
    //        mVisualObjects.push_back(temp);
    //        if(x%10 == 0)
    //        {
    //            x = 0;
    //            y++;
    //        }
    //    }

    //    for(auto obj: mVisualObjects)
    //    {
    //        mMainWindow->objectNames.push_back(QString::fromStdString(obj->mName));
    //    }
    //    mMainWindow->addItemsInTree();

    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::Vector3D(1.f, 1.f, 4.4f));
    //    mCurrentCamera->yaw(45.f);
    //    mCurrentCamera->pitch(5.f);

    //new system - shader sends uniforms so needs to get the view and projection matrixes from camera
    mShaderProgram[0]->setCurrentCamera(mCurrentCamera);
    mShaderProgram[1]->setCurrentCamera(mCurrentCamera);
    mShaderProgram[2]->setCurrentCamera(mCurrentCamera);
}

std::vector<gsl::Vector3D> RenderWindow:: mapToGrid(const std::vector<gsl::Vector3D> &points, int gridX, int gridZ, gsl::Vector3D min, gsl::Vector3D max)
{
    //Make Grid, initialize it with empty points
    std::vector<std::pair<float, unsigned int>> grid;
    grid.resize(gridX * gridZ);


    //For each point find the nearest point in the grid and add it.
    for (auto point : points)
    {
        int closestIndex[2]{0, 0};
        for (int z{0}; z < gridZ; ++z)
        {
            for (int x{0}; x < gridX; ++x)
            {
                //Devide points to get avrege
                gsl::Vector3D gridPoint{
                    x * ((max.x - min.x) / gridX) + min.x,
                            0,
                            z * ((max.z - min.z) / gridZ) + min.z
                };


                gsl::Vector3D lastClosestPoint
                {
                    closestIndex[0] * ((max.x - min.x) / gridX) + min.x,
                            0,
                            closestIndex[1] * ((max.z- min.z) / gridZ) + min.z
                };


                //Compare and Find the closest point
                if ((gsl::Vector3D{point.x, 0, point.y} - gridPoint).length() < (gsl::Vector3D{point.x, 0, point.y} - lastClosestPoint).length())
                {
                    closestIndex[0] = x;
                    closestIndex[1] = z;
                }
            }
        }
        // std::cout << "point is: " << point << std::endl;

        //Use the last nearest point and add it to current nearest point
        auto& p = grid.at(closestIndex[0] + closestIndex[1] * gridZ);

        p.first += point.y;
        ++p.second;
    }

    // convert pair into only first of pair
    std::vector<gsl::Vector3D> outputs{};
    outputs.reserve(grid.size());
    for(int z{0}; z < gridZ; ++z)
    {
        for(int x{0}; x < gridX; ++x)
        {
            auto &p = grid.at(x + z * gridZ);

            outputs.emplace_back(x *((max.x - min.x)/ gridX) + min.x, (0 < p.second) ? p.first/ static_cast<float>(p.second): 0,
                                 z * ((max.z - min.z)/ gridZ) + min.z);
        }
    }
    return outputs;
}

///Called each frame - doing the rendering
void RenderWindow::render()
{
    //calculate the time since last render-call
    //this should be the same as xxx in the mRenderTimer->start(xxx) set in RenderWindow::exposeEvent(...)
    //    auto now = std::chrono::high_resolution_clock::now();
    //    std::chrono::duration<float> duration = now - mLastTime;
    //    std::cout << "Chrono deltaTime " << duration.count()*1000 << " ms" << std::endl;
    //    mLastTime = now;

    //input
    handleInput();
    mCurrentCamera->update();

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    VisualObject *temp{nullptr};

    //    if(mMainWindow->isAddCube == true)
    //    {

    //        temp = new TriangleSurface("box2.txt");
    //        temp->init();
    //        temp->mMatrix.rotateY(180.f);
    //        temp->setShader(mShaderProgram[0]);
    //        mVisualObjects.push_back(temp);
    //        mMainWindow->isAddCube = false;
    //    }

    for (auto visObject: mVisualObjects)
    {
        visObject->draw();
        // checkForGLerrors();
    }

    gsl::Matrix4x4 mMatrix4;
    mMatrix4.setToIdentity();
    glUseProgram(mShaderProgram[0]->getProgram());
    int location = glGetUniformLocation(mShaderProgram[0]->getProgram(),"mMatrix");
    glUniformMatrix4fv(location, 1, GL_TRUE, mMatrix4.constData());
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram[0]->getProgram(), "vMatrix"),1,GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram[0]->getProgram(), "pMatrix"),1,GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
    glBindVertexArray(mTerrainVAO);
    glDrawElements(GL_TRIANGLES, mTerrainTriangles.size()*3, GL_UNSIGNED_INT, 0);

    gsl::Vector3D a;
    gsl::Vector3D b;
    gsl::Vector3D c;

//    a= mTerrainVertices.at(mTerrainTriangles.at(5).index[0]).mXYZ;
//    b= mTerrainVertices.at(mTerrainTriangles.at(3).index[1]).mXYZ;
//    c= mTerrainVertices.at(mTerrainTriangles.at(5).index[2]).mXYZ;
//    std::cout << "NORMAL: " << mPhysics->calcNormal(a,b,c) << std::endl;
//    mPhysics->newtonSecondLaw();

    //Calculate framerate before
    // checkForGLerrors() because that takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    //    auto start = std::chrono::high_resolution_clock::now();
    mContext->swapBuffers(this);
    //    auto end = std::chrono::high_resolution_clock::now();
    //    std::chrono::duration<float> duration = end - start;
    //    std::cout << "Chrono deltaTime " << duration.count()*1000 << " ms" << std::endl;
    //    calculateFramerate();
}

//********************** Terrain Data **************************


//******************************** Terrain *******************************************


void RenderWindow::setupPlainShader(int shaderIndex)
{
    mMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "mMatrix" );
    vMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "vMatrix" );
    pMatrixUniform0 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "pMatrix" );
}

void RenderWindow::setupTextureShader(int shaderIndex)
{
    mMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "mMatrix" );
    vMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "vMatrix" );
    pMatrixUniform1 = glGetUniformLocation( mShaderProgram[shaderIndex]->getProgram(), "pMatrix" );
    mTextureUniform = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "textureSampler");
}

float RenderWindow::cameraSpeed() const
{
    return mCameraSpeed;
}

//This function is called from Qt when window is exposed (shown)
//and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed())
    {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact..)
        mRenderTimer->start(1);
        mTimeStart.start();
    }
    mAspectratio = static_cast<float>(width()) / height();
    //    qDebug() << mAspectratio;
    mCurrentCamera->mProjectionMatrix.perspective(45.f, mAspectratio, 1.f, 100.f);
    //    qDebug() << mCamera.mProjectionMatrix;
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void RenderWindow::toggleWireframe()
{
    mWireframe = !mWireframe;
    if (mWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}

//The way this is set up is that we start the clock before doing the draw call,
//and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate()
{
    long long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)    //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30) //once pr 30 frames = update the message twice pr second (on a 60Hz monitor)
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed/1000000., 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            frameCount = 0;     //reset to show a new message in 60 frames
        }
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if(mOpenGLDebugLogger)
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}

void RenderWindow::setCameraSpeed(float value)
{
    mCameraSpeed += value;

    //Keep within min and max values
    if(mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}

void RenderWindow::handleInput()
{
    //Camera
    mCurrentCamera->setSpeed(0.f);  //cancel last frame movement
    if(mInput.RMB)
    {
        if(mInput.W)
            mCurrentCamera->setSpeed(-mCameraSpeed);
        if(mInput.S)
            mCurrentCamera->setSpeed(mCameraSpeed);
        if(mInput.D)
            mCurrentCamera->moveRight(mCameraSpeed);
        if(mInput.A)
            mCurrentCamera->moveRight(-mCameraSpeed);
        if(mInput.Q)
            mCurrentCamera->updateHeigth(-mCameraSpeed);
        if(mInput.E)
            mCurrentCamera->updateHeigth(mCameraSpeed);
    }
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
    }

    //    You get the keyboard input like this
    if(event->key() == Qt::Key_W)
    {
        mInput.W = true;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = true;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = true;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = true;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = true;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = true;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = true;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = true;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = true;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = true;
    }
    if(event->key() == Qt::Key_U)
    {
    }
    if(event->key() == Qt::Key_O)
    {
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        mInput.W = false;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = false;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = false;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = false;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = false;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = false;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = false;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = false;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = false;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = false;
    }
    if(event->key() == Qt::Key_U)
    {
    }
    if(event->key() == Qt::Key_O)
    {
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void RenderWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB)
    {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mInput.RMB)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}
