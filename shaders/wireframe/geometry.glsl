#version 400

uniform mat4 Modelview;
uniform mat3 NormalMatrix;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 te_position[3];
in vec3 te_patch_distance[3];
in vec3 te_norm[3];

out vec3 g_normal;
out vec3 g_patch_distance;
out vec3 g_tri_distance;
out vec4 g_position;

void main()
{
    g_patch_distance = te_patch_distance[0];
    g_normal = te_norm[0];
    g_tri_distance = vec3(1, 0, 0);
    g_position = gl_in[0].gl_Position;
    gl_Position = g_position;
    EmitVertex();

    g_patch_distance = te_patch_distance[1];
    g_normal = te_norm[1];
    g_tri_distance = vec3(0, 1, 0);
    g_position = gl_in[1].gl_Position;
    gl_Position = g_position;
    EmitVertex();

    g_patch_distance = te_patch_distance[2];
    g_normal = te_norm[2];
    g_tri_distance = vec3(0, 0, 1);
    g_position = gl_in[2].gl_Position;
    gl_Position = g_position;
    EmitVertex();

    EndPrimitive();
}