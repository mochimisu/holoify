#version 400

in vec3 vp_loc;
in float vp_phase;

out vec2 v_tex_coord;
out float v_phase;

void main(void)
{
    vec2 pos = sign(vp_loc.xy);
    v_tex_coord = pos * 0.5f + 0.5f;
    v_phase = vp_phase;
    gl_Position = vec4(pos, 0.f, 1.f);
}