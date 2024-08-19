// This has been adapted from the Vulkan tutorial
#define JSON_DIAGNOSTICS 1
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define _CRT_SECURE_NO_WARNINGS
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/Scene.hpp"

std::vector<SingleText> outText = {
	{2, {"Plane Simulator", "Press SPACE to start the engine","",""}, 0, 0}
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct SimpleUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct SimpleMatParUniformBufferObject {
	alignas(4)  float Power;
};

struct SimpleVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};


// MAIN ! 
class CGProject : public BaseProject {
	protected:

	DescriptorSetLayout DSLGlobal;
    DescriptorSet DSGlobal;
	DescriptorSetLayout DSLCity;
	DescriptorSet DSPlane;

    VertexDescriptor VDSimple;
	Pipeline PSimple;

	Scene SC;

	Model Mplane;
	Texture Tcity;
    TextMaker txt;

	glm::vec3 CamPos = glm::vec3(9.5, 2.5, 8.0);
	glm::mat4 ViewMatrix;

	float Ar;

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

		PSimple.init(this, &VDSimple, "shaders/SimpleVert.spv", "shaders/SimpleFrag.spv", { &DSLGlobal, &DSLCity });
		PSimple.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		Mplane.init(this, &VDSimple, "models/transport_air_008_transport_air_008.001.mgcg", MGCG);
		SC.init(this, &VDSimple, DSLCity, PSimple, "models/scene.json");
		

		Tcity.init(this, "textures/Textures_City.png");

		DPSZs.uniformBlocksInPool = 1000;
		DPSZs.texturesInPool = 1000;
		DPSZs.setsInPool = 1000;

		std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
		Mplane.Wm = glm::rotate(Mplane.Wm, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Mplane.Wm = glm::rotate(Mplane.Wm, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Mplane.Wm = glm::translate(Mplane.Wm, glm::vec3(-8.0f, 2.0f, 10.0f));
		Mplane.Wm *= glm::scale(glm::mat4(1.0f), glm::vec3(0.04f));
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		PSimple.create();

		DSGlobal.init(this, &DSLGlobal, {});
		DSPlane.init(this, &DSLCity, { &Tcity });
		SC.pipelinesAndDescriptorSetsInit(DSLCity);
		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		PSimple.cleanup();

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
		Tcity.cleanup();
		Mplane.cleanup();
		SC.localCleanup();
		PSimple.destroy();
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		PSimple.bind(commandBuffer);
		Mplane.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);
		DSPlane.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mplane.indices.size()), 1, 0, 0, 0);

		SC.populateCommandBuffer(commandBuffer, currentImage, PSimple);
		txt.populateCommandBuffer(commandBuffer, currentImage, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	bool start = false;

	float X_SPEED = 0.5f;
	float Y_SPEED = 0.25f;
	float Z_SPEED = 0.75f;
	float SPEED = 10.0f;

	float followSpeed = 0.5f;
	float minDistance = -0.2f;
	float maxDistance = -1.0f;
	float camUpOffset = 15.0f;

	float maxX = 100.0;
	float maxZ = 100.0;
	float maxDown = 1.0;
	float maxUp = 50.0;

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	void updateUniformBuffer(uint32_t currentImage) {

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f);
		float speedFactor;
		int cameraDirection;
		bool instantCamera;

		getSixAxis(deltaT, m);
		handle_commands(start, speedFactor, cameraDirection, instantCamera);

		if (start)
		{
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			bool block = false;

			if (Mplane.Wm[3].y <= maxDown) {
				rotationMatrix = glm::rotate(rotationMatrix, -X_SPEED * deltaT, glm::vec3(1.0f, 0.0f, 0.0f));
				block = true;
			}
			if (Mplane.Wm[3].y >= maxUp) {
				rotationMatrix = glm::rotate(rotationMatrix, X_SPEED * deltaT, glm::vec3(1.0f, 0.0f, 0.0f));
				block = true;
			}
			if (glm::abs(Mplane.Wm[3].x) >= maxX || glm::abs(Mplane.Wm[3].z) >= maxZ) {
				rotationMatrix = glm::rotate(rotationMatrix, -Y_SPEED * deltaT, glm::vec3(0.0f, 1.0f, 0.0f));
				block = true;
			}

			if (!block) {
				rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f), m.x * X_SPEED * deltaT, glm::vec3(1.0f, 0.0f, 0.0f)), m.y * Y_SPEED * deltaT, glm::vec3(0.0f, 1.0f, 0.0f)), m.z * Z_SPEED * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			Mplane.Wm = glm::translate(Mplane.Wm * rotationMatrix, glm::vec3(0.0f, 0.0f, 1.0f) * SPEED * speedFactor * deltaT);
		}

		glm::vec3 planePosition = glm::vec3(Mplane.Wm[3]);
		glm::vec3 desiredCamPos;

		if (start)
		{
			glm::vec3 cameraOffset = glm::vec3(cameraDirection) * glm::normalize(glm::vec3(Mplane.Wm[2])) * ((cameraDirection < 0) ? glm::mix(minDistance, maxDistance, speedFactor) : glm::mix(2 * minDistance, 2 * maxDistance, speedFactor));
			desiredCamPos = planePosition - cameraOffset + glm::vec3(Mplane.Wm[1]) * camUpOffset;
			
			if (instantCamera) {
				CamPos = desiredCamPos;
			}
			else {
				CamPos += (desiredCamPos - CamPos) * followSpeed * speedFactor * deltaT;
			}
			if (CamPos.y < 0)
			{
				CamPos.y = 0;
			}
			up += (glm::vec3(Mplane.Wm[1]) - up) * followSpeed * deltaT;
		}

		ViewMatrix = glm::lookAt(CamPos, planePosition, up);

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj = M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);

		static float cTime = 50.0;
		const float turnTime = 300.0f;

		if (!glfwGetKey(window, GLFW_KEY_Z))
			cTime += deltaT;

		float angle = cTime * 2.0f * M_PI / turnTime;

		GlobalUniformBufferObject gubo{};

		float lightIntensity = glm::clamp(sin(angle) * 0.5f + 0.5f, 0.0f, 1.0f);
		
		glm::vec3 dayColor = glm::vec3(0.4f, 0.7f, 1.0f);  
		glm::vec3 nightColor = glm::vec3(0.01f, 0.01f, 0.05f);

		glm::vec3 backgroundColor = glm::mix(nightColor, dayColor, lightIntensity);
		setBackgroundColor(backgroundColor);

		if (sin(angle) > 0.01) {
			gubo.lightDir = glm::vec3(0.0f, sin(angle), cos(angle));;
			gubo.lightColor = glm::vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
			gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		}
		DSGlobal.map(currentImage, &gubo, 0);

		// objects

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
