#version 450
#extension GL_ARB_separate_shader_objects : enable
#define NUMLIGHTS 63
#define MAXLIGHTS 20
#define VIEW_MARGIN 2

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightDir[NUMLIGHTS];
    vec3 lightPos[NUMLIGHTS];
    vec4 lightColor[NUMLIGHTS];
    float cosIn;
    float cosOut;
    vec3 eyePos;
    vec4 eyeDir;
    vec4 lightOn;
    mat4 mvp;
} gubo;

layout(set = 1, binding = 2) uniform SimpleMatParUniformBufferObject {
    float Pow;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D texDiff;

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
    float diff = max(dot(Norm, LD), 0.0);
    vec3 Diffuse = Albedo * diff;
    vec3 halfVector = normalize(EyeDir + LD);  
    float spec = pow(max(dot(Norm, halfVector), 0.0), mubo.Pow); 
    vec3 Specular = vec3(spec);
    return Diffuse + Specular;
}

vec3 BRDFSimple(vec3 Albedo, vec3 Norm, vec3 LD) {
    vec3 Diffuse = Albedo * max(dot(Norm, LD), 0.0);
    return Diffuse;
}

bool is_light_in_view(vec3 lightPos) {
    vec4 clipSpacePos = gubo.mvp * vec4(lightPos, 1.0);
    return abs(clipSpacePos.x) <= clipSpacePos.w * VIEW_MARGIN &&
           abs(clipSpacePos.y) <= clipSpacePos.w * VIEW_MARGIN &&
           clipSpacePos.z >= 0.0 && clipSpacePos.z <= clipSpacePos.w * VIEW_MARGIN;
}

void main() {
    vec3 norm = normalize(fragNorm);
    vec3 eyeDir = normalize(gubo.eyePos - fragPos);
    vec3 albedo = texture(texDiff, fragUV).rgb;
    vec3 rendEqSol = vec3(0);
    
    // DAY NIGHT CYCLE

	rendEqSol += BRDF(albedo, norm, eyeDir, gubo.lightDir[0]) * gubo.lightColor[0].rgb * gubo.lightOn.y;

	// PLANE LIGHT

	vec3 LD = normalize(gubo.lightPos[1] - fragPos);
    vec3 dir = normalize(gubo.lightPos[1] - fragPos);
	float power = pow(gubo.lightColor[1].a/length(gubo.lightPos[1] - fragPos), 2.0);
	vec3 point_light = gubo.lightColor[1].rgb * power;
	float to_clamp = (dot(dir,gubo.lightDir[1])-gubo.cosOut)/(gubo.cosIn-gubo.cosOut);
	vec3 LC = point_light * clamp(to_clamp, 0, 1);
	rendEqSol += BRDFSimple(albedo, norm, LD) * LC * gubo.lightOn.z;
    
    // POINT LIGHTS

    int numLights = 0;

    for (int i = 2; i < (NUMLIGHTS - 2); i++) {            
        if (gubo.lightOn.x > 0.0f && numLights < MAXLIGHTS && is_light_in_view(gubo.lightPos[i])) {
            vec3 dir = normalize(gubo.lightPos[i] - fragPos);
            float power = pow(gubo.lightColor[i].a/length(gubo.lightPos[i] - fragPos), 2.0);
	        vec3 light = gubo.lightColor[i].rgb * power;
            rendEqSol += BRDFSimple(albedo, norm, dir) * light * gubo.lightOn.x;
            numLights ++;
        }
    }

    const vec3 cxp = vec3(1.0, 0.5, 0.5) * 0.1;
    const vec3 cxn = vec3(0.9, 0.6, 0.4) * 0.1;
    const vec3 cyp = vec3(0.3, 1.0, 1.0) * 0.1;
    const vec3 cyn = vec3(0.5, 0.5, 0.5) * 0.1;
    const vec3 czp = vec3(0.8, 0.2, 0.4) * 0.1;
    const vec3 czn = vec3(0.3, 0.6, 0.7) * 0.1;

    vec3 ambient = ((norm.x > 0 ? cxp : cxn) * (norm.x * norm.x) +
                    (norm.y > 0 ? cyp : cyn) * (norm.y * norm.y) +
                    (norm.z > 0 ? czp : czn) * (norm.z * norm.z)) * albedo;
    rendEqSol += ambient * gubo.lightOn.w;

    outColor = vec4(rendEqSol, 1.0f);
}