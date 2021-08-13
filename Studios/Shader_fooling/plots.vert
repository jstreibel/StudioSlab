attribute vec4 color;
attribute vec2 position;
uniform sampler1D samples;
varying vec4 v_color;


void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	v_color = color;
}
