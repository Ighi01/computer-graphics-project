#version 450
#extension GL_ARB_separate_shader_objects : enable
// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
// This defines the color computed by this shader. Generally is always location 0.
layout(location = 0) out vec4 outColor;

// Here the Uniform buffers are defined. In this case, the Global Uniforms of Set 0
// The texture of Set 1 (binding 1), and the Material parameters (Set 1, binding 2)
// are used. Note that each definition must match the one used in the CPP code
// put GLOBAL instead of Blinn
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightDir[6];
	vec3 lightPos[6];
	vec4 lightColor[6];
	float cosIn;
	float cosOut;
	vec3 eyePos;
	vec4 eyeDir;
	vec4 lightOn;
} gubo;

// add Par
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
	vec3 dir = normalize(gubo.lightPos[i] - pos);
	return dir;
}

vec3 point_light_color(vec3 pos, int i) {
	float power = pow(gubo.lightColor[i].a/length(gubo.lightPos[i] - pos), 2.0);
	vec3 light = gubo.lightColor[i].rgb*power;
	return light;
}

vec3 spot_light_dir(vec3 pos, int i) {
	vec3 dir = normalize(gubo.lightPos[i] - pos);
	return dir;
}

vec3 spot_light_color(vec3 pos, int i) {
	vec3 dir = normalize(gubo.lightPos[i] - pos);
	float power = pow(gubo.lightColor[i].a/length(gubo.lightPos[i] - pos), 2.0);
	vec3 point_light = gubo.lightColor[i].rgb*power;
	float to_clamp = (dot(dir,gubo.lightDir[i])-gubo.cosOut)/(gubo.cosIn-gubo.cosOut);
	vec3 light = point_light*clamp(to_clamp, 0, 1);

	return light;
}


vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
// Compute the BRDF, with a given color <Albedo>, in a given position characterized bu a given normal vector <Norm>,
// for a light direct according to <LD>, and viewed from a direction <EyeDir>
	vec3 Diffuse;
	vec3 Specular;
	Diffuse = Albedo * max(dot(Norm, LD),0.0f);
	Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)),0.0f), 160.0f));
	
	return Diffuse + Specular;
}
// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model
// The scene is lit by a single Spot Light
void main() {

	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(texDiff, fragUV).rgb;

	vec3 LD;	// light direction
	vec3 LC;	// light color

	vec3 RendEqSol = vec3(0);

	// First light
	LD = point_light_dir(fragPos, 0);
	LC = point_light_color(fragPos, 0);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	// Second light
	LD = point_light_dir(fragPos, 1);
	LC = point_light_color(fragPos, 1);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	// Third light
	LD = point_light_dir(fragPos, 2);
	LC = point_light_color(fragPos, 2);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	// New light
	LD = point_light_dir(fragPos, 3);
	LC = point_light_color(fragPos, 3);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	// Fourth light
	LD = direct_light_dir(fragPos, 4);
	LC = direct_light_color(fragPos, 4);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.y;

	// Fift light
	LD = spot_light_dir(fragPos, 5);
	LC = spot_light_color(fragPos, 5);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.z;

	// DAY NIGHT CYCLE
	LD = direct_light_dir(fragPos, 4);
	LC = direct_light_color(fragPos, 4);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.y;

	// Indirect illumination simulation
	// A special type of non-uniform ambient color, invented for this course
	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.1;
	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.1;
	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.1;
	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.1;
	const vec3 czp = vec3(0.8,0.2,0.4) * 0.1;
	const vec3 czn = vec3(0.3,0.6,0.7) * 0.1;
	
	vec3 Ambient =((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +
				   (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +
				   (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) * Albedo;
	RendEqSol += Ambient         * gubo.lightOn.w;
	
	// Output color
	outColor = vec4(RendEqSol, 1.0f);
}