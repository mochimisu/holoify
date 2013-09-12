#version 120
// Output our color, and have some simple alpha there
// Our composition shader is going to take the x value as the main intensity.
// The y component is a phase-shifted [0,1] sinusoidal function for use
// in other places.

varying vec3 g_normal;
varying vec4 g_position;
varying float g_phase;
varying float g_dist;


void main () {
    gl_FragColor = vec4(0.85,sin(g_phase*1.2),0,0.25);
}
