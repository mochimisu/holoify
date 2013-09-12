#version 120
// Our geometry shader!
// Takes in a triangle, and spits out the 3 corner points corresponding to it.
// This will output multiple points per vertex, but they are likely to be
// phase shifted.
//
// Outputting the center of the triangle doesn't seem to give good results.

varying in vec3 v_position[3];
varying in vec3 v_norm[3];
varying in float v_phase[3];

varying out vec3 g_normal;
varying out vec3 g_patch_distance;
varying out vec3 g_tri_distance;
varying out vec4 g_position;
varying out float g_phase;
varying out float g_dist;

void main()
{
    g_normal = v_norm[0];
    g_position = gl_PositionIn[0];
    g_phase = v_phase[0];
    gl_Position = g_position;
    EmitVertex();

    g_normal = v_norm[1];
    g_position = gl_PositionIn[1];
    g_phase = v_phase[1];
    gl_Position = g_position;
    EmitVertex();

    g_normal = v_norm[2];
    g_position = gl_PositionIn[2];
    g_phase = v_phase[2];
    gl_Position = g_position;
    EmitVertex();

    EndPrimitive();
}
