#version 400
// Bloom!
// This one is a bit weird - I am doing a weighted gaussian blur
// My bloom is going to be one color in the end, and its intensity is
// controlled by frag_color.x
//
// I weigh by a sum of the y component, which is typically [0,1], and is a
// sinusoid-like-kinda function of time, varied by phase, and was generated
// in the main shader.
//
// This shader is ran twice, once with scale as (something,0) and again as
// (0,something), for a two-pass gaussian.

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
    frag_color = vec4((0.65+2.f*sum.y)*sum.x,sum.y,0,1);
    //frag_color = vec4(sum.x,sum.y,0,1);
}