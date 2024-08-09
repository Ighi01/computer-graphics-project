// This has been adapted from the Vulkan tutorial
#define GLM_ENABLE_EXPERIMENTAL
#define _CRT_SECURE_NO_WARNINGS
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
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

	Model Mblack_car;
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
		windowWidth = 800;
		windowHeight = 600;
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
		//PSimple.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
		//	VK_CULL_MODE_NONE, false);
		Mblack_car.init(this, &VDSimple, "models/transport_cool_003_transport_cool_003.001.mgcg", MGCG);
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

		Mblack_car.cleanup();
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
		Mblack_car.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);
		DSCity.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mblack_car.indices.size()), 1, 0, 0, 0);

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
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		static float autoTime = true;
		static float cTime = 0.0;
		const float turnTime = 72.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		if(autoTime) {
			cTime = cTime + deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}

		static float tTime = 0.0;
		const float TturnTime = 60.0f;
		const float TangTurnTimeFact = 1.0f / TturnTime;
		
		if(autoTime) {
			tTime = tTime + deltaT;
			tTime = (tTime > TturnTime) ? (tTime - TturnTime) : tTime;
		}
		
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 20.0f;
		
		static float ShowCloud = 1.0f;
		static float ShowTexture = 1.0f;
		
		// The Fly model update proc.
		ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT,
								 glm::vec3(1, 0, 0)) * ViewMatrix;
		ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.y * deltaT,
								 glm::vec3(0, 1, 0)) * ViewMatrix;
		ViewMatrix = glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT,
								 glm::vec3(0, 0, 1)) * ViewMatrix;
		ViewMatrix = glm::translate(glm::mat4(1), -glm::vec3(
								   MOVE_SPEED * m.x * deltaT, MOVE_SPEED * m.y * deltaT, MOVE_SPEED * m.z * deltaT))
													   * ViewMatrix;
		static float subpassTimer = 0.0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if(currScene != 1) {
					currScene = (currScene+1) % outText.size();

				}
				if(currScene == 1) {
					if(subpass >= 4) {
						currScene = 0;
					}
				}
				std::cout << "Scene : " << currScene << "\n";
				
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printMat4("ViewMatrix  ", ViewMatrix);				
				std::cout << "cTime    = " << cTime    << ";\n";
				std::cout << "tTime    = " << tTime    << ";\n";
				std::cout << "ShowCloud    = " << ShowCloud    << ";\n";
				std::cout << "ShowTexture    = " << ShowTexture    << ";\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_C)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_C;
				
				ShowCloud = 1.0f - ShowCloud;
			}
		} else {
			if((curDebounce == GLFW_KEY_C) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_T)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_T;
				
				ShowTexture = 1.0f - ShowTexture;
			}
		} else {
			if((curDebounce == GLFW_KEY_T) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

	
		if(currScene == 1) {
			switch(subpass) {
			  case 0:
ViewMatrix   = glm::mat4(-0.0656882, -0.162777, 0.984474, 0, 0.0535786, 0.984606, 0.166374, 0, -0.996401, 0.0636756, -0.0559558, 0, 0.0649244, -0.531504, -3.26128, 1);
cTime    = 22.3604;
tTime    = 22.3604;
ShowCloud    = 1;
ShowTexture    = 1;
autoTime = false;
				break;
			  case 1:
ViewMatrix   = glm::mat4(-0.312507, -0.442291, 0.840666, 0, 0.107287, 0.862893, 0.493868, 0, -0.943837, 0.24453, -0.222207, 0, -0.0157694, -0.186147, -1.54649, 1);
cTime    = 38.9919;
tTime    = 38.9919;
ShowCloud    = 0;
ShowTexture    = 1;
				break;
			  case 2:
ViewMatrix   = glm::mat4(-0.992288, 0.00260993, -0.12393, 0, -0.0396232, 0.940648, 0.337063, 0, 0.117454, 0.339374, -0.93329, 0, 0.0335061, -0.0115242, -2.99662, 1);
cTime    = 71.0587;
tTime    = 11.0587;
ShowCloud    = 1;
ShowTexture    = 1;
				break;
			  case 3:
ViewMatrix   = glm::mat4(0.0942192, -0.242781, 0.965495, 0, 0.560756, 0.814274, 0.150033, 0, -0.822603, 0.527272, 0.212861, 0, -0.567191, -0.254532, -1.79143, 1);
cTime    = 55.9355;
tTime    = 7.93549;
ShowCloud    = 1;
ShowTexture    = 0;
				break;
			}
		}
		
		if(currScene == 1) {
			subpassTimer += deltaT;
			if(subpassTimer > 4.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A10_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 4) {
					ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
					cTime    = 0;
					tTime    = 0;
					ShowCloud    = 1;
					ShowTexture    = 1;
					autoTime = true;
					
					
					currScene = 0;
					std::cout << "Scene : " << currScene << "\n";
					RebuildPipeline();
				}
			}
		}


		// Here is where you actually update your uniforms
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj =  M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);								

		// updates global uniforms
		// Global
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		DSGlobal.map(currentImage, &gubo, 0);

		// objects

		SimpleUniformBufferObject simpleUbo{};
		SimpleMatParUniformBufferObject simpleMatParUbo{};

		//simpleUbo.mMat = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,5));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSCity.map(currentImage, &simpleUbo, 0);

		//simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0,5,0));
		simpleUbo.mMat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0,1,0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile4.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 8))* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile002.map(currentImage, &simpleUbo, 0);

		simpleMatParUbo.Power = 200.0;
		DSCity.map(currentImage, &simpleMatParUbo, 2);
		DStile4.map(currentImage, &simpleMatParUbo, 2);
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
