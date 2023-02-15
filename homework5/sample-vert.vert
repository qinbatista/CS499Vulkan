#version 400
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable
#extension  GL_EXT_nonuniform_qualifier : enable

// Vulkan Sample Program Vertex Shader:

layout(std140, set = 0, binding = 0) uniform sporadicBuf {
    int uMode;
    int uUseLighting;
    int uNumInstances;
} Sporadic;

layout(std140, set = 1, binding = 0) uniform sceneBuf {
    mat4 uProjection;
    mat4 uView;
    mat4 uSceneOrient;
    vec4 uLightPos;			// x, y, z
    vec4 uLightColor;			// r, g, b
    vec4 uLightKaKdKs;		// ka, kd, ks
    float uTime;
} Scene;

layout(std140, set = 2, binding = 0) uniform objectBuf {
    mat4 uModel;
    mat4 uNormal;
    vec4 uColor;
    float uShininess;
} Object;

//************************P4
struct atom {
    vec3 position;
    int atomicNumber;
};

layout(std140, set = 4, binding = 0) uniform moleculeBuf {
    atom atoms[24];
};
//************************P4

//************************P5
layout(push_constant) uniform arm {
    mat4 armMatrix;
    vec3 armColor;
    float armScale;		// scale factor in x
} RobotArm;

//************************P5




layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

layout(location = 4) out vec3 vColor;
layout(location = 5) out vec2 vTexCoord;
layout(location = 6) out vec3 vN;
layout(location = 7) out vec3 vL;
layout(location = 8) out vec3 vE;

void main() {
    mat4 P = Scene.uProjection;
    mat4 V = Scene.uView;
    mat4 SO = Scene.uSceneOrient;
    mat4 M = Object.uModel;
    mat4 VM = V * SO * M;
    mat4 PVM = P * VM;

    vColor = aColor;
    vTexCoord = aTexCoord;

    vN = normalize(mat3(Object.uNormal) * aNormal);		// surface normal vector

    vec4 ECposition = VM * vec4(aVertex, 1.);
    vec4 lightPos = vec4(Scene.uLightPos.xyz, 1.);        // light source in fixed location because not transformed
    vL = normalize(lightPos.xyz - ECposition.xyz);      // vector from the point to the light

    vec4 eyePos = vec4(0., 0., 0., 1.);					// eye position after applying the viewing matrix
    vE = normalize(eyePos.xyz - ECposition.xyz);         // vector from the point to the eye

//************************P5
    // int atomicNumber = atoms[gl_InstanceIndex].atomicNumber;
    // vec3 position = atoms[gl_InstanceIndex].position;
    // float radius;
    // if(atomicNumber == 1) {
    //     vColor = vec3(1., 1., 1.);
    //     radius = 0.37;
    // } else if(atomicNumber == 6) {
    //     vColor = vec3(0., 1., 0.);
    //     radius = 0.77;
    // } else if(atomicNumber == 7) {
    //     vColor = vec3(0., 0., 1.);
    //     radius = 0.70;
    // } else if(atomicNumber == 8) {
    //     vColor = vec3(1., 0., 0.);
    //     radius = 0.66;
    // } else {
    //     vColor = vec3(1., 0., 1.);	// big magenta ball to tell us something is wrong
    //     radius = 1.0;
    // }

    vec3 bVertex = aVertex;
    // do to bVertex just what the cube needs to become a robot arm:
    bVertex.x += 1.;
    bVertex.x /= 2.;
    bVertex.x *= RobotArm.armScale;
    bVertex = vec3(RobotArm.armMatrix * vec4(bVertex, 1.));
    // now do to bVertex what the cube needed before when it was just a cube (lighting, transformation):
    //? ? ?
    gl_Position = PVM * vec4( bVertex, 1. );
//************************P5
}
