#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/intersect.hpp"

#include "vertex.hpp"
#include "meshcomponent.hpp"
#include "loader.hpp"
#include "basicshader.hpp"
#include "polyhedron.hpp"
#include "meshanalysis.hpp"
#include "subdivision.hpp"
#include "meshfactory.hpp"
#include "mousepicker.hpp"
#include "camera.hpp"
#include "perlinnoise.hpp"
#include "spherical.hpp"
#include "curvecomponent.hpp"
#include "lineshader.hpp"




/********************************************************************************/
/********************************************************************************/
/********************************** USER INPUT **********************************/
/********************************************************************************/
/********************************************************************************/

// Motion of the scene when rotating or zooming:
const float rotationFactor = { 0.01f };
const float scaleFactor = { 0.005f };
const float minScaleFactor = { 0.008f };

// Scroll wheel values:
const int scrollWheelUp = { 3 };
const int scrollWheelDown = { 4 };
const float scrollWheelClickFactor = { 5.0f };

// Mouse button values:
const int leftMouseButton   = { 4 };
const int middleMouseButton = { 2 };
const int rightMouseButton  = { 1 };

// User interface:
const int windowWidth = 1280;
const int windowHeight = 800;
int	mainWindow; 
int	activeMouseButton;
float zoomScale;
int	mouseX, mouseY;
float rotationX, rotationY;




/*********************************************************************************/
/*********************************************************************************/
/********************************** OPENGL/GLUT **********************************/
/*********************************************************************************/
/*********************************************************************************/
void Animate();
void Display();
void DoMainMenu(int id);
void InitGraphics();
void InitLists();
void InitMenus();
void Keyboard(unsigned char c, int x, int y);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void Resize(int x, int y);
void Visibility(int);
void Reset();





/*********************************************************************************/
/*********************************************************************************/
/************************************ FILE IO ************************************/
/*********************************************************************************/
/*********************************************************************************/
unsigned char*	BmpToTexture( char *, int *, int * );
int	ReadInt( FILE * );
short ReadShort( FILE * );




/**********************************************************************************/
/**********************************************************************************/
/*********************************** PROTOTYPES ***********************************/
/**********************************************************************************/
/**********************************************************************************/

Polyhedron* SubdivideMesh(Polyhedron* p, int n);
void LoadMeshFromFile(std::string fileName, int subdivisions);
void LoadMeshFromFile(std::string fileName, int subdivisions, int meshIndex, Curvature curvature);
void LoadMeshFromFile(std::string fileName, int subdivisions, std::vector<Curvature> curvature);
void LoadMeshFromFile(std::string fileName, int subdivisions, int meshIndex, glm::vec3 color);
void LoadUniformSphereMesh(float length, uint numPointsPerSide, int meshIndex);
std::string ToString(Curvature c);


/*********************************************************************************/
/*********************************************************************************/
/*********************************** VARIABLES ***********************************/
/*********************************************************************************/
/*********************************************************************************/

// Objects to draw:
MeshComponent mesh;
std::vector<MeshComponent> meshes;

// Curve objects to draw:
CurveComponent curve;
CurveComponent dualCurve;
CurveComponent maxPrincipalDirections;
CurveComponent minPrincipalDirections;
uint renderMaxMinPrincipalDirection = 0;

// Mesh selection:
std::vector<std::vector<MeshComponent>> meshList;
uint activeMesh = 0;
std::vector<Curvature> curvatureList;

// Copies of polyhedra.
Polyhedron* poly;

// Shaders:
BasicShader shader;
LineShader lineShader;

// Perspective:
glm::mat4 perspectiveMatrix;
glm::mat4 lightPerspectiveMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelViewProjectionMatrix;
float near = 0.1f;
float far = 1000.0f;

// View properties:
glm::vec3 lightPosition;
glm::vec3 lightEye;
bool viewFromSun = false;
Camera camera;

// Textures:
GLuint textureHandle;

// Lighting:
const float AMBIENT = 1.0f;
const float DIFFUSE = 0.0f;
const float SPECULAR = 0.0f;

/*
const float AMBIENT = 0.6f;
const float DIFFUSE = 0.3f;
const float SPECULAR = 1 - AMBIENT - DIFFUSE;
*/
const float SHININESS = 10.0f;
float ambient = AMBIENT;
float diffuse = DIFFUSE;
float specular = SPECULAR;
float shininess = SHININESS;
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

// Rendering effects:
bool enableWireframe = false;

// Animation:
bool animate = true;
float currentTime = 0;
#define MS_IN_THE_ANIMATION_CYCLE 10000

// User input:
MousePicker mousePicker;
bool selectTriangle = false;
bool lockCamera = true;


/*********************************************************************************/
/*********************************************************************************/
/*********************************** FUNCTIONS ***********************************/
/*********************************************************************************/
/*********************************************************************************/

// main program:
int main(int argc, char* argv[])
{
	// Initialize GLUT:
	glutInit(&argc, argv);
	//glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	std::cout << "***** INITIALIZED GLUT. *****" << std::endl;
	std::cout << std::endl;

	// Initialize graphics:
	InitGraphics();
	std::cout << "***** INITIALIZED LISTS. *****" << std::endl;
	std::cout << std::endl;

	// Initialize static items needed for rendering:
	InitLists();
	std::cout << "***** INITIALIZED LISTS. *****" << std::endl;
	std::cout << std::endl;

	// Set all variables to default values:
	Reset();
	std::cout << "***** INITIALIZED VARIABLES. *****" << std::endl;
	std::cout << std::endl;

	// Initialize menus:
	InitMenus();
	std::cout << "***** INITIALIZED MENUS. *****" << std::endl;
	std::cout << std::endl;

	std::cout << "***** GRAPHICS INITIALIZED. *****" << std::endl;
	std::cout << std::endl;

	// Begin drawing:
	glutSetWindow( mainWindow );
	glutMainLoop( );

	return 0;
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************** RENDERING ***********************************/
/*********************************************************************************/
/*********************************************************************************/

// Initialize graphics properties.
void InitGraphics()
{
	// Red-green-blue-alpha color, double-buffering, and z-buffering:
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// Set the initial window configuration:
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);

	// Open the window and set its title:
	mainWindow = glutCreateWindow("Computer Graphics Renderer");
	glutSetWindowTitle("Computer Graphics Renderer");

	// Set the framebuffer clear values:
	glClearColor( 1, 1, 1, 1);

	glutSetWindow( mainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// Init glew (a window must be open to do this):
	GLenum err = glewInit( );
}

std::string ToString(Curvature c)
{
	std::string s;
	switch (c)
	{
		case Curvature::HORIZON:
			s = "Horizon measure";
			break;
		case Curvature::MEAN:
			s = "Unsigned mean curvature";
			break;
		case Curvature::GAUSSIAN:
			s = "Gaussian curvature";
			break;
		case Curvature::ORIGINAL:
			s = "Original horizon measure";
			break;
		case Curvature::DISTORTION:
			s = "Unsigned distortion";
			break;
		case Curvature::CONE:
			s = "Cone curvature";
			break;
		case Curvature::DISTORTION_SIGNED:
			s = "Distortion";
			break;
		case Curvature::MEAN_SIGNED:
			s = "Mean curvature";
			break;
		case Curvature::MAX_PRINCIPAL_DISTORTION:
			s = "Max principal distortion";
			break;
		case Curvature::MIN_PRINCIPAL_DISTORTION:
			s = "Min principal distortion";
			break;
		case Curvature::FALSE_GAUSSIAN:
			s = "False Gaussian curvature";
			break;
		case Curvature::FALSE_MEAN:
			s = "False mean curvature";
			break;
		case Curvature::PRINCIPAL_DEVIATION:
			s = "Principal deviation";
			break;
	}
	return s;
}

Polyhedron* SubdivideMesh(Polyhedron* p, int n)
{
	std::vector<Polyhedron*> loops;
	Polyhedron* lp;
	lp = p;
	loops.push_back(lp);
	for (int i = 0; i < n; ++i)
	{
		Polyhedron* q;
		q = Subdivision::LoopSubdivisionHeap(loops[i]);
		q->Initialize();
		loops.push_back(q);
		delete(loops[i]);
	}
	return loops.back();
}

void LoadMeshFromFile(std::string fileName, int subdivisions, int meshIndex, glm::vec3 color)
{
	Polyhedron* p = new Polyhedron(fileName);
	p->Initialize();

	Polyhedron* lp = SubdivideMesh(p, subdivisions);

	mesh = MeshComponent(lp, color);

	delete(lp);
	//poly = lp;

	Loader::PrepareMesh(mesh);
	meshList[meshIndex].push_back(mesh);
}
void LoadMeshFromFile(std::string fileName, int subdivisions, std::vector<Curvature> curvatures)
{
	Polyhedron* p = new Polyhedron(fileName);
	p->Initialize();

	Polyhedron* lp = SubdivideMesh(p, subdivisions);

	// Currently hard-coded as up to four curvatures that can be displayed.
	for (int i = 0; i < 4; ++i)
	{
		if (i < curvatures.size())
		{
			std::vector<double> curvatureData = MeshAnalysis::GetVertexCurvatures(lp, curvatures[i]);
			meshList[i].push_back(MeshComponent(lp, curvatureData, curvatures[i]));
			Loader::PrepareMesh(meshList[i][0]);
		}
	}
	
	poly = lp;

	/*
	for (int i = 0; i < meshList.size() - 1; ++i)
	{
		Loader::PrepareMesh(meshList[i][0]);
	}
	*/
}
void LoadMeshFromFile(std::string fileName, int subdivisions)
{
	Polyhedron* p = new Polyhedron(fileName);
	p->Initialize();

	Polyhedron* lp = SubdivideMesh(p, subdivisions);

	std::cout << "**** Computing curvatures. ****" << std::endl;

	std::vector<double> gaussianCurvatures  = MeshAnalysis::GetVertexCurvatures(lp, Curvature::GAUSSIAN);
	std::cout << "Gaussian curvature. " << std::endl;

	std::vector<double> meanCurvatures	    = MeshAnalysis::GetVertexCurvatures(lp, Curvature::MEAN);
	std::cout << "Mean curvature. " << std::endl;

	std::vector<double> horizonMeasures     = MeshAnalysis::GetVertexCurvatures(lp, Curvature::HORIZON);
	std::cout << "Horizon measure. " << std::endl;

	std::vector<double> horizonMeasuresTest = MeshAnalysis::GetVertexCurvatures(lp, Curvature::ORIGINAL);
	std::cout << "Original horizon measure. " << std::endl;

	std::vector<double> distortion          = MeshAnalysis::GetVertexCurvatures(lp, Curvature::DISTORTION);
	std::cout << "Distortion. " << std::endl;

	std::vector<double> distortionSigned    = MeshAnalysis::GetVertexCurvatures(lp, Curvature::DISTORTION_SIGNED);
	std::cout << "Signed distortion. " << std::endl;

	std::vector<double> gaussianCone        = MeshAnalysis::GetVertexCurvatures(lp, Curvature::CONE);
	std::cout << "Gaussian cone. " << std::endl;
	
	std::vector<double> meanSigned			= MeshAnalysis::GetVertexCurvatures(lp, Curvature::MEAN_SIGNED);
	std::cout << "Signed mean curvature. " << std::endl;

	std::vector<double> minPrincipal		= MeshAnalysis::GetVertexCurvatures(lp, Curvature::MIN_PRINCIPAL_DISTORTION);
	std::cout << "Min principal curvature from distortion. " << std::endl;

	std::vector<double> maxPrincipal		= MeshAnalysis::GetVertexCurvatures(lp, Curvature::MAX_PRINCIPAL_DISTORTION);
	std::cout << "Max principal curvature from distortion. " << std::endl;

	std::vector<double> falseGaussian		= MeshAnalysis::GetVertexCurvatures(lp, Curvature::FALSE_GAUSSIAN);
	std::cout << "Gaussian curvature from distortion. " << std::endl;

	std::vector<double> falseMean			= MeshAnalysis::GetVertexCurvatures(lp, Curvature::FALSE_MEAN);
	std::cout << "Mean curvature from distortion. " << std::endl;


	meshList[0].push_back(MeshComponent(lp, meanSigned, Curvature::MEAN_SIGNED));
	meshList[1].push_back(MeshComponent(lp, falseMean, Curvature::FALSE_MEAN));
	//meshList[2].push_back(MeshComponent(lp, distortionSigned, Curvature::DISTORTION_SIGNED));
	meshList[2].push_back(MeshComponent(lp, falseGaussian, Curvature::FALSE_GAUSSIAN));
	meshList[3].push_back(MeshComponent(lp, gaussianCurvatures, Curvature::GAUSSIAN));
	
	poly = lp;
	//delete(lp);

	for (int i = 0; i < meshList.size() - 1; ++i)
	{
		Loader::PrepareMesh(meshList[i][0]);
	}
}
void LoadMeshFromFile(std::string fileName, int subdivisions, int meshIndex, Curvature curvature)
{
	Polyhedron* p = new Polyhedron(fileName);
	p->Initialize();

	Polyhedron* lp = SubdivideMesh(p, subdivisions);

	std::vector<double> triangleCurvatureData = MeshAnalysis::GetVertexCurvatures(lp, curvature);
	mesh = MeshComponent(lp, triangleCurvatureData, curvature);

	//delete(lp);
	poly = lp;

	Loader::PrepareMesh(mesh);
	meshList[meshIndex].push_back(mesh);
}


void InitLists()
{
	// Perspective Matrices:
	perspectiveMatrix = glm::perspective(glm::pi<float>() / 3.0f, (float)windowWidth / (float)windowHeight, near, far);
	lightPerspectiveMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 500.0f);

	// Shaders:
	shader = BasicShader();
	shader.Initialize();	

	lineShader = LineShader();
	lineShader.Initialize();

	// Mouse picker:
	mousePicker = MousePicker(windowWidth, windowHeight, perspectiveMatrix);

	// Mesh list:
	meshList.push_back(meshes);
	meshList.push_back(meshes);
	meshList.push_back(meshes);
	meshList.push_back(meshes);
	meshList.push_back(meshes);
	meshList.push_back(meshes);

	camera.position = glm::vec3(0, 0, 3.0f);
	lightPosition = camera.position;
	lightEye = camera.GetDirection();

	std::vector<Curvature> curvatures;
	curvatures.push_back(Curvature::MEAN_SIGNED);
	curvatures.push_back(Curvature::FALSE_MEAN);
	curvatures.push_back(Curvature::DISTORTION_SIGNED);
	curvatures.push_back(Curvature::HORIZON);
	LoadMeshFromFile("./tempmodels/bunny.ply", 0, curvatures);
	curvatureList = curvatures;

	std::cout << "Computed curvatures. " << std::endl;

	std::cout << "Attempting to compute principal directions. " << std::endl;
	std::vector<Edge*> principals = MeshAnalysis::GetPrincipalDirections(poly);
	std::vector<LineVertex> maxPrincipals;
	std::vector<LineVertex> minPrincipals;
	maxPrincipals.reserve(poly->vlist.size() * 2);
	minPrincipals.reserve(poly->vlist.size() * 2);
	for (int i = 0; i < principals.size(); i += 2)
	{
		// [i] is the max principal direction, [i+1] is the min principal direction.
		Edge* max = principals[i];
		Edge* min = principals[i + 1];

		glm::vec3 max0 = (glm::vec3)(max->vertices[0]->GetPosition());
		glm::vec3 max1 = (glm::vec3)(max->vertices[1]->GetPosition());
		glm::vec3 min0 = (glm::vec3)(min->vertices[0]->GetPosition());
		glm::vec3 min1 = (glm::vec3)(min->vertices[1]->GetPosition());
		maxPrincipals.push_back(LineVertex(max0, glm::vec3(1.0f, 0.0f, 0.0f)));
		maxPrincipals.push_back(LineVertex(max1, glm::vec3(1.0f, 0.0f, 0.0f)));
		minPrincipals.push_back(LineVertex(min0, glm::vec3(0.0f, 0.0f, 1.0f)));
		minPrincipals.push_back(LineVertex(min1, glm::vec3(0.0f, 0.0f, 1.0f)));
	}
	std::cout << maxPrincipals.size() << " " << minPrincipals.size() << std::endl;
	maxPrincipalDirections = CurveComponent(maxPrincipals);
	minPrincipalDirections = CurveComponent(minPrincipals);
	Loader::PrepareCurve(maxPrincipalDirections);
	Loader::PrepareCurve(minPrincipalDirections);

	// Load blank copy of mesh.
	MeshComponent blank = MeshComponent(poly, glm::vec3(0.9f, 0.9f, 0.9f));
	Loader::PrepareMesh(blank);
	std::cout << meshList.size() << std::endl;
	meshList[4] = std::vector<MeshComponent> { blank };

	// Load sphere for Gauss map visualization.
	LoadMeshFromFile("./tempmodels/sphere.ply", 0, 5, glm::vec3(0, 0.9f, 0.8f));
}


// Main loop: draw the scene.
void Display()
{
	// Parameters for the window we will draw to:
	glutSetWindow( mainWindow );
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// Line rendering settings.
	glEnable(GL_LINE_SMOOTH);


	/***** SCENE RENDERING *****/
	
	// Erase the background:
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Camera:
	viewMatrix = camera.GetViewMatrix();
	if (zoomScale < minScaleFactor)
	{
		zoomScale = minScaleFactor;
	}
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(zoomScale, zoomScale, zoomScale));
	glm::mat4 rotateX = glm::rotate(glm::mat4(1), rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1), rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = viewMatrix * rotateY * rotateX * scale;

	// Update the mouse picker to the new camera:
	mousePicker.UpdateViewMatrix(viewMatrix);

	// Render meshes.
	if (activeMesh >= 0 && activeMesh < meshList.size())
	{
		shader.Start();
		std::vector<MeshComponent>& activeMeshList = meshList[activeMesh];
		for (int i = 0; i < activeMeshList.size(); ++i)
		{
			glBindVertexArray(activeMeshList[i].getVAO());

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);

			shader.LoadProjectionMatrix(perspectiveMatrix);
			shader.LoadViewMatrix(viewMatrix);
			shader.LoadTransformMatrix(activeMeshList[i].transform);
			shader.LoadLighting(ambient, diffuse, specular, shininess, lightColor, lightPosition, camera.position);
			shader.LoadTexture(3);
			shader.LoadWireframe(enableWireframe);
			
			// Draw calls:
			glDrawElements(GL_TRIANGLES, activeMeshList[i].getCount(), GL_UNSIGNED_INT, nullptr);
		}
		shader.Stop();
	}
		
	// Render Gauss map.
	if (activeMesh == 5)
	{
		// Do not allow the select tool to be used while rendering Gauss map.
		selectTriangle = false;

		// Do not render principal directions
		renderMaxMinPrincipalDirection = 0;

		lineShader.Start();

		// Render Gauss map.
		glBindVertexArray(curve.getVAO());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		lineShader.LoadProjectionMatrix(perspectiveMatrix);
		lineShader.LoadViewMatrix(viewMatrix);
		lineShader.LoadTransformMatrix(curve.transform);

		glDrawArrays(GL_LINE_STRIP, 0, curve.getCount());

		// Render polar dual.
		glBindVertexArray(dualCurve.getVAO());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		lineShader.LoadProjectionMatrix(perspectiveMatrix);
		lineShader.LoadViewMatrix(viewMatrix);
		lineShader.LoadTransformMatrix(curve.transform);
		glDrawArrays(GL_LINE_STRIP, 0, dualCurve.getCount());

		lineShader.Stop();
	}

	// Render principal directions but not if we are rendering the Gauss map.
	if (renderMaxMinPrincipalDirection)
	{
		glLineWidth((GLfloat)2.0);

		// If value is 1, then render just max.
		lineShader.Start();
		if (renderMaxMinPrincipalDirection == 1)
		{
			glBindVertexArray(maxPrincipalDirections.getVAO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			lineShader.LoadProjectionMatrix(perspectiveMatrix);
			lineShader.LoadViewMatrix(viewMatrix);
			lineShader.LoadTransformMatrix(maxPrincipalDirections.transform);
			glDrawArrays(GL_LINES, 0, maxPrincipalDirections.getCount());
		}
		// If value is 2, then render just min.
		else if (renderMaxMinPrincipalDirection == 2)
		{
			glBindVertexArray(minPrincipalDirections.getVAO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			lineShader.LoadProjectionMatrix(perspectiveMatrix);
			lineShader.LoadViewMatrix(viewMatrix);
			lineShader.LoadTransformMatrix(minPrincipalDirections.transform);
			glDrawArrays(GL_LINES, 0, minPrincipalDirections.getCount());
		}
		// Render both.
		else
		{
			glBindVertexArray(maxPrincipalDirections.getVAO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			lineShader.LoadProjectionMatrix(perspectiveMatrix);
			lineShader.LoadViewMatrix(viewMatrix);
			lineShader.LoadTransformMatrix(maxPrincipalDirections.transform);
			glDrawArrays(GL_LINES, 0, maxPrincipalDirections.getCount());

			glBindVertexArray(minPrincipalDirections.getVAO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			lineShader.LoadProjectionMatrix(perspectiveMatrix);
			lineShader.LoadViewMatrix(viewMatrix);
			lineShader.LoadTransformMatrix(minPrincipalDirections.transform);
			glDrawArrays(GL_LINES, 0, minPrincipalDirections.getCount());
		}
		lineShader.Stop();
	}


	// Be sure the graphics buffer has been sent:
	// Note: be sure to use glFlush( ) here, not glFinish( ) !
	glutSwapBuffers( );
	glFlush( );
}


// Call when GLUT has nothing else to do - good for animation parameters.
void Animate()
{
	const int animationCycleMilliseconds = 10000;
	int ms = glutGet(GLUT_ELAPSED_TIME); // In milliseconds.
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	currentTime = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE; // [0, 1).

	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}





/*****************************************************************************/
/*****************************************************************************/
/*********************************** MENUS ***********************************/
/*****************************************************************************/
/*****************************************************************************/
void DoMainMenu(int id)
{
	switch( id )
	{
		case 0:
			Reset( );
			break;

		case 1:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( mainWindow );
			glFinish( );
			glutDestroyWindow( mainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}

void DoWireframeMenu(int id)
{
	enableWireframe = id;
}

void InitMenus()
{
	glutSetWindow(mainWindow);

	int wireframeMenu = glutCreateMenu(DoWireframeMenu);
	glutAddMenuEntry("Enable", 1);
	glutAddMenuEntry("Disable", 0);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Wireframe", wireframeMenu);
	glutAddMenuEntry("Reset", 0);
	glutAddMenuEntry("Quit", 1);

	// Have the menu appear when the right mouse button is clicked.
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}





/**********************************************************************************/
/**********************************************************************************/
/*********************************** USER INPUT ***********************************/
/**********************************************************************************/
/**********************************************************************************/
void Keyboard(unsigned char c, int x, int y)
{
	switch( c )
	{
		case 'c':
			lockCamera = !lockCamera;
			if (lockCamera)
				std::cout << "Camera locked." << std::endl;
			else
				std::cout << "Camera unlocked." << std::endl;
			break;

		// Camera motion.
		case 'a':
			if (!lockCamera)
				camera.StepHorizontal(-1);
			break;

		case 'd':
			if (!lockCamera)
				camera.StepHorizontal(1);
			break;

		case 's': 
			if (!lockCamera)
				camera.StepFacingDirection(-1);
			break;

		case 'w': 
			if (!lockCamera)
				camera.StepFacingDirection(1);
			break;

		case 'g': 
			if (!lockCamera)
				camera.roll += 0.2f;
			break;

		// Mesh selection.
		case '1':
			activeMesh = 0;
			std::cout << "Visualizing the " << ToString(curvatureList[0]) << ". " << std::endl;
			break;

		case '2':
			if (2 <= meshList.size())
			{
				activeMesh = 1;
				std::cout << "Visualizing the " << ToString(curvatureList[1]) << ". " << std::endl;
			}
			break;

		case '3':
			if (3 <= meshList.size())
			{
				activeMesh = 2;
				std::cout << "Visualizing the " << ToString(curvatureList[2]) << ". " << std::endl;
			}
			break;

		case '4':
			if (4 <= meshList.size())
			{
				activeMesh = 3;
				std::cout << "Visualizing the " << ToString(curvatureList[3]) << ". " << std::endl;
			}
			break;

		case '5':
			{
				activeMesh = 5;
				std::cout << "Visualizing the Gauss map (blue) and polar dual (red). " << std::endl;
			}
			break;

		// Draw blank mesh.
		case '0':
			{
				activeMesh = 4;
			}
			break;

		case 'r':
			Reset();
			break;

		case 'f':
			animate = !animate;
			if (animate)
				glutIdleFunc(Animate);
			else
				glutIdleFunc(NULL);
			break;

		case 'q':
			DoMainMenu(1);	// will not return here
			break;				// happy compiler

		case 'm':
			renderMaxMinPrincipalDirection = 1;
			break;

		case 'n':
			renderMaxMinPrincipalDirection = 2;
			break;

		case 'b':
			renderMaxMinPrincipalDirection = 3;
			break;

		case 'v':
			renderMaxMinPrincipalDirection = 0;
			break;

		// Triangle select tool.
		case 't':
			selectTriangle = !selectTriangle;
			if (selectTriangle)
				std::cout << "Select tool active. " << std::endl;
			else
				std::cout << "Select tool off." << std::endl;
			break;

		//default:
			//fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// Force a call to Display( ):
	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}

void InfoDumpSelectedTriangle(uint activeMeshID, uint meshIndex, uint triangleIndex, uint v0, uint v1, uint v2)
{
	Vertex& w0 = meshList[meshIndex][meshIndex].getVertices()[v0];
	Vertex& w1 = meshList[meshIndex][meshIndex].getVertices()[v1];
	Vertex& w2 = meshList[meshIndex][meshIndex].getVertices()[v2];

	//float perimeter = MeshAnalysis::ComputePerimeter(w0, w1, w2);
	//float horizonArea = MeshAnalysis::ComputeHorizonArea(w0, w1, w2);

	std::cout << std::endl;
	std::cout << "Triangle #" << triangleIndex << " selected on mesh #" << meshIndex << std::endl;
	std::cout << "Position vectors: " << std::endl;
	std::cout << glm::to_string(w0.getPosition()) << std::endl;
	std::cout << glm::to_string(w1.getPosition()) << std::endl;
	std::cout << glm::to_string(w2.getPosition()) << std::endl;
	std::cout << "Normal vectors: " << std::endl;
	std::cout << glm::to_string(w0.getNormal()) << std::endl;
	std::cout << glm::to_string(w1.getNormal()) << std::endl;
	std::cout << glm::to_string(w2.getNormal()) << std::endl;
	//std::cout << "Triangle perimeter: " << perimeter << std::endl;
	//std::cout << "Horizon area: " << horizonArea << std::endl;
	//std::cout << "Horizon measure: " << horizonArea / perimeter << std::endl;;
	std::cout << std::endl;
}

void SetHighlight(uint activeMeshID, uint meshID, uint vertexIndex, glm::vec4 color)
{
	MeshComponent& mesh = meshList[activeMeshID][meshID];
	Loader::UpdateHighlight(mesh.getVBO(), vertexIndex, color);
	//InfoDumpSelectedTriangle(activeMeshID, meshID, triangleIndex, v0, v1, v2);
}

void MouseRayTriangleIntersection(glm::vec3& ray)
{
	// We have to make sure that the ray starts in the right place: bring the starting point of the ray to the correct camera point.
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(zoomScale, zoomScale, zoomScale));
	glm::mat4 rotateX = glm::rotate(glm::mat4(1), rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1), rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 cameraEyePosition = glm::vec4(camera.position.x, camera.position.y, camera.position.z, 1);
	cameraEyePosition = glm::inverse(scale) * glm::inverse(rotateX) * glm::inverse(rotateY) * cameraEyePosition;
	//cameraEyePosition = glm::inverse(rotateX) * glm::inverse(rotateY) * cameraEyePosition;
	glm::vec3 eyePos = glm::vec4(cameraEyePosition);

	// Check if the ray intersects any triangle from the active mesh.
	// meshIndex is the index of the mesh piece within the active mesh.
	// index is the triangle index of the found intersection.
	int meshIndex = -1;
	int index = -1;
	glm::vec2 position;
	float distance;
	float min = std::numeric_limits<float>::max();
	glm::vec2 baryIntersect;

	// Check for each mesh within the active mesh list.
	std::vector<MeshComponent>& activeMeshList = meshList[activeMesh];
	for (int j = 0; j < activeMeshList.size(); ++j)
	{
		std::vector<uint>& triangles = activeMeshList[j].getTriangles();
		std::vector<Vertex>& vertices = activeMeshList[j].getVertices();
		for (int i = 0; i < triangles.size(); i += 3)
		{
			// Get the vertices of the triangle.
			glm::vec3 v0 = vertices[triangles[i + 0]].getPosition();
			glm::vec3 v1 = vertices[triangles[i + 1]].getPosition();
			glm::vec3 v2 = vertices[triangles[i + 2]].getPosition();

			bool intersection = glm::intersectRayTriangle(eyePos, ray, v0, v1, v2, position, distance);
			if (intersection)
			{
				// Identify the closest intersection.
				if (distance < min)
				{
					meshIndex = j;
					index = i;
					min = distance;
					baryIntersect = position;
				}
			}
		}
	}

	if (index == -1)
	{
		std::cout << "No intersection found. " << std::endl;
	}
	else
	{
		// Determine which point was closest to the intersection using the barycentric coordinates.
		// By experiment, the 2D position (b, c) means a + b + c = 1, where a is determined by a = 1 - b - c:
		// (c, a, b) correspond to the vertices given in order to the glm function.
		float a = baryIntersect.x;
		float b = baryIntersect.y;
		float c = 1.0f - a - b;

		uint selectedVertex;
		std::vector<uint>& triangles = activeMeshList[meshIndex].getTriangles();
		if (a > b && a > c)
			selectedVertex = triangles[index + 1];
		else if (b > a && b > c)
			selectedVertex = triangles[index + 2];
		else
			selectedVertex = triangles[index + 0];

		Vert* vertex = &poly->vlist[selectedVertex];

		/*
		std::cout << std::endl;
		std::cout << "Intersection at vertex " << selectedVertex << " in triangle " << index << std::endl;
		*/
		SetHighlight(activeMesh, meshIndex, selectedVertex, glm::vec4(1.0f, 215.0f / 255.0f, 0.0f, 1.0f));

		// Calculate the curvature values at this vertex that are being visualized.
		for (int i = 0; i < curvatureList.size(); ++i)
		{
			Curvature c = curvatureList[i];
			double value = MeshAnalysis::GetVertexCurvature(vertex, c);
			std::cout << "The " << ToString(c) << " of the selected vertex is " << value << std::endl;
		}

		// Get Gauss map.
		std::vector<Triangle*> star = MeshAnalysis::GetVertexStar(&(poly->vlist[selectedVertex]));
		std::vector<glm::vec3> gaussMap = Spherical::GetGaussMap(star, 200, 1.001f);

		// Get polar dual to the Gauss map.
		std::vector<glm::vec3> normals;
		normals.reserve(star.size());
		for (Triangle* t : star)
		{
			normals.push_back((glm::vec3)t->normal);
		}
		std::vector<glm::vec3> dualPolygon = Spherical::GetPolarPolygon(normals);
		//dualPolygon = Spherical::GetPolarPolygon(dualPolygon);
		std::vector<glm::vec3> dualGaussMap = Spherical::GetGaussMap(dualPolygon, 200, 1.001f);

		std::cout << "Found " << star.size() << " elements in the vertex star: " << std::endl;
		/*
		for (int i = 0; i < star.size(); ++i)
		{
			star[i]->Print();
		}
		std::cout << std::endl;
		*/

		// Create curve: boundary of the Gauss map of the star of the vertex and the polar dual.
		std::vector<LineVertex> curveVertices;
		curveVertices.reserve(gaussMap.size());
		std::vector<LineVertex> polarVertices;
		polarVertices.reserve(dualGaussMap.size());

		glm::vec3 gaussColor = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 polarColor = glm::vec3(1.0f, 0.0f, 0.0f);
		for (int i = 0; i < gaussMap.size(); ++i)
		{
			curveVertices.push_back(LineVertex(gaussMap[i], gaussColor - glm::vec3(0.0f, 0.0f, (float)i / gaussMap.size())));
			polarVertices.push_back(LineVertex(dualGaussMap[i], polarColor - glm::vec3((float)i / gaussMap.size(), 0.0f, 0.0f)));
		}
		CurveComponent sphericalImage(curveVertices);
		CurveComponent polarSphericalImage(polarVertices);
		curve = sphericalImage;
		dualCurve = polarSphericalImage;

		Loader::PrepareCurve(curve);
		Loader::PrepareCurve(dualCurve);
	}
}


// When the mouse button transitions down or up:
void MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// Get the proper button bit mask:
	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = leftMouseButton;		break;

		case GLUT_MIDDLE_BUTTON:
			b = middleMouseButton;		break;

		case GLUT_RIGHT_BUTTON:
			b = rightMouseButton;		break;

		case scrollWheelUp:
			if (lockCamera)
				zoomScale += minScaleFactor * scrollWheelClickFactor;
			else
				camera.StepUp(1);
			// keep object from turning inside-out or disappearing:
			if (zoomScale < minScaleFactor)
				zoomScale = minScaleFactor;
			break;

		case scrollWheelDown:
			if (lockCamera)
				zoomScale -= minScaleFactor * scrollWheelClickFactor;
			else
				camera.StepUp(-1);
			// keep object from turning inside-out or disappearing:
			if (zoomScale < minScaleFactor)
				zoomScale = minScaleFactor;
			break;

		default:
			b = 0;
			//fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		mouseX = x;
		mouseY = y;
		activeMouseButton |= b;		// set the proper bit
	}
	else
	{
		activeMouseButton &= ~b;		// clear the proper bit
	}

	// Update the mouse picker only on a LEFT CLICK... or else lag everything to death when zooming in/out.
	// Also, only do this while holding down the selectTriangle key (currently set to t).
	if (selectTriangle && b == leftMouseButton && state == GLUT_DOWN)
	{
		mousePicker.setMouseCoordinates(x, y);
		glm::vec3 ray = mousePicker.ComputeRay(meshes[0].transform);
		mousePicker.UpdateViewMatrix(viewMatrix);
		//std::cout << x << " " << y << "--> " << ray.x << " " << ray.y << " " << ray.z << std::endl;
		MouseRayTriangleIntersection(ray);
	}

	glutSetWindow(mainWindow);
	glutPostRedisplay();

}


// When the mouse moves while a button is down:
void MouseMotion(int x, int y)
{
	int dx = x - mouseX;		// change in mouse coords
	int dy = y - mouseY;

	if( ( activeMouseButton & leftMouseButton ) != 0 )
	{
		if (lockCamera)
		{
			rotationX += ( rotationFactor*dy );
			rotationY += ( rotationFactor*dx );
		}
		if (!lockCamera)
		{
			camera.pitch += rotationFactor * dy;
			camera.yaw += rotationFactor * dx;
		}
		/*
		camera.pitch = rotationX;
		camera.yaw = rotationY;
		*/
	}


	if( ( activeMouseButton & middleMouseButton ) != 0 )
	{
		zoomScale += minScaleFactor * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( zoomScale < minScaleFactor )
			zoomScale = minScaleFactor;
	}

	mouseX = x;			// new current position
	mouseY = y;

	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}


// Set variables to default state: call this when the program starts!
void Reset()
{
	activeMouseButton = 0;
	zoomScale  = 1.0;
	rotationX = rotationY = 0.;
	camera.pitch = 0;
	camera.yaw = 0;
	camera.position = glm::vec3(0, 0, 3);
	ambient = AMBIENT;
	diffuse = DIFFUSE;
	specular = SPECULAR;
}


// When the window is resized.
void Resize(int width, int height)
{
	// Don't really need to do anything since window size is checked each frame in Display().

	glutSetWindow(mainWindow);
	glutPostRedisplay();
}


// When the visibility of the window is changed.
void Visibility(int state)
{
	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( mainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// Could optimize by keeping track of the fact that the window is not visible and avoid animating/redrawing.
	}
}





/*******************************************************************************/
/*******************************************************************************/
/*********************************** FILE IO ***********************************/
/*******************************************************************************/
/*******************************************************************************/
void InitializeTexture(char* file, int width, int height)
{
	// Load a texture from file:
	unsigned char* texture;
	texture = BmpToTexture(file, &width, &height);

	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

// Read a BMP file into a Texture:
unsigned char* BmpToTexture(char *filename, int* width, int* height)
{
	FILE *fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not 0x4d42, the file is not a bmp:

	if( FileHeader.bfType != 0x4d42 )
	{
		fprintf( stderr, "File '%s' is the wrong type of file: 0x%0x\n", filename, FileHeader.bfType );
		fclose( fp );
		return NULL;
	}

	FileHeader.bfSize = ReadInt( fp );
	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );
	FileHeader.bfOffBits = ReadInt( fp );

	InfoHeader.biSize = ReadInt( fp );
	InfoHeader.biWidth = ReadInt( fp );
	InfoHeader.biHeight = ReadInt( fp );

	int nums = InfoHeader.biWidth;
	int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	InfoHeader.biBitCount = ReadShort( fp );
	InfoHeader.biCompression = ReadInt( fp );
	InfoHeader.biSizeImage = ReadInt( fp );
	InfoHeader.biXPelsPerMeter = ReadInt( fp );
	InfoHeader.biYPelsPerMeter = ReadInt( fp );
	InfoHeader.biClrUsed = ReadInt( fp );
	InfoHeader.biClrImportant = ReadInt( fp );

	fprintf( stderr, "Image size in file '%s' is: %d x %d\n", filename, nums, numt );

	unsigned char * texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\b" );
		return NULL;
	}

	// extra padding bytes:

	int numextra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;

	// we do not support compression:

	if( InfoHeader.biCompression != birgb )
	{
		fprintf( stderr, "Image file '%s' has the wrong type of image compression: %d\n", filename, InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}

	rewind( fp );
	fseek( fp, 14+40, SEEK_SET );

	if( InfoHeader.biBitCount == 24 )
	{
		unsigned char *tp = texture;
		for( int t = 0; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numextra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int ReadInt(FILE* fp)
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	b2 = fgetc( fp );
	b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short ReadShort(FILE* fp)
{
	unsigned char b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}
