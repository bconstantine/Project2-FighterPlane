#include "include_header.h" 
//rendering fps
#define TARGET_FPS 30


//degree and radiant conversion if needed
#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))
#define rad2deg(x) ((180.0f) / ((x)*(3.1415926f)))

//body parts number
//#define EXTRAPARTS 2
//#define PARTSNUM 19
#define EXTRAPARTS 0
#define EXTRAPARTSDEFECT 2
#define PARTSNUM 1
#define PARTSTOTAL PARTSNUM+EXTRAPARTS

//for fire
#define FIREPARTICLESIZE 0.1
#define PARTICLESPERFIRE 100
#define FIRERADIUS 2.0
#define FIRELIFETIME 3 //in second
#define FIREUPAMIN 0.1 //verticle up acceleration minimum
#define FIREUPAMAX 0.5
#define FIREROTATEXMAX 2 //in degree, for fire up acceleration displacement
#define FIREROTATEXMIN -2 //in degree, for fire up acceleration displacement
#define FIREROTATEZMAX 2 //in degree, for fire up acceleration displacement
#define FIREROTATEZMIN -2 //in degree, for fire up acceleration displacement

//for walking purposes
#define WALKSPEED 0.3
#define TRANSLATEXLIMIT 10 //preventing out of bound
#define TRANSLATEZLIMIT 10 //preventing out of bound
#define TRANSLATEYLOWERLIMIT 20.0 //for lighting
#define TRANSLATEYUPPERLIMIT 30.0 //for lighting



//background usage
bool useBackground = false;
glm::mat4 ModelBackground;
float backGroundShiftUp = 30.02f;
float robotShiftUp = 2; //height of from crotch to ankle + height of foot

//for imgui usage
int SCREENWIDTH; //obtained later during init
int SCREENHEIGHT; //obtained later during init
const char* glsl_version = "#version 130";
bool show_demo_window = true;
ImFont* font1 = NULL;
ImFont* titleFont = NULL;
ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
void makeCenter(std::string text) {
	float font_size = ImGui::GetFontSize() * text.size() / 2;
	ImGui::SameLine(
		ImGui::GetWindowSize().x / 2 -
		font_size + (font_size / 2)
	);

	ImGui::Text(text.c_str());
}

#define WINGXPOSITION 5
#define WINGYPOSITION 0
#define WINGZPOSITION -10
#define FUSELAGEXPOSITION 0
#define FUSELAGEYPOSITION 0
#define FUSELAGEZPOSITION 10
#define FUSELAGEEXHAUSTYPOSITION 3.5
#define FUSELAGEEXHAUSTZPOSITION -17
glm::vec3 blueColor = glm::vec3(43.0/255.0,124.0/255.0,255/255.0);
FireBillboardObject* fireGenerate;
AuraBillboardObject* blueAura;
AuraBillboardObject* wing1Aura;
AuraBillboardObject* wing2Aura;
AuraBillboardObject* fuselageAura;
SmokeBurnerObject* centralSmoke;
ExplodingObject* explodingObject;
HorizontalSmokeObject* thrusterSmoke;

WaterQuad* waterPlatform;

bool modeChangeExist = false;
void resetGlobalVariables();

enum PLANEMODE {
	NORMAL,
	REFLECT,
	REFRACT, 
	AURA,
	ONFIRE,
	FLYNEARWATER, 
	ACTION, 
	TOONSHADER, 
	MOZAIC,
	SHADERMATERIAL
};

enum PLANEMODE planeMode = NORMAL;

void myUpdateModel();

//for using with GLFW
GLFWwindow* initProgramGLFW();
void resetModel();
void displayOnly(bool isShadow, bool isReflect, glm::vec4 clipPlane, bool isDepthPlane); //render scene only, no GUI
void displayGLFW(GLFWwindow* window); //displayGLFW()
void displayInitial(int currentIndices, GLFWwindow* window); //only for initial, displaying loading progress
bool loadingProgress[PARTSTOTAL+1]; //parts total is not including background, + 1 to include background
void KeyboardGLFW(GLFWwindow*, int key, int scancode, int action, int mods);
//for GLFW
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void Obj2Buffer(GLFWwindow* window);
void load2Buffer( string obj,int);
float clip(float &var, float min, float max);

bool isFrame;

GLuint VAO; //vertex attribute
GLuint VBO; //buffer for vertices
GLuint uVBO; //buffer for uv
GLuint nVBO; //buffer for normal
GLuint tangentVBO;
GLuint bitangVBO;
GLuint mVBO; //
GLuint VBOs[PARTSTOTAL+1];
GLuint uVBOs[PARTSTOTAL+1];
GLuint nVBOs[PARTSTOTAL+1];
GLuint tangVBOs[PARTSTOTAL + 1];
GLuint bitangVBOs[PARTSTOTAL + 1];

//GLuint program;
GLuint program;
GLint MatricesIdx; //Matvp
GLuint ModelID; //Model
GLuint lightPosID; //vec3 of vLightPosition
//for material
GLuint M_KaID;
GLuint M_KdID;
GLuint M_KsID;
//two viewPos vector, different just in case if we need to see from reflection point of view
GLuint varyingViewPosID; 
GLuint permanentViewPosID; 
GLuint clipPlaneID; 
GLuint lightSpaceOrigID; //for passing in phong vertex shader, same with lightSpaceID, just different program
GLuint UBO; //UBO for loading MaTvp
//for cubemap and environmental mapping
GLuint cubemapID;
GLuint cubemapEnumerationID; //for main shader connection
GLuint depthmapEnumerationID; //for main shader connection
GLuint skyboxEnumerationID; //connection of cubemap to skybox shader
GLuint isReflectionModeID; //for inside main shader
GLuint waterPlanePointID; //for inside main shader
GLuint waterPlaneNormalID; //should be normalized
//do the same, but for the skybox shader
GLuint isReflectionModeSkyboxID; 
GLuint waterPlanePointSkyboxID; //for inside main shader
GLuint waterPlaneNormalSkyboxID; //should be normalized
//-----------------------------------------------
vector<string> textures_faces = {
	"../Assets/textures/skybox/right.png",
	"../Assets/textures/skybox/left.png",
	"../Assets/textures/skybox/top.png",
	"../Assets/textures/skybox/bottom.png",
	"../Assets/textures/skybox/front.png",
	"../Assets/textures/skybox/back.png",
};
//-----------------------------------------------
GLuint cubeMapToonID;
vector<string> textures_faces_toon = {
	"../Assets/textures/skybox-toon/right_toon.png",
	"../Assets/textures/skybox-toon/left_toon.png",
	"../Assets/textures/skybox-toon/top_toon.png",
	"../Assets/textures/skybox-toon/bottom_toon.png",
	"../Assets/textures/skybox-toon/front_toon.png",
	"../Assets/textures/skybox-toon/back_toon.png",
};
//float skyboxVertices[] = {
//	// positions          
//	-1.0f,  1.0f, -1.0f,
//	-1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	-1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f
//};

float skyboxVertices[] = {
	// positions          
	-200.0f,  200.0f, -200.0f,
	-200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	-200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f
};

unsigned int skyboxVAO, skyboxVBO;
unsigned int skyboxProgram;
GLuint skyboxViewID;
GLuint skyboxProjID;

//for viewing
float viewPos[3]; //eye Position

bool viewChange = false;
float eyeAngley = 0.0;
float eyedistance = 30.0;
float FoV = 80; //in degree, between 30-90
float nearClip = 0.1;
float farClip = 200;


int vertices_size[PARTSTOTAL+1];
int uvs_size[PARTSTOTAL+1];
int normals_size[PARTSTOTAL+1];
int tangents_size[PARTSTOTAL + 1];
int bitangents_size[PARTSTOTAL + 1];
int materialCount[PARTSTOTAL+1];

//for shadow and lighting
void displayLightSource(glm::mat4,glm::mat4);
GLuint lightVAO; //create a different VAO for lighting, so it won't get mix up with object VAO, for easier debugging
bool renderLightBox = true;
bool lightingFollowRobocop = false;
bool discoticLighting = false;
const float lightScale = 1.f;
GLuint lightBoxBuffer;
GLuint lightBoxIndices;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjectionMatrixID;
const float lightModel[] = {
	//x-y-z element
	//0 = right up back
	//1 = left up back
	//2 = right up front
	//3 = left up front
	//4 = right bottom back
	//5 = left bottom back
	//6 = left bottom front
	//7 = right bottom front
	1.f,1.f,-1.f,
	-1.f,1.f,-1.f,
	1.f,1.f,1.f,
	-1.f,1.f,1.f,
	1.f,-1.f,-1.f,
	-1.f,-1.f,-1.f,
	-1.f,-1.f,1.f,
	1.f,-1.f,1.f
};
float lightPosition[] = { 10, 20 ,10 };
GLuint renderLightProgram;
float ambientColor[4] = { 0.1,0.1,0.1, 1 };
float diffuseColor[4] = { 0.8,0.8,0.8, 1 };
float specularColor[4] = { 1,1,1,1 };
float Shininess = 32.0;
GLuint ambientID;
GLuint diffuseID;
GLuint  specularID;
GLuint ShininessID; 
int emACK = 0;
GLuint enviromentMappingACKID;
float lightColor[3] = { 1,1,1 }; //for light box color
GLuint depthMapFBO; //for frame buffer
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
unsigned int depthMap; //for textures
float lightProjectX = 34.146; //set to variable for debugging
float lightProjectY = 123.577; //set to variable for debugging
float lightProjectZ = 4.756; //set to variable for debugging
float lightProjectZFar = 51.220; //set to variable for debugging
glm::mat4 lightSpace; //projection x view of light source, for vertex shader
glm::mat4 lightProjection = glm::ortho(-lightProjectX, lightProjectX, -lightProjectY, lightProjectY, lightProjectZ, lightProjectZFar);
GLuint depthProgram;
GLuint lightSpaceID; //projection x view of light source to depth shader
GLuint lightModelID; //model matrix for light box
bool firstTimeDiscoticLighting = true; //when false, discoticLighting already repeat
double lastDiscoticLighting; //to count discotic periodic
bool fromGangnamMoonwalk = false; //discotic disable

GLuint depthMapPlaneFBO; 
const unsigned int SHADOW_PLANE_WIDTH = 4096, SHADOW_PLANE_HEIGHT = 4096;
unsigned int depthMapPlane; //only to get depth of plane, to do manual depth testing

//for loading object
std::vector<std::string> mtls[PARTSTOTAL+1];//use material
std::vector<unsigned int> faces[PARTSTOTAL+1];//face count
map<string, glm::vec3> KAs;
map<string,glm::vec3> KDs;
map<string,glm::vec3> KSs;

//for rendering in every swap buffers
glm::mat4 Projection ;
glm::mat4 View;
glm::mat4 Models[PARTSNUM];

//for monitor, shift due to GUI window
int widthStart = 400;
int heightStart = 0;

float clampValMin(float x, float clampToMin);
float clampValMax(float x, float clampToMax);
float clampValMaxMin(float, float, float);
float getTime();
float fireStart = 0;

////////////////////////tools for animation/////////////////////////
float startTime;

//for all animation that is timed, for example gangnam style 16 second, moonwalk 12 second, and walk in place 5 second
double startTimeMove;

//background music
ISoundEngine* SoundEngine;
ISound* smokeLeakSound = NULL;
ISound* boomSound = NULL;
ISound* fireSound = NULL; 
ISound* waterAmbientSound = NULL;
ISound * jetAmbientSound = NULL;
//loading cubemap

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		cout << i << "tes" << endl;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			cout << i << " " << nrChannels << endl;
			//
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadCubemapPNG(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		TextureData cubemapData = Common::Load_png(faces[i].c_str(), false);
		if (cubemapData.data == nullptr) {
			std::cout << "error on loading png cubemap data!\n";
			std::exit(EXIT_FAILURE);
		}
		//
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubemapData.width, cubemapData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubemapData.data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void setMainViewport() {
	glViewport(widthStart, heightStart, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
}


int fireMode = 0;
void fireEval(PLANEMODE planeMode) {
	if (planeMode == ONFIRE) {
		if (fireMode == 0) {
			if (smokeLeakSound != NULL) {
				smokeLeakSound->stop();
				smokeLeakSound->drop();
				smokeLeakSound = NULL;
			}
			if (boomSound != NULL) {
				boomSound->stop();
				boomSound->drop();
				boomSound = NULL;
			}
			if (fireSound != NULL) {
				fireSound->stop();
				fireSound->drop();
				fireSound = NULL;
			}
			if (jetAmbientSound != NULL) {
				jetAmbientSound->stop();
				jetAmbientSound->drop();
				jetAmbientSound = NULL; 
			}
			if (waterAmbientSound != NULL) {
				waterAmbientSound->stop();
				waterAmbientSound->drop();
				waterAmbientSound = NULL;
			}
			smokeLeakSound = SoundEngine->play2D("../Assets/music/gas.mp3", false, false, true);
			fireMode = 1;
		}
		else if (fireMode == 1) {
			if (glfwGetTime() - fireStart > 3) {
				smokeLeakSound->stop();
				smokeLeakSound->drop();
				smokeLeakSound = NULL;
				boomSound = SoundEngine->play2D("../Assets/music/boom.mp3", false, false, true);
				fireMode = 2;
			}
		}
		else if (fireMode == 2) {
			if (glfwGetTime() - fireStart > 5) {
				boomSound->stop();
				boomSound->drop();
				boomSound = NULL;
				fireSound = SoundEngine->play2D("../Assets/music/fire.mp3", true, false, true);
				fireMode = 3;
			}
		}
		else {

		}
	}
	else {
		if (smokeLeakSound != NULL) {
			smokeLeakSound->stop();
			smokeLeakSound->drop();
			smokeLeakSound = NULL;
		}
		if (boomSound != NULL) {
			boomSound->stop();
			boomSound->drop();
			boomSound = NULL;
		}
		if (fireSound != NULL) {
			fireSound->stop();
			fireSound->drop();
			fireSound = NULL;
		}

		if (jetAmbientSound == NULL) {
			jetAmbientSound = SoundEngine->play2D("../Assets/music/jet.mp3", true, false, true);
		}
		if (planeMode == FLYNEARWATER) {
			if(waterAmbientSound == NULL) waterAmbientSound = SoundEngine->play2D("../Assets/music/ocean.mp3", true, false, true);
		}
		else {
			if (waterAmbientSound != NULL) {
				waterAmbientSound->stop();
				waterAmbientSound->drop();
				waterAmbientSound = NULL;
			}
		}
	}
}

//-----------------------------------------------
GLfloat window_positions[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

GLint Shader_now_Loc;
GLfloat Mosaic_val_Loc;
GLfloat Final_Width_Loc;
GLfloat Final_Height_Loc;
int shader_now = 0;
float mosaic_val = 0.02f;
float width_size;
float height_size;

GLint Toon_active_Loc;
GLint Toon_active_Loc2;
int toon_active = 0;

bool doSpin = false;
bool getReady = false;
bool getReady2 = false;
bool spin360 = true;
bool finishing = true;

GLuint			program2;
GLuint          vao2;
GLuint			vbo2;
GLuint			buffer;
GLuint			FBO;
GLuint			rbo;
GLuint			textureData;
GLuint tex_Loc;
GLuint tex;

GLuint motionBlurProgram;
GLuint tex1Loc;
GLuint tex2Loc;
GLuint tex3Loc;
GLuint tex4Loc;
GLuint tex5Loc;

GLuint			fboLast1;
GLuint			rboLast1;
GLuint			textureLast1;

GLuint			fboLast2;
GLuint			rboLast2;
GLuint			textureLast2;

GLuint			fboLast3;
GLuint			rboLast3;
GLuint			textureLast3;

GLuint			fboLast4;
GLuint			rboLast4;
GLuint			textureLast4;

GLuint			fboLast5;
GLuint			rboLast5;
GLuint			textureLast5;

bool motionBlur_toggle = false;
int step = 0;

void planeSpin();
float dGangnamFunction(float followSpeed, float followTarget, float imiTarget);


GLuint textureObj;
int imageWidth = 0;
int imageHeight = 0;
int imageBPP = 0;

GLuint textureLava1;
GLuint textureLava2;
GLuint textureNew;
GLuint textureNewNormal;

GLuint texPlane_Loc;
GLuint texLava1_Loc;
GLuint texLava2_Loc;
GLuint textureNewID;
GLuint textureNewNormalID;

int shaderMaterial_toggle = 1;
GLfloat currentTime_Loc;
float currentTime;
GLfloat mixVal_Loc;
float mixVal;
//-----------------------------------------------

