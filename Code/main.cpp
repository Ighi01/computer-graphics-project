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
class CGProject : public BaseProject {
	protected:

	DescriptorSetLayout DSLGlobal;
    DescriptorSet DSGlobal;

	DescriptorSetLayout DSLCity;
	DescriptorSet DSPlane;
	DescriptorSet DStile004;
	DescriptorSet DStile002;
	DescriptorSet DSport003;
	DescriptorSet DSport003_2;
	DescriptorSet DSport003_3;
	DescriptorSet DSrail004;
	DescriptorSet DScruise;
	DescriptorSet DSmerch;
	DescriptorSet DS3cross;
	DescriptorSet DS3cross_1;
	DescriptorSet DS3cross_2;
	DescriptorSet DS3cross_3;
	DescriptorSet DSport004;
	DescriptorSet DSport005;
	DescriptorSet DSport004_1;
	DescriptorSet DSport005_1;
	DescriptorSet DSport005_2;
	DescriptorSet DSport005_3;
	DescriptorSet DSpier1;
	DescriptorSet DSpier2;
	DescriptorSet DSship003;
	DescriptorSet DSship007;
	DescriptorSet DSship008;
	DescriptorSet DSship010;
	DescriptorSet DSsea;
	DescriptorSet DSsilos;
	DescriptorSet DSmanufacturer004;
	DescriptorSet DSmanufacturer009;
	DescriptorSet DSpetrolST;
	DescriptorSet DSgrey;
	DescriptorSet DSgrey_1;
	DescriptorSet DSbeach005;
	DescriptorSet DSbeach005_1;
	DescriptorSet DSbeach005_2;
	DescriptorSet DSbeach005_3;
	DescriptorSet DScontainer;
	DescriptorSet DScrane;
	DescriptorSet DShangar;
	//DescriptorSet DS;
	DescriptorSet DSbeach003;
	DescriptorSet DSbeach004;
	DescriptorSet DSbeach006;
	DescriptorSet DSbeach007;
	DescriptorSet DSparking;
	DescriptorSet DSroad009;
	DescriptorSet DSroad009_1;
	DescriptorSet DSroad009_2;
	DescriptorSet DSroad009_3;
	DescriptorSet DSroad009_4;
	DescriptorSet DStennis;
	DescriptorSet DScurve;
	DescriptorSet DScurve_1;
	DescriptorSet DSroad001;
	DescriptorSet DSroad001_1;
	DescriptorSet DSroad001_2;
	DescriptorSet DSfootball;
	DescriptorSet DSbasket;
	DescriptorSet DShotel001;
	DescriptorSet DShotel003;
	DescriptorSet DSburger;
	DescriptorSet DSdrink;
	DescriptorSet DSstore005;
	DescriptorSet DSgreen;


    VertexDescriptor VDSimple;
	Pipeline PSimple;

    TextMaker txt;

	Model Mplane;
	Model Mtile004;
	Model Mtile002;
	Model Mport003;
	Model Mrail004;
	Model Msea;
	Model Mcruise;
	Model Mmerch;
	Model M3cross;
	Model Mport004;
	Model Mport005;
	Model Mpier;
	Model Mship003;
	Model Mship007;
	Model Mship008;
	Model Mship010;
	Model Msilos;
	Model Mmanufacturer004;
	Model Mmanufacturer009;
	Model MpetrolST;
	Model Mgrey;
	Model Mbeach005;
	Model Mcontainer;
	Model Mcrane;
	Model Mhangar;
	Model Mbeach003;
	Model Mbeach004;
	Model Mbeach006;
	Model Mbeach007;
	Model Mparking;
	Model Mroad009;
	Model Mtennis;
	//Model M;
	Model Mroad001;
	Model Mcurve;
	Model Mfootball;
	Model Mbasket;
	Model Mhotel001;
	Model Mhotel003;
	Model Mburger;
	Model Mdrink;
	Model Mstore005;
	Model Mgreen;

	Texture Tcity;
	Texture Tmedieval;

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
		Mtile004.init(this, &VDSimple, "models/road_tile_1x1_007.mgcg", MGCG);
		Mtile002.init(this, &VDSimple, "models/road_tile_1x1_002.mgcg", MGCG);
		Mport003.init(this, &VDSimple, "models/port/port_tile_1x1_003.mgcg", MGCG);
		Mrail004.init(this, &VDSimple, "models/port/railing_004.mgcg", MGCG);
		Msea.init(this, &VDSimple, "models/port/tile_for_home_1x1_023.mgcg", MGCG);
		Mcruise.init(this, &VDSimple, "models/port/transport_water_006_transport_water_006.001.mgcg", MGCG);
		Mmerch.init(this, &VDSimple, "models/port/transport_water_005_transport_water_005.002.mgcg", MGCG);
		M3cross.init(this, &VDSimple, "models/port/road_tile_1x1_008.mgcg", MGCG);
		Mport004.init(this, &VDSimple, "models/port/port_tile_1x1_004.mgcg", MGCG);
		Mport005.init(this, &VDSimple, "models/port/port_tile_1x1_005.mgcg", MGCG);
		Mpier.init(this, &VDSimple, "models/port/SPW_Medieval_Props_Stand_01.mgcg", MGCG);
		Mship003.init(this, &VDSimple, "models/port/transport_water_003_transport_water_003.001.mgcg", MGCG);
		Mship007.init(this, &VDSimple, "models/port/transport_water_007_transport_water_007.001.mgcg", MGCG);
		Mship008.init(this, &VDSimple, "models/port/transport_water_008_transport_water_008.001.mgcg", MGCG);
		Mship010.init(this, &VDSimple, "models/port/transport_water_010_transport_water_010.001.mgcg", MGCG);
		Msilos.init(this, &VDSimple, "models/industry/house_004.mgcg", MGCG);
		Mmanufacturer004.init(this, &VDSimple, "models/industry/manufacture_004.mgcg", MGCG);
		Mmanufacturer009.init(this, &VDSimple, "models/industry/manufacture_009.mgcg", MGCG);
		MpetrolST.init(this, &VDSimple, "models/industry/production_001.mgcg", MGCG);
		Mgrey.init(this, &VDSimple, "models/industry/tile_for_home_1x1_007.mgcg", MGCG);
		Mbeach005.init(this, &VDSimple, "models/industry/beach_tile_1x1_005.mgcg", MGCG);
		Mcontainer.init(this, &VDSimple, "models/industry/container_004.mgcg", MGCG);
		Mcrane.init(this, &VDSimple, "models/industry/Crane_001.mgcg", MGCG);
		Mhangar.init(this, &VDSimple, "models/industry/Hangar_001.mgcg", MGCG);
		Mbeach003.init(this, &VDSimple, "models/beach/beach_tile_1x1_003.mgcg", MGCG);
		Mbeach004.init(this, &VDSimple, "models/beach/beach_tile_1x1_004.mgcg", MGCG);
		Mbeach006.init(this, &VDSimple, "models/beach/beach_tile_1x1_006.mgcg", MGCG);
		Mbeach007.init(this, &VDSimple, "models/beach/beach_tile_1x1_007.mgcg", MGCG);
		Mparking.init(this, &VDSimple, "models/beach/parking_tile_2x2_002.mgcg", MGCG);
		Mroad009.init(this, &VDSimple, "models/beach/road_tile_1x1_009.mgcg", MGCG);
		Mtennis.init(this, &VDSimple, "models/sport/landscape_entertainments_007.mgcg", MGCG);
		Mroad001.init(this, &VDSimple, "models/sport/road_tile_1x1_001.mgcg", MGCG);
		Mcurve.init(this, &VDSimple, "models/sport/road_tile_1x1_003.mgcg", MGCG);
		Mfootball.init(this, &VDSimple, "models/sport/landscape_entertainments_008.mgcg", MGCG);
		Mbasket.init(this, &VDSimple, "models/sport/landscape_entertainments_010.mgcg", MGCG);
		Mhotel001.init(this, &VDSimple, "models/beach/apartment_1_001_apartment_1_001.004.mgcg", MGCG);
		Mhotel003.init(this, &VDSimple, "models/beach/apartment_1_003.mgcg", MGCG);
		Mburger.init(this, &VDSimple, "models/beach/store_004.mgcg", MGCG);
		Mdrink.init(this, &VDSimple, "models/beach/store_003.mgcg", MGCG);
		Mstore005.init(this, &VDSimple, "models/beach/store_005.mgcg", MGCG);
		Mgreen.init(this, &VDSimple, "models/beach/tile_for_home_1x1_006.mgcg", MGCG);

		Tcity.init(this, "textures/Textures_City.png");
		Tmedieval.init(this, "textures/SPW_Medieval.png");

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
		DStile004.init(this, &DSLCity, {&Tcity});
		DStile002.init(this, &DSLCity, { &Tcity });
		DSport003.init(this, &DSLCity, { &Tcity });
		DSport003_2.init(this, &DSLCity, { &Tcity });
		DSport003_3.init(this, &DSLCity, { &Tcity });
		DSrail004.init(this, &DSLCity, { &Tcity });
		DSsea.init(this, &DSLCity, { &Tcity });
		DSgrey.init(this, &DSLCity, { &Tcity });
		DSgrey_1.init(this, &DSLCity, { &Tcity });
		DScruise.init(this, &DSLCity, { &Tcity });
		DSmerch.init(this, &DSLCity, { &Tcity });
		DS3cross.init(this, &DSLCity, { &Tcity });
		DS3cross_1.init(this, &DSLCity, { &Tcity });
		DS3cross_2.init(this, &DSLCity, { &Tcity });
		DS3cross_3.init(this, &DSLCity, { &Tcity });
		DSport004.init(this, &DSLCity, { &Tcity });
		DSport005.init(this, &DSLCity, { &Tcity });
		DSport004_1.init(this, &DSLCity, { &Tcity });
		DSport005_1.init(this, &DSLCity, { &Tcity });
		DSport005_2.init(this, &DSLCity, { &Tcity });
		DSport005_3.init(this, &DSLCity, { &Tcity });
		DSpier1.init(this, &DSLCity, { &Tmedieval });
		DSpier2.init(this, &DSLCity, { &Tmedieval });
		DSship003.init(this, &DSLCity, { &Tcity });
		DSship007.init(this, &DSLCity, { &Tcity });
		DSship008.init(this, &DSLCity, { &Tcity });
		DSship010.init(this, &DSLCity, { &Tcity });
		DSsilos.init(this, &DSLCity, { &Tcity });
		DSmanufacturer004.init(this, &DSLCity, { &Tcity });
		DSmanufacturer009.init(this, &DSLCity, { &Tcity });
		DSpetrolST.init(this, &DSLCity, { &Tcity });
		DSbeach005.init(this, &DSLCity, { &Tcity });
		DSbeach005_1.init(this, &DSLCity, { &Tcity });
		DSbeach005_2.init(this, &DSLCity, { &Tcity });
		DSbeach005_3.init(this, &DSLCity, { &Tcity });
		DScontainer.init(this, &DSLCity, { &Tcity });
		DScrane.init(this, &DSLCity, { &Tcity });
		DShangar.init(this, &DSLCity, { &Tcity });
		DSbeach003.init(this, &DSLCity, { &Tcity });
		DSbeach004.init(this, &DSLCity, { &Tcity });
		DSbeach006.init(this, &DSLCity, { &Tcity });
		DSbeach007.init(this, &DSLCity, { &Tcity });
		DSparking.init(this, &DSLCity, { &Tcity });
		DSroad009.init(this, &DSLCity, { &Tcity });
		DSroad009_1.init(this, &DSLCity, { &Tcity });
		DSroad009_2.init(this, &DSLCity, { &Tcity });
		DSroad009_3.init(this, &DSLCity, { &Tcity });
		DSroad009_4.init(this, &DSLCity, { &Tcity });
		DStennis.init(this, &DSLCity, { &Tcity });
		DSroad001.init(this, &DSLCity, { &Tcity });
		DSroad001_1.init(this, &DSLCity, { &Tcity });
		DSroad001_2.init(this, &DSLCity, { &Tcity });
		DScurve.init(this, &DSLCity, { &Tcity });
		DScurve_1.init(this, &DSLCity, { &Tcity });
		DSfootball.init(this, &DSLCity, { &Tcity });
		DSbasket.init(this, &DSLCity, { &Tcity });
		DShotel001.init(this, &DSLCity, { &Tcity });
		DShotel003.init(this, &DSLCity, { &Tcity });
		DSburger.init(this, &DSLCity, { &Tcity });
		DSdrink.init(this, &DSLCity, { &Tcity });
		DSstore005.init(this, &DSLCity, { &Tcity });
		DSgreen.init(this, &DSLCity, { &Tcity });

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		PSimple.cleanup();

		DSGlobal.cleanup();
		DStile004.cleanup();
		DStile002.cleanup();
		DSPlane.cleanup();
		DSport003.cleanup();
		DSport003_2.cleanup();
		DSport003_3.cleanup();
		DSrail004.cleanup();
		DSsea.cleanup();
		DScruise.cleanup();
		DSmerch.cleanup();
		DS3cross.cleanup();
		DS3cross_1.cleanup();
		DS3cross_2.cleanup();
		DS3cross_3.cleanup();
		DSport004.cleanup();
		DSport005.cleanup();
		DSport004_1.cleanup();
		DSport005_1.cleanup();
		DSport005_2.cleanup();
		DSport005_3.cleanup();
		DSpier1.cleanup();
		DSpier2.cleanup();
		DSship003.cleanup();
		DSship007.cleanup();
		DSship008.cleanup();
		DSship010.cleanup();
		DSsilos.cleanup();
		DSmanufacturer004.cleanup();
		DSmanufacturer009.cleanup();
		DSpetrolST.cleanup();
		DSgrey.cleanup();
		DSgrey_1.cleanup();
		DSbeach005.cleanup();
		DSbeach005_1.cleanup();
		DSbeach005_2.cleanup();
		DSbeach005_3.cleanup();
		DScontainer.cleanup();
		DScrane.cleanup();
		DShangar.cleanup();
		DSbeach003.cleanup();
		DSbeach004.cleanup();
		DSbeach006.cleanup();
		DSbeach007.cleanup();
		DSparking.cleanup();
		DSroad009.cleanup();
		DSroad009_1.cleanup();
		DSroad009_2.cleanup();
		DSroad009_3.cleanup();
		DSroad009_4.cleanup();
		DStennis.cleanup();
		DSroad001.cleanup();
		DSroad001_1.cleanup();
		DSroad001_2.cleanup();
		DScurve.cleanup();
		DScurve_1.cleanup();
		DSfootball.cleanup();
		DSbasket.cleanup();
		DShotel001.cleanup();
		DShotel003.cleanup();
		DSburger.cleanup();
		DSdrink.cleanup();
		DSstore005.cleanup();
		DSgreen.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		Tcity.cleanup();
		Tmedieval.cleanup();

		Mplane.cleanup();
		Mtile004.cleanup();
		Mtile002.cleanup();
		Mport003.cleanup();
		Mrail004.cleanup();
		Msea.cleanup();
		Mcruise.cleanup();
		Mmerch.cleanup();
		M3cross.cleanup();
		Mport004.cleanup();
		Mport005.cleanup();
		Mpier.cleanup();
		Mship003.cleanup();
		Mship007.cleanup();
		Mship008.cleanup();
		Mship010.cleanup();
		Msilos.cleanup();
		Mmanufacturer004.cleanup();
		Mmanufacturer009.cleanup();
		MpetrolST.cleanup();
		Mgrey.cleanup();
		Mbeach005.cleanup();
		Mcontainer.cleanup();
		Mcrane.cleanup();
		Mhangar.cleanup();
		Mbeach003.cleanup();
		Mbeach004.cleanup();
		Mbeach006.cleanup();
		Mbeach007.cleanup();
		Mparking.cleanup();
		Mroad009.cleanup();
		Mtennis.cleanup();
		Mroad001.cleanup();
		Mcurve.cleanup();
		Mfootball.cleanup();
		Mbasket.cleanup();
		Mhotel001.cleanup();
		Mhotel003.cleanup();
		Mburger.cleanup();
		Mdrink.cleanup();
		Mstore005.cleanup();
		Mgreen.cleanup();

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
		DSGlobal.bind(commandBuffer, PSimple, 0, currentImage);

		//major pier and roads
		Mplane.bind(commandBuffer);
		DSPlane.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mplane.indices.size()), 1, 0, 0, 0);

		Mtile004.bind(commandBuffer);
		DStile004.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mtile004.indices.size()), 1, 0, 0, 0);

		Mtile002.bind(commandBuffer);
		DStile002.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mtile002.indices.size()), 1, 0, 0, 0);

		Mport003.bind(commandBuffer);
		DSport003.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport003.indices.size()), 1, 0, 0, 0);

		Mport003.bind(commandBuffer);
		DSport003_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport003.indices.size()), 1, 0, 0, 0);

		Mport003.bind(commandBuffer);
		DSport003_3.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport003.indices.size()), 1, 0, 0, 0);

		Mrail004.bind(commandBuffer);
		DSrail004.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mrail004.indices.size()), 1, 0, 0, 0);

		//harbour
		Mcruise.bind(commandBuffer);
		DScruise.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mcruise.indices.size()), 1, 0, 0, 0);

		Mmerch.bind(commandBuffer);
		DSmerch.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mmerch.indices.size()), 1, 0, 0, 0);

		M3cross.bind(commandBuffer);
		DS3cross.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M3cross.indices.size()), 1, 0, 0, 0);

		M3cross.bind(commandBuffer);
		DS3cross_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M3cross.indices.size()), 1, 0, 0, 0);

		M3cross.bind(commandBuffer);
		DS3cross_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M3cross.indices.size()), 1, 0, 0, 0);

		M3cross.bind(commandBuffer);
		DS3cross_3.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M3cross.indices.size()), 1, 0, 0, 0);

		//piers and roads
		Mport004.bind(commandBuffer);
		DSport004.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport004.indices.size()), 1, 0, 0, 0);

		Mport005.bind(commandBuffer);
		DSport005.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport005.indices.size()), 1, 0, 0, 0);

		Mport004.bind(commandBuffer);
		DSport004_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport004.indices.size()), 1, 0, 0, 0);

		Mport005.bind(commandBuffer);
		DSport005_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport005.indices.size()), 1, 0, 0, 0);

		Mport005.bind(commandBuffer);
		DSport005_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport005.indices.size()), 1, 0, 0, 0);

		Mport005.bind(commandBuffer);
		DSport005_3.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mport005.indices.size()), 1, 0, 0, 0);

		Mpier.bind(commandBuffer);
		DSpier1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mpier.indices.size()), 1, 0, 0, 0);

		Mpier.bind(commandBuffer);
		DSpier2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mpier.indices.size()), 1, 0, 0, 0);

		Mship003.bind(commandBuffer);
		DSship003.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mship003.indices.size()), 1, 0, 0, 0);

		Mship007.bind(commandBuffer);
		DSship007.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mship007.indices.size()), 1, 0, 0, 0);

		Mship008.bind(commandBuffer);
		DSship008.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mship008.indices.size()), 1, 0, 0, 0);

		Mship010.bind(commandBuffer);
		DSship010.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mship010.indices.size()), 1, 0, 0, 0);

		//industrial zone
		Mmanufacturer004.bind(commandBuffer);
		DSmanufacturer004.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mmanufacturer004.indices.size()), 1, 0, 0, 0);

		Mmanufacturer009.bind(commandBuffer);
		DSmanufacturer009.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mmanufacturer009.indices.size()), 1, 0, 0, 0);

		Mbeach005.bind(commandBuffer);
		DSbeach005.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach005.indices.size()), 1, 0, 0, 0);

		Mbeach005.bind(commandBuffer);
		DSbeach005_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach005.indices.size()), 1, 0, 0, 0);

		Mbeach005.bind(commandBuffer);
		DSbeach005_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach005.indices.size()), 1, 0, 0, 0);

		Mbeach005.bind(commandBuffer);
		DSbeach005_3.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach005.indices.size()), 1, 0, 0, 0);

		Mcontainer.bind(commandBuffer);
		DScontainer.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mcontainer.indices.size()), 1, 0, 0, 0);

		Mcrane.bind(commandBuffer);
		DScrane.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mcrane.indices.size()), 1, 0, 0, 0);

		Mhangar.bind(commandBuffer);
		DShangar.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mhangar.indices.size()), 1, 0, 0, 0);

		Msilos.bind(commandBuffer);
		DSsilos.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Msilos.indices.size()), 1, 0, 0, 0);

		//beach zone
		Mbeach003.bind(commandBuffer);
		DSbeach003.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach003.indices.size()), 1, 0, 0, 0);

		Mbeach004.bind(commandBuffer);
		DSbeach004.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach004.indices.size()), 1, 0, 0, 0);

		Mbeach006.bind(commandBuffer);
		DSbeach006.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach006.indices.size()), 1, 0, 0, 0);

		Mbeach007.bind(commandBuffer);
		DSbeach007.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbeach007.indices.size()), 1, 0, 0, 0);

		Mparking.bind(commandBuffer);
		DSparking.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mparking.indices.size()), 1, 0, 0, 0);

		Mroad009.bind(commandBuffer);
		DSroad009.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad009.indices.size()), 1, 0, 0, 0);

		Mroad009.bind(commandBuffer);
		DSroad009_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad009.indices.size()), 1, 0, 0, 0);

		Mhotel001.bind(commandBuffer);
		DShotel001.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mhotel001.indices.size()), 1, 0, 0, 0);

		Mhotel003.bind(commandBuffer);
		DShotel003.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mhotel003.indices.size()), 1, 0, 0, 0);

		Mburger.bind(commandBuffer);
		DSburger.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mburger.indices.size()), 1, 0, 0, 0);

		Mdrink.bind(commandBuffer);
		DSdrink.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mdrink.indices.size()), 1, 0, 0, 0);

		Mstore005.bind(commandBuffer);
		DSstore005.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mstore005.indices.size()), 1, 0, 0, 0);

		//sport
		Mtennis.bind(commandBuffer);
		DStennis.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mtennis.indices.size()), 1, 0, 0, 0);

		Mroad001.bind(commandBuffer);
		DSroad001.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad001.indices.size()), 1, 0, 0, 0);

		Mroad001.bind(commandBuffer);
		DSroad001_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad001.indices.size()), 1, 0, 0, 0);

		Mroad001.bind(commandBuffer);
		DSroad001_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad001.indices.size()), 1, 0, 0, 0);

		Mcurve.bind(commandBuffer);
		DScurve.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mcurve.indices.size()), 1, 0, 0, 0);

		Mcurve.bind(commandBuffer);
		DScurve_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mcurve.indices.size()), 1, 0, 0, 0);

		Mfootball.bind(commandBuffer);
		DSfootball.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mfootball.indices.size()), 1, 0, 0, 0);

		Mbasket.bind(commandBuffer);
		DSbasket.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbasket.indices.size()), 1, 0, 0, 0);

		Mroad009.bind(commandBuffer);
		DSroad009_2.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad009.indices.size()), 1, 0, 0, 0);

		Mroad009.bind(commandBuffer);
		DSroad009_3.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad009.indices.size()), 1, 0, 0, 0);

		Mroad009.bind(commandBuffer);
		DSroad009_4.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mroad009.indices.size()), 1, 0, 0, 0);

		//SEA
		Msea.bind(commandBuffer);
		DSsea.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Msea.indices.size()), 1, 0, 0, 0);

		//GREY BASE
		Mgrey.bind(commandBuffer);
		DSgrey.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mgrey.indices.size()), 1, 0, 0, 0);

		Mgrey.bind(commandBuffer);
		DSgrey_1.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mgrey.indices.size()), 1, 0, 0, 0);

		//GREEN BASE
		Mgreen.bind(commandBuffer);
		DSgreen.bind(commandBuffer, PSimple, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mgreen.indices.size()), 1, 0, 0, 0);

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

		simpleUbo.mMat = Mplane.Wm * baseTr;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSPlane.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -40)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile004.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -32))* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStile002.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.125, 40));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport003.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, -0.125, 40));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport003_2.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(16, -0.125, 40));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport003_3.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(19.9, 0, 40)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSrail004.map(currentImage, &simpleUbo, 0);

		// ships
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, -1, 35)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DScruise.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, -1, 45)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.4)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSmerch.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(5, -1, 15.5)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSship003.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, 26)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.9)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));;
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSship007.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(10, -1, 15)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.9));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSship008.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, 21)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSship010.map(currentImage, &simpleUbo, 0);

		//harbour streets
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 40));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DS3cross.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DS3cross_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 24));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport004.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 32));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport005.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport004_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport005_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport005_2.map(currentImage, &simpleUbo, 0);

		// pier
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5, -1.5, 24)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSpier1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, -1.5, 14.5)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSpier2.map(currentImage, &simpleUbo, 0);

		//industrial area
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-18, 0, 34)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSmanufacturer004.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-31, 0, 21)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.8));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSmanufacturer009.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-17, 0, 21)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSsilos.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-16, 0, 44));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach005.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-24, 0, 44));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach005_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-32, 0, 44));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach005_2.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8, 0, 44));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach005_3.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-30, 0, 32)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DScontainer.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-32, 0, 40)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DScrane.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-24, 0, 20)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DShangar.map(currentImage, &simpleUbo, 0);
		
		//beach area
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(32, 0, 16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach003.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(40, 0, 16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach004.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(24, 0, 16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach006.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(16, 0, 16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbeach007.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(28, 0, -4));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSparking.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(32, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad009.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(16, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSport005_3.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(24, 0, 8));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DS3cross_2.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(16, 0, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSburger.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSdrink.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, 0, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSstore005.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(64, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DShotel001.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(64, 0, -12)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DShotel003.map(currentImage, &simpleUbo, 0);

		//sport area
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(44, 0, -4));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DStennis.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(40, 0, 8));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad009_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(32, 0, -16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad009_2.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(40, 0, -16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad009_3.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(56, 0, -8)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad009_4.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(48, 0, 8));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad001.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(48, 0, -16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad001_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(56, 0, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSroad001_2.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(56, 0, 8)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DScurve.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(56, 0, -16));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DScurve_1.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(24, 0, -16)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DS3cross_3.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(44, 0, -28));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSfootball.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(28, 0, -28));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSbasket.map(currentImage, &simpleUbo, 0);

		//SEA
		float sea_depth = -1;
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, sea_depth, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSsea.map(currentImage, &simpleUbo, 0);

		//GREYBASE
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-24, 0, 28)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f,1.0f,4.0f));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSgrey.map(currentImage, &simpleUbo, 0);

		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(8, 0, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 1.0f));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSgrey_1.map(currentImage, &simpleUbo, 0);

		//GREEN BASE
		simpleUbo.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(68, 0, -4)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 4.0f));
		simpleUbo.nMat = glm::inverse(glm::transpose(simpleUbo.mMat));
		simpleUbo.mvpMat = ViewPrj * simpleUbo.mMat;
		DSgreen.map(currentImage, &simpleUbo, 0);

		simpleMatParUbo.Power = 300.0;
		DSPlane.map(currentImage, &simpleMatParUbo, 2);
		DStile004.map(currentImage, &simpleMatParUbo, 2);
		DStile002.map(currentImage, &simpleMatParUbo, 2);
		DSport003.map(currentImage, &simpleMatParUbo, 2);
		DSport003_2.map(currentImage, &simpleMatParUbo, 2);
		DSport003_3.map(currentImage, &simpleMatParUbo, 2);
		DSrail004.map(currentImage, &simpleMatParUbo, 2);
		DSsea.map(currentImage, &simpleMatParUbo, 2);
		DSgrey.map(currentImage, &simpleMatParUbo, 2);
		DSgrey_1.map(currentImage, &simpleMatParUbo, 2);
		DScruise.map(currentImage, &simpleMatParUbo, 2);
		DSmerch.map(currentImage, &simpleMatParUbo, 2);
		DSship003.map(currentImage, &simpleMatParUbo, 2);
		DSship007.map(currentImage, &simpleMatParUbo, 2);
		DSship008.map(currentImage, &simpleMatParUbo, 2);
		DSship010.map(currentImage, &simpleMatParUbo, 2);
		DS3cross.map(currentImage, &simpleMatParUbo, 2);
		DS3cross_1.map(currentImage, &simpleMatParUbo, 2);
		DS3cross_2.map(currentImage, &simpleMatParUbo, 2);
		DS3cross_3.map(currentImage, &simpleMatParUbo, 2);
		DSport004.map(currentImage, &simpleMatParUbo, 2);
		DSport005.map(currentImage, &simpleMatParUbo, 2);
		DSport004_1.map(currentImage, &simpleMatParUbo, 2);
		DSport005_1.map(currentImage, &simpleMatParUbo, 2);
		DSport005_2.map(currentImage, &simpleMatParUbo, 2);
		DSport005_3.map(currentImage, &simpleMatParUbo, 2);
		DSpier1.map(currentImage, &simpleMatParUbo, 2);
		DSpier2.map(currentImage, &simpleMatParUbo, 2);
		DSmanufacturer004.map(currentImage, &simpleMatParUbo, 2);
		DSmanufacturer009.map(currentImage, &simpleMatParUbo, 2);
		DSsilos.map(currentImage, &simpleMatParUbo, 2);
		DSbeach005.map(currentImage, &simpleMatParUbo, 2);
		DSbeach005_1.map(currentImage, &simpleMatParUbo, 2);
		DSbeach005_2.map(currentImage, &simpleMatParUbo, 2);
		DSbeach005_3.map(currentImage, &simpleMatParUbo, 2);
		DScontainer.map(currentImage, &simpleMatParUbo, 2);
		DScrane.map(currentImage, &simpleMatParUbo, 2);
		DShangar.map(currentImage, &simpleMatParUbo, 2);
		DSbeach003.map(currentImage, &simpleMatParUbo, 2);
		DSbeach004.map(currentImage, &simpleMatParUbo, 2);
		DSbeach006.map(currentImage, &simpleMatParUbo, 2);
		DSbeach007.map(currentImage, &simpleMatParUbo, 2);
		DSparking.map(currentImage, &simpleMatParUbo, 2);
		DSroad009.map(currentImage, &simpleMatParUbo, 2);
		DSroad009_1.map(currentImage, &simpleMatParUbo, 2);
		DSroad009_2.map(currentImage, &simpleMatParUbo, 2);
		DSroad009_3.map(currentImage, &simpleMatParUbo, 2);
		DSroad009_4.map(currentImage, &simpleMatParUbo, 2);
		DStennis.map(currentImage, &simpleMatParUbo, 2);
		DSroad001.map(currentImage, &simpleMatParUbo, 2);
		DSroad001_1.map(currentImage, &simpleMatParUbo, 2);
		DSroad001_2.map(currentImage, &simpleMatParUbo, 2);
		DScurve.map(currentImage, &simpleMatParUbo, 2);
		DScurve_1.map(currentImage, &simpleMatParUbo, 2);
		DSsilos.map(currentImage, &simpleMatParUbo, 2);
		DSfootball.map(currentImage, &simpleMatParUbo, 2);
		DSbasket.map(currentImage, &simpleMatParUbo, 2);
		DShotel001.map(currentImage, &simpleMatParUbo, 2);
		DShotel003.map(currentImage, &simpleMatParUbo, 2);
		DSburger.map(currentImage, &simpleMatParUbo, 2);
		DSdrink.map(currentImage, &simpleMatParUbo, 2);
		DSstore005.map(currentImage, &simpleMatParUbo, 2);
		DSgreen.map(currentImage, &simpleMatParUbo, 2);
		
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
