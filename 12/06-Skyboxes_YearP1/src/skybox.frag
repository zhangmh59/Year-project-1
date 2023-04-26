#version 330 core
out vec4 FragColor;

in vec3 texCoords;          // direction vector representing a 3D texture coordinate

uniform samplerCube skybox;

void main()
{
    
    FragColor = texture(skybox, normalize(vec3(texCoords.x,texCoords.y,texCoords.z)));//*0.0001
////+vec4(normalize(vec3(texCoords.x,texCoords.y,texCoords.z))*.5+.5,1);
    //FragColor = vec4(1,0,0,1);
}
