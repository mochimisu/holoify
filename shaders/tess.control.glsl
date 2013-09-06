#version 400
 
layout (vertices = 3) out; // number of CPs in patch
in vec3 controlpoint_wor[]; // from VS (use empty modifier [] so we can say anything)
in vec3 controlpoint_norm[];
out vec3 evaluationpoint_wor[]; // to evluation shader. will be used to guide positioning of generated points
out vec3 evaluationpoint_norm[];
 
uniform float tessLevel; // controlled by keyboard buttons
 
void main () {
  evaluationpoint_wor[gl_InvocationID] = controlpoint_wor[gl_InvocationID];
  evaluationpoint_norm[gl_InvocationID] = controlpoint_norm[gl_InvocationID];
 
  // Calculate the tessellation levels
  gl_TessLevelInner[0] = tessLevel; // number of nested primitives to generate
  gl_TessLevelOuter[gl_InvocationID] = tessLevel; // times to subdivide first side
}