#version 400
// Output our color, and have some simple alpha there
// Our composition shader is going to take the x value as the main intensity.
// The y component is a phase-shifted [0,1] sinusoidal function for use
// in other places.

in vec3 g_normal;
in vec4 g_position;
in float g_phase;
in float g_dist;

out vec4 frag_color;

void main () {
    frag_color = vec4(0.85,sin(g_phase*1.2),0,0.25);
}