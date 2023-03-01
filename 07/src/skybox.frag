#version 330 core
out vec4 FragColor;

in vec3 texCoords;          // direction vector representing a 3D texture coordinate

uniform samplerCube skybox;

void main()
{
    
    FragColor = texture(skybox, vec3(texCoords.x,texCoords.y,texCoords.z));
}
