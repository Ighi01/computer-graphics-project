#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 TexCoords;

layout(set = 0, binding = 0) uniform skyBoxMatUniformBufferObject {
    mat4 mvpMat;
    float lightIntensity;
    float rotationAngle;
} ubo;


void main()
{
    TexCoords = inPosition;
    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);

}