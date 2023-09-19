attribute vec3 vertex;
attribute vec2 tex_coord;

void main()
{
    gl_TexCoord[0].xy = tex_coord.xy;
    gl_Position       = vec4(vertex,1.0);
}