attribute vec4 a_position;
attribute vec4 a_color;

#ifndef USE_UBO
	uniform mat4 u_MVPMatrix;
#endif

varying vec4 color;

void main()
{
	gl_Position = u_MVPMatrix * a_position;
	color = a_color;
}
