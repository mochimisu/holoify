#version 120

attribute vec3 vp_loc;
varying vec2 v_tex_coord;

void main(void)
{
    vec2 pos = sign(vp_loc.xy);
    v_tex_coord = pos * 0.5f + 0.5f;
    gl_Position = vec4(pos, 0.f, 1.f);
}
