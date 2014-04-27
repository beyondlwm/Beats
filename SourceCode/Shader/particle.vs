attribute vec3 a_position;
attribute vec3 a_color;

#ifndef USE_UBO
	uniform mat4 u_MVPMatrix;
	uniform mat4 u_MVMatrix;
#endif

varying vec3 colortmp;

void main()
{
	vec4 pPosition = u_MVMatrix * vec4(a_position, 1.0);
	gl_Position = u_MVPMatrix * pPosition;
	colortmp = a_color;
}
