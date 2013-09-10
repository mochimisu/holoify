#version 400

in vec2 v_tex_coord;
in float v_phase;

uniform sampler2D texture_source;
uniform vec2 scale;
uniform float time;

out vec4 frag_color;

void main () {
    vec4 sum = vec4(0.f);
    float max_phase = 0;

    vec4 cur_samp = texture2D(texture_source,
        vec2(v_tex_coord.x, v_tex_coord.y));

    // 9 samples
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x - 4.0 * scale.x, v_tex_coord.y - 4.0 * scale.y))
        * 0.05;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x - 3.0 * scale.x, v_tex_coord.y - 3.0 * scale.y))
        * 0.09;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x - 2.0 * scale.x, v_tex_coord.y - 2.0 * scale.y))
        * 0.12;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x - 1.0 * scale.x, v_tex_coord.y - 1.0 * scale.y))
        * 0.15;
    sum += cur_samp * 0.16;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x + 1.0 * scale.x, v_tex_coord.y + 1.0 * scale.y))
        * 0.15;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x + 2.0 * scale.x, v_tex_coord.y + 2.0 * scale.y))
        * 0.12;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x + 3.0 * scale.x, v_tex_coord.y + 3.0 * scale.y))
        * 0.09;
    sum += texture2D(texture_source,
        vec2(v_tex_coord.x + 4.0 * scale.x, v_tex_coord.y + 4.0 * scale.y))
        * 0.05;

    //frag_color = vec4(0,sum.x*(2+0.5*cos(sum.y*1000+time)),0,1);
    //frag_color = vec4(sum,cur_samp.y,sum*cur_samp.y,1);
    //frag_color = vec4(cur_samp);
    frag_color = vec4((1+sum.y)*sum.x,sum.y,0,1);
}