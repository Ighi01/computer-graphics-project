// This has been adapted from the Vulkan tutorial
#define GLM_ENABLE_EXPERIMENTAL
#define _CRT_SECURE_NO_WARNINGS
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

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
class A10 : public BaseProject {
	protected:

	DescriptorSetLayout DSLGlobal;
    DescriptorSet DSGlobal;

	DescriptorSetLayout DSLCity;
	DescriptorSet DSCity;
	DescriptorSet DStile4;
	DescriptorSet DStile002;

    VertexDescriptor VDSimple;
	Pipeline PSimple;

    TextMaker txt;

	Model Mplane;
	Model Mtile4;
	Model Mtile002;

	Texture Tcity;

	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;

	float Ar;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1920;  
		windowHeight = 1080; 
		windowTitle = "CG-PROJECT";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
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
		Mtile4.init(this, &VDSimple, "models/road_tile_1x1_007.mgcg", MGCG);
		Mtile002.init(this, &VDSimple, "models/road_tile_1x1_002.mgcg", MGCG);

		Tcity.init(this, "textures/Textures_City.png");

		DPSZs.uniformBlocksInPool = 100;
		DPSZs.texturesInPool = 100;
		DPSZs.setsInPool = 100;

		std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
		Mplane.Wm = glm::rotate(Mplane.Wm, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Mplane.Wm = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f)) * Mplane.Wm;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		PSimple.create();
		DSGlobal.init(this, &DSLGlobal, {});
		DSCity.init(this, &DSLCity, { &Tcity });
		DStile4.init(this, &DSLCity, {&Tcity});
		DStile002.init(this, &DSLCity, { &Tcity });
		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		PSimple.cleanup();
		DSGlobal.cleanup();
		DStile4.cleanup();
		DStile002.cleanup();
		DSCity.cleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		Tcity.cleanup();

		Mplane.cleanup();
		Mtile4.cleanup();
		Mtile002.cleanup();

		DSLCity.cleanup();
		DSLGlobal.cleanup();

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
		DSCity.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mplane.indices.size()), 1, 0, 0, 0);

		PSimple.bind(commandBuffer);
		Mtile4.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);
		DStile4.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mtile4.indices.size()), 1, 0, 0, 0);

		PSimple.bind(commandBuffer);
		Mtile002.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);
		DStile002.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mtile002.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	bool start = false;

	float X_SPEED = 0.5f;
	float Y_SPEED = 0.25f;
	float Z_SPEED = 1.5f;
	float SPEED = 10.0f;

	float followSpeed = 1.0f;
	float minDistance = -4.0f;
	float maxDistance = -20.0f;

	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

	void updateUniformBuffer(uint32_t currentImage) {

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f);
		float speedFactor;
		
		getSixAxis(deltaT, m);
		handle_commands(start, speedFactor);

		static float cTime = 0.0;
		const float turnTime = 72.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		cTime = cTime + deltaT;
		cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;

		if (start)
		{
			glm::mat4 rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1.0f),m.x * X_SPEED * deltaT, right),m.y * Y_SPEED * deltaT, up),m.z * Z_SPEED * deltaT, forward);
			Mplane.Wm = glm::translate(Mplane.Wm * rotationMatrix, forward * SPEED * speedFactor * deltaT);
		}
		
		glm::vec3 planePosition = glm::vec3(Mplane.Wm[3]);

		if (start)
		{
			glm::vec3 cameraOffset = - glm::normalize(glm::vec3(Mplane.Wm[2])) * glm::mix(minDistance, maxDistance, speedFactor / 2.0f);
			glm::vec3 desiredCamPos = planePosition - cameraOffset;
			if (CamPos.y < 0.0f)
			{
				CamPos.y = 0.0f;
			}
			CamPos += (desiredCamPos - CamPos) * followSpeed * deltaT;
		}

		ViewMatrix = glm::lookAt(CamPos, planePosition, glm::vec3(Mplane.Wm[1]));

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj = M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);					

		// updates global uniforms
		// Global
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		DSGlobal.map(currentImage, &gubo, 0);

		// Objects
		SimpleUniformBufferObject simpleUbo{};
		SimpleMatParUniformBufferObject simpleMatParUbo{};

		simpleUbo.mMat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile4.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = Mplane.Wm * baseTr;
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		DSCity.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 8))* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile002.map(currentImage, &simpleUbo, 0);

		simpleMatParUbo.Power = 200.0;
		DStile4.map(currentImage, &simpleMatParUbo, 2);
		DSCity.map(currentImage, &simpleMatParUbo, 2);
		DStile002.map(currentImage, &simpleMatParUbo, 2);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A10 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
