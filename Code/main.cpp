#define JSON_DIAGNOSTICS 1
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define _CRT_SECURE_NO_WARNINGS
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/Scene.hpp"

constexpr auto NUMLIGHTS = 35;
constexpr auto MAXLIGHTS = 16;

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

struct SceneUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct SceneMatParUniformBufferObject {
	alignas(4)  float Power;
};

struct SunMoonUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct skyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(4) float lightintensity;
	alignas(4) float rotation;

};

struct SceneVertex {
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
	DescriptorSetLayout DSLScene;
	DescriptorSetLayout DSLSunMoon;
	DescriptorSetLayout DSLSkyBox;

	DescriptorSet DSGlobal;
	DescriptorSet DSPlane;
	DescriptorSet DSSun;
	DescriptorSet DSMoon;
	DescriptorSet DSSkyBox;

    VertexDescriptor VDScene;
	VertexDescriptor VDSkyBox;
	VertexDescriptor VDSunMoon;

	Pipeline PScene;
	Pipeline PSkyBox;
	Pipeline PSunMoon;

	Model MPlane;
	Model MSun;
	Model MMoon;
	Model MSkyBox;

	Texture TScene;
	Texture TSun;
	Texture TMoon;
	Texture TSkyBox;

	Scene SC;

    TextMaker txt;

	float Ar;

	int currScene = 0, prevCurrScene = 0;
	bool start = false;

	float planeScaleFactor = 0.05f;
	float sunScaleFactor = 10.0f;
	float moonScaleFactor = 10.0f;

	float xSpeed = 0.75f;
	float ySpeed = 0.5f;
	float zSpeed = 1.0f;
	float speed = 10.0f;
	float skySpeed = 0.02f;

	float followSpeedThirdCamera = 1.0f;
	float followSpeedFirstCamera = 2.0f;
	float minDistanceThirdCamera = -0.5f;
	float maxDistanceThirdCamera = -3.5f;
	float offsetThirdCamera = 0.5f;

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
	float speedFactor = 1.5f;
	bool thirdPerson = true;

	glm::vec3 firstCameraTarget;
	glm::vec3 thirdCamPos;
	glm::mat4 ViewMatrix;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 LWm[NUMLIGHTS - 2];
	glm::vec3 LCol[NUMLIGHTS - 2];
	float LInt[NUMLIGHTS - 2];
	float ScosIn = 0.99f;
	float ScosOut = 0.97f;
	float planeLightOn = 0.0f;

	float turnTime = 360.0f;
	float sunMoonAngle = 30.0f * (M_PI / 180.0f);

	void setWindowParameters() {
		windowWidth = 1920;
		windowHeight = 1080;
		windowTitle = "CG-PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.4f, 0.7f, 1.0f, 1.0f };
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	void localInit() {
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
			});

		DSLScene.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SceneUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(SceneMatParUniformBufferObject), 1}
			});

		DSLSunMoon.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SunMoonUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
			});

		DSLSkyBox.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(skyBoxUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
			});


		VDScene.init(this, {
				  {0, sizeof(SceneVertex), VK_VERTEX_INPUT_RATE_VERTEX}
		}, {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SceneVertex, pos),
					sizeof(glm::vec3), POSITION},
			{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SceneVertex, norm),
					sizeof(glm::vec3), NORMAL},
			{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(SceneVertex, UV),
					sizeof(glm::vec2), UV}
		});

		VDSkyBox.init(this, {
				  {0, sizeof(skyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(skyBoxVertex, pos),
					 sizeof(glm::vec3), POSITION}
			});

		VDSunMoon.init(this, {
				  {0, sizeof(SceneVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SunMoonVertex, pos),
						sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(SunMoonVertex, UV),
						sizeof(glm::vec2), UV}
			});

		PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLSkyBox });
		PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT, true);
		PSunMoon.init(this, &VDSunMoon, "shaders/SunMoonVert.spv", "shaders/SunMoonFrag.spv", { &DSLSunMoon });
		PSunMoon.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);
		PScene.init(this, &VDScene, "shaders/SceneVert.spv", "shaders/SceneFrag.spv", { &DSLGlobal, &DSLScene });
		PScene.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		MSkyBox.init(this, &VDSkyBox, "models/SkyBoxCube.obj", OBJ);
		MSun.init(this, &VDScene, "models/Sphere.obj", OBJ);
		MMoon.init(this, &VDScene, "models/Sphere1.obj", OBJ);
		MPlane.init(this, &VDScene, "models/transport_air_008_transport_air_008.001.mgcg", MGCG);
		
		SC.init(this, &VDScene, DSLScene, PScene, "models/scene.json");
		
		TSkyBox.init(this, "textures/sky.png");
		TScene.init(this, "textures/Textures_City.png");
		TSun.init(this, "textures/sun.png");
		TMoon.init(this, "textures/moon.png");

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

		MSun.Wm *= glm::scale(glm::mat4(1.0f), glm::vec3(sunScaleFactor));
		MMoon.Wm *= glm::scale(glm::mat4(1.0f), glm::vec3(moonScaleFactor));
		MMoon.Wm *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		std::cout << "Light initialization completed!\n";
	}
	
	void pipelinesAndDescriptorSetsInit() {
		PSkyBox.create1();
		PSunMoon.create();
		PScene.create();
		
		DSSkyBox.init(this, &DSLSkyBox, { &TSkyBox });
		DSSun.init(this, &DSLSunMoon, { &TSun });
		DSMoon.init(this, &DSLSunMoon, { &TMoon });
		DSGlobal.init(this, &DSLGlobal, {});
		DSPlane.init(this, &DSLScene, { &TScene });
		
		SC.pipelinesAndDescriptorSetsInit(DSLScene);
		txt.pipelinesAndDescriptorSetsInit();		
	}

	void pipelinesAndDescriptorSetsCleanup() {
		PSkyBox.cleanup();
		PSunMoon.cleanup();
		PScene.cleanup();

		DSSkyBox.cleanup();
		DSSun.cleanup();
		DSMoon.cleanup();
		DSGlobal.cleanup();
		DSPlane.cleanup();
		
		SC.pipelinesAndDescriptorSetsCleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	void localCleanup() {
		DSLScene.cleanup();
		DSLGlobal.cleanup();
		DSLSunMoon.cleanup();
		DSLSkyBox.cleanup();

		TScene.cleanup();
		TSun.cleanup();
		TMoon.cleanup();
		TSkyBox.cleanup();

		MPlane.cleanup();
		MSun.cleanup();
		MMoon.cleanup();
		MSkyBox.cleanup();
		
		SC.localCleanup();
		PSkyBox.destroy();
		PScene.destroy();
		PSunMoon.destroy();
		txt.localCleanup();		
	}
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		PSkyBox.bind(commandBuffer);
		MSkyBox.bind(commandBuffer);
		DSSkyBox.bind(commandBuffer, PSkyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);
		
		PSunMoon.bind(commandBuffer);
		MSun.bind(commandBuffer);
		DSSun.bind(commandBuffer, PSunMoon, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSun.indices.size()), 1, 0, 0, 0);

		PSunMoon.bind(commandBuffer);
		MMoon.bind(commandBuffer);
		DSMoon.bind(commandBuffer, PSunMoon, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MMoon.indices.size()), 1, 0, 0, 0);

		PScene.bind(commandBuffer);
		MPlane.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PScene, 0, currentImage);
		DSPlane.bind(commandBuffer, PScene, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPlane.indices.size()), 1, 0, 0, 0);

		SC.populateCommandBuffer(commandBuffer, currentImage, PScene);
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	void updateUniformBuffer(uint32_t currentImage) {

		float deltaT;
		bool instantCameraChange;
		Direction direction;
		glm::vec3 firstCameraDirection = glm::vec3(0.0f);
		glm::vec3 movement = glm::vec3(0.0f);

		// ------------ COMMANDS ------------

		handleCommands(deltaT, movement, start, zoom, speedFactor, direction, instantCameraChange, thirdPerson, currScene, planeLightOn, currentImage);

		// ------------ MOVEMENT ------------

		if (start)
		{
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			bool block = false;

			if (MPlane.Wm[3].y <= maxDown) {
				if (accumulatedRotationX < glm::radians(180.0f)) {
					float rotationStep = - xSpeed * speedFactor * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(1.0f, 0.0f, 0.0f));
					accumulatedRotationX += glm::abs(rotationStep);
				}
				block = true;
			}
			else{
				accumulatedRotationX = 0.0;
			}

			if (MPlane.Wm[3].y >= maxUp) {
				if (accumulatedRotationX < glm::radians(180.0f)){
					float rotationStep = xSpeed * speedFactor / 2 * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(1.0f, 0.0f, 0.0f));
					accumulatedRotationX += glm::abs(rotationStep);
				}
				block = true;
			}
			else {
				accumulatedRotationX = 0.0;
			}

			if (MPlane.Wm[3].x >= maxX || MPlane.Wm[3].z >= maxZ || MPlane.Wm[3].x <= minX || MPlane.Wm[3].z <= minZ) {
				if (accumulatedRotationY < glm::radians(180.0f)) {
					float rotationStep = - ySpeed * speedFactor / 2 * deltaT;
					rotationMatrix = glm::rotate(rotationMatrix, rotationStep, glm::vec3(0.0f, 1.0f, 0.0f));
					accumulatedRotationY += glm::abs(rotationStep);
				}
				block = true;
			}
			else {
				accumulatedRotationY = 0.0;
			}

			if (!block) {
				rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f), movement.x * xSpeed * speedFactor / 2 * deltaT, glm::vec3(1.0f, 0.0f, 0.0f)), movement.y * ySpeed * speedFactor / 2 * deltaT, glm::vec3(0.0f, 1.0f, 0.0f)), movement.z * zSpeed * speedFactor / 2 * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			if (block && currScene == 4) {
				currScene = 6;
			}
			else if(currScene == 6){
				currScene = 4;
			}

			MPlane.Wm = glm::translate(MPlane.Wm * rotationMatrix, glm::vec3(0.0f, 0.0f, 1.0f) * speed * speedFactor * deltaT);
		}
		else {
			switch (currScene) {
			case 0:
				MPlane.Wm = glm::mat4(
					planeScaleFactor, 0.0f, 0.0f, 0.0f,
					0.0f, planeScaleFactor, 0.0f, 0.0f,
					0.0f, 0.0f, planeScaleFactor, 0.0f,
					48.0f, 2.0f, -112.0f, 1.0f
				);
				up = glm::vec3(0.0f, 1.0f, 0.0f);
				zoom = 1.0f;
				speedFactor = 1.5f;
				thirdPerson = true;
				break;
			case 1:
				MPlane.Wm = glm::mat4(
					0.0f, 0.0f, -planeScaleFactor, 0.0f,
					0.0f, planeScaleFactor, 0.0f, 0.0f,
					planeScaleFactor, 0.0f, 0.0f, 0.0f,
					-24.0f, 2.0f, 40.0f, 1.0f
				);
				break;
			case 2:
				MPlane.Wm = glm::mat4(
					planeScaleFactor, 0.0f, 0.0f, 0.0f,
					0.0f, planeScaleFactor, 0.0f, 0.0f,
					0.0f, 0.0f, planeScaleFactor, 0.0f,
					120.0f, 2.0f, -56.0f, 1.0f
				);
				break;
			case 3:
				MPlane.Wm = glm::mat4(
					0.0f, 0.0f, planeScaleFactor, 0.0f,
					0.0f, planeScaleFactor, 0.0f, 0.0f,
					-planeScaleFactor, 0.0f, 0.0f, 0.0f,
					16.0f, 2.0f, -72.0f, 1.0f
				);
				break;
			}
		}

		glm::vec3 planePosition = glm::vec3(MPlane.Wm[3]);
		glm::vec3 desiredThirdCamPos{};
		glm::vec3 desiredFirstCamTarget{};
		glm::vec3 thirdCameraDirection;

		if (start)
		{
			switch (direction) {			
			case FRONT:
				thirdCameraDirection = - glm::normalize(glm::vec3(MPlane.Wm[2])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom);
				desiredThirdCamPos = planePosition - thirdCameraDirection + glm::normalize(glm::vec3(MPlane.Wm[1])) * offsetThirdCamera;
				desiredFirstCamTarget = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(MPlane.Wm[2]));
				break;			
			case BACK:
				thirdCameraDirection = glm::normalize(glm::vec3(MPlane.Wm[2])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom);
				desiredThirdCamPos = planePosition - thirdCameraDirection + glm::normalize(glm::vec3(MPlane.Wm[1])) * offsetThirdCamera;
				desiredFirstCamTarget = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(MPlane.Wm[2]));
				break;
			case UP:
				thirdCameraDirection = glm::normalize(glm::vec3(MPlane.Wm[1])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom);
				desiredThirdCamPos = planePosition - thirdCameraDirection + glm::normalize(glm::vec3(MPlane.Wm[1])) * offsetThirdCamera;
				desiredFirstCamTarget = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(MPlane.Wm[2]));
				break;
			case LEFT:
				thirdCameraDirection = glm::normalize(glm::vec3(MPlane.Wm[0])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom);
				desiredThirdCamPos = planePosition - thirdCameraDirection + glm::normalize(glm::vec3(MPlane.Wm[0])) * offsetThirdCamera;
				desiredFirstCamTarget = planePosition + glm::vec3(10.0f) * glm::normalize(glm::vec3(MPlane.Wm[0]));
				break;
			case RIGHT:
				thirdCameraDirection = - glm::normalize(glm::vec3(MPlane.Wm[0])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom);
				desiredThirdCamPos = planePosition - thirdCameraDirection - glm::normalize(glm::vec3(MPlane.Wm[0])) * offsetThirdCamera;
				desiredFirstCamTarget = planePosition - glm::vec3(10.0f) * glm::normalize(glm::vec3(MPlane.Wm[0]));
				break;
			}

			if (instantCameraChange) {
				thirdCamPos = desiredThirdCamPos;
			}
			else{
				thirdCamPos += (desiredThirdCamPos - thirdCamPos) * followSpeedThirdCamera * speedFactor * deltaT;
			}

			if (thirdCamPos.y < 0)
			{
				thirdCamPos.y = 0;
			}

			up += (glm::normalize(glm::vec3(MPlane.Wm[1])) - up) * followSpeedThirdCamera  * speedFactor * deltaT;
			firstCameraTarget += (desiredFirstCamTarget - firstCameraTarget) * followSpeedFirstCamera * speedFactor * deltaT;
		}
		else {
			firstCameraTarget = planePosition + glm::vec3(10.0) * glm::normalize(glm::vec3(MPlane.Wm[2]));
			thirdCamPos = planePosition + glm::normalize(glm::vec3(MPlane.Wm[2])) * glm::mix(minDistanceThirdCamera, maxDistanceThirdCamera, zoom / maxZoom) + glm::normalize(glm::vec3(MPlane.Wm[1])) * offsetThirdCamera;
		}

		if (thirdPerson) {
			glm::vec3 thirdCamTarget = planePosition;
			ViewMatrix = glm::lookAt(thirdCamPos, thirdCamTarget, up);
		}
		else {
			glm::vec3 firstCamPos = planePosition - glm::vec3(0.1f) * glm::normalize(glm::vec3(MPlane.Wm[2])) + glm::vec3(0.125f) * glm::normalize(glm::vec3(MPlane.Wm[1]));
			ViewMatrix = glm::lookAt(firstCamPos, firstCameraTarget, glm::vec3(MPlane.Wm[1]));
		}

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 650.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj = M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);

		// ------------ TIME ------------

		static float t = 60.0;

		if (start) {
			t += deltaT;
			t = (t > turnTime) ? (t - turnTime) : t;
		}
		else {
			t = 60.0;
		}

		float angle = t * 2.0f * M_PI / turnTime;

		// ----- MOON SUN MOVEMENT -------

		MSun.Wm[3].x = 300.0f * (cos(angle) * cos(sunMoonAngle));
		MSun.Wm[3].y = 300.0f * sin(angle);
		MSun.Wm[3].z = 300.0f * (cos(angle) * sin(sunMoonAngle));

		MMoon.Wm[3].x = -MSun.Wm[3].x;
		MMoon.Wm[3].y = -MSun.Wm[3].y;
		MMoon.Wm[3].z = -MSun.Wm[3].z;

		// -------- GLOBAL LIGHTS --------

		GlobalUniformBufferObject gubo{};

		// DAY NIGHT CYCLE

		float lightIntensity;

		if (160.0f <= glm::radians(angle) <= 220.0f) {
			gubo.lightDir[0] = glm::vec3(cos(glm::radians(160.0f)) * cos(sunMoonAngle), sin(glm::radians(160.0f)), cos(glm::radians(160.0f)) * sin(sunMoonAngle));
			lightIntensity = glm::clamp(sin(angle) * 0.5f + 0.5f, 0.0f, 1.0f);
		}
		else if (320.0f < glm::radians(angle) <= 360.0f || 0.0f <= glm::radians(angle) <= 20.0f) {
			gubo.lightDir[0] = glm::vec3(cos(glm::radians(20.0f) * cos(sunMoonAngle)), sin(glm::radians(20.0f)), cos(glm::radians(20.0f)) * sin(sunMoonAngle));
			lightIntensity = glm::clamp(sin(angle) * 0.5f + 0.5f, 0.0f, 1.0f);
		}
		else if (sin(angle) > 0) {
			gubo.lightDir[0] = glm::vec3(cos(angle) * cos(sunMoonAngle), sin(angle), cos(angle) * sin(sunMoonAngle));
			lightIntensity = 1.0f;
		}
		else {
			gubo.lightDir[0] = - glm::vec3(cos(angle) * cos(sunMoonAngle), sin(angle), cos(angle) * sin(sunMoonAngle));
			lightIntensity = 0.05f;
		}

		gubo.lightColor[0] = glm::vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		gubo.lightOn.x = 1.0f;

		// AMBIENT LIGHT

		gubo.lightOn.w = 1.0f;

		// PLANE SPOT LIGHT

		gubo.lightColor[1] = glm::vec4(1.5);
		gubo.lightColor[1].a = 10.0f;
		gubo.lightDir[1] = - glm::normalize(glm::vec3(MPlane.Wm[2]));
		gubo.lightPos[1] = planePosition;

		gubo.cosIn = ScosIn;
		gubo.cosOut = ScosOut;

		// CITY POINT LIGHTS

		if (sin(angle) < 0.0) {

			std::vector<std::pair<int, float>> lightScores; 

			for (int i = 0; i < (NUMLIGHTS - 2); i++) {
				glm::vec3 lightPos = LWm[i] * glm::vec4(0, 0, 0, 1);
				if (isLightInView(lightPos, ViewPrj)) {
					float score = calculateLightScore(lightPos, ViewPrj);
					lightScores.push_back(std::make_pair(i, score));
				}
			}

			std::sort(lightScores.begin(), lightScores.end(),
				[](const std::pair<int, float>& a, const std::pair<int, float>& b) {
					return a.second > b.second;
				});
			
			for (int i = 0; i < (MAXLIGHTS - 2); i++) {
				if (i < lightScores.size()) {
					int lightIndex = lightScores[i].first;
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

		// ---- SKY BOX LIGHTS AND VERTEX ----

		skyBoxUniformBufferObject sbubo{};
		sbubo.lightintensity = lightIntensity;
		sbubo.mvpMat = M * glm::mat4(glm::mat3(Mv));

		static float skyTime = 0.0;
		skyTime += skySpeed * deltaT;

		if (skyTime >= 1.0f) {
			skyTime = 0.0;
		}

		if (start) {
			sbubo.rotation = skyTime;
		}
		else {
			sbubo.rotation = 0.0f;
		}
		DSSkyBox.map(currentImage, &sbubo, 0);

		// -------- SUN MOON VERTEX --------

		SunMoonUniformBufferObject sunmoonUbo{};
		sunmoonUbo.mvpMat = ViewPrj * MSun.Wm * baseTr;
		DSSun.map(currentImage, &sunmoonUbo, 0);

		sunmoonUbo.mvpMat = ViewPrj * MMoon.Wm * baseTr;
		DSMoon.map(currentImage, &sunmoonUbo, 0);

		// ---------- CITY VERTEX ----------

		SceneUniformBufferObject sceneUbo{};
		SceneMatParUniformBufferObject simpleMatParUbo{};

		for (int i = 0; i < SC.InstanceCount; i++) {
			sceneUbo.mMat = SC.I[i].Wm * baseTr;
			sceneUbo.mvpMat = ViewPrj * sceneUbo.mMat;
			sceneUbo.nMat = glm::inverse(glm::transpose(sceneUbo.mMat));
			simpleMatParUbo.Power = 300.0;
			SC.DS[i]->map(currentImage, &sceneUbo, 0);
			SC.DS[i]->map(currentImage, &simpleMatParUbo, 2);
		}

		sceneUbo.mMat = MPlane.Wm * baseTr;
		sceneUbo.mvpMat = ViewPrj * sceneUbo.mMat;
		sceneUbo.nMat = glm::inverse(glm::transpose(sceneUbo.mMat));
		DSPlane.map(currentImage, &sceneUbo, 0);
		simpleMatParUbo.Power = 300.0;
		DSPlane.map(currentImage, &simpleMatParUbo, 2);

		// -------- CHANGE SCENE --------

		if (currScene != prevCurrScene) {
			reloadCommandBuffersAsync();
		}

		prevCurrScene = currScene;
	}
};

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