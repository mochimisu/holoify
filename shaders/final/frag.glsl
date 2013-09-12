#version 120
// All I'm doing is displaying the texture.

varying vec2 v_tex_coord;

uniform sampler2D texture_final;

void main () {
    gl_FragColor = texture2D(texture_final, vec2(v_tex_coord.x, v_tex_coord.y));
}
