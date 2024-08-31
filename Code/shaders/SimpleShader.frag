#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAXLIGHTS 16

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightDir[MAXLIGHTS];
    vec3 lightPos[MAXLIGHTS];
    vec4 lightColor[MAXLIGHTS];
    vec3 isOn[MAXLIGHTS];
    float cosIn;
    float cosOut;
    vec3 eyePos;
    vec4 eyeDir;
    vec4 lightOn;
} gubo;

layout(set = 1, binding = 2) uniform SimpleMatParUniformBufferObject {
    float Pow;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D texDiff;

vec3 direct_light_dir(vec3 pos, int i) {
    return gubo.lightDir[i];
}

vec3 direct_light_color(vec3 pos, int i) {
    return gubo.lightColor[i].rgb;
}

vec3 point_light_dir(vec3 pos, int i) {
    return normalize(gubo.lightPos[i] - pos);
}

vec3 point_light_color(vec3 pos, int i) {
    return gubo.lightColor[i].rgb * pow(gubo.lightColor[i].a / length(gubo.lightPos[i] - pos), 2.0);
}

vec3 spot_light_dir(vec3 pos, int i) {
    return normalize(gubo.lightPos[i] - pos);
}

vec3 spot_light_color(vec3 pos, int i) {
    return point_light_color(pos, i) * clamp(( dot(spot_light_dir(pos, i),gubo.lightDir[i]) - gubo.cosOut)/(gubo.cosIn - gubo.cosOut),0.0,1.0);
}

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
    float diff = max(dot(Norm, LD), 0.0);
    vec3 Diffuse = Albedo * diff;
    vec3 halfVector = normalize(EyeDir + LD);  
    float spec = pow(max(dot(Norm, halfVector), 0.0), 160.0f); 
    vec3 Specular = vec3(spec);
    return Diffuse + Specular;
}

vec3 BRDFSimple(vec3 Albedo, vec3 Norm, vec3 LD) {
    vec3 Diffuse = Albedo * max(dot(Norm, LD), 0.0);
    return Diffuse;
}

void main() {
    vec3 norm = normalize(fragNorm);
    vec3 eyeDir = normalize(gubo.eyePos - fragPos);
    vec3 albedo = texture(texDiff, fragUV).rgb;
    vec3 rendEqSol = vec3(0);
    
    // DAY NIGHT CYCLE

	rendEqSol += BRDF(albedo, norm, eyeDir, direct_light_dir(fragPos, 0)) * direct_light_color(fragPos, 0) * gubo.lightOn.x;

	// PLANE LIGHT

	rendEqSol += BRDFSimple(albedo, norm, spot_light_dir(fragPos, 1)) * spot_light_color(fragPos, 1) * gubo.lightOn.y;
    
    // POINT LIGHTS

    if (gubo.lightOn.z > 0.0f) {
        for (int i = 2; i < MAXLIGHTS ; i++) {
            if(gubo.isOn[i].x>0.0){
                rendEqSol += BRDFSimple(albedo, norm, point_light_dir(fragPos, i)) * point_light_color(fragPos, i) * gubo.lightOn.z;
            }
       }
    }

    //INDIRECT LIGHT

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