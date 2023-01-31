#include <iostream>
#include <SDL.h>

#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform.hpp>

#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>

#include <libs/tiny_gltf/tiny_gltf.h>

using namespace ge::gl;

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

class GModel{
  public:
    ~GModel(){
      meshes.clear();
      buffers.clear();
      textures.clear();
      roots.clear();
    }
    struct Node{
      glm::mat4        modelMatrix = glm::mat4(1.f);
      int32_t          mesh = -1;                   
      std::vector<Node>children;                    
    };
    struct Mesh{
      std::shared_ptr<ge::gl::VertexArray>vao;

      GLenum   indexType   = GL_UNSIGNED_INT;
      uint32_t nofIndices  =               0;
      uint64_t indexOffset =               0;
      bool     hasIndices  =           false;

      glm::vec4    diffuseColor = glm::vec4(1.f)  ;
      int          diffuseTexture = -1            ;
    };

    GModel(){}
    void load(std::string const&fileName){
      tinygltf::Model model;
      tinygltf::TinyGLTF loader;
      std::string err;
      std::string warn;
      bool ret;
      if(fileName.find(".glb")==fileName.length()-4)
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, fileName.c_str());
  
      if(fileName.find(".gltf")==fileName.length()-5)
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName.c_str());
  
      if(!ret){
        std::cerr << "model: " << fileName << "was not be loaded" << std::endl;
        return;
      }
  
      auto const&scene = model.scenes.at(0);
      for(auto const&node_id:scene.nodes){
        auto const&root = model.nodes.at(node_id);
        roots.push_back(loadNode(root,model));
      }

      for(auto const&img:model.images){
        GLenum internalFormat = GL_RGB;
        if(img.component == 3)internalFormat = GL_RGB ;
        if(img.component == 4)internalFormat = GL_RGBA;
        auto tex = std::make_shared<ge::gl::Texture>(GL_TEXTURE_2D,internalFormat,0,img.width,img.height);
        tex->setData2D(img.image.data());
        tex->generateMipmap();
        textures.push_back(tex);
      }

      for(auto const&b:model.buffers)
        buffers.push_back(std::make_shared<ge::gl::Buffer>(b.data));

      for(auto const&mesh:model.meshes){
        for(auto const&primitive:mesh.primitives){
          if(primitive.mode != TINYGLTF_MODE_TRIANGLES)continue;

          meshes.push_back({});
          auto&m_mesh = meshes.back();
          m_mesh.vao = std::make_shared<ge::gl::VertexArray>();

          if (primitive.material >= 0) {
              auto const& mat = model.materials.at(primitive.material);
              auto baseColorTextureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
              for (size_t i = 0; i < mat.pbrMetallicRoughness.baseColorFactor.size(); ++i)
                  m_mesh.diffuseColor[(uint32_t)i] = (float)mat.pbrMetallicRoughness.baseColorFactor.at(i);
            if(baseColorTextureIndex<0){
              m_mesh.diffuseTexture = -1;
            }else{
              m_mesh.diffuseTexture = model.textures.at(mat.pbrMetallicRoughness.baseColorTexture.index).source;
            }
          }else
            m_mesh.diffuseTexture = -1;

          if(primitive.indices >= 0){
              auto const&ia  = model.accessors.at(primitive.indices);
              auto const&ibv = model.bufferViews.at(ia.bufferView);
              m_mesh.vao->addElementBuffer(buffers.at(ibv.buffer));
              m_mesh.indexOffset = ibv.byteOffset + ia.byteOffset;
              m_mesh.nofIndices  = (uint32_t)ia.count;
              m_mesh.hasIndices  = true;
              if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT  )m_mesh.indexType = GL_UNSIGNED_INT  ;
              if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)m_mesh.indexType = GL_UNSIGNED_SHORT;
              if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE )m_mesh.indexType = GL_UNSIGNED_BYTE ;
          }else{
            //std::cerr << "dont have indices" << std::endl;
          }

          for(auto const&attrib:primitive.attributes){

            auto const&accessor = model.accessors.at(attrib.second);

            auto addAttrib = [&](uint32_t att){
              auto const&bufferView = model.bufferViews.at(accessor.bufferView);
              auto bufId  = bufferView.buffer;
              auto stride = bufferView.byteStride;
              auto offset = bufferView.byteOffset+accessor.byteOffset;
              auto size   = bufferView.byteLength;
              auto bptr   = model.buffers.at(bufId).data.data() + accessor.byteOffset;
              uint32_t nofC;

              if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT){
                if(accessor.type == TINYGLTF_TYPE_SCALAR)nofC = 1;
                if(accessor.type == TINYGLTF_TYPE_VEC2  )nofC = 2;
                if(accessor.type == TINYGLTF_TYPE_VEC3  )nofC = 3;
                if(accessor.type == TINYGLTF_TYPE_VEC4  )nofC = 4;
                if(stride == 0)stride = sizeof(float)*(uint32_t)nofC;
                m_mesh.vao->addAttrib(buffers.at(bufId),att,nofC,GL_FLOAT,stride,offset);
              }
            };
            if(std::string(attrib.first) == "POSITION"  )addAttrib(0);
            if(std::string(attrib.first) == "NORMAL"    )addAttrib(1);
            if(std::string(attrib.first) == "TEXCOORD_0")addAttrib(2);
          }
        }
      }
    }
      
    Node loadNode(tinygltf::Node const&root,tinygltf::Model const&model){
      Node res;
      res.mesh = root.mesh;
      if(root.matrix.size() == 16){
        for(int i=0;i<16;++i)
          res.modelMatrix[i/4][i%4] = (float)root.matrix[i];
      }else{
        if(root.translation.size() == 3){
          auto*p = root.translation.data();
          res.modelMatrix = res.modelMatrix*glm::translate(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
        }
        if(root.rotation.size() == 4){
          auto*p = root.rotation.data();
          glm::quat q;
          q[0]=(float)p[0];
          q[1]=(float)p[1];
          q[2]=(float)p[2];
          q[3]=(float)p[3];
          res.modelMatrix = res.modelMatrix*glm::toMat4(q);
        }
        if(root.scale.size() == 3){
          auto*p = root.scale.data();
          res.modelMatrix = res.modelMatrix*glm::scale(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
        }
      }
      for(auto c:root.children)
        res.children.emplace_back(loadNode(model.nodes.at(c),model));
      return res;
    }

    void drawNode(Node const&node,ge::gl::Program*prg,glm::mat4 const&modelMatrix){
      if(node.mesh>=0){
        auto const&mesh = meshes.at(node.mesh);


        if(mesh.diffuseTexture != -1){
          prg->set1i("useTexture",1);
          glBindTextureUnit(0,textures.at(mesh.diffuseTexture)->getId());
        }else{
          prg->set1i("useTexture",0);
          prg->set4fv("diffuseColor",glm::value_ptr(mesh.diffuseColor));
        }

        prg->setMatrix4fv("model",glm::value_ptr(modelMatrix*node.modelMatrix));

        mesh.vao->bind();
        if(mesh.hasIndices)
          glDrawElements(GL_TRIANGLES,mesh.nofIndices,mesh.indexType,(GLvoid*)mesh.indexOffset);
        else
          glDrawArrays(GL_TRIANGLES,0,mesh.nofIndices);

      }

      for(auto const&n:node.children){
        drawNode(n,prg,modelMatrix*node.modelMatrix);
      }

    }
    void draw(glm::mat4 const&proj,glm::mat4 const&view,ge::gl::Program*prg){
      prg->use();
      prg->setMatrix4fv("proj",glm::value_ptr(proj));
      prg->setMatrix4fv("view",glm::value_ptr(view));
    
      for(auto const&root:roots)
        drawNode(root,prg,glm::mat4(1.f));
    }
  
    std::vector<Mesh                            >meshes  ;
    std::vector<Node                            >roots   ;
    std::vector<std::shared_ptr<ge::gl::Texture>>textures;
    std::vector<std::shared_ptr<ge::gl::Buffer >>buffers ;
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
 //vNormal = normal  ;
  vNormal = vec3(inverse(transpose(model))*vec4(normal,0));
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


int main(int argc,char*argv[])
{
  SDL_Init(SDL_INIT_VIDEO);//init. video

  uint32_t width = 1024;
  uint32_t height = 768;

  auto window = SDL_CreateWindow("PGRe_examples",0,0,width,height,SDL_WINDOW_OPENGL);

  unsigned version = 450;//context version
  unsigned profile = SDL_GL_CONTEXT_PROFILE_CORE;//context profile
  unsigned flags    = SDL_GL_CONTEXT_DEBUG_FLAG;//context flags
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version/100    );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,(version%100)/10);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK ,profile         );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS        ,flags           );

  auto context = SDL_GL_CreateContext(window);
  //auto context = SDL_GL_CreateContext(window);

  ge::gl::init();

  auto model = GModel();
  model.load(std::string(CMAKE_ROOT_DIR)+"/resources/models/window.glb");

  auto prg = std::make_shared<ge::gl::Program>(
      std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER    ,"#version 460\n#define   VERTEX_SHADER\n"  +source),
      std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER  ,"#version 460\n#define FRAGMENT_SHADER\n"  +phongLightingShader+source)
      );

  glEnable(GL_DEPTH_TEST);

  float scale = 1.f;

  glm::mat4 view = glm::mat4(1.f);
  glm::mat4 proj = glm::mat4(1.f);

  glm::vec3 vCamPosition = glm::vec3(0.f);

  float angleY = 0.f;
  float angleX = 0.f;
 
  auto cameraRotationMatrix =
      glm::rotate(angleX, glm::vec3(1.f, 0.f, 0.f)) *
      glm::rotate(angleY, glm::vec3(0.f,1.f,0.f));


  float aspectRatio = (float)width / (float) height;

  float near = 0.1f;
  float far  = 1000.f;
  proj = glm::perspective(glm::half_pi<float>(),aspectRatio,near,far);

  glm::vec3 lightPosition = glm::vec3(30.f,30.f,30.f);



  float angle = 0.f;
  bool running = true;
  while(running){//Main Loop
    SDL_Event event;



    while(SDL_PollEvent(&event)){ // Event Loop
      if(event.type == SDL_QUIT)
        running = false;
      if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_a)scale -= .01f;
        if(event.key.keysym.sym == SDLK_d)scale += .01f;

        if(event.key.keysym.sym == SDLK_i)lightPosition.z += 2.f;
        if(event.key.keysym.sym == SDLK_k)lightPosition.z -= 2.f;
        if(event.key.keysym.sym == SDLK_j)lightPosition.x += 2.f;
        if(event.key.keysym.sym == SDLK_l)lightPosition.x -= 2.f;
        if(event.key.keysym.sym == SDLK_u)lightPosition.y += 2.f;
        if(event.key.keysym.sym == SDLK_o)lightPosition.y -= 2.f;

        if (event.key.keysym.sym == SDLK_f)vCamPosition += glm::vec3(glm::transpose(cameraRotationMatrix)[0]) * 0.1f;
        if (event.key.keysym.sym == SDLK_h)vCamPosition -= glm::vec3(glm::transpose(cameraRotationMatrix)[0]) * 0.1f;
        if (event.key.keysym.sym == SDLK_t)vCamPosition += glm::vec3(glm::transpose(cameraRotationMatrix)[2]) * 0.1f;
        if (event.key.keysym.sym == SDLK_g)vCamPosition -= glm::vec3(glm::transpose(cameraRotationMatrix)[2]) * 0.1f;
      }
      float sensitivity = 0.01f;
      if(event.type == SDL_MOUSEMOTION)
      {
        if(event.motion.state == SDL_BUTTON_RMASK)
        {
            angleX += sensitivity * event.motion.yrel;
            angleY += sensitivity * event.motion.xrel;
            cameraRotationMatrix =
              glm::rotate(angleX, glm::vec3(1.f, 0.f, 0.f)) * glm::rotate(angleY, glm::vec3(0.f, 1.f, 0.f));
        }
      }
    }

   
   view = cameraRotationMatrix* glm::translate(vCamPosition);

  

    glClearColor(0.3f,0.3f,0.3f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   
    prg->set3fv("lightPosition",glm::value_ptr(lightPosition));

    model.draw(proj,view,&*prg);
    angle += 0.01f;
    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);

  SDL_DestroyWindow(window);
  return 0;
}

