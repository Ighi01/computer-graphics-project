#version 450

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform skyBoxMatUniformBufferObject {
    mat4 mvpMat;
    float lightIntensity;
    float rotationDelta;
} ubo;

layout(binding = 1) uniform sampler2D skyTexture;

const vec4 topColor = vec4(0.776, 0.909, 0.980, 1.0); 
const vec4 bottomColor = vec4(0.541, 0.776, 0.941, 1.0);

const float minLight = 0.0;
const float brightOffset = 0.15;

void main()
{
    vec2 uv;
    vec4 texColor;
    
    float offset = mod(ubo.rotationDelta, 1.0);
    
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
        uv.x = mod(uv.x + offset, 1.0);
        texColor = texture(skyTexture, uv);
    } else {
        uv = TexCoords.xy / abs(TexCoords.z);
        uv.x = (TexCoords.z > 0.0) ? -uv.x : uv.x;
        uv = uv * 0.5 + 0.5;
        uv = 1.0 - uv;
        uv.x = mod(uv.x + offset, 1.0);
        texColor = texture(skyTexture, uv);
    }
    float lightIntensity = max(minLight, ubo.lightIntensity) + brightOffset;
    FragColor = vec4(texColor.rgb * lightIntensity, 1.0);
}