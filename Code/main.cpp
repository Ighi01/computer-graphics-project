// This has been adapted from the Vulkan tutorial
#define JSON_DIAGNOSTICS 1
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define _CRT_SECURE_NO_WARNINGS
#define NUMLIGHTS 63
#define MAXLIGHTS 16

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/Scene.hpp"

std::vector<SingleText> outText = {
	{3, {"Plane Simulator", "COUNTRY", "Press SPACE to start the engine, TAB to change place.",""}, 0, 0},
	{3, {"Plane Simulator", "PORT", "Press SPACE to start the engine, TAB to change place.",""}, 0, 0},
	{3, {"Plane Simulator", "BEACH", "Press SPACE to start the engine, TAB to change place.",""}, 0, 0},
	{3, {"Plane Simulator", "CITY", "Press SPACE to start the engine, TAB to change place.",""}, 0, 0},
	{0, {"","","",""}, 0, 0},
	{1, {"ScreenShoot Saved !", "", "",""}, 0, 0},
	{1, {"Don't go out of the scene!", "", "",""}, 0, 0}
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir[MAXLIGHTS];
	alignas(16) glm::vec3 lightPos[MAXLIGHTS];
	alignas(16) glm::vec4 lightColor[MAXLIGHTS];
	alignas(16) glm::vec3 isOn[MAXLIGHTS];
	alignas(4) float cosIn;
	alignas(4) float cosOut;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 eyeDir;
	alignas(16) glm::vec4 lightOn;
};

struct SimpleUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct SimpleMatParUniformBufferObject {
	alignas(4)  float Power;
};

struct SunMoonUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct skyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};


struct SimpleVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct skyBoxVertex {
	glm::vec3 pos;
};

struct SunMoonVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

// MAIN ! 
class CGProject : public BaseProject {
	protected:

	DescriptorSetLayout DSLGlobal;
    DescriptorSet DSGlobal;
	DescriptorSetLayout DSLCity;
	DescriptorSet DSPlane;
	DescriptorSetLayout DSLSunMoon;
	DescriptorSet DSSun;
	DescriptorSet DSMoon;
	DescriptorSetLayout DSLskyBox;
	DescriptorSet DSskyBox;

    VertexDescriptor VDSimple;
	VertexDescriptor VDskyBox;
	VertexDescriptor VDsunMoon;
	Pipeline PSimple;
	Pipeline PskyBox;
	Pipeline PSunMoon;

	Scene SC;

	Model Mplane;
	Model Msun;
	Model Mmoon;

	Texture Tcity;
	Texture Tsun;
	Texture Tmoon;

	Model MskyBox;
	Texture TskyBox;
	
    TextMaker txt;

	glm::vec3 CamPos;
	glm::mat4 ViewMatrix;

	float Ar;
	int currScene = 0, prevCurrScene = 0;
	float scaleFactor = 0.05f;

	bool start = false;

	float X_SPEED = 0.75f;
	float Y_SPEED = 0.5f;
	float Z_SPEED = 1.0f;
	float SPEED = 10.0f;

	float followSpeed = 1.0f;
	float followSpeedFirst = 2.0f;
	float minDistance = -0.5f;
	float maxDistance = -3.5f;
	float camOffset = 0.5f;

	float minX = -150.0f;
	float maxX = 200.0f;
	float minZ = -200.0f;
	float maxZ = 100.0f;
	float maxDown = 1.0f;
	float maxUp = 50.0f;

	float accumulatedRotationY = 0.0f;
	float accumulatedRotationZ = 0.0f;
	float accumulatedRotationX = 0.0f;

	float zoom = 1.0f;
	float speedFactor = 1.0f;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	bool thirdPerson = true;
	glm::vec3 targetPos;

	glm::mat4 LWm[61];
	glm::vec3 LCol[61];
	float LInt[61];
	float ScosIn, ScosOut;
	float planeLightOn = 0.0f;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1920;
		windowHeight = 1080;
		windowTitle = "CG-PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.4f, 0.7f, 1.0f, 1.0f };
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
			});

		DSLCity.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SimpleUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(SimpleMatParUniformBufferObject), 1}
			});

		DSLSunMoon.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SunMoonUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
			});

		DSLskyBox.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(skyBoxUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
			});

		VDSimple.init(this, {
				  {0, sizeof(SimpleVertex), VK_VERTEX_INPUT_RATE_VERTEX}
		}, {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SimpleVertex, pos),
					sizeof(glm::vec3), POSITION},
			{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SimpleVertex, norm),
					sizeof(glm::vec3), NORMAL},
			{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(SimpleVertex, UV),
					sizeof(glm::vec2), UV}
		});

		VDskyBox.init(this, {
				  {0, sizeof(skyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(skyBoxVertex, pos),
					 sizeof(glm::vec3), POSITION}
			});

		VDsunMoon.init(this, {
				  {0, sizeof(SimpleVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SunMoonVertex, pos),
						sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(SunMoonVertex, UV),
						sizeof(glm::vec2), UV}
			});


		PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLskyBox });
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT, true);
		PSunMoon.init(this, &VDsunMoon, "shaders/SunMoonVert.spv", "shaders/SunMoonFrag.spv", { &DSLSunMoon });
		PSunMoon.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);
		PSimple.init(this, &VDSimple, "shaders/SimpleVert.spv", "shaders/SimpleFrag.spv", { &DSLGlobal, &DSLCity });
		PSimple.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
		Msun.init(this, &VDSimple, "models/Sphere.obj", OBJ);
		Mmoon.init(this, &VDSimple, "models/Sphere1.obj", OBJ);
		Mplane.init(this, &VDSimple, "models/transport_air_008_transport_air_008.001.mgcg", MGCG);
		
		SC.init(this, &VDSimple, DSLCity, PSimple, "models/scene.json");
		
		TskyBox.init(this, "textures/sky.png");
		Tcity.init(this, "textures/Textures_City.png");
		Tsun.init(this, "textures/sun.png");
		Tmoon.init(this, "textures/moon.png");

		DPSZs.uniformBlocksInPool = 1000;
		DPSZs.texturesInPool = 1000;
		DPSZs.setsInPool = 1000;

		std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		nlohmann::json js;
		std::ifstream ifs("models/Lights.json");

		if (!ifs.is_open()) {
			std::cout << "Error! Lights file not found!";
			exit(-1);
		}
		try {
			std::cout << "Parsing JSON\n";
			ifs >> js;
			ifs.close();
			nlohmann::json ns = js["nodes"];
			nlohmann::json ld = js["extensions"]["KHR_lights_punctual"]["lights"];
			for (int i = 0; i < (NUMLIGHTS - 2); i++) {
				glm::vec3 T;
				glm::vec3 S;
				glm::quat Q;
				if (ns[i].contains("translation")) {
					T = glm::vec3(ns[i]["translation"][0],
						ns[i]["translation"][1],
						ns[i]["translation"][2]);
				}
				else {
					T = glm::vec3(0);
				}
				if (ns[i].contains("rotation")) {
					Q = glm::quat(ns[i]["rotation"][3],
						ns[i]["rotation"][0],
						ns[i]["rotation"][1],
						ns[i]["rotation"][2]);
				}
				else {
					Q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				}
				if (ns[i].contains("scale")) {
					S = glm::vec3(ns[i]["scale"][0],
						ns[i]["scale"][1],
						ns[i]["scale"][2]);
				}
				else {
					S = glm::vec3(1);
				}
				LWm[i] = glm::translate(glm::mat4(1), T) *
					glm::mat4(Q) *
					glm::scale(glm::mat4(1), S);
				nlohmann::json cl = ld[i]["color"];
				LCol[i] = glm::vec3(cl[0], cl[1], cl[2]);
				LInt[i] = ld[i]["intensity"];
			}
		}
		catch (const nlohmann::json::exception& e) {
			std::cout << e.what() << '\n';
		}

		Msun.Wm *= glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		Mmoon.Wm *= glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));

		std::cout << "Light initialization completed!\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		PskyBox.create1();
		PSunMoon.create();
		PSimple.create();
		
		DSskyBox.init(this, &DSLskyBox, { &TskyBox });
		DSSun.init(this, &DSLSunMoon, { &Tsun });
		DSMoon.init(this, &DSLSunMoon, { &Tmoon });
		DSGlobal.init(this, &DSLGlobal, {});
		DSPlane.init(this, &DSLCity, { &Tcity });
		
		SC.pipelinesAndDescriptorSetsInit(DSLCity);
		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		PskyBox.cleanup();
		PSunMoon.cleanup();
		PSimple.cleanup();

		DSskyBox.cleanup();
		DSSun.cleanup();
		DSMoon.cleanup();
		DSGlobal.cleanup();
		DSPlane.cleanup();
		
		SC.pipelinesAndDescriptorSetsCleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		DSLCity.cleanup();
		DSLGlobal.cleanup();
		DSLSunMoon.cleanup();

		Tcity.cleanup();
		Tsun.cleanup();
		Tmoon.cleanup();
		TskyBox.cleanup();

		Mplane.cleanup();
		Msun.cleanup();
		Mmoon.cleanup();
		MskyBox.cleanup();
		
		SC.localCleanup();
		PskyBox.destroy();
		PSimple.destroy();
		PSunMoon.destroy();
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		PskyBox.bind(commandBuffer);
		MskyBox.bind(commandBuffer);
		DSskyBox.bind(commandBuffer, PskyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);
		
		PSunMoon.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSSun.bind(commandBuffer, PSunMoon, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);

		PSunMoon.bind(commandBuffer);
		Mmoon.bind(commandBuffer);
		DSMoon.bind(commandBuffer, PSunMoon, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mmoon.indices.size()), 1, 0, 0, 0);

		PSimple.bind(commandBuffer);
		Mplane.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);
		DSPlane.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mplane.indices.size()), 1, 0, 0, 0);

		SC.populateCommandBuffer(commandBuffer, currentImage, PSimple);
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.

	void updateUniformBuffer(uint32_t currentImage) {

		float deltaT;
		glm::vec3 movement = glm::vec3(0.0f);
		bool instantCamera;
		Direction direction;
		glm::vec3 firstDirection = glm::vec3(0.0f);

		handleCommands(deltaT, movement, start, zoom, speedFactor, direction, instantCamera, thirdPerson, currScene, planeLightOn, currentImage);

		if (start)
		{
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			bool block = false;

			if (Mplane.Wm[3].y <= maxDown) {
				if (accumulatedRotationX < glm::radians(180.0f)) {
					float rotationStep = - X_SPEED * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(1.0f, 0.0f, 0.0f));
					accumulatedRotationX += glm::abs(rotationStep);
				}
				block = true;
			}
			else{
				accumulatedRotationX = 0.0;
			}

			if (Mplane.Wm[3].y >= maxUp) {
				if (accumulatedRotationX < glm::radians(180.0f)){
					float rotationStep = X_SPEED * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(1.0f, 0.0f, 0.0f));
					accumulatedRotationX += glm::abs(rotationStep);
				}
				block = true;
			}
			else {
				accumulatedRotationX = 0.0;
			}

			if (Mplane.Wm[3].x >= maxX || Mplane.Wm[3].z >= maxZ || Mplane.Wm[3].x <= minX || Mplane.Wm[3].z <= minZ) {
				if (accumulatedRotationY < glm::radians(180.0f)) {
					float rotationStep = -Y_SPEED * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(0.0f, 1.0f, 0.0f));
					accumulatedRotationY += glm::abs(rotationStep);
				}
				block = true;
			}
			else {
				accumulatedRotationY = 0.0;
			}

			if (!block) {
				rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f), movement.x * X_SPEED * deltaT, glm::vec3(1.0f, 0.0f, 0.0f)), movement.y * Y_SPEED * deltaT, glm::vec3(0.0f, 1.0f, 0.0f)), movement.z * Z_SPEED * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			if (block && currScene == 4) {
				currScene = 6;
			}
			else if(currScene == 6){
				currScene = 4;
			}

			Mplane.Wm = glm::translate(Mplane.Wm * rotationMatrix, glm::vec3(0.0f, 0.0f, 1.0f) * SPEED * speedFactor * deltaT);
		}
		else {
			switch (currScene) {
			case 0:
				Mplane.Wm = glm::mat4(
					scaleFactor, 0.0f, 0.0f, 0.0f,
					0.0f, scaleFactor, 0.0f, 0.0f,
					0.0f, 0.0f, scaleFactor, 0.0f,
					48.0f, 2.0f, -112.0f, 1.0f
				);
				up = glm::vec3(0.0f, 1.0f, 0.0f);
				zoom = 1.0f;
				speedFactor = 1.0f;
				thirdPerson = true;
				break;
			case 1:
				Mplane.Wm = glm::mat4(
					0.0f, 0.0f, -scaleFactor, 0.0f,
					0.0f, scaleFactor, 0.0f, 0.0f,
					scaleFactor, 0.0f, 0.0f, 0.0f,
					-24.0f, 2.0f, 40.0f, 1.0f
				);
				break;
			case 2:
				Mplane.Wm = glm::mat4(
					scaleFactor, 0.0f, 0.0f, 0.0f,
					0.0f, scaleFactor, 0.0f, 0.0f,
					0.0f, 0.0f, scaleFactor, 0.0f,
					120.0f, 2.0f, -56.0f, 1.0f
				);
				break;
			case 3:
				Mplane.Wm = glm::mat4(
					0.0f, 0.0f, scaleFactor, 0.0f,
					0.0f, scaleFactor, 0.0f, 0.0f,
					-scaleFactor, 0.0f, 0.0f, 0.0f,
					16.0f, 2.0f, -72.0f, 1.0f
				);
				break;
			}
		}

		glm::vec3 planePosition = glm::vec3(Mplane.Wm[3]);
		glm::vec3 desiredCamPos{};
		glm::vec3 desiredTargetPos{};
		glm::vec3 cameraOffset;

		if (start)
		{
			switch (direction) {			
			case FRONT:
				cameraOffset = - glm::normalize(glm::vec3(Mplane.Wm[2])) * glm::mix(minDistance, maxDistance, zoom / maxZoom);
				desiredCamPos = planePosition - cameraOffset + glm::normalize(glm::vec3(Mplane.Wm[1])) * camOffset;
				desiredTargetPos = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(Mplane.Wm[2]));
				break;			
			case BACK:
				cameraOffset = glm::normalize(glm::vec3(Mplane.Wm[2])) * glm::mix(minDistance, maxDistance, zoom / maxZoom);
				desiredCamPos = planePosition - cameraOffset + glm::normalize(glm::vec3(Mplane.Wm[1])) * camOffset;
				desiredTargetPos = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(Mplane.Wm[2]));
				break;
			case UP:
				cameraOffset = glm::normalize(glm::vec3(Mplane.Wm[1])) * glm::mix(minDistance, maxDistance, zoom / maxZoom);
				desiredCamPos = planePosition - cameraOffset + glm::normalize(glm::vec3(Mplane.Wm[1])) * camOffset;
				desiredTargetPos = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(Mplane.Wm[2]));
				break;
			case LEFT:
				cameraOffset = glm::normalize(glm::vec3(Mplane.Wm[0])) * glm::mix(minDistance, maxDistance, zoom / maxZoom);
				desiredCamPos = planePosition - cameraOffset + glm::normalize(glm::vec3(Mplane.Wm[0])) * camOffset;
				desiredTargetPos = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(Mplane.Wm[0]));
				break;
			case RIGHT:
				cameraOffset = - glm::normalize(glm::vec3(Mplane.Wm[0])) * glm::mix(minDistance, maxDistance, zoom / maxZoom);
				desiredCamPos = planePosition - cameraOffset - glm::normalize(glm::vec3(Mplane.Wm[0])) * camOffset;
				desiredTargetPos = planePosition - glm::vec3(10.0f) * glm::normalize(glm::vec3(Mplane.Wm[0]));
				break;
			}

			if (instantCamera) {
				CamPos = desiredCamPos;
			}
			else{
				CamPos += (desiredCamPos - CamPos) * followSpeed * deltaT;
			}

			if (CamPos.y < 0)
			{
				CamPos.y = 0;
			}

			up += (glm::normalize(glm::vec3(Mplane.Wm[1])) - up) * followSpeed * deltaT;
			targetPos += (desiredTargetPos - targetPos) * followSpeedFirst * deltaT;
		}
		else {
			targetPos = planePosition + glm::vec3(10.0) * glm::normalize(glm::vec3(Mplane.Wm[2]));
			CamPos = planePosition + glm::normalize(glm::vec3(Mplane.Wm[2])) * glm::mix(minDistance, maxDistance, zoom / maxZoom) + glm::normalize(glm::vec3(Mplane.Wm[1])) * camOffset;
		}

		if (thirdPerson) {
			ViewMatrix = glm::lookAt(CamPos, planePosition, up);
		}
		else {
			glm::vec3 firstCamPos = planePosition - glm::vec3(0.1f) * glm::normalize(glm::vec3(Mplane.Wm[2])) + glm::vec3(0.125f) * glm::normalize(glm::vec3(Mplane.Wm[1]));
			ViewMatrix = glm::lookAt(firstCamPos, targetPos, glm::vec3(Mplane.Wm[1]));
		}

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 650.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj = M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);

		// ------------ LIGHTS ------------

 		GlobalUniformBufferObject gubo{};


		// DAY NIGHT CYCLE

		static float cTime = 5.0;
		const float turnTime = 30.0f;

		if (start) {
			cTime += deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}
		else {
			cTime = 5.0;
		}

		float angle = cTime * 2.0f * M_PI / turnTime;

		float lightIntensity;

		float zAngle = 30.0f * (M_PI / 180.0f);

		Msun.Wm[3].x = 300.0f * (cos(angle) * cos(zAngle));
		Msun.Wm[3].y = 300.0f * sin(angle);
		Msun.Wm[3].z = 300.0f * (cos(angle) * sin(zAngle));

		Mmoon.Wm[3].x = -Msun.Wm[3].x;
		Mmoon.Wm[3].y = -Msun.Wm[3].y; 
		Mmoon.Wm[3].z = -Msun.Wm[3].z;


		if (160.0f <= glm::radians(angle) <= 220.0f) {
			gubo.lightDir[0] = glm::vec3(cos(glm::radians(160.0f)) * cos(zAngle), sin(glm::radians(160.0f)), cos(glm::radians(160.0f)) * sin(zAngle));
			lightIntensity = glm::clamp(sin(angle) * 0.5f + 0.5f, 0.0f, 1.0f);
		}
		else if (320.0f < glm::radians(angle) <= 360.0f || 0.0f <= glm::radians(angle) <= 20.0f) {
			gubo.lightDir[0] = glm::vec3(cos(glm::radians(20.0f) * cos(zAngle)), sin(glm::radians(20.0f)), cos(glm::radians(20.0f)) * sin(zAngle));
			lightIntensity = glm::clamp(sin(angle) * 0.5f + 0.5f, 0.0f, 1.0f);
		}
		else if (sin(angle) > 0) {
			gubo.lightDir[0] = glm::vec3(cos(angle) * cos(zAngle), sin(angle), cos(angle) * sin(zAngle));
			lightIntensity = 1.0f;
		}
		else {
			gubo.lightDir[0] = - glm::vec3(cos(angle) * cos(zAngle), sin(angle), cos(angle) * sin(zAngle));
			lightIntensity = 0.05f;
		}

		gubo.lightColor[0] = glm::vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		gubo.lightOn.x = 1.0f;
		gubo.lightOn.w = 1.0f;

		// PLANE SPOT LIGHT

		gubo.lightColor[1] = glm::vec4(1.5);
		gubo.lightColor[1].a = 5.0f;
		gubo.lightDir[1] = - glm::normalize(glm::vec3(Mplane.Wm[2]));
		gubo.lightPos[1] = planePosition;

		ScosIn = 0.6;
		ScosOut = 0.0;

		gubo.cosIn = ScosIn;
		gubo.cosOut = ScosOut;

		glm::vec3 fragPos = glm::vec3(ViewMatrix * glm::vec4(planePosition, 1.0f));

		// CITY POINT LIGHTS
		if (sin(angle) < 0.1) {
			std::vector<std::pair<int, float>> lightDistances; 

			for (int i = 0; i < (NUMLIGHTS - 2); i++) {
				glm::vec3 lightPos = LWm[i] * glm::vec4(0, 0, 0, 1);
				if (is_light_in_view(lightPos, ViewPrj)) {
					float dist = glm::length(lightPos - fragPos);
					lightDistances.push_back(std::make_pair(i, dist));
				}
			}

			std::sort(lightDistances.begin(), lightDistances.end(),
				[](const std::pair<int, float>& a, const std::pair<int, float>& b) {
					return a.second < b.second;
				});
			
			for (int i = 0; i < (MAXLIGHTS - 2); i++) {
				if (i < lightDistances.size()) {
					int lightIndex = lightDistances[i].first;
					gubo.lightColor[i + 2] = glm::vec4(LCol[lightIndex], LInt[lightIndex]);
					gubo.lightDir[i + 2] = LWm[lightIndex] * glm::vec4(0, 0, 1, 0);
					gubo.lightPos[i + 2] = LWm[lightIndex] * glm::vec4(0, 0, 0, 1);
					gubo.isOn[i + 2].x = 1.0f;
				}
				else {
					gubo.isOn[i + 2].x = 0.0f;
				}
			}

			gubo.lightOn.z = 1.0f; 
		}
		else {
			gubo.lightOn.z = 0.0f;
		}

		gubo.lightOn.y = planeLightOn;

		DSGlobal.map(currentImage, &gubo, 0);

		// SCENE
		skyBoxUniformBufferObject sbubo{};
		sbubo.mvpMat = M * glm::mat4(glm::mat3(Mv));
		DSskyBox.map(currentImage, &sbubo, 0);

		SunMoonUniformBufferObject sunmoonUbo{};

		sunmoonUbo.mvpMat = ViewPrj * Msun.Wm * baseTr;
		DSSun.map(currentImage, &sunmoonUbo, 0);

		sunmoonUbo.mvpMat = ViewPrj * Mmoon.Wm * baseTr;
		DSMoon.map(currentImage, &sunmoonUbo, 0);

		SimpleUniformBufferObject simpleUbo{};
		SimpleMatParUniformBufferObject simpleMatParUbo{};

		for (int i = 0; i < SC.InstanceCount; i++) {
			simpleUbo.mMat = SC.I[i].Wm * baseTr;
			simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
			simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
			simpleMatParUbo.Power = 300.0;
			SC.DS[i]->map(currentImage, &simpleUbo, 0);
			SC.DS[i]->map(currentImage, &simpleMatParUbo, 2);
		}

		simpleUbo.mMat = Mplane.Wm * baseTr;
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		DSPlane.map(currentImage, &simpleUbo, 0);
		simpleMatParUbo.Power = 300.0;
		DSPlane.map(currentImage, &simpleMatParUbo, 2);
		
		if (currScene != prevCurrScene) {
		}

		prevCurrScene = currScene;
	}
};

// This is the main: probably you do not need to touch this!
int main() {
	CGProject app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
