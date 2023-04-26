#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;


uniform mat4 mvp =mat4(1);

void main()
{
    vec4 pos = mvp * vec4(aPos, 0.0f);//如果代码出错了，就让aPos * 1000

    // Having z equal w will always result in a depth of 1.0f
    gl_Position = pos.xyww;

    //flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3(aPos.x, aPos.y, aPos.z);
  
}    
