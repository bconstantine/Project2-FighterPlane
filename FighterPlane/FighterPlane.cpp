#include "main.h"

const float fireColorR = 226.0 / 255;
const float fireColorG = 88.0 / 255;
const float fireColorB = 34.0 / 255;

string partsList[PARTSNUM] = {"10593_Fighter_Jet_SG_v1_iterations-2"};

float initialOffset[][3] = {
	{0,0,0}
};

float translatePart[PARTSNUM][3];
float rotatePart[PARTSNUM][3];

int main(int argc, char** argv) {
	//init glfw, parameters, shaders, gui
	GLFWwindow* window = initProgramGLFW();
	
	//init fonts (need to be same scope with main)
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	font1 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
	if (font1 == NULL) {
		cout << "font load unsuccessful\n";
	}
	else {
		cout << "font load successful\n";
	}
	IM_ASSERT(font1 != NULL);
	io.Fonts->Build();
	titleFont = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 30.0f);
	if (titleFont == NULL) {
		cout << "title font load unsuccessful\n";
	}
	else {
		cout << "title font load successful\n";
	}
	IM_ASSERT(titleFont != NULL);
	io.Fonts->Build();

	//loadObj and show the progress to the window
	for (int i = 0; i < PARTSTOTAL+1; i++) {
		loadingProgress[i] = false;
	}
	Obj2Buffer(window);

	double lastTime = glfwGetTime();
	double delta = 1.0 / TARGET_FPS;
	double sz = 400;
	/*quat qt2(1.0, 0, 0, 0);
	vec3 light(1.0, 0, 0);*/
	while (!glfwWindowShouldClose(window)) {
		currentTime = glfwGetTime();
		mixVal = 0.25 * sin(currentTime * 0.5) + 0.5;
		// cout << mixVal << endl;
		//Keep running, put the code here
		//correct eyeAngleY
		if (eyeAngley < 0) {
			eyeAngley = 360 + eyeAngley;
		}
		else if (eyeAngley > 360) {
			eyeAngley = eyeAngley - 360;
		}


		 // Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//push font
		ImGui::PushFont(font1);
		
		//ImGui::ShowDemoWindow(&show_demo_window);

		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(400,SCREENHEIGHT), ImVec2(400,SCREENHEIGHT)); //width x height fixed
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::Begin("Miscellaneous Toolbox");

			//title 
			ImGui::PushFont(titleFont);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
			ImGui::Text("Fighter Plane Project");
			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("Warning! If you are using BoPoMoFo Keyboard");
			ImGui::Text("Press Shift once, change the keyboard to English...");
			ImGui::PopStyleColor();
			ImGui::Text("Press ESC to quit...");

			//for debugging only
			//ImGui::Checkbox("Demo Window", &show_demo_window);
			if (ImGui::CollapsingHeader("Lighting related")) {
				ImGui::Checkbox("Show Lighting box", &renderLightBox);
				ImGui::SliderFloat("Light position X", &lightPosition[0], -TRANSLATEXLIMIT, TRANSLATEXLIMIT);
				ImGui::SliderFloat("Light position Y", &lightPosition[1], TRANSLATEYLOWERLIMIT, TRANSLATEYUPPERLIMIT);
				ImGui::SliderFloat("Light position Z", &lightPosition[2], -TRANSLATEZLIMIT, TRANSLATEZLIMIT);
				static int tempRadioVar; 
				if (ImGui::Button("Reset Lighting")) {
					lightingFollowRobocop = false;
					discoticLighting = false;
					lightPosition[0] = 10;
					lightPosition[1] = 25;
					lightPosition[2] = 10;
					ambientColor[0] = 0.1;
					ambientColor[1] = 0.1;
					ambientColor[2] = 0.1;
					diffuseColor[0] = 0.8;
					diffuseColor[1] = 0.8;
					diffuseColor[2] = 0.8;
					specularColor[0] = 1;
					specularColor[1] = 1;
					specularColor[2] = 1;
					Shininess = 32;
				}
				if (ImGui::TreeNode("Ambient Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&ambientColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Diffuse Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&diffuseColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Specular Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&specularColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Shininess")) {
					ImGui::SliderFloat("Shininess", &Shininess, 2.0, 200);
					ImGui::TreePop();
				}
				/*if (ImGui::TreeNode("Point Lighting Variables")) {
					ImGui::SliderFloat("Kconstant", &kConstant, 0.1, 5);
					ImGui::SliderFloat("Klinear", &kLinear, 0.005, 0.8);
					ImGui::SliderFloat("Kquadratic", &kQuadratic, 0.001, 0.05);
					ImGui::TreePop();
				}*/
			}
			if (ImGui::CollapsingHeader("Projection Related")) {
				ImGui::SliderFloat("Change FoV: ", &FoV, 30.0f, 90.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("Change near clip: ", &nearClip, 0.1f, 10.f);
				ImGui::SliderFloat("Change far clip: ", &farClip, 50.f, 200.f);
			}
			if (ImGui::CollapsingHeader("View Related ")) {
				viewChange = true;
				ImGui::Text("Control for keyboard: ");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW UP "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for zoom in");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW DOWN "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for zoomout");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW LEFT "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for turning left");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW RIGHT "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for turning right");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ESC "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for quit");
				ImGui::Separator();
				ImGui::Text("Or use the slider below...");
				ImGui::SliderFloat("Change zoom", &eyedistance, 10.0f, 40.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("Change eye rotation", &eyeAngley, 0.f, 360.f);
			}
			//comment, this is only for debugging
			//if (ImGui::CollapsingHeader("lightproject: ")) {
			//	ImGui::SliderFloat("Change x: ", &lightProjectX, 0, 400.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//	ImGui::SliderFloat("Change y: ", &lightProjectY, 0.f, 400.f);
			//	ImGui::SliderFloat("Change z: ", &lightProjectZ, 0.f, 10.f);
			//	ImGui::SliderFloat("Change z far: ", &lightProjectZFar, 10.f, 400.f);
			//	lightProjection = glm::ortho(-lightProjectX, lightProjectX, -lightProjectY, lightProjectY, lightProjectZ, lightProjectZFar);
			//}
			if (ImGui::CollapsingHeader("Current Modes")) {
				ImGui::Text("Press the circle buttons below...");
				static int currentMode;
				static string animateInfo;
				if (planeMode == NORMAL) {
					currentMode = 0;
				}
				else if (planeMode == REFLECT) {
					currentMode = 1;
				}
				else if (planeMode == REFRACT) {
					currentMode = 2;
				}
				else if (planeMode == AURA) {
					currentMode = 3;
				}
				else if (planeMode == ONFIRE) {
					currentMode = 4;
				}
				else if (planeMode == FLYNEARWATER) {
					currentMode = 5;
				}
				else if (planeMode == ACTION) {
					currentMode = 6;
				}
				//-----------------------------------------------
				else if (planeMode == TOONSHADER) {
					currentMode = 7;
				}
				else if (planeMode == MOZAIC) {
					currentMode = 8;
				}
				else if (planeMode == SHADERMATERIAL) {
					currentMode = 9;
				}
				//-----------------------------------------------
				ImGui::RadioButton("Normal", &currentMode, 0);
				ImGui::RadioButton("Reflect", &currentMode, 1);
				ImGui::RadioButton("Refract", &currentMode, 2); 
				ImGui::RadioButton("Aura", &currentMode, 3); 
				ImGui::RadioButton("On Fire (LOUD SOUND WARNING)", &currentMode, 4); 
				ImGui::RadioButton("Fly near water", &currentMode, 5); 
				ImGui::RadioButton("Motion Action", &currentMode, 6); 
				ImGui::RadioButton("Toon Shader", &currentMode, 7);
				ImGui::RadioButton("Mozaic", &currentMode, 8);
				ImGui::RadioButton("Shader Material", &currentMode, 9);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Separator();
				//ImGui::Text("Wait until each move is done before starting the next one!");
				ImGui::PopStyleColor();
				if (currentMode == 0) {
					if (planeMode != NORMAL) {
						modeChangeExist = true;
					}
					planeMode = NORMAL;
					animateInfo = "NORMAL";
				}
				else if (currentMode == 1) {
					if (planeMode != REFLECT) {
						modeChangeExist = true;
					}
					planeMode = REFLECT;
					animateInfo = "REFLECT";
				}
				else if (currentMode == 2) {
					if (planeMode != REFRACT) {
						modeChangeExist = true;
					}
					planeMode = REFRACT;
					animateInfo = "REFRACT";
				}
				else if (currentMode == 3) {
					if (planeMode != AURA) {
						modeChangeExist = true;
					}
					planeMode = AURA;
					animateInfo = "AURA";
				}
				else if (currentMode == 4) {
					if (planeMode != ONFIRE) {
						modeChangeExist = true;
					}
					planeMode = ONFIRE;
					animateInfo = "ON FIRE";
				}
				else if (currentMode == 5) {
					if (planeMode != FLYNEARWATER) {
						modeChangeExist = true;
					}
					planeMode = FLYNEARWATER;
					animateInfo = "FLY NEAR WATER";
				}
				else if (currentMode == 6) {
					if (planeMode != ACTION) {
						modeChangeExist = true;
					}
					planeMode = ACTION;
					animateInfo = "MOTION ACTION";
				}
				//-----------------------------------------------
				else if (currentMode == 7) {
					if (planeMode != TOONSHADER) {
						modeChangeExist = true;
					}
					planeMode = TOONSHADER;
					animateInfo = "TOON SHADER";
				}
				else if (currentMode == 8) {
					if (planeMode != MOZAIC) {
						modeChangeExist = true;
					}
					planeMode = MOZAIC;
					animateInfo = "MOZAIC";
				}
				else if (currentMode == 9) {
					if (planeMode != SHADERMATERIAL) {
						modeChangeExist = true;
					}
					planeMode = SHADERMATERIAL;
					animateInfo = "SHADER RMATERIAL";
				}
				if (currentMode == 8 && ImGui::TreeNode("Mozaic Mode")) {
					ImGui::Text("Select the desired Mozaic");
					ImGui::Text("and press PLOK to toggle mozaic intensity...");
					ImGui::Text("Control for keyboard: ");
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
					ImGui::BulletText("P "); ImGui::SameLine();
					ImGui::PopStyleColor();
					ImGui::Text("for intensity += 1.0");
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
					ImGui::BulletText("L "); ImGui::SameLine();
					ImGui::PopStyleColor();
					ImGui::Text("for intensity -= 1.0");
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
					ImGui::BulletText("O "); ImGui::SameLine();
					ImGui::PopStyleColor();
					ImGui::Text("for intensity += 0.01");
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
					ImGui::BulletText("K "); ImGui::SameLine();
					ImGui::PopStyleColor();
					ImGui::Text("for intensity -= 0.01");
					static int currentMozaic;
					currentMozaic = shader_now;
					//-----------------------------------------------
					ImGui::RadioButton("None", &currentMozaic, 0);
					ImGui::RadioButton("Mozaic1", &currentMozaic, 8);
					ImGui::RadioButton("Mozaic2", &currentMozaic, 9);
					ImGui::RadioButton("Mozaic3", &currentMozaic, 10);
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					ImGui::Separator();
					//clampign mozaic val
					if (shader_now == 8) {
						mosaic_val = clampValMaxMin(mosaic_val, 30, 15);
					}
					else if (shader_now == 9) {
						mosaic_val = clampValMaxMin(mosaic_val, 0.05, 0.005);
					}
					else if (shader_now == 10) {
						mosaic_val = clampValMaxMin(mosaic_val, 0.05, 0.005);
					}
					//ImGui::Text("Wait until each move is done before starting the next one!");
					ImGui::PopStyleColor();
					shader_now = currentMozaic;
					ImGui::Text("Mozaic intensity = "); ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					ImGui::Text("%.2f", mosaic_val);
					ImGui::PopStyleColor();
					ImGui::TreePop();
				}
				//-----------------------------------------------
				ImGui::Text("Current Mode = "); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
				ImGui::Text("%s", animateInfo.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::Separator();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			/*ImGui::Text("Current X Position: %.3f", translatePart[0][0]);
			ImGui::Text("Current Y Position: %.3f", translatePart[0][1]);
			ImGui::Text("Current Z Position: %.3f", translatePart[0][2]);*/
			ImGui::End();
		}
		//popping font
		ImGui::PopFont();

		//check based on animation
		if (modeChangeExist) {
			resetGlobalVariables(); 
			resetModel();
			viewChange = true;
			shader_now = 0;
			toon_active = 0;
			motionBlur_toggle = false;
			doSpin = false;
			
			if (planeMode == REFLECT) {
				emACK = 1;
			}
			else if (planeMode == REFRACT) {
				emACK = 2;
			}
			//-----------------------------------------------
			else if (planeMode == SHADERMATERIAL) {
				emACK = 4;
			}
			//-----------------------------------------------
			/*else if (planeMode == TOONSHADER) {
				emACK = 5;
			}*/
			else if (planeMode == ONFIRE) {
				emACK = 0;
				fireStart = glfwGetTime();
			}
			else {
				emACK = 0;
			}
			//cout << emACK << endl;
			modeChangeExist = false;
		}
		
		if (planeMode == NORMAL) {
			
		}
		else if (planeMode == REFLECT) {
			
		}
		else if (planeMode == REFRACT) {
			
		}
		else if (planeMode == AURA) {
			
		}
		else if (planeMode == ONFIRE) {
			
		}
		else if (planeMode == FLYNEARWATER) {
			
		}
		//-----------------------------------------------
		else if (planeMode == ACTION) {
			motionBlur_toggle = true;
			doSpin = true;
			planeSpin();
		}
		else if (planeMode == TOONSHADER) {
			toon_active = 1;
		}
		else if (planeMode == MOZAIC) {
			//shader_now = 8;
		}
		else if (planeMode == SHADERMATERIAL) {

		}
		//-----------------------------------------------

		displayGLFW(window);

		//frame rate limit
		while (glfwGetTime() < lastTime + delta) {

		}
		lastTime = glfwGetTime();
		
	}


	//stop imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//stop glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void resetGlobalVariables() {
	///////////
	

	//for fire
	fireGenerate->resetData(); 
	//for aura
	blueAura->resetData();
	wing1Aura->resetData();
	wing2Aura->resetData();
	fuselageAura->resetData();
	//for smoke
	centralSmoke->resetData();
	thrusterSmoke->resetData();
	//exploding object
	explodingObject->resetData();

	fireMode = 0;
}

void resetModel() {
	for (int i = 0; i < PARTSNUM; i++) {
		if (true) {
			translatePart[i][0] = 0.f;
			translatePart[i][1] = 0.f;
			translatePart[i][2] = 0.f;
			rotatePart[i][0] = 0.f;
			rotatePart[i][1] = 0.f;
			rotatePart[i][2] = 0.f;
		}
		//else {
		//	//for crotch, only reset the y of translate, but skip the y axis of rotate
		//	translatePart[i][1] = 0.f;
		//	rotatePart[i][0] = 0.f;
		//	rotatePart[i][2] = 0.f;
		//}
	}
}

GLFWwindow* initProgramGLFW() {
	resetModel();
	ModelBackground = glm::mat4(1.0);
	ModelBackground = glm::translate(ModelBackground, glm::vec3(0, backGroundShiftUp, 0));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(error_callback);

	//create window and context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_REFRESH_RATE, TARGET_FPS);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCREENWIDTH = mode->width;
	SCREENHEIGHT = mode->height;

	//fullscreen
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Project2 - FighterPlane", glfwGetPrimaryMonitor(), NULL);
	
	//windowed full screen
	/*glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Project2 - FighterPlane", NULL, NULL);*/

	if (!window) {
		//window creation failed, check whether version apply with the machine
		cout << "window creation failed\n";
		exit(EXIT_FAILURE);
	}

	//callback functions
	glfwSetKeyCallback(window, KeyboardGLFW);
	glfwMakeContextCurrent(window);

	//init glew
	glewExperimental = GLFW_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "glew init failed: " << glewGetErrorString(err) << endl;
		exit(EXIT_FAILURE);
	}



	//multisample for polygons smooth
	//glfw by default already use double buffering, depth buffer
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glfwSwapInterval(0); // how many frame update before updating the screen? set 0 for no limit

	//init imgui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform / Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//for white light boxes
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glGenBuffers(1, &lightBoxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lightBoxBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightModel), lightModel, GL_STATIC_DRAW);
	glGenBuffers(1, &lightBoxIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightBoxIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(recTriangleStripOrder), recTriangleStripOrder, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//for shadow mapping
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//depth map we only care to its depth, so set format ot GL_DEPTH_COMPONENT. Set the width and height as desired
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//parameterized the texture
	//minification filter. It is applied when an image is zoomed out so far that multiple pixels
	//Nearest neigbor filtering, does not attempt to scale the image
	//Result is sharp / pixelated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//fix oversampling the outer region that is not reached by the light, make them area unreachable with the border to CLAMP to border
	//and also set the border color to white, which means that the unreachable light basically clamped to it, no shadow
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//bind the frame buffer to the generated depth frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	//we only need depth information when rendering shadow, color will not affect, so no need to put color
	//However, framebuffer object however needs color buffer, so we tell opengl we won't need color data by telling
	//DrawBuffer and ReadBuffer to GL_NONE
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//for shadow mapping
	glGenFramebuffers(1, &depthMapPlaneFBO);
	glGenTextures(1, &depthMapPlane);
	glBindTexture(GL_TEXTURE_2D, depthMapPlane);
	//depth map we only care to its depth, so set format ot GL_DEPTH_COMPONENT. Set the width and height as desired
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_PLANE_WIDTH, SHADOW_PLANE_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//parameterized the texture
	//minification filter. It is applied when an image is zoomed out so far that multiple pixels
	//Nearest neigbor filtering, does not attempt to scale the image
	//Result is sharp / pixelated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//fix oversampling the outer region that is not reached by the light, make them area unreachable with the border to CLAMP to border
	//and also set the border color to white, which means that the unreachable light basically clamped to it, no shadow
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//bind the frame buffer to the generated depth frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapPlaneFBO);

	//we only need depth information when rendering shadow, color will not affect, so no need to put color
	//However, framebuffer object however needs color buffer, so we tell opengl we won't need color data by telling
	//DrawBuffer and ReadBuffer to GL_NONE
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapPlane, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//check the frame buffer, if no problem, unbind
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind frame buffer

	//-----------------------------------------------+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	width_size = SCREENWIDTH - widthStart;
	height_size = SCREENHEIGHT - heightStart;

	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vbo2);
	glBindVertexArray(vao2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(window_positions), &window_positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureData);
	glBindTexture(GL_TEXTURE_2D, textureData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureData, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: FBO is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboLast1);
	glBindFramebuffer(GL_FRAMEBUFFER, fboLast1);

	glGenTextures(1, &textureLast1);
	glBindTexture(GL_TEXTURE_2D, textureLast1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLast1, 0);

	glGenRenderbuffers(1, &rboLast1);
	glBindRenderbuffer(GL_RENDERBUFFER, rboLast1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboLast1);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: fboLast1 is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboLast2);
	glBindFramebuffer(GL_FRAMEBUFFER, fboLast2);

	glGenTextures(1, &textureLast2);
	glBindTexture(GL_TEXTURE_2D, textureLast2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLast2, 0);

	glGenRenderbuffers(1, &rboLast2);
	glBindRenderbuffer(GL_RENDERBUFFER, rboLast2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboLast2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: fboLast2 is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboLast3);
	glBindFramebuffer(GL_FRAMEBUFFER, fboLast3);

	glGenTextures(1, &textureLast3);
	glBindTexture(GL_TEXTURE_2D, textureLast3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLast3, 0);

	glGenRenderbuffers(1, &rboLast3);
	glBindRenderbuffer(GL_RENDERBUFFER, rboLast3);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboLast3);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: fboLast1 is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboLast4);
	glBindFramebuffer(GL_FRAMEBUFFER, fboLast4);

	glGenTextures(1, &textureLast4);
	glBindTexture(GL_TEXTURE_2D, textureLast4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLast4, 0);

	glGenRenderbuffers(1, &rboLast4);
	glBindRenderbuffer(GL_RENDERBUFFER, rboLast4);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboLast4);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: fboLast4 is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &fboLast5);
	glBindFramebuffer(GL_FRAMEBUFFER, fboLast5);

	glGenTextures(1, &textureLast5);
	glBindTexture(GL_TEXTURE_2D, textureLast5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLast4, 0);

	glGenRenderbuffers(1, &rboLast5);
	glBindRenderbuffer(GL_RENDERBUFFER, rboLast5);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboLast5);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: fboLast5 is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TextureData planeTex = Common::Load_png("../Assets/Obj/10593_Fighter_Jet_SG_v1_diffuse.jpg");
	if (planeTex.data == nullptr) {
		std::cout << "error on loading smoke texture data!\n";
		std::exit(EXIT_FAILURE);
	}
	//initialize and set texture parameter
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, planeTex.width, planeTex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, planeTex.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	TextureData lavaTex1 = Common::Load_png("../Assets/textures/lava/lava1.jpg");
	if (lavaTex1.data == nullptr) {
		std::cout << "error on loading smoke texture data!\n";
		std::exit(EXIT_FAILURE);
	}
	//initialize and set texture parameter
	glGenTextures(1, &textureLava1);
	glBindTexture(GL_TEXTURE_2D, textureLava1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lavaTex1.width, lavaTex1.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, lavaTex1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	TextureData lavaTex2 = Common::Load_png("../Assets/textures/lava/lava2.jpg");
	if (lavaTex2.data == nullptr) {
		std::cout << "error on loading smoke texture data!\n";
		std::exit(EXIT_FAILURE);
	}
	//initialize and set texture parameter
	glGenTextures(1, &textureLava2);
	glBindTexture(GL_TEXTURE_2D, textureLava2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lavaTex2.width, lavaTex2.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, lavaTex2.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//-----------------------------------------------+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	TextureData newTex = Common::Load_png("../Assets/textures/normalTexture/tex.jpg");
	if (newTex.data == nullptr) {
		std::cout << "error on loading TEX texture data!\n";
		std::exit(EXIT_FAILURE);
	}
	//initialize and set texture parameter
	glGenTextures(1, &textureNew);
	glBindTexture(GL_TEXTURE_2D, textureNew);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTex.width, newTex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newTex.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	TextureData newTexNorm = Common::Load_png("../Assets/textures/normalTexture/texnorm.png");
	if (newTexNorm.data == nullptr) {
		std::cout << "error on loading TEXNORM texture data!\n";
		std::exit(EXIT_FAILURE);
	}
	//initialize and set texture parameter
	glGenTextures(1, &textureNewNormal);
	glBindTexture(GL_TEXTURE_2D, textureNewNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTexNorm.width, newTexNorm.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newTexNorm.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//upload shaders
	//main shader
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "blinn_phong_obj.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "blinn_phong_obj.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	program = LoadShaders(shaders);

	//white box shader, separate so not affected by lighting
	ShaderInfo lightshaders[] = {
		{ GL_VERTEX_SHADER, "light_box.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "light_box.fp.glsl" },//fragment shader
		{ GL_NONE, NULL }
	};
	renderLightProgram = LoadShaders(lightshaders);

	//for shadow mapping shader
	ShaderInfo depthshaders[] = {
		{ GL_VERTEX_SHADER, "shadow.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "shadow.fp.glsl" },//fragment shader
		{ GL_NONE, NULL }
	};
	depthProgram = LoadShaders(depthshaders);

	//Skybox
	ShaderInfo skyboxShaders[] = {
		{ GL_VERTEX_SHADER, "skybox_shader.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "skybox_shader.fp.glsl" },//fragment shader
		{ GL_NONE, NULL }
	};
	skyboxProgram = LoadShaders(skyboxShaders);

	//-----------------------------------------------
	ShaderInfo shaders2[] = {
		{ GL_VERTEX_SHADER, "program2.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "program2.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	program2 = LoadShaders(shaders2);

	ShaderInfo motionBlurShaders[] = {
		{ GL_VERTEX_SHADER, "motion_blur.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "motion_blur.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	motionBlurProgram = LoadShaders(motionBlurShaders);
	//-----------------------------------------------

	//get uniform locations
	//for white light box
	glUseProgram(renderLightProgram);
	ProjectionMatrixID = glGetUniformLocation(renderLightProgram, "Projection");
	ViewMatrixID = glGetUniformLocation(renderLightProgram, "View");
	ModelMatrixID = glGetUniformLocation(renderLightProgram, "ModelMatrix");

	//for shadow mapping shader
	glUseProgram(depthProgram);
	lightSpaceID = glGetUniformLocation(depthProgram, "lightSpaceMatrix");
	lightModelID = glGetUniformLocation(depthProgram, "model");

	//for main shader
	glUseProgram(program);
	MatricesIdx = glGetUniformBlockIndex(program, "MatVP"); //view+projection
	ModelID = glGetUniformLocation(program, "Model"); // model
	lightPosID = glGetUniformLocation(program, "vLightPosition"); //vec3 of light position
	//Ka, Kd, Ks of each used mtl
	M_KaID = glGetUniformLocation(program, "Material.Ka");
	M_KdID = glGetUniformLocation(program, "Material.Kd");
	M_KsID = glGetUniformLocation(program, "Material.Ks");
	//vec4 of chosen ambient, diffuse and specular color
	ambientID = glGetUniformLocation(program, "ambientColor");
	diffuseID = glGetUniformLocation(program, "diffuseColor");
	specularID = glGetUniformLocation(program, "specularColor");
	//float to indicate shininess index for specularity
	ShininessID = glGetUniformLocation(program, "Shininess");
	//vec3 position of where my camera location is now, for blinn-phong technique
	varyingViewPosID = glGetUniformLocation(program, "varyingViewPos");
	permanentViewPosID = glGetUniformLocation(program, "permanentViewPos");
	//clip plane for the water
	clipPlaneID = glGetUniformLocation(program, "clipPlane");
	//the same mat4 lightspacematrix for depth map, but used again for this shader
	lightSpaceOrigID = glGetUniformLocation(program, "lightSpaceMatrix");
	//whether enviromnemt mapping is activated or not
	enviromentMappingACKID = glGetUniformLocation(program, "EnvironmentMappingACK");
	//depth map
	depthmapEnumerationID = glGetUniformLocation(program, "shadowMap");
	glUniform1i(depthmapEnumerationID, 0);
	cubemapEnumerationID = glGetUniformLocation(program, "cubemap");
	glUniform1i(cubemapEnumerationID, 1);
	//-----------------------------------------------
	texPlane_Loc = glGetUniformLocation(program, "texPlane");
	glUniform1i(texPlane_Loc, 2);
	texLava1_Loc = glGetUniformLocation(program, "texLava1");
	glUniform1i(texLava1_Loc, 3);
	texLava2_Loc = glGetUniformLocation(program, "texLava2");
	glUniform1i(texLava2_Loc, 4);
	textureNewID = glGetUniformLocation(program, "textureNew");
	glUniform1i(textureNewID, 5);
	textureNewNormalID = glGetUniformLocation(program, "textureNewNormal");
	glUniform1i(textureNewNormalID, 6);
	//-----------------------------------------------
	//whether now is reflection
	isReflectionModeID = glGetUniformLocation(program, "isReflection");
	waterPlanePointID = glGetUniformLocation(program, "waterPlanePoint");
	waterPlaneNormalID = glGetUniformLocation(program, "waterPlaneNormal");
	//-----------------------------------------------(((
	Toon_active_Loc = glGetUniformLocation(program, "toon_active");
	currentTime_Loc = glGetUniformLocation(program, "currentTime");
	mixVal_Loc = glGetUniformLocation(program, "mixVal");
	
	//-----------------------------------------------(((


	glUseProgram(skyboxProgram);
	skyboxViewID = glGetUniformLocation(skyboxProgram, "view");
	skyboxProjID = glGetUniformLocation(skyboxProgram, "projection");
	skyboxEnumerationID = glGetUniformLocation(skyboxProgram, "skybox");
	isReflectionModeSkyboxID = glGetUniformLocation(skyboxProgram, "isReflection");
	waterPlanePointSkyboxID = glGetUniformLocation(skyboxProgram, "waterPlanePoint");
	waterPlaneNormalSkyboxID = glGetUniformLocation(skyboxProgram, "waterPlaneNormal");
	//-----------------------------------------------
	Toon_active_Loc2 = glGetUniformLocation(skyboxProgram, "toon_active");
	//-----------------------------------------------
	glUniform1i(skyboxEnumerationID, 0);
	//cubemap for background
	cubemapID = loadCubemapPNG(textures_faces); // INI JUGA

	//-----------------------------------------------
	// for program2 shader
	glUseProgram(program2);
	Shader_now_Loc = glGetUniformLocation(program2, "shader_now");
	Mosaic_val_Loc = glGetUniformLocation(program2, "mosaic_val");
	Final_Width_Loc = glGetUniformLocation(program2, "width_size");
	Final_Height_Loc = glGetUniformLocation(program2, "height_size");
	tex_Loc = glGetUniformLocation(program2, "tex");
	glUniform1i(tex_Loc, 0);

	glUseProgram(motionBlurProgram);
	tex1Loc = glGetUniformLocation(motionBlurProgram, "tex1");
	tex2Loc = glGetUniformLocation(motionBlurProgram, "tex2");
	tex3Loc = glGetUniformLocation(motionBlurProgram, "tex3");
	tex4Loc = glGetUniformLocation(motionBlurProgram, "tex4");
	tex5Loc = glGetUniformLocation(motionBlurProgram, "tex5");
	glUniform1i(tex1Loc, 0);
	glUniform1i(tex2Loc, 1);
	glUniform1i(tex3Loc, 2);
	glUniform1i(tex4Loc, 3);
	glUniform1i(tex5Loc, 4);
	//-----------------------------------------------
	
	//projection matrix, FOR USER NOT FOR LIGHT!
	Projection = glm::perspective(80.0f, (float)(SCREENWIDTH) / (SCREENHEIGHT), 0.1f, 100.f);
	
	//view matrix for user, not light
	//viewPos[0] = translatePart[0][0];
	//viewPos[1] = translatePart[0][1]+robotShiftUp+2;
	//viewPos[2] = translatePart[0][2]+eyedistance;
	//View = glm::lookAt(
	//	glm::vec3(viewPos[0],viewPos[1], viewPos[2]), // Camera is at (0,0,20), in World Space), // Camera is at (0,10,25), in World Space
	//	glm::vec3(translatePart[0][0], translatePart[0][1] + robotShiftUp, translatePart[0][2]), // and looks at the origin
	//	glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	//);

	viewPos[0] = 0;
	viewPos[1] = robotShiftUp + 10;
	viewPos[2] = eyedistance;
	View = glm::lookAt(
		glm::vec3(viewPos[0], viewPos[1], viewPos[2]), // Camera is at (0,0,20), in World Space), // Camera is at (0,10,25), in World Space
		glm::vec3(0, robotShiftUp, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);

	//UBO
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, UBOsize);
	glUniformBlockBinding(program, MatricesIdx, 0);

	//init fireEngine
	fireGenerate = new FireBillboardObject();
	blueAura = new AuraBillboardObject(blueColor, glm::vec3(0,0,0));
	wing1Aura = new AuraBillboardObject(blueColor, glm::vec3(WINGXPOSITION, WINGYPOSITION, WINGZPOSITION));
	wing2Aura = new AuraBillboardObject(blueColor, glm::vec3(-WINGXPOSITION, WINGYPOSITION, WINGZPOSITION));
	fuselageAura = new AuraBillboardObject(blueColor, glm::vec3(FUSELAGEXPOSITION, FUSELAGEYPOSITION, FUSELAGEZPOSITION));
	waterPlatform = new WaterQuad();
	
	//init smoke engine
	centralSmoke = new SmokeBurnerObject();
	thrusterSmoke = new HorizontalSmokeObject(glm::vec3(0.45, 0.509, 0.467), glm::vec3(FUSELAGEXPOSITION, FUSELAGEEXHAUSTYPOSITION, FUSELAGEEXHAUSTZPOSITION));

	//init exploding
	explodingObject = new ExplodingObject();
	//init bgm
	SoundEngine = createIrrKlangDevice();

	//reset global variables for animation
	resetGlobalVariables();
	return window;
}

void displayLightSource(glm::mat4 view, glm::mat4 proj) {
	glBindBuffer(GL_ARRAY_BUFFER, lightBoxBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightBoxIndices);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]));
	model = glm::scale(model, glm::vec3(lightScale, lightScale, lightScale));
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &model[0][0]);
	glm::mat4 viewMatrix = view;
	glm::mat4 projMatrix = proj;
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &projMatrix[0][0]);
	glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, sizeof(recTriangleStripOrder) / sizeof(int), GL_UNSIGNED_INT, NULL,0);
}

void displayInitial(int currentIndices, GLFWwindow* window) {
	//create new imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

		int windowWidth = 700;
		int windowHeight = 800; 

		const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered); //color for buttom
		const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button); //color for background color
		ImGui::SetNextWindowSizeConstraints(ImVec2(windowWidth, windowHeight), ImVec2(windowWidth, windowHeight)); //width x height fixed
		ImGui::SetNextWindowPos(ImVec2(SCREENWIDTH/2-windowWidth/2.0, SCREENHEIGHT / 2 - windowHeight / 2.0 - 50));
		ImGui::Begin("Loading Obj...");
		ImGui::PushFont(titleFont);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
		ImGui::Text("Welcome! Obj files are loading..."); ImGui::SameLine();
		ImGui::Spinner("##spinner", 15, 6, col);
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(font1);
		ImGui::BufferingBar("##buffer_bar", ((float)currentIndices)/(PARTSTOTAL+1), ImVec2(400, 6), bg, col);
		ImGui::Text("Please wait... Progress: ");
		bool tempProgress[PARTSTOTAL];
		//create separate boolean array so user pressing the checkbox will not change the global variable
		for (int j = 0; j < PARTSTOTAL; j++) {
			tempProgress[j] = loadingProgress[j];
		}
		//print the checkbox and the name in every line
		for (int j = 0; j < PARTSTOTAL; j++) {
			if (currentIndices == j) 
			{
				//set different color for the parts that is used right now for uploading
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
			}
			if (j < PARTSNUM) {
				ImGui::Checkbox(partsList[j].c_str(), &tempProgress[j]);
			}
			else {
				ImGui::Checkbox("background", &tempProgress[j]);
			}
			if (currentIndices == j)
			{
				ImGui::PopStyleColor();
			}
		}

		
		ImGui::PopFont();
		ImGui::End();


	//for the input inside current indices, it will be indicated with color red
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void displayOnly(bool depthMode, bool reflectMode, glm::vec4 clippingPlane, bool depthPlane) {
	// function purely for drawing, choose depthMode as true if we want to render with
	//respect to depth shader
	//glDisable(GL_CULL_FACE);
	if (renderLightBox && !depthMode) {
		glBindVertexArray(lightVAO);
		glUseProgram(renderLightProgram);
		displayLightSource(View, Projection);
	}
	if (depthMode) {
		if (depthPlane) {
			//REMEMBER FOR DEPTH SHADER, WE DONT NEED TO RENDER THE BACKGROUND. WE ONLY NEED TO RENDER THE ROBOT ITSELF
			//ALSO, ONLY NEED VERTICES, DISCARD UV AND NORMAL  IN THIS RENDER
			//glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK); //to fix peter panning
			glUseProgram(depthProgram);
			glBindVertexArray(VAO); //bind regular VAO, just in case the previous one is lightVAO
			myUpdateModel();

			//update data to UBO for MVP
			GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
			glm::mat4 projview = Projection * View;
			glUniformMatrix4fv(lightSpaceID, 1, GL_FALSE, &projview[0][0]);
			for (int i = 0; i < PARTSNUM; i++) {
				glUniformMatrix4fv(lightModelID, 1, GL_FALSE, &Models[i][0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glEnableVertexAttribArray(0);

				//handle extra part substitution
				int currentIndex = i; //for later mtls accessing
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3);
				int vertexIDoffset = 0;
				for (int j = 0; j < mtls[currentIndex].size(); j++) {//
					glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
					vertexIDoffset += faces[currentIndex][j + 1] * 3;
				}
			}
			glCullFace(GL_BACK); //return to original for usual render
			glEnable(GL_CULL_FACE);
		}
		else {
			//REMEMBER FOR DEPTH SHADER, WE DONT NEED TO RENDER THE BACKGROUND. WE ONLY NEED TO RENDER THE ROBOT ITSELF
			//ALSO, ONLY NEED VERTICES, DISCARD UV AND NORMAL  IN THIS RENDER
			//glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK); //to fix peter panning
			glUseProgram(depthProgram);
			glBindVertexArray(VAO); //bind regular VAO, just in case the previous one is lightVAO
			myUpdateModel();

			//update data to UBO for MVP
			GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
			glUniformMatrix4fv(lightSpaceID, 1, GL_FALSE, &lightSpace[0][0]);
			for (int i = 0; i < PARTSNUM; i++) {
				glUniformMatrix4fv(lightModelID, 1, GL_FALSE, &Models[i][0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glEnableVertexAttribArray(0);

				//handle extra part substitution
				int currentIndex = i; //for later mtls accessing
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3);
				int vertexIDoffset = 0;
				for (int j = 0; j < mtls[currentIndex].size(); j++) {//
					glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
					vertexIDoffset += faces[currentIndex][j + 1] * 3;
				}
			}
			glCullFace(GL_BACK); //return to original for usual render
			glEnable(GL_CULL_FACE);
		}
	}
	else {
		//usual render
		glBindVertexArray(VAO);
		glUseProgram(program);
		myUpdateModel(); //based on the contents of the translatePart and rotatePart, update the Models mat4 array
		if (reflectMode) {
			//take care the flipping camera first
			//glDisable(GL_CULL_FACE);

			float tempviewPos[3] = { viewPos[0], viewPos[1], viewPos[2] };
			glm::mat4 tempView = View;

			float waterHeight = WATERQUADY;
			glm::vec3 arbitraryPoint(-WATERQUADXZ, WATERQUADY, -WATERQUADXZ);
			glm::vec3 planeNormal(0, -1, 0);
			glUniform3fv(waterPlanePointID, 1, &arbitraryPoint[0]);
			glUniform3fv(waterPlaneNormalID, 1, &planeNormal[0]);
			glUniform1i(isReflectionModeID, 1);

			if (planeMode == ONFIRE) {
				if (glfwGetTime() - fireStart < 3) {
					emACK = 0;
				}
				else {
					emACK = 3;
				}
			}

			//update data to UBO for MVP
			glBindBuffer(GL_UNIFORM_BUFFER, UBO);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &tempView);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &Projection);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glUniform4fv(ambientID, 1, &ambientColor[0]);
			glUniform4fv(diffuseID, 1, &diffuseColor[0]);
			glUniform4fv(specularID, 1, &specularColor[0]);
			glUniform1fv(ShininessID, 1, &Shininess);
			glUniform3fv(varyingViewPosID, 1, &tempviewPos[0]);
			glUniform3fv(permanentViewPosID, 1, &tempviewPos[0]);
			glUniform4fv(clipPlaneID, 1, &clippingPlane[0]);
			glUniformMatrix4fv(lightSpaceOrigID, 1, GL_FALSE, &lightSpace[0][0]);
			glUniform3fv(lightPosID, 1, &lightPosition[0]);
			glUniform1i(enviromentMappingACKID, emACK);
			glUniform1f(currentTime_Loc, currentTime);
			glUniform1f(mixVal_Loc, mixVal);
			glUniform1i(Toon_active_Loc, toon_active);
			//use the depth map we have generated previously
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glActiveTexture(GL_TEXTURE0 + 1);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
			
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, textureObj);
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, textureLava1);
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, textureLava2);
			glActiveTexture(GL_TEXTURE0 + 5);
			glBindTexture(GL_TEXTURE_2D, textureNew);
			glActiveTexture(GL_TEXTURE0 + 6);
			glBindTexture(GL_TEXTURE_2D, textureNewNormal);
			//-----------------------------------------------
			GLuint offset[5] = { 0,0,0,0,0 };//offset for vertices , uvs , normals
			for (int i = 0; i < PARTSNUM; i++) {
				//bind model exclusively with each model matrix
				glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Models[i][0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glEnableVertexAttribArray(0);


				//handle extra part substitution
				int currentIndex = i; //for later mtls accessing, because we might access different index for extra parts

				//first, vertices
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3); //for next vertices starting point

				//next, uv
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uVBO);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset[1]);
				offset[1] += uvs_size[i] * sizeof(glm::vec2);

				//finally, normal
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, nVBO);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[2]);
				offset[2] += normals_size[i] * sizeof(glm::vec3);
				int vertexIDoffset = 0;//glVertexID's offset 
				string mtlname;//material name

				//tangents
				glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[3]);//buffer offset
				offset[3] += tangents_size[i] * sizeof(glm::vec3);
				//bitangents
				glBindBuffer(GL_ARRAY_BUFFER, bitangVBO);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[4]);//buffer offset
				offset[4] += bitangents_size[i] * sizeof(glm::vec3);
				for (int j = 0; j < mtls[currentIndex].size(); j++) {//
					mtlname = mtls[currentIndex][j];
					//bind Ka, Kd and Ks of current material
					glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
					glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
					glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
					glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
					vertexIDoffset += faces[currentIndex][j + 1] * 3;
				}
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glEnable(GL_CULL_FACE);
			//render cubemap
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			glUseProgram(skyboxProgram);
			glm::mat4 tempviewmat = glm::mat4(glm::mat3(tempView)); // remove translation from the view matrix
			glUniformMatrix4fv(skyboxViewID, 1, GL_FALSE, &tempviewmat[0][0]);
			glUniformMatrix4fv(skyboxProjID, 1, GL_FALSE, &Projection[0][0]);
			glUniform3fv(waterPlanePointID, 1, &arbitraryPoint[0]);
			glUniform3fv(waterPlaneNormalID, 1, &planeNormal[0]);
			glUniform1i(isReflectionModeID, 1);
			//-----------------------------------------------
			glUniform1i(Toon_active_Loc2, toon_active);
			//-----------------------------------------------
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
		}
		else {
			//update data to UBO for MVP
			//glDisable(GL_CULL_FACE);
			glEnable(GL_CULL_FACE);
			if (planeMode == ONFIRE) {
				if (glfwGetTime() - fireStart < 3) {
					emACK = 0;
				}
				else {
					emACK = 3;
				}
			}
			glm::vec3 arbitraryPoint(WATERQUADXZ, WATERQUADY, WATERQUADXZ);
			glm::vec3 planeNormal(0, 1, 0);
			glUniform3fv(waterPlanePointID, 1, &arbitraryPoint[0]);
			glUniform3fv(waterPlaneNormalID, 1, &planeNormal[0]);
			glUniform1i(isReflectionModeID, 0);
			glBindBuffer(GL_UNIFORM_BUFFER, UBO);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &View);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &Projection);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glUniform4fv(ambientID, 1, &ambientColor[0]);
			glUniform4fv(diffuseID, 1, &diffuseColor[0]);
			glUniform4fv(specularID, 1, &specularColor[0]);
			glUniform1fv(ShininessID, 1, &Shininess);
			glUniform3fv(varyingViewPosID, 1, &viewPos[0]);
			glUniform3fv(permanentViewPosID, 1, &viewPos[0]);
			glUniform4fv(clipPlaneID, 1, &clippingPlane[0]);
			glUniformMatrix4fv(lightSpaceOrigID, 1, GL_FALSE, &lightSpace[0][0]);
			glUniform3fv(lightPosID, 1, &lightPosition[0]);
			glUniform1i(enviromentMappingACKID, emACK);
			glUniform1f(currentTime_Loc, currentTime);
			glUniform1f(mixVal_Loc, mixVal);
			glUniform1i(Toon_active_Loc, toon_active);
			//use the depth map we have generated previously
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, textureObj);
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, textureLava1);
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, textureLava2);
			glActiveTexture(GL_TEXTURE0 + 5);
			glBindTexture(GL_TEXTURE_2D, textureNew);
			glActiveTexture(GL_TEXTURE0 + 6);
			glBindTexture(GL_TEXTURE_2D, textureNewNormal);
			//-----------------------------------------------
			GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
			for (int i = 0; i < PARTSNUM; i++) {
				//bind model exclusively with each model matrix
				glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Models[i][0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glEnableVertexAttribArray(0);


				//handle extra part substitution
				int currentIndex = i; //for later mtls accessing, because we might access different index for extra parts

				//first, vertices
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3); //for next vertices starting point

				//next, uv
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uVBO);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset[1]);
				offset[1] += uvs_size[i] * sizeof(glm::vec2);

				//finally, normal
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, nVBO);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[2]);
				offset[2] += normals_size[i] * sizeof(glm::vec3);
				int vertexIDoffset = 0;//glVertexID's offset 
				string mtlname;//material name

				//tangents
				glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[3]);//buffer offset
				offset[3] += tangents_size[i] * sizeof(glm::vec3);
				//bitangents
				glBindBuffer(GL_ARRAY_BUFFER, bitangVBO);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[4]);//buffer offset
				offset[4] += bitangents_size[i] * sizeof(glm::vec3);
				for (int j = 0; j < mtls[currentIndex].size(); j++) {//
					mtlname = mtls[currentIndex][j];
					//bind Ka, Kd and Ks of current material
					glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
					glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
					glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
					glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
					vertexIDoffset += faces[currentIndex][j + 1] * 3;
				}
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glEnable(GL_CULL_FACE);

			//render cubemap
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			glUseProgram(skyboxProgram);
			//glm::mat4 tempview = glm::mat4(glm::mat3(View)); // remove translation from the view matrix
			glm::mat4 tempview = View;
			glUniformMatrix4fv(skyboxViewID, 1, GL_FALSE, &tempview[0][0]);
			glUniformMatrix4fv(skyboxProjID, 1, GL_FALSE, &Projection[0][0]);
			glUniform3fv(waterPlanePointID, 1, &arbitraryPoint[0]);
			glUniform3fv(waterPlaneNormalID, 1, &planeNormal[0]);
			glUniform1i(isReflectionModeID, 0);
			//-----------------------------------------------
			glUniform1i(Toon_active_Loc2, toon_active);
			//-----------------------------------------------
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default

			//fire and effect
			if (planeMode == ONFIRE) {
				float curTime = glfwGetTime() - fireStart;
				if (curTime < 3) {
					centralSmoke->Render(View, Projection);
				}
				else if (curTime < 5){
					explodingObject->Render(View, Projection);
				}
				else {
					fireGenerate->Render(View, Projection);
				}
			}
			else if (planeMode == AURA) {
				blueAura->Render(View, Projection);
				wing1Aura->Render(View, Projection);
				wing2Aura->Render(View, Projection);
				fuselageAura->Render(View, Projection);
			}
			else if (planeMode == NORMAL) {
				glm::vec3 tempview(viewPos[0], viewPos[1], viewPos[2]);
				thrusterSmoke->Render(View, Projection, depthMapPlane, tempview);
			}
		}
	}
}

void displayGLFW(GLFWwindow* window) {
	//render pipeline

	//accumulate data for gui to be rendered
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//only update View when angle change, to save computation
	if (viewChange) {
		float eyeAngleRad = deg2rad(eyeAngley);
		//viewPos[0] = translatePart[0][0] + eyedistance * sin(eyeAngleRad);
		//viewPos[1] = translatePart[0][1] + robotShiftUp + 2;
		//viewPos[2] = translatePart[0][2] + eyedistance * cos(eyeAngleRad);
		//View = glm::lookAt(
		//	glm::vec3(viewPos[0],viewPos[1],viewPos[2] ), //camera position
		//	glm::vec3(translatePart[0][0], translatePart[0][1] + robotShiftUp, translatePart[0][2]), //point to crotch
		//	glm::vec3(0, 1, 0) //up vector parallel to y axis
		//);

		viewPos[0] = eyedistance * sin(eyeAngleRad);
		viewPos[1] = robotShiftUp + 10;
		viewPos[2] = eyedistance * cos(eyeAngleRad);
		View = glm::lookAt(
			glm::vec3(viewPos[0], viewPos[1], viewPos[2]), // Camera is at (0,0,20), in World Space), // Camera is at (0,10,25), in World Space
			glm::vec3(0, robotShiftUp, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
		);
		viewChange = false;
	}
	//projection matrix update, just in case it is changed in GuI input
	Projection = glm::perspective(FoV, (float)(SCREENWIDTH - widthStart) / (SCREENHEIGHT - heightStart), nearClip, farClip);
	//handles discoticLighting
	if (discoticLighting) {
		glm::mat4 lightPosMV(1.0);
		float currentTime = getTime();
		lightPosMV = glm::translate(lightPosMV, glm::vec3(sinf(5.1f * currentTime) * 2.f, cosf(5.7f * currentTime) * 2.f, sinf(5.3f * currentTime) * cosf(5.5f * currentTime) * 2.f));
		lightPosMV = glm::rotate(lightPosMV, currentTime * 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosMV = glm::rotate(lightPosMV, currentTime * 81.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		lightPosMV = glm::translate(lightPosMV, glm::vec3(0, 40, 0));
		glm::vec4 robocopPos(translatePart[0][0], translatePart[0][1], translatePart[0][2], 1.0f);
		robocopPos = lightPosMV * robocopPos;
		//offset based on robocop Position
		
		//update global variable
		lightPosition[0] = robocopPos[0];
		lightPosition[1] = robocopPos[1];
		lightPosition[2] = robocopPos[2];

		//clamp light position
		if (lightPosition[0] < -TRANSLATEXLIMIT) {
			lightPosition[0] = -TRANSLATEXLIMIT;
		}
		else if (lightPosition[0] > TRANSLATEXLIMIT) {
			lightPosition[0] = TRANSLATEXLIMIT;
		}
		if (lightPosition[2] < -TRANSLATEZLIMIT) {
			lightPosition[2] = -TRANSLATEZLIMIT;
		}
		else if (lightPosition[2] > TRANSLATEZLIMIT) {
			lightPosition[2] = TRANSLATEZLIMIT;
		}
		if (lightPosition[1] < TRANSLATEYLOWERLIMIT) {
			lightPosition[1] = TRANSLATEYLOWERLIMIT;
		}
		else if (lightPosition[1] > TRANSLATEYUPPERLIMIT) {
			lightPosition[1] = TRANSLATEYUPPERLIMIT;
		}

	}

	//update light space matrix
	glm::vec3 curLightPos(lightPosition[0], lightPosition[1], lightPosition[2]);
	glm::mat4 lightView = glm::lookAt(curLightPos,
		glm::vec3(translatePart[0][0], translatePart[0][1], translatePart[0][2]),
		glm::vec3(0, 1, 0));
	lightSpace = lightProjection * lightView; //light projection will never change, so just set to global variable.
	//-------------- for the record, this is lightProjection content  -----------------
	//lightProjection = glm::ortho(-79, 79, -75, 75, 0.4, 106.7);
	//----------------------------------------------------------------------------------
	
	//render to depth map, for shadow mapping
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT); 
		displayOnly(true, false, glm::vec4(0,0,0,0), false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*if (planeMode == NORMAL) {
		glViewport(0, 0, SHADOW_PLANE_WIDTH, SHADOW_PLANE_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapPlaneFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		displayOnly(true, false, glm::vec4(0, 0, 0, 0), true);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}*/

	if (planeMode == FLYNEARWATER) {
		////render to water reflection framebuffer
		glEnable(GL_CLIP_DISTANCE0);
		waterPlatform->bindReflectFBO(TEXREFLECTWIDTH, TEXREFLECTHEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		displayOnly(false, true, glm::vec4(0, 1, 0, -WATERQUADY), false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		////render to water refraction framebuffer
		waterPlatform->bindRefractFBO(TEXREFRACTWIDTH, TEXREFRACTHEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		displayOnly(false, false, glm::vec4(0, -1, 0, WATERQUADY), false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //CAREFUL HERE
		glDisable(GL_CLIP_DISTANCE0);
	}

	//-----------------------------------------------=============================================================================
	//usual rendering
	//Motion Blur here
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, depthMap);
		displayOnly(false, false, glm::vec4(0,0,0,0), false);
		//displayOnly(false, true, glm::vec4(0, 0, 0, 0));
		if (planeMode == FLYNEARWATER) {
			//waterquad
			glm::vec3 tempViewPos(viewPos[0], viewPos[1], viewPos[2]);
			glm::vec3 tempSpecColor(specularColor[0], specularColor[1], specularColor[2]);
			waterPlatform->Render(View, Projection, tempViewPos, curLightPos, tempSpecColor, depthMap, lightSpace);
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render special mosaic
	glViewport(widthStart, heightStart, SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program2);
	glUniform1i(Shader_now_Loc, shader_now);
	glUniform1f(Mosaic_val_Loc, mosaic_val);
	glUniform1f(Final_Width_Loc, width_size);
	glUniform1f(Final_Height_Loc, height_size);
	glUniform1i(tex_Loc, 0);

	if (motionBlur_toggle && doSpin)
	// if(motionBlur_toggle)
	{
		glBindVertexArray(vao2);
		glUseProgram(program2);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureData);

		glBindVertexArray(vao2);
		glUseProgram(motionBlurProgram);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureLast1);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, textureLast2);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, textureLast3);

		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, textureLast4);

		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, textureLast5);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else
	{
		glBindVertexArray(vao2);
		glUseProgram(program2);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textureData);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	if (step % 2 == 1)
	{
		glCopyImageSubData(textureLast4, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureLast5, GL_TEXTURE_2D, 0, 0, 0, 0,
			SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 1);

		glCopyImageSubData(textureLast3, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureLast4, GL_TEXTURE_2D, 0, 0, 0, 0,
			SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 1);

		glCopyImageSubData(textureLast2, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureLast3, GL_TEXTURE_2D, 0, 0, 0, 0,
			SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 1);

		glCopyImageSubData(textureLast1, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureLast2, GL_TEXTURE_2D, 0, 0, 0, 0,
			SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 1);

		glCopyImageSubData(textureData, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureLast1, GL_TEXTURE_2D, 0, 0, 0, 0,
			SCREENWIDTH - widthStart, SCREENHEIGHT - heightStart, 1);
	}
	step++;
	//-----------------------------------------------=============================================================================
	
	

	//for song
	fireEval(planeMode);
	//render gui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Obj2Buffer(GLFWwindow* window) {
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;

	//load MTLs, for every parts, extra parts, and backgrounds. 
	for (int i = 0; i < PARTSNUM; i++) {
		loadMTL(("../Assets/Obj/" + partsList[i] + ".mtl").c_str(), Kds, Kas, Kss, Materials, texture);
	}
	//identify make a mapping of MTLs KD,KA and KS based on each name, just in case there is duplicity
	for (int i = 0; i < Materials.size(); i++) {
		string mtlname = Materials[i];
		KDs[mtlname] = Kds[i];
		KAs[mtlname] = Kas[i];
		KSs[mtlname] = Kss[i];
	}

	displayInitial(-1, window); //show progress of loading on GUI
	for (int i = 0; i < PARTSNUM; i++) {
		displayInitial(i,window); //for displaying loading progress;
		load2Buffer("../Assets/Obj/"+partsList[i]+".obj", i);
		loadingProgress[i] = true; //updateProgress
	}
	displayInitial(-1,window); //for displaying loading progress;
	
	//count total data for the vertices, uvs and also the normal size
	GLuint totalSize[5] = { 0,0,0,0,0 };
	GLuint offset[5] = { 0,0,0,0,0 };
	//Parts total is all body parts+extra parts
	for (int i = 0; i < PARTSTOTAL; i++) {
		totalSize[0] += vertices_size[i] * sizeof(glm::vec3);
		totalSize[1] += uvs_size[i] * sizeof(glm::vec2);
		totalSize[2] += normals_size[i] * sizeof(glm::vec3);
		totalSize[3] += tangents_size[i] * sizeof(glm::vec3);
		totalSize[4] += bitangents_size[i] * sizeof(glm::vec3);
	}
	//cout << "total size 4: " << totalSize[3] << endl;
	//generate 3 vbo for the vertices, uv and  also the normal
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &uVBO);
	glGenBuffers(1, &nVBO);
	glGenBuffers(1, &tangentVBO);
	glGenBuffers(1, &bitangVBO);
	//allocate space for the three VBOs. Since the data aren't changed often, use GL_STATIC_DRAW
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[0], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[1], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[2], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[3], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, bitangVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[4], NULL, GL_STATIC_DRAW);
	
	for (int i = 0; i < PARTSTOTAL; i++) {
		//copy individual VBO vertices into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[0], vertices_size[i] * sizeof(glm::vec3));
		offset[0] += vertices_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(VBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		//copy individual VBO uv into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[1], uvs_size[i] * sizeof(glm::vec2));
		offset[1] += uvs_size[i] * sizeof(glm::vec2);
		glInvalidateBufferData(uVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		//copy individual VBO normals into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[2], normals_size[i] * sizeof(glm::vec3));
		offset[2] += normals_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(nVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

		glBindBuffer(GL_COPY_WRITE_BUFFER, tangentVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, tangVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[3], tangents_size[i] * sizeof(glm::vec3));
		offset[3] += tangents_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(tangVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

		glBindBuffer(GL_COPY_WRITE_BUFFER, bitangVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, bitangVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[4], bitangents_size[i] * sizeof(glm::vec3));
		offset[4] += bitangents_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(bitangVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}


void myUpdateModel() {
	//reset state, glm::translate to initial place
	for (int i = 0; i < PARTSNUM; i++) {
		Models[i] = glm::mat4(1.0f);
	}
	Models[0] = glm::translate(Models[0], glm::vec3(0, robotShiftUp, 0));
	Models[0] = glm::translate(Models[0], glm::vec3(translatePart[0][0], translatePart[0][1], translatePart[0][2]));
	Models[0] = glm::translate(Models[0], glm::vec3(initialOffset[0][0], initialOffset[0][1], initialOffset[0][2]));
	Models[0] = glm::rotate(Models[0], rotatePart[0][2], glm::vec3(0, 0, 1));
	Models[0] = glm::rotate(Models[0], rotatePart[0][1], glm::vec3(0, 1, 0));
	Models[0] = glm::rotate(Models[0], rotatePart[0][0], glm::vec3(1, 0, 0));
	//Models[0] = glm::scale(Models[0], glm::vec3(0.2, 0.2, 0.2));
	Models[0] = glm::scale(Models[0], glm::vec3(4, 4, 4));
}

void load2Buffer(string obj, int i) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> materialIndices;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<bool> checkFlag;
	const char* c = obj.c_str();
	bool res = loadOBJ(c, vertices, uvs, normals, faces[i], mtls[i]);
	if (!res) printf("load failed\n");

	//count tangents and bitangents
	for (int i = 0; i < vertices.size(); i+=3) {
		// Shortcuts for vertices
		glm::vec3& v0 = vertices[i + 0];
		glm::vec3& v1 = vertices[i + 1];
		glm::vec3& v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2& uv0 = uvs[i + 0];
		glm::vec2& uv1 = uvs[i + 1];
		glm::vec2& uv2 = uvs[i + 2];

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		// set same tangent for one face
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// set same bitangent for one face
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);

		checkFlag.push_back(false); //for later checking
		checkFlag.push_back(false);
		checkFlag.push_back(false);
	}
	//std::cout << "reach here1" <<  bitangents.size() << "\n";
	//averaging the tangent and bitangent of similar vertices
	for (int i = 0; i < vertices.size() - 1; i++) {
		//cout << "i: " << i << std::endl;
		if (checkFlag[i]) {
			continue;
		}
		for (int j = i + 1; j < vertices.size(); j++) {
			bool found = false;
			if (vertices[i] == vertices[j] && uvs[i] == uvs[j] && normals[i] == normals[j]) {
				found = true;
				checkFlag[i] = true;
				checkFlag[j] = true;
				std::vector<int> indicesCollection;
				indicesCollection.push_back(i);
				indicesCollection.push_back(j);
				glm::vec3 sumTangent = tangents[i] + tangents[j];
				glm::vec3 sumBitangent = bitangents[i] + bitangents[j];
				int foundSame = 2;
				for (int k = j + 1; k < vertices.size(); k++) {
					if (checkFlag[k]) {
						continue;
					}
					if (vertices[i] == vertices[k] && uvs[i] == uvs[k] && normals[i] == normals[k]) {
						foundSame++;
						checkFlag[k] = true;
						indicesCollection.push_back(k);
						sumTangent += tangents[k];
						sumBitangent += bitangents[k];
					}
				}

				//average and update each tangent
				sumTangent /= foundSame;
				sumBitangent /= foundSame;

				for (int parse = 0; parse < indicesCollection.size(); parse++) {
					tangents[indicesCollection[parse]] = sumTangent;
					bitangents[indicesCollection[parse]] = sumBitangent;
				}
			}
			if (found) {
				break;
			}
		}
	}

	glGenBuffers(1, &VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	glGenBuffers(1, &uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1, &nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();

	glGenBuffers(1, &tangVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, tangVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	tangents_size[i] = tangents.size();

	glGenBuffers(1, &bitangVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, bitangVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
	bitangents_size[i] = bitangents.size();
}
//Keyboard for GLFW
void KeyboardGLFW(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//related to animation
	//related to viewing and exit
	//-----------------------------------------------=============================================================================
	if (key == GLFW_KEY_P)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			mosaic_val += 1.0f;
		}
		//cout << "Mosaic value: " << mosaic_val << endl;
	}
	else if (key == GLFW_KEY_L)
	{
		if (action == GLFW_PRESS|| action == GLFW_REPEAT) {
			mosaic_val -= 1.0f;
		}
		//cout << "Mosaic value: " << mosaic_val << endl;
	}
	if (key == GLFW_KEY_O)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			mosaic_val += 0.01f;
		}
		//cout << "Mosaic value: " << mosaic_val << endl;
	}
	else if (key == GLFW_KEY_K)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			mosaic_val -= 0.01f;
		}
		//cout << "Mosaic value: " << mosaic_val << endl;
	}

	//-----------------------------------------------=============================================================================
	/*else if (key == GLFW_KEY_M)
	{
		doSpin = true;
		planeSpin();
	}*/

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_UP:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyedistance -= 0.2;
			clip(eyedistance, 5, 40);
		}
		break;
	case GLFW_KEY_DOWN:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyedistance += 0.2;
			clip(eyedistance, 5, 40);
		}
		break;
	case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyeAngley += 2;
		}
		break;
	case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			eyeAngley -= 2;
			viewChange = true;//update view matrix
		}
		break;
	default:
		break;
	}
}

float clip(float &var, float min, float max) {
	if (var < min) {
		var = min;
	}
	else if (var > max) {
		var = max;
	}
	return var;
}

float getTime()
{
	return glfwGetTime();
}

float clampValMin(float x, float clampToMin)
{
	if (x <= clampToMin)
	{
		return clampToMin;
	}
	else
	{
		return x;
	}
}

float clampValMaxMin(float x, float clampToMax, float clampToMin)
{
	if (x > clampToMax)
	{
		return clampToMax;
	}
	else if (x < clampToMin)
	{
		return clampToMin;
	}
	else
	{
		return x;
	}
}

float clampValMax(float x, float clampToMax)
{
	if (x >= clampToMax)
	{
		return clampToMax;
	}
	else
	{
		return x;
	}
}

void planeSpin()
{
	// getReady
	if (!getReady)
	{
		const float radiusTurn = 20.0f;
		float pivotSpeed = 2.0f;
		float pivotTarget = -20.0f;

		rotatePart[0][2] -= pivotSpeed;
		translatePart[0][0] = clampValMin(translatePart[0][0] - dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), -10.0f);
		translatePart[0][1] = radiusTurn - sqrt(pow(radiusTurn, 2) - pow(translatePart[0][0], 2));

		if (rotatePart[0][2] < pivotTarget)
		{
			rotatePart[0][2] = pivotTarget;
			getReady = true;
		}
	}
	else if (!getReady2)
	{
		float radiusTurn = 20.0f;
		float pivotSpeed = 5.0f;
		float pivotTarget = 70.0f;

		rotatePart[0][2] += pivotSpeed;
		float pT, iD, mV;
		if (rotatePart[0][2] < 0.0f)
		{
			pT = 20.0f;
			iD = 10.0f;
			mV = 0.0f;
			translatePart[0][0] = clampValMax(translatePart[0][0] + dGangnamFunction(pivotSpeed, pT, iD), mV);
			translatePart[0][1] = radiusTurn - sqrt(pow(radiusTurn, 2) - pow(translatePart[0][0], 2));
		}
		else if (rotatePart[0][2] >= 0.0f && rotatePart[0][2] < 70.0f)
		{
			pT = 90.0f;
			iD = radiusTurn;
			mV = radiusTurn;
			translatePart[0][0] = clampValMax(translatePart[0][0] + dGangnamFunction(pivotSpeed, pT, iD), mV);
			translatePart[0][1] = radiusTurn - sqrt(pow(radiusTurn, 2) - pow(translatePart[0][0], 2));
		}
		else
		{

		}

		if (rotatePart[0][2] >= pivotTarget)
		{
			rotatePart[0][2] = pivotTarget;
			getReady2 = true;
		}

	}
	else if (spin360)
	{
		float pivotSpeed = 17.0f;
		float pivotTarget = 400.0f;

		rotatePart[0][2] += pivotSpeed;

		if (rotatePart[0][2] >= pivotTarget)
		{
			rotatePart[0][2] = 40.0f;
			spin360 = false;
		}
	}
	else if (finishing)
	{
		float pivotSpeed = 1.0f;
		float pivotTarget = 0.0f;

		rotatePart[0][2] -= pivotSpeed;
		translatePart[0][0] = clampValMin(translatePart[0][0] - dGangnamFunction(pivotSpeed, pivotTarget + 40.0f, 20.0f), 0.0f);
		translatePart[0][1] = clampValMin(translatePart[0][1] - dGangnamFunction(pivotSpeed, pivotTarget + 40.0f, 20.0f), 0.0f);

		if (rotatePart[0][2] <= pivotTarget)
		{
			rotatePart[0][2] = 0.0f;
			finishing = false;
			doSpin = false;
			getReady = false;
			getReady2 = false;
			spin360 = true;
			finishing = true;
		}
	}
}

float dGangnamFunction(float followSpeed, float followTarget, float imiDist)
{
	return followSpeed * imiDist / abs(followTarget);
}