#version 410 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 MVP;

void main()
{
    TexCoord = vec3(aPos.xy ,-aPos.z);
    gl_Position = MVP * vec4(aPos, 1.0);
}