#pragma once
#include "include_header.h"
class MainShader :public ShaderObject {
public:
	MainShader() {

	}
	~MainShader() {

	}
	bool Init() {

	}
	void SetMatVP(glm::mat4 v, glm::mat4 p);
	void SetModel(glm::mat4 m);
	void SetvLightPosition(glm::vec3 vec);
	void SetvViewPos(glm::vec3 vec);
	void SetLightSpaceMatrix(glm::mat4 mat);
	void SetMaterialInfo(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks);
	void Set3LightingColors(glm::vec4 a, glm::vec4 d, glm::mat4 s);
	void SetShininess(float s);
	void SetTextureID(float s);
	void DisableTexture();
private:
};