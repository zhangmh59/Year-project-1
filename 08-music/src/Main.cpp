//#include<filesystem>
#include <iostream>
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

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

//#include<json/json.hpp>
#include<libs/tiny_gltf/tiny_gltf.h>

//only needed in Windows
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
				glDrawElements(GL_TRIANGLES, mesh.nofIndices, mesh.indexType, (GLvoid*)mesh.indexOffset);
			else
				glDrawArrays(GL_TRIANGLES, 0, mesh.nofIndices);

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




// Number of samples per pixel for MSAA
unsigned int samples = 8;

// Controls the gamma function
float gamma = 2.2f;



float rectangleVertices[] =
{
	//  Coords   // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


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

	SDL_Init(SDL_INIT_EVERYTHING);
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
	gladLoadGL();
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
	Shader shaderProgram(
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/default.vert").c_str(),
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/default.frag").c_str());
	Shader skyboxShader (
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/skybox.vert").c_str(), 
      std::string(std::string(CMAKE_ROOT_DIR)+"/src/skybox.frag").c_str());
	Shader framebufferProgram(
		std::string(std::string(CMAKE_ROOT_DIR) + "/src/framebuffer.vert").c_str(),
		std::string(std::string(CMAKE_ROOT_DIR) + "/src/framebuffer.frag").c_str());
	Shader shadowMapProgram(
		std::string(std::string(CMAKE_ROOT_DIR) + "/src/shadowMap.vert").c_str(),
		std::string(std::string(CMAKE_ROOT_DIR) + "/src/shadowMap.frag").c_str());
	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos   = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.ID, "screenTexture"), 0);
	glUniform1f(glGetUniformLocation(framebufferProgram.ID, "gamma"), gamma);


	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);
	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(10.0f, 10.0f, 50.0f));


    std::string parentDir;
    parentDir = CMAKE_ROOT_DIR;
    //std::cerr << parentDir << std::endl;
	//std::string modelPath = "/Resources/Skyboxes/models/nyra/scene.gltf";
	//auto tiny_model = openModel((parentDir + modelPath).c_str());


	/////////////////////////////////////////////////////1234
	auto model = GModel();
	model.load(std::string(CMAKE_ROOT_DIR) + "/Resources/Skyboxes/models/nyra/scene.gltf");


	
	
	auto prg = std::make_shared<ge::gl::Program>(
		std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + source),
		std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + phongLightingShader + source)
		);


	/////////////////////////////////////////////////////
	// 
	// 
	// Load in models
	//Model model((parentDir + modelPath).c_str());

	// Prepare framebuffer rectangle VBO and VAO
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	// Create Frame Buffer Object
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create Framebuffer Texture
	unsigned int framebufferTexture;
	ge::gl::glGenTextures(1, &framebufferTexture);
	ge::gl::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

	// Create Render Buffer Object
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


	// Error checking framebuffer
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	// Create Frame Buffer Object
	unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

	// Create Framebuffer Texture
	unsigned int postProcessingTexture;
	ge::gl::glGenTextures(1, &postProcessingTexture);
	ge::gl::glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	// Error checking framebuffer
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;


	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
	unsigned int shadowMap;
	ge::gl::glGenTextures(1, &shadowMap);
	ge::gl::glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));





	//disable VSync 
	//glfwSwapInterval(0);


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


	/////////////////////////////////////////////////////1234

	float scale = 1.f;

	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 proj = glm::mat4(1.f);

	float angleY = 0.f;
	float angleX = 0.f;
	float distance = 2;

	float aspectRatio = (float)width / (float)height;

	float near = 0.1f;
	float far = 1000.f;
	proj = glm::perspective(glm::half_pi<float>(), aspectRatio, near, far);

	glm::vec3 lightPosition = glm::vec3(30.f, 30.f, 30.f);

	glm::vec2 panning = glm::vec2(0.f);

	/////////////////////////////////////////////////////


	// initialize lastTime variable
	auto lastTime = std::chrono::high_resolution_clock::now();
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//double last;
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
		lastTime = currentTime;


		/////////////////////////////////////////////////////1234
		auto T = glm::translate(glm::mat4(1.f), glm::vec3(panning, -distance));
		auto Ry = glm::rotate(glm::mat4(1.f), glm::radians(angleY), glm::vec3(0.f, 1.f, 0.f));
		auto Rx = glm::rotate(glm::mat4(1.f), glm::radians(angleX), glm::vec3(1.f, 0.f, 0.f));
		view = T * Rx * Ry;
		prg->set3fv("lightPosition", glm::value_ptr(lightPosition));
		model.draw(proj, view, &*prg);
		/////////////////////////////////////////////////////



		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		// Preparations for the Shadow Map
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		//model.Draw(shadowMapProgram, camera);


		// Switch back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Switch back to the default viewport
		glViewport(0, 0, width, height);
		// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// Specify the color of the background
		glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable depth testing since it's disabled when drawing the framebuffer rectangle
		glEnable(GL_DEPTH_TEST);

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

		// Send the light matrix to the shader
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		// Bind the Shadow Map
		glActiveTexture(GL_TEXTURE0 + 2);
		ge::gl::glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 2);

		// Draw the normal model
		//Model myModel;
		//myModel.load(tiny_model);
		//myModel.draw();
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

		// Draws the cubemap as the last object 
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		ge::gl::glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		ge::gl::glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		// Make it so the multisampling FBO is read while the post-processing FBO is drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
		// Conclude the multisampling and copy it to the post-processing FBO
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		framebufferProgram.Activate();
		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		ge::gl::glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
		ge::gl::glDrawArrays(GL_TRIANGLES, 0, 6);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Check GLFW events
		glfwPollEvents();
	}


	endAudio();
	shaderProgram.Delete();
	skyboxShader.Delete();
	glDeleteFramebuffers(1, &FBO);
	glDeleteFramebuffers(1, &postProcessingFBO);
	//SDL_CloseAudio();
	//SDL_FreeWAV(wavBuffer);
	SDL_Quit();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
