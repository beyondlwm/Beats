attribute vec3 a_position;
attribute vec2 a_texCoord0;
attribute ivec4 a_boneIndices;
attribute vec4 a_weights;

uniform mat4 u_BoneMatrices[100];
#ifndef USE_UBO
	uniform mat4 u_MVPMatrix;
#endif

varying vec2 TexCoord0;

void main()
{
	vec4 finalPos = vec4(a_position, 1);
	vec4 finalPos2 = vec4(0,0,0,0);
	mat4 matrix0 = u_BoneMatrices[a_boneIndices.x];
	finalPos2 = matrix0 * finalPos;
	finalPos2 *= a_weights.x;
	if(a_boneIndices.y >= 0)
	{
		mat4 matrix1 = u_BoneMatrices[a_boneIndices.y];
		vec4 finalPos3 = matrix1 * finalPos;
		finalPos3 *= a_weights.y;
		finalPos2 += finalPos3;
	}
	if(a_boneIndices.z >= 0)
	{
		mat4 matrix2 = u_BoneMatrices[a_boneIndices.z];
		vec4 finalPos3 = matrix2 * finalPos;
				finalPos3 *= a_weights.z;
		finalPos2 += finalPos3;
	}
	if(a_boneIndices.w >= 0)
	{
		mat4 matrix3 = u_BoneMatrices[a_boneIndices.w];
		vec4 finalPos3 = matrix3 * finalPos;
		finalPos3 *= a_weights.w;
		finalPos2 += finalPos3;
	}
	gl_Position = u_MVPMatrix * finalPos2;
	TexCoord0 = a_texCoord0;
}
