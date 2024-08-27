#version 450

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D skyboxTexture;

void main()
{
    // Calcoliamo le coordinate UV da TexCoords
    vec2 uv = vec2(
        atan(TexCoords.z, TexCoords.x) / (2.0 * 3.14159265359) + 0.5,
        asin(TexCoords.y) / 3.14159265359 + 0.5
    );

    FragColor = texture(skyboxTexture, uv);
}
