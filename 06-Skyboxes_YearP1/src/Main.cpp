#include<filesystem>
#include<iostream>
#include<vector>
//#include"Model.h"
#include <chrono>   // Provides high-precision clocks and time points
#include<SDL.h>
#include <GLFW/glfw3.h>
#include"audio.h"


#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<libs/tiny_gltf/tiny_gltf.h>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

//just needed in Windows
#if _WIN32
#include<windows.h>
#endif


#undef main

const unsigned int width = 1500;
const unsigned int height = 1500;

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif





using namespace ge::gl;

///////////////////////////////////////////////////////

class GModel {
public:
	~GModel() {
		meshes.clear();
		buffers.clear();
		textures.clear();
		roots.clear();
	}
	struct Node {
		glm::mat4        modelMatrix = glm::mat4(1.f);
		int32_t          mesh = -1;
		std::vector<Node>children;
	};
	struct Mesh {
		std::shared_ptr<ge::gl::VertexArray>vao;

		GLenum   indexType = GL_UNSIGNED_INT;
		uint32_t nofIndices = 0;
		uint64_t indexOffset = 0;
		bool     hasIndices = false;

		glm::vec4    diffuseColor = glm::vec4(1.f);
		int          diffuseTexture = -1;
	};

	GModel() {}
	void load(std::string const& fileName) {
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret;
		if (fileName.find(".glb") == fileName.length() - 4)
			ret = loader.LoadBinaryFromFile(&model, &err, &warn, fileName.c_str());

		if (fileName.find(".gltf") == fileName.length() - 5)
			ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName.c_str());

		if (!ret) {
			std::cerr << "model: " << fileName << "was not be loaded" << std::endl;
			return;
		}

		auto const& scene = model.scenes.at(0);
		for (auto const& node_id : scene.nodes) {
			auto const& root = model.nodes.at(node_id);
			roots.push_back(loadNode(root, model));
		}

		for (auto const& img : model.images) {
			GLenum internalFormat = GL_RGB;
			if (img.component == 3)internalFormat = GL_RGB;
			if (img.component == 4)internalFormat = GL_RGBA;
			auto tex = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D, internalFormat, 0, img.width, img.height);
			tex->setData2D(img.image.data());
			tex->generateMipmap();
			textures.push_back(tex);
		}

		for (auto const& b : model.buffers)
			buffers.push_back(std::make_shared<ge::gl::Buffer>(b.data));

		for (auto const& mesh : model.meshes) {
			for (auto const& primitive : mesh.primitives) {
				if (primitive.mode != TINYGLTF_MODE_TRIANGLES)continue;

				meshes.push_back({});
				auto& m_mesh = meshes.back();
				m_mesh.vao = std::make_shared<ge::gl::VertexArray>();

				if (primitive.material >= 0) {
					auto const& mat = model.materials.at(primitive.material);
					auto baseColorTextureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
					for (size_t i = 0; i < mat.pbrMetallicRoughness.baseColorFactor.size(); ++i)
						m_mesh.diffuseColor[(uint32_t)i] = (float)mat.pbrMetallicRoughness.baseColorFactor.at(i);
					if (baseColorTextureIndex < 0) {
						m_mesh.diffuseTexture = -1;
					}
					else {
						m_mesh.diffuseTexture = model.textures.at(mat.pbrMetallicRoughness.baseColorTexture.index).source;
					}
				}
				else
					m_mesh.diffuseTexture = -1;

				if (primitive.indices >= 0) {
					auto const& ia = model.accessors.at(primitive.indices);
					auto const& ibv = model.bufferViews.at(ia.bufferView);
					m_mesh.vao->addElementBuffer(buffers.at(ibv.buffer));
					m_mesh.indexOffset = ibv.byteOffset + ia.byteOffset;
					m_mesh.nofIndices = (uint32_t)ia.count;
					m_mesh.hasIndices = true;
					if (ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)m_mesh.indexType = GL_UNSIGNED_INT;
					if (ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)m_mesh.indexType = GL_UNSIGNED_SHORT;
					if (ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)m_mesh.indexType = GL_UNSIGNED_BYTE;
				}
				else {
					//std::cerr << "dont have indices" << std::endl;
				}

				for (auto const& attrib : primitive.attributes) {

					auto const& accessor = model.accessors.at(attrib.second);

					auto addAttrib = [&](uint32_t att) {
						auto const& bufferView = model.bufferViews.at(accessor.bufferView);
						auto bufId = bufferView.buffer;
						auto stride = bufferView.byteStride;
						auto offset = bufferView.byteOffset + accessor.byteOffset;
						auto size = bufferView.byteLength;
						auto bptr = model.buffers.at(bufId).data.data() + accessor.byteOffset;
						uint32_t nofC;

						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
							if (accessor.type == TINYGLTF_TYPE_SCALAR)nofC = 1;
							if (accessor.type == TINYGLTF_TYPE_VEC2)nofC = 2;
							if (accessor.type == TINYGLTF_TYPE_VEC3)nofC = 3;
							if (accessor.type == TINYGLTF_TYPE_VEC4)nofC = 4;
							if (stride == 0)stride = sizeof(float) * (uint32_t)nofC;
							m_mesh.vao->addAttrib(buffers.at(bufId), att, nofC, GL_FLOAT, stride, offset);
						}
					};
					if (std::string(attrib.first) == "POSITION")addAttrib(0);
					if (std::string(attrib.first) == "NORMAL")addAttrib(1);
					if (std::string(attrib.first) == "TEXCOORD_0")addAttrib(2);
				}
			}
		}
	}

	Node loadNode(tinygltf::Node const& root, tinygltf::Model const& model) {
		Node res;
		res.mesh = root.mesh;
		if (root.matrix.size() == 16) {
			for (int i = 0;i < 16;++i)
				res.modelMatrix[i / 4][i % 4] = (float)root.matrix[i];
		}
		else {
			if (root.translation.size() == 3) {
				auto* p = root.translation.data();
				res.modelMatrix = res.modelMatrix * glm::translate(glm::mat4(1.f), glm::vec3(p[0], p[1], p[2]));
			}
			if (root.rotation.size() == 4) {
				auto* p = root.rotation.data();
				glm::quat q;
				q[0] = (float)p[0];
				q[1] = (float)p[1];
				q[2] = (float)p[2];
				q[3] = (float)p[3];
				res.modelMatrix = res.modelMatrix * glm::toMat4(q);
			}
			if (root.scale.size() == 3) {
				auto* p = root.scale.data();
				res.modelMatrix = res.modelMatrix * glm::scale(glm::mat4(1.f), glm::vec3(p[0], p[1], p[2]));
			}
		}
		for (auto c : root.children)
			res.children.emplace_back(loadNode(model.nodes.at(c), model));
		return res;
	}

	void drawNode(Node const& node, ge::gl::Program* prg, glm::mat4 const& modelMatrix) {
		if (node.mesh >= 0) {
			auto const& mesh = meshes.at(node.mesh);


			if (mesh.diffuseTexture != -1) {
				prg->set1i("useTexture", 1);
				glBindTextureUnit(0, textures.at(mesh.diffuseTexture)->getId());
			}
			else {
				prg->set1i("useTexture", 0);
				prg->set4fv("diffuseColor", glm::value_ptr(mesh.diffuseColor));
			}

			prg->setMatrix4fv("model", glm::value_ptr(modelMatrix * node.modelMatrix));

			mesh.vao->bind();
			if (mesh.hasIndices)
				ge::gl::glDrawElements(GL_TRIANGLES, mesh.nofIndices, mesh.indexType, (GLvoid*)mesh.indexOffset);
			else
				ge::gl::glDrawArrays(GL_TRIANGLES, 0, mesh.nofIndices);

		}

		for (auto const& n : node.children) {
			drawNode(n, prg, modelMatrix * node.modelMatrix);
		}

	}
	void draw(glm::mat4 const& proj, glm::mat4 const& view, ge::gl::Program* prg) {
		prg->use();
		prg->setMatrix4fv("proj", glm::value_ptr(proj));
		prg->setMatrix4fv("view", glm::value_ptr(view));

		for (auto const& root : roots)
			drawNode(root, prg, glm::mat4(1.f));
	}

	std::vector<Mesh                            >meshes;
	std::vector<Node                            >roots;
	std::vector<std::shared_ptr<ge::gl::Texture>>textures;
	std::vector<std::shared_ptr<ge::gl::Buffer >>buffers;
};

std::string const phongLightingShader = R".(
vec3 phongLighting(
  vec3  position     ,
  vec3  normal       ,
  vec3  lightPosition,
  vec3  camera       ,
  vec3  lightColor   ,
  vec3  ambient      ,
  vec3  diffuseColor ,
  float shininess    ,
  float spec         ){

  vec3  L  = normalize(lightPosition-position);
  float Df = max(dot(normal,L),0);

  vec3  V  = normalize(camera-position);
  vec3  R  = -reflect(L,normal);
  float Sf = pow(max(dot(R,V),0),shininess);

  vec3 ambientLighting  = ambient*diffuseColor;
  vec3 diffuseLighting  = Df*diffuseColor*lightColor;
  vec3 specularLighting = Sf*vec3(1,1,1)*lightColor*spec;

  return ambientLighting + diffuseLighting + specularLighting;
}
).";

std::string const source = R".(

#ifdef VERTEX_SHADER
uniform mat4 view  = mat4(1.f);
uniform mat4 proj  = mat4(1.f);
uniform mat4 model = mat4(1.f);

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal  ;
layout(location = 2)in vec2 texCoord;

out vec2 vCoord;
out vec3 vNormal;
out vec3 vPosition;
out vec3 vCamPosition;
void main(){
  vCoord  = texCoord;
  vNormal = normal  ;
  vPosition = vec3(model*vec4(position,1.f));
  vCamPosition = vec3(inverse(view)*vec4(0,0,0,1));
  gl_Position = proj*view*model*vec4(position,1.f);
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 vPosition;
in vec2 vCoord;
in vec3 vNormal;
in vec3 vCamPosition;

uniform vec3  lightPosition = vec3(30,30,30)   ;
uniform vec3  lightColor    = vec3(1,1,1)      ;
uniform vec3  lightAmbient  = vec3(0.3,0.1,0.0);
uniform float shininess     = 60.f             ;


uniform sampler2D diffuseTexture;
uniform vec4      diffuseColor = vec4(1.f);
uniform int       useTexture   = 0;

layout(location=0)out vec4 fColor;
void main(){
  vec3 diffuseColor = vec3(0.3);
  fColor = vec4(vNormal,1);

  if(useTexture == 1)
    diffuseColor = texture(diffuseTexture,vCoord).rgb;
  else
    diffuseColor = diffuseColor.rgb;


  vec3 finalColor = phongLighting(
      vPosition          ,
      normalize(vNormal) ,
      lightPosition      ,
      vCamPosition       ,
      lightColor         ,
      lightAmbient       ,
      diffuseColor       ,
      shininess          ,
      0.f                );

    fColor = vec4(finalColor,1.f);

}
#endif
).";



///////////////////////////////////////////////////////









float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};




int main()
{

	//just needed in Windows
    #if _WIN32
    SetEnvironmentVariable("SDL_AUDIODRIVER","directsound");
    #endif

	//SDL_Init(SDL_INIT_EVERYTHING);
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create a GLFWwindow object of 1500 by 1500 pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	//gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 1500, y = 1500
	glViewport(0, 0, width, height);



	 /* Initialize only SDL Audio on default device */
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		return 1;
	}

	/* Init Simple-SDL2-Audio */
	initAudio();


	// Generates Shader objects
	ShaderC shaderProgram(
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/default.vert").c_str(),
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/default.frag").c_str());
	ShaderC skyboxShader (
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/skybox.vert").c_str(), 
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/skybox.frag").c_str());

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos   = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);



	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));



	std::string parentDir;
    parentDir = CMAKE_ROOT_DIR;
    //std::cerr << parentDir << std::endl;
	//std::string modelPath = "/Resources/Skyboxes/models/crow/scene.gltf";
	
	// Load in models
	//Model model((parentDir + modelPath).c_str());
	ge::gl::init();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	auto model = GModel();
	model.load(std::string(CMAKE_ROOT_DIR) + "/Resources/Skyboxes/models/house/firstFloor - Copy.gltf");
	auto prg = std::make_shared<ge::gl::Program>(
		std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + source),
		std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + phongLightingShader + source)
		);

	glm::vec3 lightPosition = glm::vec3(30.f, 30.f, 30.f);
	//disable VSync 
	//glfwSwapInterval(0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// All the faces of the cubemap 
	std::string facesCubemap[6] =
	{
		parentDir + "/Resources/Skyboxes/texture/right.jpg",
		parentDir + "/Resources/Skyboxes/texture/left.jpg",
		parentDir + "/Resources/Skyboxes/texture/top.jpg",
		parentDir + "/Resources/Skyboxes/texture/bottom.jpg",
		parentDir + "/Resources/Skyboxes/texture/front.jpg",
		parentDir + "/Resources/Skyboxes/texture/back.jpg"
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	ge::gl::glGenTextures  (1, &cubemapTexture);
	ge::gl::glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	//Set the filtering and wrapping method of the texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	int width, height, nrChannels;
	unsigned char* data;
	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		
		data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}

		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	// initialize lastTime variable
	auto lastTime = std::chrono::high_resolution_clock::now();
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//double last;
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
		lastTime = currentTime;

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.speed = 5.0f * deltaTime;
		if (camera.musicPlayer)
			playMusic(CMAKE_ROOT_DIR "/music/Always with me.wav", SDL_MIX_MAXVOLUME);
		if (camera.musicPause) pauseAudio();
		if (camera.unpause) unpauseAudio();
		//camera.sensitivity = 100.0f * deltaTime;
		//Handles camera inputs 
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);


		// Draw the normal model
		//model.Draw(shaderProgram, camera);
		


		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		//Delete the translation part of the transformation matrix
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		prg->set3fv("lightPosition", glm::value_ptr(lightPosition));
		model.draw(projection, view, &*prg);

		// Draws the cubemap as the last object 
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		ge::gl::glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		ge::gl::glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Check GLFW events
		glfwPollEvents();
	}



	shaderProgram.Delete();
	skyboxShader.Delete();
	//SDL_CloseAudio();
	//SDL_FreeWAV(wavBuffer);
	SDL_Quit();
	//glfwDestroyWindow(menuWindow);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
