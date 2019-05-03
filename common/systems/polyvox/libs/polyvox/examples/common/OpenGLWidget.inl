#include "OpenGLWidget.h"

#include <QMouseEvent>
#include <QTimer>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Protected functions
////////////////////////////////////////////////////////////////////////////////
template <typename QOpenGLFunctionsType>
OpenGLWidget<QOpenGLFunctionsType>::OpenGLWidget(QWidget *parent)
:QGLWidget(parent)
{
}

template <typename QOpenGLFunctionsType>
const QMatrix4x4& OpenGLWidget<QOpenGLFunctionsType>::viewMatrix()
{
	return mViewMatrix;
}

template <typename QOpenGLFunctionsType>
const QMatrix4x4& OpenGLWidget<QOpenGLFunctionsType>::projectionMatrix()
{
	return mProjectionMatrix;
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::setCameraTransform(QVector3D position, float pitch, float yaw)
{
	mCameraPosition = position;
	mCameraYaw = yaw;
	mCameraPitch = pitch;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////
template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::initializeGL()
{
	if (!this->initializeOpenGLFunctions())
	{
		std::cerr << "Could not initialize OpenGL functions" << std::endl;
		exit(EXIT_FAILURE);
	}

	//Print out some information about the OpenGL implementation.
	std::cout << "OpenGL Implementation Details:" << std::endl;
	if (this->glGetString(GL_VENDOR))
		std::cout << "\tGL_VENDOR: " << this->glGetString(GL_VENDOR) << std::endl;
	if (this->glGetString(GL_RENDERER))
		std::cout << "\tGL_RENDERER: " << this->glGetString(GL_RENDERER) << std::endl;
	if (this->glGetString(GL_VERSION))
		std::cout << "\tGL_VERSION: " << this->glGetString(GL_VERSION) << std::endl;
	if (this->glGetString(GL_SHADING_LANGUAGE_VERSION))
		std::cout << "\tGL_SHADING_LANGUAGE_VERSION: " << this->glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	//Set up the clear colour
	this->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	this->glClearDepth(1.0f);

	this->glEnable(GL_DEPTH_TEST);
	this->glDepthMask(GL_TRUE);
	this->glDepthFunc(GL_LEQUAL);
	this->glDepthRange(0.0, 1.0);

	initialize();

	// Start a timer to drive the main rendering loop.
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(0);

	mElapsedTimer.start();
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::resizeGL(int w, int h)
{
	//Setup the viewport
	this->glViewport(0, 0, w, h);

	auto aspectRatio = w / (float)h;
	float zNear = 1.0;
	float zFar = 1000.0;

	mProjectionMatrix.setToIdentity();
	mProjectionMatrix.perspective(mCameraFOV, aspectRatio, zNear, zFar);
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::paintGL()
{
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	QVector3D cameraForward(
		cos(mCameraPitch) * sin(mCameraYaw),
		sin(mCameraPitch),
		cos(mCameraPitch) * cos(mCameraYaw)
		);

	// Right vector
	QVector3D cameraRight(
		sin(mCameraYaw - 3.14f / 2.0f),
		0,
		cos(mCameraYaw - 3.14f / 2.0f)
		);

	// Up vector
	QVector3D cameraUp = QVector3D::crossProduct(cameraRight, cameraForward);

	// Get the elapsed time since last frame and convert to seconds.
	float deltaTime = mElapsedTimer.restart() / 1000.0f;

	// Move forward
	if ((mPressedKeys.contains(Qt::Key_Up)) || (mPressedKeys.contains(Qt::Key_W)))
	{
		mCameraPosition += cameraForward * deltaTime * mCameraMoveSpeed;
	}
	// Move backward
	if ((mPressedKeys.contains(Qt::Key_Down)) || (mPressedKeys.contains(Qt::Key_S)))
	{
		mCameraPosition -= cameraForward * deltaTime * mCameraMoveSpeed;
	}
	// Strafe right
	if ((mPressedKeys.contains(Qt::Key_Right)) || (mPressedKeys.contains(Qt::Key_D)))
	{
		mCameraPosition += cameraRight * deltaTime * mCameraMoveSpeed;
	}
	// Strafe left
	if ((mPressedKeys.contains(Qt::Key_Left)) || (mPressedKeys.contains(Qt::Key_A)))
	{
		mCameraPosition -= cameraRight * deltaTime * mCameraMoveSpeed;
	}

	mViewMatrix.setToIdentity();
	mViewMatrix.lookAt(
		mCameraPosition,           // Camera is here
		mCameraPosition + cameraForward, // and looks here : at the same position, plus "direction"
		cameraUp                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	//Clear the screen
	this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderOneFrame();

	// Check for errors.
	GLenum errCode = this->glGetError();
	if (errCode != GL_NO_ERROR)
	{
		std::cerr << "OpenGL Error: " << errCode << std::endl;
	}
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::mousePressEvent(QMouseEvent* event)
{
	// Initialise these variables which will be used when the mouse actually moves.
	m_CurrentMousePos = event->pos();
	m_LastFrameMousePos = m_CurrentMousePos;
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::mouseMoveEvent(QMouseEvent* event)
{
	// Update the x and y rotations based on the mouse movement.
	m_CurrentMousePos = event->pos();
	QPoint diff = m_CurrentMousePos - m_LastFrameMousePos;
	mCameraYaw -= diff.x() * mCameraRotateSpeed;
	mCameraPitch -= diff.y() * mCameraRotateSpeed;
	m_LastFrameMousePos = m_CurrentMousePos;
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		close();
	}

	mPressedKeys.append(event->key());
}

template <typename QOpenGLFunctionsType>
void OpenGLWidget<QOpenGLFunctionsType>::keyReleaseEvent(QKeyEvent* event)
{
	mPressedKeys.removeAll(event->key());
}