#version 410

layout (vertices = 3) out;
in vec3 controlpoint_wor[];
in vec3 controlpoint_norm[];
in float controlpoint_phase[];

out vec3 evaluationpoint_wor[];
out vec3 evaluationpoint_norm[];
out float evaluationpoint_phase[];

uniform float tessLevel;

void main () {
    evaluationpoint_wor[gl_InvocationID] = controlpoint_wor[gl_InvocationID];
    evaluationpoint_norm[gl_InvocationID] = controlpoint_norm[gl_InvocationID];
    evaluationpoint_phase[gl_InvocationID] = controlpoint_phase[gl_InvocationID];

    gl_TessLevelInner[0] = 0;
    gl_TessLevelOuter[gl_InvocationID] = 1;
}