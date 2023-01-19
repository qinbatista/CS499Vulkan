#version 400
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

// non-opaque must be in a uniform block:
layout( std140, set = 0, binding = 0 ) uniform sporadicBuf
{
	int		uMode;
	int		uUseLighting;
	int		uNumInstances;
} Sporadic;
        
layout( std140, set = 1, binding = 0 ) uniform sceneBuf
{
	mat4		uProjection;
	mat4		uView;
	mat4		uSceneOrient;
	vec4		uLightPos; 
	vec4		uLightColor;
	vec4		uLightKaKdKs;
	float		uTime;
} Scene;

layout( std140, set = 2, binding = 0 ) uniform objectBuf
{
	mat4		uModel;
	mat4		uNormal;
	vec4		uColor;
	float		uShininess;
} Object; 



layout( location = 0 ) in vec3 aVertex;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec3 aColor;
layout( location = 3 ) in vec2 aTexCoord;
//layout( location = 4 ) in vec3 aInstanceColor;


layout ( location = 0 ) out vec3 vColor;
layout ( location = 1 ) out vec2 vTexCoord;
layout ( location = 2 ) out vec3 vN;
layout ( location = 3 ) out vec3 vL;
layout ( location = 4 ) out vec3 vE;


void
main( ) 
{
	mat4  P = Scene.uProjection;
	mat4  V = Scene.uView;
	mat4  SO = Scene.uSceneOrient;
	mat4  M  = Object.uModel;
	mat4 VM = V * SO * M;
	mat4 PVM = P * VM;

	vColor    = aColor;
	vTexCoord = aTexCoord;

	vN = normalize( mat3( Object.uNormal ) * aNormal );
	                                                        // surface normal vector

	vec4 ECposition = VM * vec4( aVertex, 1. );
	vec4 lightPos = vec4( Scene.uLightPos.xyz, 1. );        // light source in fixed location
	                                                        // because not transformed
	vL = normalize( lightPos.xyz  -  ECposition.xyz );      // vector from the point
	                                                        // to the light

	//vec4 eyePos = Light.uEyePos;
	vec4 eyePos = vec4( 0., 0., 0., 1. );
	vE = normalize( eyePos.xyz -  ECposition.xyz );          // vector from the point
	                                                         // to the eye
	gl_Position = PVM * vec4( aVertex, 1. );


#define DO_INSTANCING


#ifdef DO_INSTANCING
	const float DELTA         =  3.0;	
	const float MAXINSTANCES  = 16.;
	float s = sqrt(  float( Sporadic.uNumInstances )  );
	float c = ceil( float(s) );
	int cols = int( c );
	int fullRows  = gl_InstanceIndex / cols;
	int remainder = gl_InstanceIndex % cols;
			
	float xdelta = DELTA * float( remainder );
	float ydelta = DELTA * float( fullRows );
	vColor = vec3( 1., float( (1.+gl_InstanceIndex) ) / float( Sporadic.uNumInstances ), 0. );

	xdelta -= DELTA * sqrt( float(MAXINSTANCES) ) / 2.;
	ydelta -= DELTA * sqrt( float(MAXINSTANCES) ) / 2.;
	vec4 vertex = vec4( aVertex.xyz + vec3( xdelta, ydelta, 0. ), 1. );
	gl_Position = PVM * vertex;
#endif
}
