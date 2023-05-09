#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<libs/tiny_gltf/tiny_gltf.h>

using namespace ge::gl;

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