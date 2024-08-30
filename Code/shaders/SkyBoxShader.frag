#version 450

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D skyTexture;

const vec4 topColor = vec4(0.776, 0.909, 0.980, 1.0); 
const vec4 bottomColor = vec4(0.541, 0.776, 0.941, 1.0);

void main()
{
    vec2 uv;
    vec4 texColor;

    if (abs(TexCoords.y) > abs(TexCoords.x) && abs(TexCoords.y) > abs(TexCoords.z)) {
        if (TexCoords.y > 0.0) {
            texColor = bottomColor; 
        } else {
            texColor = topColor;
        }
    } else if (abs(TexCoords.x) > abs(TexCoords.z)) {
        uv = TexCoords.zy / abs(TexCoords.x);
        uv.x = (TexCoords.x > 0.0) ? uv.x : -uv.x;
        uv = uv * 0.5 + 0.5;
        uv = 1.0 - uv;
        texColor = texture(skyTexture, uv);
    } else {
        uv = TexCoords.xy / abs(TexCoords.z);
        uv.x = (TexCoords.z > 0.0) ? -uv.x : uv.x;
        uv = uv * 0.5 + 0.5;
        uv = 1.0 - uv;
        texColor = texture(skyTexture, uv);
    }

    FragColor = vec4(texColor.rgb, 1.0);
}