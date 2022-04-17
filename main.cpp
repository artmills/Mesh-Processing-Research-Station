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
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/intersect.hpp"

#include "vertex.hpp"
#include "meshcomponent.hpp"
#include "loader.hpp"
#include "basicshader.hpp"
#include "shadowshader.hpp"
#include "polyhedron.hpp"
#include "meshanalysis.hpp"
#include "subdivision.hpp"
#include "meshfactory.hpp"
#include "mousepicker.hpp"
#include "camera.hpp"
#include "perlinnoise.hpp"
#include "terrainfactory.hpp"





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
void InitializeShadows(int width, int height);
Polyhedron* SubdivideMesh(Polyhedron* p, int n);
void LoadMeshFromFile(int subdivisions, int meshIndex);
void LoadUniformSphereMesh(float length, uint numPointsPerSide, int meshIndex);

std::vector<Vertex> ComputeTerrainVertices(int xNumTiles, int zNumTiles, float xTileSize, float yTileSize, int octaves, float persistence, float lacunarity);
std::vector<uint> TriangulateVertices(std::vector<Vertex>& vertices, float xNumTiles, float zNumTiles);
void ComputeMeshNormals(std::vector<Vertex>& vertices, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize);


/*********************************************************************************/
/*********************************************************************************/
/*********************************** VARIABLES ***********************************/
/*********************************************************************************/
/*********************************************************************************/

// Objects to draw:
MeshComponent mesh;
std::vector<MeshComponent> meshes;

// Mesh selection:
std::vector<std::vector<MeshComponent>> meshList;
uint activeMesh = 0;

// Shaders:
BasicShader shader;
ShadowShader shadowShader;

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


std::vector<Vertex> ComputeTerrainVertices(int xNumTiles, int zNumTiles, float xTileSize, float zTileSize, int octaves, float persistence, float lacunarity)
{
	PerlinNoise pn;
	float noiseAmplitude = 1.0f;
	float noiseFactor = 1.0f;
	//float noiseAmplitude = 80.0f;
	//float noiseFactor = 5.27f;

	// Biomes:
	//GridComponent biomeMap(xNumTiles + 1, zNumTiles + 1);
	//TerrainFactory::GenerateBiomes(biomeMap, 15);
	//std::vector<std::vector<float>> heightMap = TerrainFactory::GetHeightMap(biomeMap, xNumTiles, zNumTiles, xTileSize, zTileSize, octaves, persistence);
	//heightMap = TerrainFactory::SmoothHeightMap(heightMap, biomeMap, 6, xNumTiles, zNumTiles, xTileSize, zTileSize, octaves, persistence);

	glm::vec4 color;
	glm::vec4 groundColor1(0.3f, 0.44f, 0.0f, 1.0f);
	glm::vec4 groundColor2(0.38f, 0.48f, 0.0f, 1.0f);
	glm::vec4 stoneColor(0.6f, 0.6f, 0.6f, 1.0f);
	glm::vec4 mountainColor(1, 1, 1, 1);
	float mountainHeight = 18.0f;
	float groundHeight = 1;

	std::vector<Vertex> vertices;

	float width = xNumTiles * xTileSize;
	float height = zNumTiles * zTileSize;

	float min = 1000;
	float max = -1000;

	fMap heightMap = TerrainFactory::GetHeightMap(xNumTiles, zNumTiles, xTileSize, zTileSize, octaves, persistence, lacunarity);

	// Compute bottom row of vertices:
	for (int j = 0; j <= zNumTiles; ++j)
	{
		for (int i = 0; i <= xNumTiles; ++i)
		{
			float xCoord = (float)i * xTileSize;
			float zCoord = (float)j * zTileSize;
			float yCoord = heightMap[i][j];
			if (yCoord < min)
				min = yCoord;
			if (yCoord > max)
				max = yCoord;

			if (yCoord >= mountainHeight)
			{
				color = mountainColor;
			}
			else if (yCoord >= groundHeight)
			{
				color = stoneColor;
			}
			else
			{
				if (i % 2 == 0 && j % 2 == 0)
					color = groundColor1;
				else
					color = groundColor2;
			}

			Vertex v(xCoord, yCoord, zCoord, color);
			vertices.push_back(v);
		}
	}
	std::cout << min << " " << max << std::endl;

	return vertices;
}

// Assumes that the vertices are ordered using the ComputeTerrainVertices() function.
std::vector<uint> TriangulateVertices(std::vector<Vertex>& vertices, float xNumTiles, float zNumTiles)
{
	std::vector<uint> triangles;
	uint step = xNumTiles + 1;
	for (uint j = 0; j < vertices.size() - step; j += step)
	{
		for (uint i = 0; i < xNumTiles; ++i)
		{
			uint botLeft = i + j;
			uint botRight = botLeft + 1;
			uint topRight = botRight + step;
			uint topLeft = botLeft + step;

			triangles.push_back(botLeft);
			triangles.push_back(botRight);
			triangles.push_back(topRight);
			triangles.push_back(botLeft);
			triangles.push_back(topRight);
			triangles.push_back(topLeft);
		}
	}
	return triangles;	
}

void ComputeMeshNormals(std::vector<Vertex>& vertices, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize)
{
	uint step = xNumTiles + 1;
	float yUp = 0;
	float yUpRight = 0;
	float yRight = 0;
	float yDown = 0;
	float yDownLeft = 0;
	float yLeft = 0;
	// Compute a normal vector for each vertex. See https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh .
	for (int i = 0; i < vertices.size(); ++i)
	{
		// Get the adjacent vertex indices:
		int up = i + step;
		int upRight = up + 1;
		int right = i + 1;
		int down = i - step;
		int downLeft = down - 1;
		int left = i - 1;

		// Now get the y-coordinates of the neighboring vertices.
		if (up >= 0 && up < vertices.size())
			yUp = vertices[up].y;

		if (upRight >= 0 && upRight < vertices.size())
			yUpRight = vertices[upRight].y;

		if (right >= 0 && right < vertices.size())
			yRight = vertices[right].y;

		if (down >= 0 && down < vertices.size())
			yDown = vertices[down].y;

		if (downLeft >= 0 && downLeft < vertices.size())
			yDownLeft = vertices[downLeft].y;

		if (left >= 0 && left < vertices.size())
			yLeft = vertices[left].y;


		float xNormal = -(yDown - 2 * yLeft - yDownLeft + 2 * yRight - yUp + yUpRight) / xTileSize;
		float yNormal = 6;
		float zNormal = (2 * yDown - yLeft + yDownLeft + yRight - 2 * yUp - yUpRight) / zTileSize;

		glm::vec3 normal = glm::normalize(glm::vec3(xNormal, yNormal, zNormal));
		vertices[i].setNormal(normal.x, normal.y, normal.z);
	}
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

void LoadMeshFromFile(int subdivisions, int meshIndex)
{
	Polyhedron* p = new Polyhedron("./tempmodels/bunny1.ply");
	//Polyhedron* p = new Polyhedron("./tempmodels/bunny.ply");
	//Polyhedron* p = new Polyhedron("./tempmodels/Crane/spot/spot_triangulated.obj", 0);
	p->Initialize();

	Polyhedron* lp = SubdivideMesh(p, subdivisions);

	std::vector<double> horizons = MeshAnalysis::GetHorizonMeasuresDouble(lp->tlist);
	//std::vector<double> horizons = MeshAnalysis::GetApproximateGaussianCurvatures(lp->tlist);
	//std::vector<double> horizons = MeshAnalysis::GetOriginalHorizonMeasuresDouble(lp->tlist);
	mesh = MeshComponent(lp, horizons);

	delete(lp);
	Loader::PrepareMesh(mesh);
	//meshes.push_back(mesh);
	//meshList[0].push_back(meshes);
	meshList[meshIndex].push_back(mesh);
}

void LoadUniformSphereMesh(float length, uint numPointsPerSide, int meshIndex)
{
	mesh = MeshFactory::GetSphereTriangles(1.0f, numPointsPerSide);
	std::vector<float> triangleHorizons;
	std::vector<Vertex>& vertices = mesh.getVertices();
	for (int i = 0; i < vertices.size(); i += 3)
	{
		Vertex& v0 = vertices[i];
		Vertex& v1 = vertices[i+1];
		Vertex& v2 = vertices[i+2];
		triangleHorizons.push_back(MeshAnalysis::ComputeHorizonMeasure(v0, v1, v2));
	}
	mesh.AssignHorizonMeasureColors(triangleHorizons);
	Loader::PrepareMesh(mesh);
	//meshes.push_back(mesh);
	meshList[meshIndex].push_back(mesh);
}


void InitLists()
{
	// Perspective Matrices:
	perspectiveMatrix = glm::perspective(glm::pi<float>() / 3.0f, (float)windowWidth / (float)windowHeight, near, far);
	lightPerspectiveMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 500.0f);

	// Test Shader:
	shader = BasicShader();
	shader.Initialize();	

	// Shadow Shader:
	shadowShader = ShadowShader();
	shadowShader.Initialize();

	// Mouse picker:
	mousePicker = MousePicker(windowWidth, windowHeight, perspectiveMatrix);

	// Mesh list:
	meshList.push_back(meshes);
	meshList.push_back(meshes);

	camera.position = glm::vec3(0, 0, 3.0f);
	lightPosition = camera.position;
	lightEye = camera.GetDirection();

	//Polyhedron* p = new Polyhedron("./tempmodels/Crane/CrumpledDevelopable.obj", 0);
	/*
	Polyhedron* p = new Polyhedron("./tempmodels/bunny1.ply");
	//Polyhedron* p = new Polyhedron("./tempmodels/bunny.ply");
	p->Initialize();
	mesh = MeshComponent(p);
	Loader::PrepareMesh(mesh);
	meshes.push_back(mesh);
	*/

	LoadMeshFromFile(0, 0);
	LoadUniformSphereMesh(1, 100, 1);
	
	// Terrain Test:
	/*
	int xTiles = 1000;
	int zTiles = 1000;
	float xTileSize = 1.0f;
	float zTileSize = 1.0f;
	int octaves = 4;
	float persistence = 0.7f;
	float lacunarity = 2.2f;
	std::vector<Vertex> vertices = TerrainFactory::GetTerrainVertices(xTiles, zTiles, xTileSize, zTileSize, octaves, persistence, lacunarity);
	std::vector<uint> triangles = TriangulateVertices(vertices, xTiles, zTiles);
	ComputeMeshNormals(vertices, xTiles, zTiles, xTileSize, zTileSize);
	mesh = MeshComponent(vertices, triangles);
	glm::vec3 middle = glm::vec3((float)xTiles * xTileSize * 0.5f, 0.0f, (float)zTiles * zTileSize * 0.5f);
	mesh.transform = glm::translate(glm::mat4(1), -middle);
	*/

	/*
	// Compute horizon measure of terrain.
	std::vector<float> triangleHorizons(triangles.size());
	for (int i = 0; i < triangles.size(); i += 3)
	{
		int triangle0 = triangles[i];
		int triangle1 = triangles[i+1];
		int triangle2 = triangles[i+2];
		Vertex& v0 = vertices[triangle0];
		Vertex& v1 = vertices[triangle1];
		Vertex& v2 = vertices[triangle2];
		triangleHorizons[i] = MeshAnalysis::ComputeHorizonMeasure(v0, v1, v2);
	}
	std::cout << triangles.size() << std::endl;
	mesh.AssignHorizonMeasureColors(triangleHorizons);
	*/

	/*
	Loader::PrepareMesh(mesh);
	meshes.push_back(mesh);

	camera.position = glm::vec3(0, 150, middle.z);
	*/
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


	/***** SCENE RENDERING *****/
	
	// Erase the background:
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Camera:
	viewMatrix = camera.GetViewMatrix();
	//glm::mat4 cameraMatrix = glm::lookAt(camera.position, camera.GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
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

	// Activate the shader:
	shader.Start();

	//for (int i = 0; i < meshes.size(); ++i)
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
		//glDrawArrays(GL_TRIANGLES, 0, meshes[i].getCount());
	}
	/*
	for (int i = 0; i < meshes.size(); ++i)
	{
		glBindVertexArray(meshes[i].getVAO());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		shader.LoadProjectionMatrix(perspectiveMatrix);
		shader.LoadViewMatrix(viewMatrix);
		shader.LoadTransformMatrix(meshes[i].transform);
		shader.LoadLighting(ambient, diffuse, specular, shininess, lightColor, lightPosition, camera.position);
		shader.LoadTexture(3);
		shader.LoadWireframe(enableWireframe);
		
		// Draw calls:
		glDrawElements(GL_TRIANGLES, meshes[i].getCount(), GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_TRIANGLES, 0, meshes[i].getCount());
	}
	*/

	shader.Stop();


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

		case '1':
			activeMesh = 0;
			break;

		case '2':
			if (2 <= meshList.size())
				activeMesh = 1;
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

		case 't':
			selectTriangle = !selectTriangle;
			if (selectTriangle)
				std::cout << "Select tool active. " << std::endl;
			else
				std::cout << "Select tool off." << std::endl;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// Force a call to Display( ):
	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}

void InfoDumpSelectedTriangle(uint meshIndex, uint triangleIndex, uint v0, uint v1, uint v2)
{
	Vertex& w0 = meshes[meshIndex].getVertices()[v0];
	Vertex& w1 = meshes[meshIndex].getVertices()[v1];
	Vertex& w2 = meshes[meshIndex].getVertices()[v2];

	float perimeter = MeshAnalysis::ComputePerimeter(w0, w1, w2);
	float horizonArea = MeshAnalysis::ComputeHorizonArea(w0, w1, w2);

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
	std::cout << "Triangle perimeter: " << perimeter << std::endl;
	std::cout << "Horizon area: " << horizonArea << std::endl;
	std::cout << "Horizon measure: " << horizonArea / perimeter << std::endl;;
	std::cout << std::endl;
}

void SetHighlight(uint meshID, uint triangleIndex, glm::vec4 color)
{
	MeshComponent& mesh = meshes[meshID];
	uint v0 = mesh.getTriangles()[triangleIndex + 0];
	uint v1 = mesh.getTriangles()[triangleIndex + 1];
	uint v2 = mesh.getTriangles()[triangleIndex + 2];
	Loader::UpdateHighlight(mesh.getVBO(), v0, v1, v2, color);
	InfoDumpSelectedTriangle(meshID, triangleIndex, v0, v1, v2);
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

	// Check if the ray intersects any triangle from any mesh.
	int meshIndex = -1;
	int index = -1;
	glm::vec2 position;
	float distance;
	float min = std::numeric_limits<float>::max();
	for (int j = 0; j < meshes.size(); ++j)
	{
		std::vector<uint>& triangles = mesh.getTriangles();
		std::vector<Vertex>& vertices = mesh.getVertices();
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
				}
			}
		}
	}
	if (meshIndex > -1 && index > -1)
		SetHighlight(meshIndex, index, glm::vec4(1.0f, 215.0f / 255.0f, 0.0f, 1.0f));
	/*
	if (index == -1)
	{
		std::cout << "No intersection found. " << std::endl;
	}
	else
	{
		std::cout << "Intersection with triangle at index " << index << std::endl;
		std::cout << "Highlighting mesh " << meshIndex << std::endl;
		SetHighlight(meshIndex, index, glm::vec4(1.0f, 215.0f / 255.0f, 0.0f, 1.0f));
	}
	*/
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
			fprintf( stderr, "Unknown mouse button: %d\n", button );
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
