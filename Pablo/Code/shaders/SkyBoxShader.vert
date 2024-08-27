#version 450

layout(location = 0) in vec3 inPosition; // Posizione del vertice

layout(location = 0) out vec3 TexCoords; // Coordinate per il campionamento della cubemap

layout(set = 0, binding = 0) uniform skyBoxUniformBufferObject {
    mat4 mvpMat;
} ubo;

void main()
{
    TexCoords = inPosition;
    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
}
