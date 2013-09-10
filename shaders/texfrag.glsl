#version 400 core
in vec2 v_tex_coord;

out vec4 frag_color;

uniform sampler2D texture_source;
uniform float time;
void main(){
    frag_color = texture2D(texture_source,
        vec2(v_tex_coord.x, v_tex_coord.y));
}