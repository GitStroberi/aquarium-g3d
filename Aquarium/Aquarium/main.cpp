#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <GL/glew.h>


#define GLM_FORCE_CTOR_INIT 
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

#define TINYOBJLOADER_IMPLEMENTATION
#include <algorithm>

#include "tiny_obj_loader.h"


#include "Camera.h"
#include "Shader.h"
#include "Plane.h"
#include "Cube.h"

//settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 900;
std::string strExePath;

Mesh* CreateObj(const std::string& strObjPath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, strObjPath.c_str());

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	std::vector<Vertex> vertices;

	tinyobj::shape_t shape = shapes[0];
	//Loop over faces(polygon)
	size_t index_offset = 0;
	for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
		size_t fv = size_t(shape.mesh.num_face_vertices[f]);

		//Loop over vertices in the face.
		for (size_t v = 0; v < fv; v++) {
			//access to vertex
			tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

			tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
			tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
			tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

			tinyobj::real_t nx = 0;
			tinyobj::real_t ny = 0;
			tinyobj::real_t nz = 0;

			tinyobj::real_t tx = 0;
			tinyobj::real_t ty = 0;

			//Check if `normal_index` is zero or positive. negative = no normal data
			if (idx.normal_index >= 0) {
				nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
				ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
				nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
			}

			//Check if `texcoord_index` is zero or positive. negative = no texcoord data
			if (idx.texcoord_index >= 0) {
				tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
				ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
			}

			vertices.emplace_back(vx, vy, vz, nx, ny, nz, tx, ty);
		}
		index_offset += fv;
	}


	Mesh* obj = new Mesh(vertices);
	return obj;
}
unsigned int CreateTexture(const std::string& strTexturePath)
{
	unsigned int textureId = -1;

	//load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture: " << strTexturePath << std::endl;
	}
	stbi_image_free(data);

	return textureId;
}
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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

bool IsPointInsideRegion(glm::vec3 reg_low, glm::vec3 reg_high, glm::vec3 point)
{
	if (reg_low.x >= point.x || point.x >= reg_high.x) return false;
	if (reg_low.y >= point.y || point.y >= reg_high.y) return false;
	if (reg_low.z >= point.z || point.z >= reg_high.z) return false;
	return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void createObjects();

void renderSky(const Shader& shader);
void renderScene(const Shader& shader);
void renderTranslucid(const Shader& shader);
void renderInsideWaterOverlay(const Shader& shader);
void subrenderObjects(const Shader& shader);


void updateFishPosition();

//timing
double deltaTime = 0.0f;	//time between current frame and last frame
double lastFrame = 0.0f;
float currentFrame = 0.0f;

Camera* pCamera = nullptr;

Plane* sandFloor;
Plane* front, * back, * right, * left;
Mesh* fish1, * fish2, * fish3, * fish4;
Mesh* coral1, * coral2, * coral3, * coral4;
Plane* overlay;
Cube* bottomAquarium;
Cube* topAquarium;

int main(int argc, char** argv)
{
	std::string strFullExeFileName = argv[0];
	const size_t last_slash_idx = strFullExeFileName.rfind('\\');
	if (std::string::npos != last_slash_idx) {
		strExePath = strFullExeFileName.substr(0, last_slash_idx);
	}

	//glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fish Tank OpenGl", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	//Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 2.0, 5.0));

	//Depth Testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Build and compile shaders
	Shader shadowMappingShader("ShadowMapping.vs", "ShadowMapping.fs");
	Shader shaderWater("Water.vs", "Water.fs");

	//configure depth map FBO
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowMappingShader.Use();
	shadowMappingShader.SetInt("diffuseTexture", 0);
	shadowMappingShader.SetInt("shadowMap", 1);

	shaderWater.Use();
	shaderWater.SetInt("diffuseTexture", 0);

	//Create objects
	createObjects();

	glm::vec3 lightPos(0.0f, 3.0f, -0.5f);

	//render loop
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window);

		//Clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render depth of scene to texture (from light's perspective)
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		renderScene(shadowMappingShader);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render scene as normal using the generated depth/shadow map
		shadowMappingShader.Use();
		glm::mat4 projection = pCamera->GetProjectionMatrix();
		glm::mat4 view = pCamera->GetViewMatrix();
		shadowMappingShader.SetMat4("projection", projection);
		shadowMappingShader.SetMat4("view", view);
		//Set light uniforms
		shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());
		shadowMappingShader.SetVec3("lightPos", lightPos);
		shadowMappingShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(shadowMappingShader);

		//Render water and translucide object
		shaderWater.Use();
		shaderWater.SetMat4("projection", projection);
		shaderWater.SetMat4("view", view);
		//Set Dynamic Water uniform
		shaderWater.SetFloat("evolution", std::sinf(currentFrame));
		//Set View Position uniform
		shaderWater.SetVec3("viewPos", pCamera->GetPosition());
		renderTranslucid(shaderWater);

		//Update fish position
		updateFishPosition();

		//Water Overlay
		if (IsPointInsideRegion({ -5,0,-10 }, { 5,4,-4 }, pCamera->GetPosition()))
		{
			shaderWater.SetMat4("projection", glm::mat4());
			shaderWater.SetMat4("view", glm::mat4());
			renderInsideWaterOverlay(shaderWater);
		}

		//glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//optional: de-allocate all resources once they've outlived their purpose:
	delete pCamera;

	glfwTerminate();
	return 0;
}

void updateFishPosition()
{
	{
		static glm::vec3 fishStartPosition = { 0.0f, 1.5, -7 };
		static float fishRadius = 1.2;
		fish1->setPosition(glm::vec3{ std::sinf(currentFrame) * fishRadius * 2.5, std::sinf(currentFrame) * std::cosf(currentFrame) * fishRadius * 1.5, std::cosf(currentFrame) * fishRadius } + fishStartPosition);
	}
	{
		static glm::vec3 fishStartPosition = { 0.0f, 2, -7 };
		static float fishRadius = 0.7;
		fish2->setPosition(glm::vec3{ std::cosf(currentFrame) * fishRadius * 5, std::sinf(currentFrame) * std::sinf(currentFrame) * std::sinf(currentFrame) * fishRadius, std::cosf(currentFrame) * fishRadius } + fishStartPosition);
	}
	{
		static glm::vec3 fishStartPosition = { -3.0f, 1, -8 };
		static float fishRadius = 0.6;
		fish3->setPosition(glm::vec3{ std::sinf(currentFrame) * fishRadius * -1.5, std::sinf(currentFrame) * fishRadius, std::cosf(currentFrame) - fishRadius } + fishStartPosition);
	}
	{
		static glm::vec3 fishStartPosition = { 3.5f, 2.5, -6 };
		static float fishRadius = 0.4;
		fish4->setPosition(glm::vec3{ std::sinf(currentFrame) * fishRadius * -1.5, std::sinf(currentFrame) * fishRadius, std::cosf(currentFrame) - fishRadius } + fishStartPosition);
	}
}

void subrenderObjects(const Shader& shader)
{
	glEnable(GL_CULL_FACE);
	glm::mat4 model;

	//Table
	bottomAquarium->renderBasic(shader);

	//fish
	model = glm::mat4();
	model = glm::translate(model, fish1->getPosition());
	model = glm::rotate(model, currentFrame, { 0,1,0 });
	fish1->renderCustomModel(shader, model);

	//Fish2
	model = glm::mat4();
	model = glm::translate(model, fish2->getPosition());
	model = glm::rotate(model, -currentFrame, { 0,1,0 });
	fish2->renderCustomModel(shader, model);

	//Fish3
	model = glm::mat4();
	model = glm::translate(model, fish3->getPosition());
	model = glm::rotate(model, -currentFrame, { 0,1,0 });
	fish3->renderCustomModel(shader, model);

	//Fish4
	model = glm::mat4();
	model = glm::translate(model, fish4->getPosition());
	model = glm::rotate(model, -currentFrame, { 0,1,0 });
	fish4->renderCustomModel(shader, model);

	//Coral
	model = glm::mat4();
	model = glm::translate(model, coral1->getPosition() + glm::vec3{ 3.3,-0.55,-5.5 });
	model = glm::scale(model, { 5, 5, 5 });
	coral1->renderCustomModel(shader, model);

	model = glm::mat4();
	model = glm::translate(model, coral2->getPosition() + glm::vec3{ 2.3,-0.63,-2 });
	model = glm::scale(model, { 1.3, 0.9, 1.1 });
	coral2->renderCustomModel(shader, model);

	model = glm::mat4();
	model = glm::translate(model, coral3->getPosition() + glm::vec3{ -3,-0.63,-5.6 });
	model = glm::scale(model, { 1, 1.9, 0.9 });
	coral3->renderCustomModel(shader, model);

	model = glm::mat4();
	model = glm::translate(model, coral3->getPosition() + glm::vec3{ -3,0.36,-1.6 });
	model = glm::scale(model, { 3, 3, 3 });
	coral4->renderCustomModel(shader, model);

	//Top Aquarium
	topAquarium->renderBasic(shader);
}

//renders the 3D scene
//--------------------
void renderScene(const Shader& shader)
{
	shader.Use();
	glDisable(GL_CULL_FACE);
	sandFloor->renderBasic(shader);
	subrenderObjects(shader);
}

void renderTranslucid(const Shader& shader)
{
	glDisable(GL_CULL_FACE);
	shader.Use();
	glm::mat4 model;

	//sorting the aquarium walls by distance to camera to render them in the right order
	std::vector<IRenderable*> sorted;

	sorted.push_back(front);
	sorted.push_back(back);
	sorted.push_back(right);
	sorted.push_back(left);

	IRendComparator mc;
	mc.camPos = pCamera->GetPosition();
	std::sort(sorted.begin(), sorted.end(), mc);

	for (const auto& item : sorted)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, item->getPosition());

		shader.SetMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, item->getDiffuseTextureId());
		item->renderMesh();
	}
}

//Render Inside of water
void renderInsideWaterOverlay(const Shader& shader)
{
	glEnable(GL_CULL_FACE);
	shader.Use();

	overlay->renderBasic(shader);
}

void createObjects()
{

	unsigned int insideWaterTexture = CreateTexture(strExePath + "\\..\\Models\\underwater.png");
	unsigned int floorTexture = CreateTexture(strExePath + "\\..\\Models\\cover.jpg");
	unsigned int waterTexture = CreateTexture(strExePath + "\\..\\Models\\water.png");
	unsigned int sandTexture = CreateTexture(strExePath + "\\..\\Models\\sand.png");

	//Aquarium top lid and bottom stand
	bottomAquarium = new Cube(10.0f, 1.0f, 6.0f, { 0.0,-0.5,-7.0 }, TEXSCALE::TS_SCALE);
	bottomAquarium->setDiffuseTextureId(floorTexture);

	topAquarium = new Cube(10.0f, 0.2f, 6.0f, { 0.0, 4.1, -7.0 }, TEXSCALE::TS_SCALE);
	topAquarium->setDiffuseTextureId(floorTexture);

	//Aquarium walls
	front = new Plane(10.0f, 4.0f, { 0.0f, 2, -4 }, TS_NO_SCALE, OR_XY);
	front->setDiffuseTextureId(waterTexture);

	back = new Plane(10.0f, 4.0f, { 0.0f, 2, -10 }, TS_NO_SCALE, OR_XY);
	back->setDiffuseTextureId(waterTexture);

	right = new Plane(4.0f, 6.0f, { 5.0f, 2, -7 }, TS_NO_SCALE, OR_ZY);
	right->setDiffuseTextureId(waterTexture);

	left = new Plane(4.0f, 6.0f, { -5.0f, 2, -7 }, TS_NO_SCALE, OR_ZY);
	left->setDiffuseTextureId(waterTexture);

	sandFloor = new Plane(10, 6, { 0, 0, -7 }, TS_NO_SCALE, OR_XZ);
	sandFloor->setDiffuseTextureId(sandTexture);

	//Aquarium fishes
	fish1 = CreateObj(strExePath + "\\..\\Models\\Blue_Tang_v1_L3\\13006_Blue_Tang_v1_l3.obj");
	unsigned fishTexture1 = CreateTexture(strExePath + "\\..\\Models\\Blue_Tang_v1_L3\\13006_Blue_Tang_v1_diff.jpg");
	fish1->setDiffuseTextureId(fishTexture1);
	{
		glm::mat4 fishModelModify = glm::rotate(glm::mat4(), -3.14f * 0.5f, glm::vec3(1, 0, 0));
		fishModelModify = glm::rotate(fishModelModify, 3.14f * 0.5f, glm::vec3(0, 0, 1));
		fishModelModify = glm::scale(fishModelModify, { 0.06, 0.06, 0.06 });
		fish1->applyModelTransform(fishModelModify);
	}

	fish2 = CreateObj(strExePath + "\\..\\Models\\Blue-Green_Reef_Chromis_v2_L3\\13007_Blue-Green_Reef_Chromis_v2_l3.obj");
	unsigned fishTexture2 = CreateTexture(strExePath + "\\..\\Models\\Blue-Green_Reef_Chromis_v2_L3\\13004_Bicolor_Blenny_v1_diff.jpg");
	fish2->setDiffuseTextureId(fishTexture2);
	{
		glm::mat4 fishModelModify = glm::mat4();
		fishModelModify = glm::scale(glm::mat4(), { 0.1, 0.1, 0.1 });
		fishModelModify = glm::rotate(fishModelModify, -3.14f * 0.5f, glm::vec3(1, 0, 0));
		fish2->applyModelTransform(fishModelModify);
	}

	fish3 = CreateObj(strExePath + "\\..\\Models\\Boesemani_Rainbow_v1_L2\\12999_Boesemani_Rainbow_v1_l2.obj");
	unsigned fishTexture3 = CreateTexture(strExePath + "\\..\\Models\\Boesemani_Rainbow_v1_L2\\12999_Boesemani_Rainbow_diff.jpg");
	{
		glm::mat4 fishModelModify = glm::rotate(glm::mat4(), -3.14f * 0.5f, glm::vec3(1, 0, 0));
		fishModelModify = glm::scale(fishModelModify, { 0.1, 0.1, 0.1 });
		fish3->applyModelTransform(fishModelModify);
	}
	fish3->setDiffuseTextureId(fishTexture3);

	fish4 = CreateObj(strExePath + "\\..\\Models\\Yellow_Tang_v1_L3\\13015_Yellow_Tang_v1_L3.obj");
	unsigned fishTexture4 = CreateTexture(strExePath + "\\..\\Models\\Yellow_Tang_v1_L3\\yellow-tang_diff.jpg");
	{
		glm::mat4 fishModelModify = glm::rotate(glm::mat4(), -3.14f * 0.5f, glm::vec3(1, 0, 0));
		fishModelModify = glm::rotate(fishModelModify, 3.14f, glm::vec3(0, 0, 1));
		fishModelModify = glm::scale(fishModelModify, { 0.02, 0.02, 0.02 });
		fish4->applyModelTransform(fishModelModify);
	}
	fish4->setDiffuseTextureId(fishTexture4);

	//Aquarium static life
	coral1 = CreateObj(strExePath + "\\..\\Models\\Brain_Coral_v1_L1\\20941_Brain_Coral_v1_NEW1.obj");
	unsigned coralTexture1 = CreateTexture(strExePath + "\\..\\Models\\Brain_Coral_v1_L1\\20941_Brain_Coral_Diff_v1.jpg");
	coral1->setDiffuseTextureId(coralTexture1);
	{
		glm::mat4 coralModelModify = glm::mat4();
		coralModelModify = glm::rotate(coralModelModify, glm::radians(-90.0f), { 1,0,0 });
		coralModelModify = glm::scale(coralModelModify, { 0.02, 0.02, 0.02 });
		coral1->applyModelTransform(coralModelModify);

		coral1->setPosition({ 0,0.65,-3 });
	}
	coral1->setDiffuseTextureId(coralTexture1);

	coral2 = CreateObj(strExePath + "\\..\\Models\\coral1\\coral1.obj");
	unsigned coralTexture2 = CreateTexture(strExePath + "\\..\\Models\\coral1\\coral1.jpg");
	coral2->setDiffuseTextureId(coralTexture2);
	{
		glm::mat4 coralModelModify = glm::mat4();
		coralModelModify = glm::rotate(coralModelModify, glm::radians(-90.0f), { 1,0,0 });
		coralModelModify = glm::scale(coralModelModify, { 0.02, 0.02, 0.02 });
		coral2->applyModelTransform(coralModelModify);

		coral2->setPosition({ 0,0.65,-3 });
	}
	coral2->setDiffuseTextureId(coralTexture2);

	coral3 = CreateObj(strExePath + "\\..\\Models\\coral1\\coral1.obj");
	unsigned coralTexture3 = CreateTexture(strExePath + "\\..\\Models\\coral1\\coral1.jpg");
	coral3->setDiffuseTextureId(coralTexture3);
	{
		glm::mat4 coralModelModify = glm::mat4();
		coralModelModify = glm::rotate(coralModelModify, glm::radians(-90.0f), { 1,0,0 });
		coralModelModify = glm::scale(coralModelModify, { 0.02, 0.02, 0.02 });
		coral3->applyModelTransform(coralModelModify);

		coral3->setPosition({ 0,0.65,-3 });
	}
	coral3->setDiffuseTextureId(coralTexture3);

	coral4 = CreateObj(strExePath + "\\..\\Models\\lov_obj\\lov_obj.obj");
	unsigned coralTexture4 = CreateTexture(strExePath + "\\..\\Models\\lov_obj\\lov_obj.jpg");
	coral4->setDiffuseTextureId(coralTexture4);
	{
		glm::mat4 coralModelModify = glm::mat4();
		coralModelModify = glm::rotate(coralModelModify, glm::radians(90.0f), { 1,0,0 });
		coralModelModify = glm::rotate(coralModelModify, glm::radians(5.0f), { 0,0,1 });
		coralModelModify = glm::rotate(coralModelModify, glm::radians(-20.0f), { 0,1,0 });
		coralModelModify = glm::scale(coralModelModify, { 0.02, 0.02, 0.02 });
		coral4->applyModelTransform(coralModelModify);

		coral4->setPosition({ 0,0.65,-3 });
	}
	coral4->setDiffuseTextureId(coralTexture4);

	overlay = new Plane(25, 25, { 0,0,0 }, TS_NO_SCALE, OR_XY);
	overlay->setDiffuseTextureId(insideWaterTexture);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//make sure the viewport matches the new window dimensions; note that width and 
	//height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}