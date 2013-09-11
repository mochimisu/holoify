#version 400
// Our geometry shader!
// Takes in a triangle, and spits out the 3 corner points corresponding to it.
// This will output multiple points per vertex, but they are likely to be
// phase shifted.
//
// Outputting the center of the triangle doesn't seem to give good results.

uniform mat4 Modelview;
uniform mat3 NormalMatrix;
layout(triangles) in;
layout(points, max_vertices = 1) out;

in vec3 te_position[3];
in vec3 te_patch_distance[3];
in vec3 te_norm[3];
in float te_phase[3];
in float te_dist[3];

out vec3 g_normal;
out vec3 g_patch_distance;
out vec3 g_tri_distance;
out vec4 g_position;
out float g_phase;
out float g_dist;

void main()
{
    g_patch_distance = te_patch_distance[0];
	g_normal = te_norm[0];
    g_tri_distance = vec3(1, 0, 0);
	g_position = gl_in[0].gl_Position;
    g_phase = te_phase[0];
    g_dist = te_dist[0];
	gl_Position = g_position;
    EmitVertex();

    g_patch_distance = te_patch_distance[1];
	g_normal = te_norm[1];
    g_tri_distance = vec3(0, 1, 0);
	g_position = gl_in[1].gl_Position;
    g_phase = te_phase[1];
    g_dist = te_dist[1];
	gl_Position = g_position;
    EmitVertex();

    g_patch_distance = te_patch_distance[2];
	g_normal = te_norm[2];
    g_tri_distance = vec3(0, 0, 1);
	g_position = gl_in[2].gl_Position;
    g_phase = te_phase[2];
    g_dist = te_dist[2];
    gl_Position = g_position;
	EmitVertex();

    EndPrimitive();
}