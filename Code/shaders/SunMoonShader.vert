#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;

layout(set = 0, binding = 0) uniform SunMoonUniformBufferObject{
	mat4 mvpMat;
} smubo;

void main() {
	gl_Position = smubo.mvpMat * vec4(inPosition, 1.0);
	fragUV = inUV;
}