#pragma once
//include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

# ifdef _MSC_VER
#  ifdef _DEBUG
#      pragma comment (lib, "glew32sd.lib")
#  else
#      pragma comment (lib, "glew32s.lib")
#  endif
# endif

#include <stdio.h> //for c usage debugging
#include <cstdlib> //for random
#include <iostream> //cout
#include <vector> //for storing obj vertex, normal and uv
#include <list>
#include <map>
#include <string> //for general string usage
#include <windows.h> //for sleep
//#include <cmath>

//for imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//include GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "LoadShaders.h"
#include "objloader.hpp"
#include "imgui/imgui.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/random.hpp> 

//include vgzmo
//#include "imgui/imGuIZMOquat.h"

//irrKlang, for music
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib") //link with dll

//stbi, for loading png
#define STB_IMAGE_IMPLEMENTATION
#include "../STB/stb_image.h"
using namespace std;
using namespace irrklang;

typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;

class Common
{
public:
	// Print OpenGL context related information.
	static void DumpInfo(void) {
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version: %s\n", glGetString(GL_VERSION));
		printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	static void ShaderLog(GLuint shader) {
		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			GLchar* errorLog = new GLchar[maxLength];
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			printf("%s\n", errorLog);
			delete[] errorLog;
		}
	}
	static bool CheckShaderCompiled(GLuint shader) {
		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			GLchar* errorLog = new GLchar[maxLength];
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

			printf("Shader Error: %s\n", errorLog);
			delete[] errorLog;

			return false;
		}

		return true;
	}
	static bool CheckProgramLinked(GLuint program) {
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {

			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* errorLog = new GLchar[maxLength + 1];
			glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
			errorLog[maxLength] = '\0';
			printf("Program link error: %s %d\n", errorLog, maxLength);

			return false;
		}
		return true;
	}
	static bool CheckFrameBufferStatus() {
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("FBO error: %d\n", fboStatus);
			return false;
		}

		return true;
	}
	static bool CheckGLError() {
		GLenum errCode = glGetError();
		if (errCode != GL_NO_ERROR)
		{
			const GLubyte* errString = gluErrorString(errCode);
			printf("%s\n", errString);

			return false;
		}
		return true;
	}
	static void PrintGLError() {
		GLenum code = glGetError();
		switch (code)
		{
		case GL_NO_ERROR:
			std::cout << "GL_NO_ERROR" << std::endl;
			break;
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM" << std::endl;
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_VALUE" << std::endl;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION" << std::endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY" << std::endl;
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW" << std::endl;
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW" << std::endl;
			break;
		default:
			std::cout << "GL_ERROR" << std::endl;
		}
	}
	static TextureData Load_png(const char* path, bool mirroredY = true) {
		TextureData texture;
		int n;
		stbi_uc* data = stbi_load(path, &texture.width, &texture.height, &n, 4);
		if (data != NULL)
		{
			texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
			memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
			// vertical-mirror image data
			if (mirroredY)
			{
				for (size_t i = 0; i < texture.width; i++)
				{
					for (size_t j = 0; j < texture.height / 2; j++)
					{
						for (size_t k = 0; k < 4; k++) {
							std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
						}
					}
				}
			}
			stbi_image_free(data);
		}
		return texture;
	}


	static char** LoadShaderSource(const char* file) {
		FILE* fp = fopen(file, "rb");
		fseek(fp, 0, SEEK_END);
		long sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* src = new char[sz + 1];
		fread(src, sizeof(char), sz, fp);
		src[sz] = '\0';
		char** srcp = new char* [1];
		srcp[0] = src;
		return srcp;
	}
	static void FreeShaderSource(char** srcp) {
		delete srcp[0];
		delete srcp;
	}

};
//
//class ShaderObject
//{
//public:
//	ShaderObject() {
//
//	}
//	~ShaderObject() {
//		for (std::list<GLuint>::iterator it = shaderList.begin(); it != shaderList.end(); ++it)
//		{
//			glDetachShader(program, *it);
//			glDeleteShader(*it);
//		}
//		shaderList.clear();
//
//		if (program != 0)
//		{
//			glDeleteProgram(program);
//			program = 0;
//		}
//	}
//
//	virtual bool Init() {
//		program = glCreateProgram();
//		if (program == 0)
//		{
//			puts("Create program error\n");
//			return false;
//		}
//
//		return true;
//	}
//	virtual bool AddShader(GLenum shaderType, std::string fileName) {
//		GLuint shader = glCreateShader(shaderType);
//		if (shader == 0)
//		{
//			printf("Create shader error: %d\n", shaderType);
//			return false;
//		}
//
//		shaderList.push_back(shader);
//
//		GLchar** shaderSource = Common::LoadShaderSource((fileName).c_str());
//
//		glShaderSource(shader, 1, Common::LoadShaderSource((fileName).c_str()), NULL);
//		Common::FreeShaderSource(shaderSource);
//
//		glCompileShader(shader);
//		bool isCompiled = Common::CheckShaderCompiled(shader);
//		if (!isCompiled)
//		{
//			printf("Shader compile error %s \n", fileName.c_str());
//			return false;
//		}
//
//		glAttachShader(program, shader);
//
//		return Common::CheckGLError();
//	}
//	virtual bool Finalize() {
//		glLinkProgram(program);
//		bool isLinked = Common::CheckProgramLinked(program);
//
//		for (std::list<GLuint>::iterator it = shaderList.begin(); it != shaderList.end(); ++it)
//		{
//			glDetachShader(program, *it);
//			glDeleteShader(*it);
//		}
//		shaderList.clear();
//
//		return isLinked;
//	}
//	virtual void Enable() {
//		glUseProgram(program);
//	}
//	virtual void Disable() {
//		glUseProgram(0);
//	}
//	virtual GLuint GetUniformLocation(const GLchar* name) {
//		return glGetUniformLocation(program, name);
//	}
//
//private:
//	std::list<GLuint> shaderList;
//	GLuint program;
//};

//fire implementation, but with 2d billboarding
#define FIREBILLBOARDPARTICLES 1000
#define FIREBILLBOARDPARTCILESIZE 100
#define FIREBILLBOARDPARTICLEDIAMETER 50
#define FIREBILLBOARDMAXUPSPEED 50
#define FIRELIFETIMEMAX 2

class FireBillboardObject {
public:
	bool startState;
	int particleNumber;
	float parcLifeTime[FIREBILLBOARDPARTICLES * 4];
	float parcCorners[FIREBILLBOARDPARTICLES*2 * 4];
	float parcTexCoords[FIREBILLBOARDPARTICLES * 2 * 4];
	int vertexIndices[FIREBILLBOARDPARTICLES * 6];
	float centerOffsets[FIREBILLBOARDPARTICLES * 3 * 4];
	float parcVelo[FIREBILLBOARDPARTICLES * 3 * 4];

	float billboardCorners[8];
	float billboardTexCoords[8];

	float startTime;
	float clockTime; 
	glm::vec4 redColor;
	glm::vec3 redPosition;
	//functions
	FireBillboardObject() {
		this->startState = true;
		this->particleNumber = FIREBILLBOARDPARTICLES;
		this->redColor = glm::vec4(0.8, 0.25, 0.25, 1.0);
		this->redPosition = glm::vec3(0, 0, 0);
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/fire-billboarding/fire-texture.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[1] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[2] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[3] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[4] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[5] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[6] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[7] = FIREBILLBOARDPARTCILESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	FireBillboardObject(glm::vec3 baseColor, glm::vec3 basePos) {
		this->startState = true;
		this->particleNumber = FIREBILLBOARDPARTICLES;
		this->redColor = glm::vec4(baseColor, 1.0);
		this->redPosition = basePos;
		cout << "a\n";
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		cout << "b\n";
		this->program = LoadShaders(shadersA);
		cout << "d\n";
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/fire-billboarding/fire-texture.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[1] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[2] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[3] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[4] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[5] = FIREBILLBOARDPARTCILESIZE;
		billboardCorners[6] = -FIREBILLBOARDPARTCILESIZE;
		billboardCorners[7] = FIREBILLBOARDPARTCILESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	~FireBillboardObject() {

	}
	void initParticles() {
		for (int i = 0; i < this->particleNumber; i++) {
			float tempLiftime = FIRELIFETIMEMAX * (rand() / static_cast<float>(RAND_MAX));
			//float tempYVelo = 0.1 * (rand() / static_cast<float>(RAND_MAX));
			float tempYVelo = FIREBILLBOARDMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));

			float centerDiameter = FIREBILLBOARDPARTICLEDIAMETER;
			float centerRadius = centerDiameter / 2;

			//decide the x y position, random generate
			float xStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float yStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float zStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;

			//scale the result
			xStart /= 3;
			yStart /= 10;
			zStart /= 3;

			//float tempXVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempXVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (xStart > 0) {
				tempXVelo *= -1;
			}

			//float tempZVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempZVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (zStart > 0) {
				tempZVelo *= -1;
			}

			//pushing to vector
			for (int rep = 0; rep < 4; rep++) {
				parcLifeTime[4 * i + rep] = tempLiftime;
				parcCorners[8 * i + 2 * rep] = billboardCorners[rep * 2];
				parcCorners[8 * i + 2 * rep + 1] = billboardCorners[rep * 2 + 1];

				parcTexCoords[8 * i + 2 * rep] = billboardTexCoords[rep * 2];
				parcTexCoords[8 * i + 2 * rep + 1] = billboardTexCoords[rep * 2 + 1];

				centerOffsets[12 * i + 3 * rep] = xStart;
				centerOffsets[12 * i + 3 * rep + 1] = yStart + abs(xStart / 2.0);
				centerOffsets[12 * i + 3 * rep + 2] = zStart;

				parcVelo[12 * i + 3 * rep] = tempXVelo;
				parcVelo[12 * i + 3 * rep + 1] = tempYVelo;
				parcVelo[12 * i + 3 * rep + 2] = tempZVelo;
			}
			vertexIndices[6 * i] = (4 * i);
			vertexIndices[6 * i+1] = (1+ 4 * i);
			vertexIndices[6 * i+2] = (2+ 4 * i);
			vertexIndices[6 * i+3] = (4 * i);
			vertexIndices[6 * i+4] = (2+ 4 * i);
			vertexIndices[6 * i+5] = (3+ 4 * i);
		}
	}
	void bindTexture() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
	}
	void enableBlending() {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resetData() {
		
		this->startState = true;
		this->startTime = glfwGetTime();
		this->clockTime = 3;
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);

		float currentTime = glfwGetTime();
		clockTime += (currentTime - this->startTime);
		this->startTime = currentTime;

		glUniform1fv(this->totalTimeID, 1, &this->clockTime);
		glUniform1fv(this->fragmentTimeID, 1, &this->clockTime);

		glUniform3fv(this->centralFirePosID, 1, &this->redPosition[0]);
		glUniform4fv(this->baseColorID, 1, &this->redColor[0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->fireVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->fireVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parcLifeTime)+ sizeof(parcCorners) + sizeof(parcTexCoords)+ sizeof(centerOffsets)+sizeof(parcVelo), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcLifeTime), parcLifeTime);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void* )offset);
		offset += sizeof(parcLifeTime);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcTexCoords), parcTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcTexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcCorners), parcCorners);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcCorners);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(centerOffsets), centerOffsets);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(centerOffsets);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcVelo), parcVelo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcVelo);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->fireEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		if (startState) {
			initParticles();
			resetData();
			startState = false;
		}
		int rn = rand() % FIREBILLBOARDPARTICLES;
		//cout << "random x texture: " << parcTexCoords[2 * rn] << " random y texture: " << parcTexCoords[2*rn+1] << "\n";
		glUseProgram(this->program);
		bindTexture();
		updateUniform(viewMatrix, projMatrix);
		updateParticles();
		enableBlending();

		//do render
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, sizeof(vertexIndices)/sizeof(int), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		shutdown();
	}
private:
	GLuint program;
	GLuint fireVAO;
	GLuint fireVBO;
	GLuint fireEBO; //for instanced rendering
	//three textures (each will use different parameter)
	GLuint fireTextureID;
	GLuint fireSamplerIDEnumeration;

	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
	GLuint totalTimeID;
	GLuint centralFirePosID;
	//for fragment shader input uniform
	GLuint baseColorID;
	GLuint fragmentTimeID;
};




#define SMOKEBURNERPARTICLES 1000
#define SMOKEBURNERPARTICLESIZE 50
#define SMOKEBURNERPARTICLEDIAMETER 50
#define SMOKEBURNERMAXUPSPEED 5
#define SMOKELIFETIMEMAX 20
class SmokeBurnerObject {
public:
	bool startState;
	int particleNumber;
	float parcLifeTime[SMOKEBURNERPARTICLES * 4];
	float parcCorners[SMOKEBURNERPARTICLES * 2 * 4];
	float parcTexCoords[SMOKEBURNERPARTICLES * 2 * 4];
	int vertexIndices[SMOKEBURNERPARTICLES * 6];
	float centerOffsets[SMOKEBURNERPARTICLES * 3 * 4];
	float parcVelo[SMOKEBURNERPARTICLES * 3 * 4];

	float billboardCorners[8];
	float billboardTexCoords[8];

	float startTime;
	float clockTime;
	glm::vec4 smokeColor;
	glm::vec3 smokePosition;
	//functions
	SmokeBurnerObject() {
		this->startState = true;
		this->particleNumber = SMOKEBURNERPARTICLES;
		this->smokeColor = glm::vec4(0.45, 0.509, 0.467, 1.0);
		this->smokePosition = glm::vec3(0, 0, 0);
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		TextureData smokeTexData = Common::Load_png("../Assets/textures/smoke-billboarding/smoke-particle-sheet-gen2.png");
		if (smokeTexData.data == nullptr) {
			std::cout << "error on loading smoke texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->smokeTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, smokeTexData.width, smokeTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smokeTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->smokeSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->smokeSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralSmokePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->smokeVAO);
		glGenBuffers(1, &this->smokeVBO);
		billboardCorners[0] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[1] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[2] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[3] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[4] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[5] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[6] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[7] = SMOKEBURNERPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->smokeEBO);
	}
	SmokeBurnerObject(glm::vec3 baseColor, glm::vec3 basePos) {
		this->startState = true;
		this->particleNumber = SMOKEBURNERPARTICLES;
		this->smokeColor = glm::vec4(baseColor, 1.0);
		this->smokePosition = basePos;
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		TextureData smokeTexData = Common::Load_png("../Assets/textures/smoke-billboarding/smoke-particle-sheet-gen2.png");
		if (smokeTexData.data == nullptr) {
			std::cout << "error on loading smoke texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->smokeTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, smokeTexData.width, smokeTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smokeTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->smokeSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->smokeSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralSmokePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->smokeVAO);
		glGenBuffers(1, &this->smokeVBO);
		billboardCorners[0] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[1] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[2] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[3] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[4] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[5] = SMOKEBURNERPARTICLESIZE;
		billboardCorners[6] = -SMOKEBURNERPARTICLESIZE;
		billboardCorners[7] = SMOKEBURNERPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->smokeEBO);
	}
	~SmokeBurnerObject() {

	}
	void initParticles() {
		for (int i = 0; i < this->particleNumber; i++) {
			float tempLiftime = SMOKELIFETIMEMAX * (rand() / static_cast<float>(RAND_MAX));
			//float tempYVelo = 0.1 * (rand() / static_cast<float>(RAND_MAX));
			float tempYVelo = SMOKEBURNERMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));

			float centerDiameter = SMOKEBURNERPARTICLEDIAMETER;
			float centerRadius = centerDiameter / 2;

			//decide the x y position, random generate
			float xStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float yStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float zStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;

			//scale the result
			xStart /= 3;
			yStart /= 10;
			zStart /= 3;

			//float tempXVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempXVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (xStart > 0) {
				tempXVelo *= -1;
			}

			//float tempZVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempZVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (zStart > 0) {
				tempZVelo *= -1;
			}

			//pushing to vector
			for (int rep = 0; rep < 4; rep++) {
				parcLifeTime[4 * i + rep] = tempLiftime;
				parcCorners[8 * i + 2 * rep] = billboardCorners[rep * 2];
				parcCorners[8 * i + 2 * rep + 1] = billboardCorners[rep * 2 + 1];

				parcTexCoords[8 * i + 2 * rep] = billboardTexCoords[rep * 2];
				parcTexCoords[8 * i + 2 * rep + 1] = billboardTexCoords[rep * 2 + 1];

				centerOffsets[12 * i + 3 * rep] = xStart;
				centerOffsets[12 * i + 3 * rep + 1] = yStart + abs(xStart / 2.0);
				centerOffsets[12 * i + 3 * rep + 2] = zStart;

				parcVelo[12 * i + 3 * rep] = tempXVelo;
				parcVelo[12 * i + 3 * rep + 1] = tempYVelo;
				parcVelo[12 * i + 3 * rep + 2] = tempZVelo;
			}
			vertexIndices[6 * i] = (4 * i);
			vertexIndices[6 * i + 1] = (1 + 4 * i);
			vertexIndices[6 * i + 2] = (2 + 4 * i);
			vertexIndices[6 * i + 3] = (4 * i);
			vertexIndices[6 * i + 4] = (2 + 4 * i);
			vertexIndices[6 * i + 5] = (3 + 4 * i);
		}
	}
	void bindTexture() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
	}
	void enableBlending() {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resetData() {

		this->startState = true;
		this->startTime = glfwGetTime();
		this->clockTime = 3;
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);

		float currentTime = glfwGetTime();
		clockTime += (currentTime - this->startTime);
		this->startTime = currentTime;

		glUniform1fv(this->totalTimeID, 1, &this->clockTime);
		glUniform1fv(this->fragmentTimeID, 1, &this->clockTime);

		glUniform3fv(this->centralSmokePosID, 1, &this->smokePosition[0]);
		glUniform4fv(this->baseColorID, 1, &this->smokeColor[0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->smokeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->smokeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parcLifeTime) + sizeof(parcCorners) + sizeof(parcTexCoords) + sizeof(centerOffsets) + sizeof(parcVelo), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcLifeTime), parcLifeTime);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcLifeTime);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcTexCoords), parcTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcTexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcCorners), parcCorners);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcCorners);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(centerOffsets), centerOffsets);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(centerOffsets);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcVelo), parcVelo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcVelo);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->smokeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		if (startState) {
			initParticles();
			resetData();
			startState = false;
		}
		int rn = rand() % SMOKEBURNERPARTICLES;
		//cout << "random x texture: " << parcTexCoords[2 * rn] << " random y texture: " << parcTexCoords[2*rn+1] << "\n";
		glUseProgram(this->program);
		bindTexture();
		updateUniform(viewMatrix, projMatrix);
		updateParticles();
		enableBlending();

		//do render
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, sizeof(vertexIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		shutdown();
	}
private:
	GLuint program;
	GLuint smokeVAO;
	GLuint smokeVBO;
	GLuint smokeEBO; //for instanced rendering
	//three textures (each will use different parameter)SMOKEBURNERPARTICLESIZE
	GLuint smokeTextureID;
	GLuint smokeSamplerIDEnumeration;

	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
	GLuint totalTimeID;
	GLuint centralSmokePosID;
	//for fragment shader input uniform
	GLuint baseColorID;
	GLuint fragmentTimeID;
};

#define HORIZONTALSMOKEPARTICLES 3000
#define HORIZONTALSMOKEPARTICLESIZE 5
#define HORIZONTALSMOKEPARTICLEDIAMETER 5
#define HORIZONTALSMOKEMAXSPEED 10
#define HORIZONTALSMOKELIFETIMEMAX 4
class HorizontalSmokeObject {
public:
	bool startState;
	int particleNumber;
	float parcLifeTime[HORIZONTALSMOKEPARTICLES * 4];
	float parcCorners[HORIZONTALSMOKEPARTICLES * 2 * 4];
	float parcTexCoords[HORIZONTALSMOKEPARTICLES * 2 * 4];
	int vertexIndices[HORIZONTALSMOKEPARTICLES * 6];
	float centerOffsets[HORIZONTALSMOKEPARTICLES * 3 * 4];
	float parcVelo[HORIZONTALSMOKEPARTICLES * 3 * 4];

	float billboardCorners[8];
	float billboardTexCoords[8];

	float startTime;
	float clockTime;
	glm::vec4 smokeColor;
	glm::vec3 smokePosition;
	//functions
	HorizontalSmokeObject() {
		this->startState = true;
		this->particleNumber = HORIZONTALSMOKEPARTICLES;
		this->smokeColor = glm::vec4(0.45, 0.509, 0.467, 1.0);
		this->smokePosition = glm::vec3(0, 0, 0);
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "back_particle_emitter.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "back_particle_emitter.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		//TextureData smokeTexData = Common::Load_png("../Assets/textures/smoke-billboarding/smoke-particle-sheet-gen2.png");
		//if (smokeTexData.data == nullptr) {
		//	std::cout << "error on loading smoke texture data!\n";
		//	std::exit(EXIT_FAILURE);
		//}
		////initialize and set texture parameter
		//glGenTextures(1, &this->smokeTextureID);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, smokeTexData.width, smokeTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smokeTexData.data);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		/*this->smokeSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		this->depthTextureIDEnumeration = glGetUniformLocation(this->program, "depthTex");*/
		glUseProgram(this->program);
		/*glUniform1i(this->smokeSamplerIDEnumeration, 0);
		glUniform1i(this->depthTextureIDEnumeration, 1);*/
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralSmokePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//this->viewPosID = glGetUniformLocation(this->program, "viewPos");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->smokeVAO);
		glGenBuffers(1, &this->smokeVBO);
		billboardCorners[0] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[1] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[2] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[3] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[4] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[5] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[6] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[7] = HORIZONTALSMOKEPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->smokeEBO);
	}
	HorizontalSmokeObject(glm::vec3 baseColor, glm::vec3 basePos) {
		this->startState = true;
		this->particleNumber = HORIZONTALSMOKEPARTICLES;
		this->smokeColor = glm::vec4(baseColor, 1.0);
		this->smokePosition = basePos;
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "back_particle_emitter.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "back_particle_emitter.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		//TextureData smokeTexData = Common::Load_png("../Assets/textures/smoke-billboarding/smoke-particle-sheet-gen2.png");
		//if (smokeTexData.data == nullptr) {
		//	std::cout << "error on loading smoke texture data!\n";
		//	std::exit(EXIT_FAILURE);
		//}
		////initialize and set texture parameter
		//glGenTextures(1, &this->smokeTextureID);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, smokeTexData.width, smokeTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smokeTexData.data);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		/*this->smokeSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		this->depthTextureIDEnumeration = glGetUniformLocation(this->program, "depthTex");*/
		glUseProgram(this->program);
		/*glUniform1i(this->smokeSamplerIDEnumeration, 0);
		glUniform1i(this->depthTextureIDEnumeration, 1);*/
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralSmokePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//this->viewPosID = glGetUniformLocation(this->program, "viewPos");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->smokeVAO);
		glGenBuffers(1, &this->smokeVBO);
		billboardCorners[0] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[1] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[2] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[3] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[4] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[5] = HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[6] = -HORIZONTALSMOKEPARTICLESIZE;
		billboardCorners[7] = HORIZONTALSMOKEPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->smokeEBO);
	}
	~HorizontalSmokeObject() {

	}
	void initParticles() {
		for (int i = 0; i < this->particleNumber; i++) {
			float tempLiftime = HORIZONTALSMOKELIFETIMEMAX * (rand() / static_cast<float>(RAND_MAX));
			//float tempYVelo = 0.1 * (rand() / static_cast<float>(RAND_MAX));
			float tempZVelo = -HORIZONTALSMOKEMAXSPEED * (rand() / static_cast<float>(RAND_MAX));

			float centerDiameter = HORIZONTALSMOKEPARTICLEDIAMETER;
			float centerRadius = centerDiameter / 2;

			//decide the x y position, random generate
			float xStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float yStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float zStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;

			//scale the result
			xStart /= 3;
			zStart /= 10;
			yStart /= 3;

			//float tempXVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempXVelo = 1.5 * (rand() / static_cast<float>(RAND_MAX));
			if (xStart > 0) {
				tempXVelo *= -1;
			}

			//float tempZVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempYVelo = 1.5 * (rand() / static_cast<float>(RAND_MAX));
			if (yStart > 0) {
				tempYVelo *= -1;
			}

			//pushing to vector
			for (int rep = 0; rep < 4; rep++) {
				parcLifeTime[4 * i + rep] = tempLiftime;
				parcCorners[8 * i + 2 * rep] = billboardCorners[rep * 2];
				parcCorners[8 * i + 2 * rep + 1] = billboardCorners[rep * 2 + 1];

				parcTexCoords[8 * i + 2 * rep] = billboardTexCoords[rep * 2];
				parcTexCoords[8 * i + 2 * rep + 1] = billboardTexCoords[rep * 2 + 1];

				centerOffsets[12 * i + 3 * rep] = xStart;
				centerOffsets[12 * i + 3 * rep + 1] = yStart + abs(xStart / 2.0);
				centerOffsets[12 * i + 3 * rep + 2] = zStart;

				parcVelo[12 * i + 3 * rep] = tempXVelo;
				parcVelo[12 * i + 3 * rep + 1] = tempYVelo;
				parcVelo[12 * i + 3 * rep + 2] = tempZVelo;
			}
			vertexIndices[6 * i] = (4 * i);
			vertexIndices[6 * i + 1] = (1 + 4 * i);
			vertexIndices[6 * i + 2] = (2 + 4 * i);
			vertexIndices[6 * i + 3] = (4 * i);
			vertexIndices[6 * i + 4] = (2 + 4 * i);
			vertexIndices[6 * i + 5] = (3 + 4 * i);
		}
	}
	void bindTexture(GLuint depthTex) {
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, this->smokeTextureID);
		//glActiveTexture(GL_TEXTURE0 + 1);
		//glBindTexture(GL_TEXTURE_2D, depthTex);
	}
	void enableBlending() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		glUseProgram(0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resetData() {

		this->startState = true;
		this->startTime = glfwGetTime();
		this->clockTime = 3;
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix, glm::vec3 viewPos) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);

		float currentTime = glfwGetTime();
		clockTime += (currentTime - this->startTime);
		this->startTime = currentTime;

		glUniform1fv(this->totalTimeID, 1, &this->clockTime);
		glUniform1fv(this->fragmentTimeID, 1, &this->clockTime);

		glUniform3fv(this->centralSmokePosID, 1, &this->smokePosition[0]);
		glUniform4fv(this->baseColorID, 1, &this->smokeColor[0]);

		//glUniform3fv(this->viewPosID, 1, &viewPos[0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->smokeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->smokeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parcLifeTime) + sizeof(parcCorners) + sizeof(parcTexCoords) + sizeof(centerOffsets) + sizeof(parcVelo), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcLifeTime), parcLifeTime);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcLifeTime);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcTexCoords), parcTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcTexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcCorners), parcCorners);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcCorners);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(centerOffsets), centerOffsets);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(centerOffsets);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcVelo), parcVelo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcVelo);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->smokeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix, GLuint depthTex, glm::vec3 viewPos) {
		if (startState) {
			initParticles();
			resetData();
			startState = false;
		}
		int rn = rand() % HORIZONTALSMOKEPARTICLES;
		//cout << "random x texture: " << parcTexCoords[2 * rn] << " random y texture: " << parcTexCoords[2*rn+1] << "\n";
		glUseProgram(this->program);
		bindTexture(depthTex);
		updateUniform(viewMatrix, projMatrix, viewPos);
		updateParticles();
		enableBlending();

		//do render
		//glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, sizeof(vertexIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//glEnable(GL_DEPTH_TEST);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		shutdown();
	}
private:
	GLuint program;
	GLuint smokeVAO;
	GLuint smokeVBO;
	GLuint smokeEBO; //for instanced rendering
	//three textures (each will use different parameter)
	/*GLuint smokeTextureID;
	GLuint smokeSamplerIDEnumeration;
	GLuint depthTextureIDEnumeration; 
	GLuint viewPosID;*/
	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
	GLuint totalTimeID;
	GLuint centralSmokePosID;
	//for fragment shader input uniform
	GLuint baseColorID;
	GLuint fragmentTimeID;
};


struct FireParticle {
	//point data for each particle
	//these data will be passed to the buffer
	glm::vec3 position;
	float startXTex; //the starting position
	float frameTime; //incrementally updated so shadow has tracking how long has the shader retrieved
};

#define PARTICLESIZE  0.01f
static int recTriangleStripOrder[] = {
	//https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
	//obtained from here
	3,2,6,7,4,2,0,3,1,6,5,4,1,0
};


#define AURABILLBOARDPARTICLES 100
#define AURABILLBOARDPARTCILESIZE 75
#define AURABILLBOARDPARTICLEDIAMETER 50
#define AURABILLBOARDMAXUPSPEED 0.2
#define AURALIFETIMEMAX 6

class AuraBillboardObject {
public:
	bool startState;
	int particleNumber;
	float parcLifeTime[AURABILLBOARDPARTICLES * 4];
	float parcCorners[AURABILLBOARDPARTICLES * 2 * 4];
	float parcTexCoords[AURABILLBOARDPARTICLES * 2 * 4];
	int vertexIndices[AURABILLBOARDPARTICLES * 6];
	float centerOffsets[AURABILLBOARDPARTICLES * 3 * 4];
	float parcVelo[AURABILLBOARDPARTICLES * 3 * 4];

	float billboardCorners[8];
	float billboardTexCoords[8];

	float startTime;
	float clockTime;
	glm::vec4 redColor;
	glm::vec3 redPosition;
	//functions
	AuraBillboardObject() {
		this->startState = true;
		this->particleNumber = AURABILLBOARDPARTICLES;
		this->redColor = glm::vec4(0.8, 0.25, 0.25, 1.0);
		this->redPosition = glm::vec3(0, 0, 0);
		//cout << "a\n";
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		//cout << "b\n";
		this->program = LoadShaders(shadersA);
		//cout << "d\n";
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/fire-billboarding/fire-texture.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[1] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[2] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[3] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[4] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[5] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[6] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[7] = AURABILLBOARDPARTCILESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	AuraBillboardObject(glm::vec3 baseColor, glm::vec3 basePos) {
		this->startState = true;
		this->particleNumber = AURABILLBOARDPARTICLES;
		this->redColor = glm::vec4(baseColor, 1.0);
		this->redPosition = basePos;
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/fire-billboarding/fire-texture.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[1] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[2] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[3] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[4] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[5] = AURABILLBOARDPARTCILESIZE;
		billboardCorners[6] = -AURABILLBOARDPARTCILESIZE;
		billboardCorners[7] = AURABILLBOARDPARTCILESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	~AuraBillboardObject() {

	}
	void initParticles() {
		for (int i = 0; i < this->particleNumber; i++) {
			float tempLiftime = AURALIFETIMEMAX * (rand() / static_cast<float>(RAND_MAX));
			//float tempYVelo = 0.1 * (rand() / static_cast<float>(RAND_MAX));
			float tempYVelo = AURABILLBOARDMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));

			float centerDiameter = AURABILLBOARDPARTICLEDIAMETER;
			float centerRadius = centerDiameter / 2;

			//decide the x y position, random generate
			float xStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float yStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float zStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;

			//scale the result
			xStart /= 3;
			yStart /= 10;
			zStart /= 3;

			//float tempXVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempXVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (xStart > 0) {
				tempXVelo *= -1;
			}

			//float tempZVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempZVelo = 1 * (rand() / static_cast<float>(RAND_MAX));
			if (zStart > 0) {
				tempZVelo *= -1;
			}

			//pushing to vector
			for (int rep = 0; rep < 4; rep++) {
				parcLifeTime[4 * i + rep] = tempLiftime;
				parcCorners[8 * i + 2 * rep] = billboardCorners[rep * 2];
				parcCorners[8 * i + 2 * rep + 1] = billboardCorners[rep * 2 + 1];

				parcTexCoords[8 * i + 2 * rep] = billboardTexCoords[rep * 2];
				parcTexCoords[8 * i + 2 * rep + 1] = billboardTexCoords[rep * 2 + 1];

				centerOffsets[12 * i + 3 * rep] = xStart;
				centerOffsets[12 * i + 3 * rep + 1] = yStart + abs(xStart / 2.0);
				centerOffsets[12 * i + 3 * rep + 2] = zStart;

				parcVelo[12 * i + 3 * rep] = tempXVelo;
				parcVelo[12 * i + 3 * rep + 1] = tempYVelo;
				parcVelo[12 * i + 3 * rep + 2] = tempZVelo;
			}
			vertexIndices[6 * i] = (4 * i);
			vertexIndices[6 * i + 1] = (1 + 4 * i);
			vertexIndices[6 * i + 2] = (2 + 4 * i);
			vertexIndices[6 * i + 3] = (4 * i);
			vertexIndices[6 * i + 4] = (2 + 4 * i);
			vertexIndices[6 * i + 5] = (3 + 4 * i);
		}
	}
	void bindTexture() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
	}
	void enableBlending() {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resetData() {

		this->startState = true;
		this->startTime = glfwGetTime();
		this->clockTime = 3;
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);

		float currentTime = glfwGetTime();
		clockTime += (currentTime - this->startTime);
		this->startTime = currentTime;

		glUniform1fv(this->totalTimeID, 1, &this->clockTime);
		glUniform1fv(this->fragmentTimeID, 1, &this->clockTime);

		glUniform3fv(this->centralFirePosID, 1, &this->redPosition[0]);
		glUniform4fv(this->baseColorID, 1, &this->redColor[0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->fireVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->fireVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parcLifeTime) + sizeof(parcCorners) + sizeof(parcTexCoords) + sizeof(centerOffsets) + sizeof(parcVelo), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcLifeTime), parcLifeTime);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcLifeTime);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcTexCoords), parcTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcTexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcCorners), parcCorners);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcCorners);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(centerOffsets), centerOffsets);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(centerOffsets);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcVelo), parcVelo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcVelo);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->fireEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		if (startState) {
			initParticles();
			resetData();
			startState = false;
		}
		int rn = rand() % AURABILLBOARDPARTICLES;
		//cout << "random x texture: " << parcTexCoords[2 * rn] << " random y texture: " << parcTexCoords[2 * rn + 1] << "\n";
		glUseProgram(this->program);
		bindTexture();
		updateUniform(viewMatrix, projMatrix);
		updateParticles();
		enableBlending();

		//do render
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, sizeof(vertexIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		shutdown();
	}
private:
	GLuint program;
	GLuint fireVAO;
	GLuint fireVBO;
	GLuint fireEBO; //for instanced rendering
	//three textures (each will use different parameter)
	GLuint fireTextureID;
	GLuint fireSamplerIDEnumeration;

	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
	GLuint totalTimeID;
	GLuint centralFirePosID;
	//for fragment shader input uniform
	GLuint baseColorID;
	GLuint fragmentTimeID;
};

#define WATERQUADXZ 200.0
#define WATERQUADY -2
#define TEXREFLECTWIDTH 1024
#define TEXREFLECTHEIGHT 1024
#define TEXREFRACTWIDTH 1024
#define TEXREFRACTHEIGHT 1024
class WaterQuad {
public:
	float quadVertices[12];
	float quadTexCoords[8];
	GLuint indices[6];
	float randomSampleIncrement = 0.03;
	float randomSample = 0;
	float startTime; 
	//functions
	WaterQuad() {
		this->quadVertices[0] = -WATERQUADXZ;
		this->quadVertices[1] = WATERQUADY;
		this->quadVertices[2] = -WATERQUADXZ;
		this->quadVertices[3] = WATERQUADXZ;
		this->quadVertices[4] = WATERQUADY;
		this->quadVertices[5] = -WATERQUADXZ;
		this->quadVertices[6] = WATERQUADXZ;
		this->quadVertices[7] = WATERQUADY;
		this->quadVertices[8] = WATERQUADXZ;
		this->quadVertices[9] = -WATERQUADXZ;
		this->quadVertices[10] = WATERQUADY;
		this->quadVertices[11] = WATERQUADXZ;

		this->quadTexCoords[0] = 0;
		this->quadTexCoords[1] = 0;
		this->quadTexCoords[2] = 1;
		this->quadTexCoords[3] = 0;
		this->quadTexCoords[4] = 1;
		this->quadTexCoords[5] = 1;
		this->quadTexCoords[6] = 0;
		this->quadTexCoords[7] = 1;

		this->startTime = glfwGetTime();

		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "waterQuad.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "waterQuad.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		if (this->program == 0) {
			cout << "water program invalid\n";
		}
		initReflectFBO();
		initRefractFBO();
		TextureData dudvTexData = Common::Load_png("../Assets/textures/dudv-noise-map/seawaterDUDV.png");
		if (dudvTexData.data == nullptr) {
			std::cout << "error on loading water dudv data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->dudvMap);
		glActiveTexture(GL_TEXTURE0+2);
		glBindTexture(GL_TEXTURE_2D, this->dudvMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dudvTexData.width, dudvTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dudvTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
		TextureData normalTexData = Common::Load_png("../Assets/textures/normal-map/normal-sea.png");
		if (normalTexData.data == nullptr) {
			std::cout << "error on loading water normal data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->normalMap);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, this->normalMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normalTexData.width, normalTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "View");
		this->projID = glGetUniformLocation(this->program, "Projection");
		this->randomSampleID = glGetUniformLocation(this->program, "randomSample");
		this->reflectTexEnumerationID = glGetUniformLocation(this->program, "reflectTex");
		this->refractTexEnumerationID = glGetUniformLocation(this->program, "refractTex");
		this->dudvMapEnumerationID = glGetUniformLocation(this->program, "dudvTex");
		this->normalMapEnumerationID = glGetUniformLocation(this->program, "normalTex");
		this->permanentViewPosID = glGetUniformLocation(this->program, "permanentViewPos"); //for shadow and fresnel effect
		this->lightColorID = glGetUniformLocation(this->program, "lightColor");
		this->lightPosID = glGetUniformLocation(this->program, "lightPos");
		this->lightSpaceMatrixID = glGetUniformLocation(this->program, "lightSpaceMatrix");
		this->externalShadowMapEnumerationID = glGetUniformLocation(this->program, "shadowMap");
		glUniform1i(this->reflectTexEnumerationID, 0);
		glUniform1i(this->refractTexEnumerationID, 1);
		glUniform1i(this->dudvMapEnumerationID, 2);
		glUniform1i(this->normalMapEnumerationID, 3);
		glUniform1i(this->externalShadowMapEnumerationID, 4);
		glGenVertexArrays(1, &this->quadVAO);
		glGenBuffers(1, &this->quadVBO);
		glGenBuffers(1, &this->quadEBO);
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	~WaterQuad() {

	}
	void enableBlending() {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);*/
	}
	void bindTexture(GLuint externalShadowMap) {
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, this->reflectTexc);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, this->refractTexc);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, this->dudvMap);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, this->normalMap);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, externalShadowMap);
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix, glm::vec3 permanentViewPos, glm::vec3 lightPos, glm::vec3 lightColor, glm::mat4 lSpaceMatrix) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);
		glUniform1fv(this->randomSampleID, 1, &randomSample);
		glUniform3fv(this->permanentViewPosID, 1, &permanentViewPos[0]);
		glUniform3fv(this->lightPosID, 1, &lightPos[0]);
		glUniform3fv(this->lightColorID, 1, &lightColor[0]);
		glUniformMatrix4fv(this->lightSpaceMatrixID, 1, GL_FALSE, &lSpaceMatrix[0][0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices)+sizeof(quadTexCoords), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(quadVertices), quadVertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(quadVertices);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(quadTexCoords), quadTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(quadTexCoords);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quadEBO);
	}

	void initReflectFBO() {
		this->reflectFBO = customCreateFBO();
		this->reflectTexc = customCreateTextureFramebuffer(this->reflectFBO, TEXREFLECTWIDTH, TEXREFLECTHEIGHT); 
		this->reflectRBd = customCreateDepthRenderFramebuffer(this->reflectFBO, TEXREFLECTWIDTH, TEXREFLECTHEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, this->reflectFBO);
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			exit(EXIT_FAILURE);
		}
	}

	void initRefractFBO() {
		this->refractFBO = customCreateFBO();
		this->refractTexc = customCreateTextureFramebuffer(this->refractFBO, TEXREFRACTWIDTH, TEXREFRACTHEIGHT);
		this->refractRBd = customCreateDepthRenderFramebuffer(this->refractFBO, TEXREFRACTWIDTH, TEXREFRACTHEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, this->refractFBO);
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			exit(EXIT_FAILURE);
		}
	}

	void bindReflectFBO(int frameWidth, int frameHeight) {
		customBindFrameBuffer(this->reflectFBO, frameWidth, frameHeight);
	}

	void bindRefractFBO(int frameWidth, int frameHeight) {
		customBindFrameBuffer(this->refractFBO, frameWidth, frameHeight);
	}

	GLuint customCreateFBO() {
		GLuint returnFBO; 
		glGenFramebuffers(1, &returnFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, returnFBO);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);//decide where to draw
		return returnFBO;
	}

	void customBindFrameBuffer(GLuint frameBufferID, int frameWidth, int frameHeight) {
		glBindTexture(GL_TEXTURE_2D, 0); 
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glViewport(0, 0, frameWidth, frameHeight);
	}

	GLuint customCreateTextureFramebuffer(GLuint frameBufferID, int width, int height) {
		GLuint returnTex;
		glGenTextures(1, &returnTex);
		glBindTexture(GL_TEXTURE_2D, returnTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //initialize to empty
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//attach to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, returnTex, 0);
		return returnTex;
	}

	GLuint customCreateDepthTextureFramebuffer(GLuint frameBufferID, int width, int height) {
		GLuint returnTex;
		glGenTextures(1, &returnTex);
		glBindTexture(GL_TEXTURE_2D, returnTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //initialize to empty
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//attach to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, returnTex, 0);
		return returnTex;
	}

	GLuint customCreateDepthRenderFramebuffer(GLuint frameBufferID, int width, int height) {
		GLuint returnRB;
		glGenRenderbuffers(1, &returnRB);
		glBindRenderbuffer(GL_RENDERBUFFER, returnRB);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		//attach to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, returnRB);
		return returnRB;
	}
	void uploadShadowMapData(GLuint shadowmap) {
		this->externalShadowMap = shadowmap;
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix, glm::vec3 permanentViewPos, glm::vec3 lightPos, glm::vec3 lightColor, GLuint shadowMap, glm::mat4 lSpaceMatrix) {
		//cout << "quad vertices: " << quadVertices[0] << "\n";
		glUseProgram(this->program);
		updateUniform(viewMatrix, projMatrix, permanentViewPos, lightPos, lightColor, lSpaceMatrix);
		updateParticles();
		bindTexture(shadowMap);
		enableBlending();

		//do render
		//glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glEnable(GL_CULL_FACE);
		//glDrawArrays(GL_TRIANGLES, 0, 2);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		float newTime = glfwGetTime();
		randomSample += randomSampleIncrement * (newTime-startTime);
		startTime = newTime;
		fmod(randomSample, 1.0);
		shutdown();
	}
private:
	GLuint program;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint quadEBO;
	GLuint reflectFBO;
	GLuint refractFBO;
	GLuint reflectTexc;
	GLuint reflectRBd;
	GLuint refractTexc;
	GLuint refractRBd;
	GLuint dudvMap;
	GLuint normalMap;
	GLuint reflectTexEnumerationID; 
	GLuint refractTexEnumerationID; 
	GLuint dudvMapEnumerationID;
	GLuint normalMapEnumerationID;
	GLuint randomSampleID;

	GLuint lightPosID; 
	GLuint lightColorID;

	//for shadow on water
	GLuint externalShadowMap;
	GLuint externalShadowMapEnumerationID;
	GLuint lightSpaceMatrixID;
	GLuint permanentViewPosID;
	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
};

//explosion implementation but with billboarding
#define EXPLODINGBILLBOARDPARTICLES 1000
#define EXPLODINGBILLBOARDPARTICLESIZE 80
#define EXPLODINGBILLBOARDPARTICLEDIAMETER 10
#define EXPLODINGBILLBOARDMAXUPSPEED 200
#define EXPLODINGLIFETIMEMAX 0.15
class ExplodingObject {
public:
	bool startState;
	int particleNumber;
	float parcLifeTime[EXPLODINGBILLBOARDPARTICLES * 4];
	float parcCorners[EXPLODINGBILLBOARDPARTICLES * 2 * 4];
	float parcTexCoords[EXPLODINGBILLBOARDPARTICLES * 2 * 4];
	int vertexIndices[EXPLODINGBILLBOARDPARTICLES * 6];
	float centerOffsets[EXPLODINGBILLBOARDPARTICLES * 3 * 4];
	float parcVelo[EXPLODINGBILLBOARDPARTICLES * 3 * 4];

	float billboardCorners[8];
	float billboardTexCoords[8];

	float startTime;
	float clockTime;
	glm::vec4 redColor;
	glm::vec3 redPosition;
	//functions
	ExplodingObject() {
		this->startState = true;
		this->particleNumber = EXPLODINGBILLBOARDPARTICLES;
		this->redColor = glm::vec4(0.8, 0.25, 0.25, 1.0);
		this->redPosition = glm::vec3(0, 0, 0);
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		this->program = LoadShaders(shadersA);
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/exploding-billboarding/exploding.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[1] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[2] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[3] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[4] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[5] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[6] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[7] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	ExplodingObject(glm::vec3 baseColor, glm::vec3 basePos) {
		this->startState = true;
		this->particleNumber = EXPLODINGBILLBOARDPARTICLES;
		this->redColor = glm::vec4(baseColor, 1.0);
		this->redPosition = basePos;
		cout << "a\n";
		ShaderInfo shadersA[] = {
		{ GL_VERTEX_SHADER, "fire_billboarding.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "fire_billboarding.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
		cout << "b\n";
		this->program = LoadShaders(shadersA);
		cout << "d\n";
		//load texture
		TextureData fireTexData = Common::Load_png("../Assets/textures/exploding-billboarding/exploding.png");
		if (fireTexData.data == nullptr) {
			std::cout << "error on loading fire texture data!\n";
			std::exit(EXIT_FAILURE);
		}
		//initialize and set texture parameter
		glGenTextures(1, &this->fireTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fireTexData.width, fireTexData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fireTexData.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//connect texture
		this->fireSamplerIDEnumeration = glGetUniformLocation(this->program, "fireTex");
		glUseProgram(this->program);
		glUniform1i(this->fireSamplerIDEnumeration, 0);
		//get uniform locations
		//for vertex shader input uniform
		this->viewID = glGetUniformLocation(this->program, "viewMatrix");
		this->projID = glGetUniformLocation(this->program, "projMatrix");
		this->totalTimeID = glGetUniformLocation(this->program, "totalTime");
		this->centralFirePosID = glGetUniformLocation(this->program, "centralFirePos");
		//for fragment shader input uniform
		this->baseColorID = glGetUniformLocation(this->program, "baseColor");
		this->fragmentTimeID = glGetUniformLocation(this->program, "fragmentTime");
		//VAO and VBO locations
		glGenVertexArrays(1, &this->fireVAO);
		glGenBuffers(1, &this->fireVBO);
		billboardCorners[0] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[1] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[2] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[3] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[4] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[5] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[6] = -EXPLODINGBILLBOARDPARTICLESIZE;
		billboardCorners[7] = EXPLODINGBILLBOARDPARTICLESIZE;
		billboardTexCoords[0] = 0;
		billboardTexCoords[1] = 0;
		billboardTexCoords[2] = 1.0;
		billboardTexCoords[3] = 0;
		billboardTexCoords[4] = 1.0;
		billboardTexCoords[5] = 1.0;
		billboardTexCoords[6] = 0;
		billboardTexCoords[7] = 1.0;
		glGenBuffers(1, &this->fireEBO);
	}
	~ExplodingObject() {

	}
	void initParticles() {
		for (int i = 0; i < this->particleNumber; i++) {
			float tempLiftime = EXPLODINGLIFETIMEMAX * (rand() / static_cast<float>(RAND_MAX));
			//float tempYVelo = 0.1 * (rand() / static_cast<float>(RAND_MAX));
			float tempYVelo = EXPLODINGBILLBOARDMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));

			float centerDiameter = EXPLODINGBILLBOARDPARTICLEDIAMETER;
			float centerRadius = centerDiameter / 2;

			//decide the x y position, random generate
			float xStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float yStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;
			float zStart = centerDiameter * (rand() / static_cast<float>(RAND_MAX)) - centerRadius;

			//scale the result
			xStart /= 10;
			yStart /= 10;
			zStart /= 10;

			//float tempXVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempXVelo = EXPLODINGBILLBOARDMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));
			if (xStart > 0) {
				tempXVelo *= -1;
			}

			//float tempZVelo = 0.02 * (rand() / static_cast<float>(RAND_MAX));
			float tempZVelo = EXPLODINGBILLBOARDMAXUPSPEED * (rand() / static_cast<float>(RAND_MAX));
			if (zStart > 0) {
				tempZVelo *= -1;
			}

			if (yStart > 0) {
				tempYVelo *= -1;
			}

			//pushing to vector
			for (int rep = 0; rep < 4; rep++) {
				parcLifeTime[4 * i + rep] = tempLiftime;
				parcCorners[8 * i + 2 * rep] = billboardCorners[rep * 2];
				parcCorners[8 * i + 2 * rep + 1] = billboardCorners[rep * 2 + 1];

				parcTexCoords[8 * i + 2 * rep] = billboardTexCoords[rep * 2];
				parcTexCoords[8 * i + 2 * rep + 1] = billboardTexCoords[rep * 2 + 1];

				centerOffsets[12 * i + 3 * rep] = xStart;
				centerOffsets[12 * i + 3 * rep + 1] = yStart + abs(xStart / 2.0);
				centerOffsets[12 * i + 3 * rep + 2] = zStart;

				parcVelo[12 * i + 3 * rep] = tempXVelo;
				parcVelo[12 * i + 3 * rep + 1] = tempYVelo;
				parcVelo[12 * i + 3 * rep + 2] = tempZVelo;
			}
			vertexIndices[6 * i] = (4 * i);
			vertexIndices[6 * i + 1] = (1 + 4 * i);
			vertexIndices[6 * i + 2] = (2 + 4 * i);
			vertexIndices[6 * i + 3] = (4 * i);
			vertexIndices[6 * i + 4] = (2 + 4 * i);
			vertexIndices[6 * i + 5] = (3 + 4 * i);
		}
	}
	void bindTexture() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->fireTextureID);
	}
	void enableBlending() {
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	void disableBlending() {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	void shutdown() {
		//only unactivate program
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void resetData() {

		this->startState = true;
		this->startTime = glfwGetTime();
		this->clockTime = 3;
	}
	void updateUniform(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		glUniformMatrix4fv(this->viewID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(this->projID, 1, GL_FALSE, &projMatrix[0][0]);

		float currentTime = glfwGetTime();
		clockTime += (currentTime - this->startTime);
		this->startTime = currentTime;

		glUniform1fv(this->totalTimeID, 1, &this->clockTime);
		glUniform1fv(this->fragmentTimeID, 1, &this->clockTime);

		glUniform3fv(this->centralFirePosID, 1, &this->redPosition[0]);
		glUniform4fv(this->baseColorID, 1, &this->redColor[0]);
	}
	void updateParticles() {
		//Update VBO array
		glBindVertexArray(this->fireVAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->fireVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(parcLifeTime) + sizeof(parcCorners) + sizeof(parcTexCoords) + sizeof(centerOffsets) + sizeof(parcVelo), NULL, GL_DYNAMIC_DRAW);

		//sub data to buffer one by one, and attach to VAO
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLuint offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcLifeTime), parcLifeTime);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcLifeTime);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcTexCoords), parcTexCoords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcTexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcCorners), parcCorners);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcCorners);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(centerOffsets), centerOffsets);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(centerOffsets);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(parcVelo), parcVelo);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		offset += sizeof(parcVelo);
		//update EBO array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->fireEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
	}
	void Render(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
		if (startState) {
			initParticles();
			resetData();
			startState = false;
		}
		int rn = rand() % EXPLODINGBILLBOARDPARTICLES;
		//cout << "random x texture: " << parcTexCoords[2 * rn] << " random y texture: " << parcTexCoords[2*rn+1] << "\n";
		glUseProgram(this->program);
		bindTexture();
		updateUniform(viewMatrix, projMatrix);
		updateParticles();
		enableBlending();

		//do render
		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, sizeof(vertexIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
		glEnable(GL_DEPTH_TEST);
		//cout << "pass here\n";
		//glDrawArrays(GL_TRIANGLES, 0, particleNumber);
		disableBlending();
		shutdown();
	}
private:
	GLuint program;
	GLuint fireVAO;
	GLuint fireVBO;
	GLuint fireEBO; //for instanced rendering
	//three textures (each will use different parameter)
	GLuint fireTextureID;
	GLuint fireSamplerIDEnumeration;

	//for vertex shader input uniform
	GLuint viewID;
	GLuint projID;
	GLuint totalTimeID;
	GLuint centralFirePosID;
	//for fragment shader input uniform
	GLuint baseColorID;
	GLuint fragmentTimeID;
};