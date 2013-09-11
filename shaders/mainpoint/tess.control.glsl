#version 410

// Controls the amount to subdivide by.
// Controlled by tess_scale (scales the adaptive subdivision) and tess_level
// (scales the uniform subdivision)

// # CPs in patch
layout (vertices = 3) out;

// Stuff from VS
in vec3 controlpoint_wor[];
in vec3 controlpoint_norm[];
in float controlpoint_phase[];

// Stuff we output to tess eval
out vec3 evaluationpoint_wor[];
out vec3 evaluationpoint_norm[];
out float evaluationpoint_phase[];

// User scale factors
uniform float tess_level;
uniform float tess_scale;

void main () {
  // Pass through the VS values
  evaluationpoint_wor[gl_InvocationID] = controlpoint_wor[gl_InvocationID];
  evaluationpoint_norm[gl_InvocationID] = controlpoint_norm[gl_InvocationID];
  evaluationpoint_phase[gl_InvocationID] = controlpoint_phase[gl_InvocationID];

  // Calculate the tessellation levels
  // by triangle area
  // (really rectangle area but who cares)
  vec3 v1 = controlpoint_wor[1] - controlpoint_wor[0];
  vec3 v2 = controlpoint_wor[2] - controlpoint_wor[0];
  vec3 v3 = cross(v1,v2);
  float area = dot(v3,v3);

  float weighted_tess_level = max(clamp(area*100000000.f,0,5)-1,0.f)*tess_scale
      + tess_level;

  gl_TessLevelInner[0] = weighted_tess_level;
  gl_TessLevelOuter[gl_InvocationID] = weighted_tess_level;
}