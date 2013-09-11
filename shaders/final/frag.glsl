#version 400
// All I'm doing is displaying the texture.

in vec2 v_tex_coord;

uniform sampler2D texture_final;

out vec4 frag_color;

void main () {
    frag_color = texture2D(texture_final, vec2(v_tex_coord.x, v_tex_coord.y));
}
