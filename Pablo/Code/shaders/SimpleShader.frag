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
	vec3 lightDir[63];
	vec3 lightPos[63];
	vec4 lightColor[63];
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

	// DAY NIGHT CYCLE
	LD = direct_light_dir(fragPos, 0);
	LC = direct_light_color(fragPos, 0);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.y;

	// Plane light
	LD = spot_light_dir(fragPos, 1);
	LC = spot_light_color(fragPos, 1);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.z;

	// Point light

	LD = point_light_dir(fragPos, 2);
	LC = point_light_color(fragPos, 2);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 3);
	LC = point_light_color(fragPos, 3);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 4);
	LC = point_light_color(fragPos, 4);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 5);
	LC = point_light_color(fragPos, 5);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 6);
	LC = point_light_color(fragPos, 6);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 7);
	LC = point_light_color(fragPos, 7);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 8);
	LC = point_light_color(fragPos, 8);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 9);
	LC = point_light_color(fragPos, 9);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 10);
	LC = point_light_color(fragPos, 10);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 11);
	LC = point_light_color(fragPos, 11);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 12);
	LC = point_light_color(fragPos, 12);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 13);
	LC = point_light_color(fragPos, 13);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 14);
	LC = point_light_color(fragPos, 14);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 15);
	LC = point_light_color(fragPos, 15);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 16);
	LC = point_light_color(fragPos, 16);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 17);
	LC = point_light_color(fragPos, 17);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 18);
	LC = point_light_color(fragPos, 18);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 19);
	LC = point_light_color(fragPos, 19);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 20);
	LC = point_light_color(fragPos, 20);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 21);
	LC = point_light_color(fragPos, 21);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 22);
	LC = point_light_color(fragPos, 22);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 23);
	LC = point_light_color(fragPos, 23);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 24);
	LC = point_light_color(fragPos, 24);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 25);
	LC = point_light_color(fragPos, 25);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 26);
	LC = point_light_color(fragPos, 26);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 27);
	LC = point_light_color(fragPos, 27);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 28);
	LC = point_light_color(fragPos, 28);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 29);
	LC = point_light_color(fragPos, 29);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 30);
	LC = point_light_color(fragPos, 30);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 31);
	LC = point_light_color(fragPos, 31);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 32);
	LC = point_light_color(fragPos, 32);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 33);
	LC = point_light_color(fragPos, 33);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 34);
	LC = point_light_color(fragPos, 34);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 35);
	LC = point_light_color(fragPos, 35);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 36);
	LC = point_light_color(fragPos, 36);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 37);
	LC = point_light_color(fragPos, 37);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 38);
	LC = point_light_color(fragPos, 38);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 39);
	LC = point_light_color(fragPos, 39);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 40);
	LC = point_light_color(fragPos, 40);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 41);
	LC = point_light_color(fragPos, 41);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 42);
	LC = point_light_color(fragPos, 42);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 43);
	LC = point_light_color(fragPos, 43);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 44);
	LC = point_light_color(fragPos, 44);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 45);
	LC = point_light_color(fragPos, 45);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 46);
	LC = point_light_color(fragPos, 46);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 47);
	LC = point_light_color(fragPos, 47);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 48);
	LC = point_light_color(fragPos, 48);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 49);
	LC = point_light_color(fragPos, 49);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 50);
	LC = point_light_color(fragPos, 50);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 51);
	LC = point_light_color(fragPos, 51);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 52);
	LC = point_light_color(fragPos, 52);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 53);
	LC = point_light_color(fragPos, 53);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 54);
	LC = point_light_color(fragPos, 54);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 55);
	LC = point_light_color(fragPos, 55);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 56);
	LC = point_light_color(fragPos, 56);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 57);
	LC = point_light_color(fragPos, 57);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 58);
	LC = point_light_color(fragPos, 58);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 59);
	LC = point_light_color(fragPos, 59);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 60);
	LC = point_light_color(fragPos, 60);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 61);
	LC = point_light_color(fragPos, 61);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;

	LD = point_light_dir(fragPos, 62);
	LC = point_light_color(fragPos, 62);
	RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;



	// int i;
	// for(i = 2; i < 14; i++) {
		
	// }


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