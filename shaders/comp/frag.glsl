#version 400

in vec2 v_tex_coord;

uniform sampler2D texture_plain;
uniform sampler2D texture_bloom;
uniform sampler2D texture_ghost;

out vec4 frag_color;

void main () {
    vec4 plain = texture2D(texture_plain, vec2(v_tex_coord.x, v_tex_coord.y));
    vec4 bloom = texture2D(texture_bloom, vec2(v_tex_coord.x, v_tex_coord.y));
    vec4 ghost = texture2D(texture_ghost, vec2(v_tex_coord.x, v_tex_coord.y));

    vec4 plain_color = vec4(1,1,1,1);
    vec4 bloom_color = vec4(0.5,0.75,1,1);

    vec4 cur_final_color = plain_color *0.2* plain.x + bloom_color *2* bloom.x;

    vec3 frag_color3 = 0.2*cur_final_color.xyz + 0.8 * ghost.xyz;
    frag_color = vec4(frag_color3, 1);
    //frag_color = vec4(bloom.x);
    //frag_color = vec4(plain.x);
}