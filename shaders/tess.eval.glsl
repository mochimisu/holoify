#version 400

layout (triangles, equal_spacing, ccw) in; // triangles, quads, or isolines
in vec3 evaluationpoint_wor[];
in vec3 evaluationpoint_norm[];
in float evaluationpoint_phase[];

out vec3 te_position;
out vec3 te_patch_distance;
out vec3 te_norm;
out float te_phase;

uniform float time;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

// gl_TessCoord is location within the patch (barycentric for triangles, UV for quads)

void main () {
  vec3 p0 = gl_TessCoord.x * evaluationpoint_wor[0]; // x is one corner
  vec3 p1 = gl_TessCoord.y * evaluationpoint_wor[1]; // y is the 2nd corner
  vec3 p2 = gl_TessCoord.z * evaluationpoint_wor[2]; // z is the 3rd corner (ignore when using quads)

  vec3 n0 = gl_TessCoord.x * evaluationpoint_norm[0];
  vec3 n1 = gl_TessCoord.y * evaluationpoint_norm[1];
  vec3 n2 = gl_TessCoord.z * evaluationpoint_norm[2];

  float ph0 = gl_TessCoord.x * evaluationpoint_phase[0];
  float ph1 = gl_TessCoord.y * evaluationpoint_phase[1];
  float ph2 = gl_TessCoord.z * evaluationpoint_phase[2];

  vec3 norm = n0 + n1 + n2;
  float phase = ph0 + ph1 + ph2;

  float sintime = sin(time+phase);
  float sintime2 = sintime*sintime;
  float sintime4 = sintime2*sintime2;
  float sintime8 = sintime4*sintime4;

  float sign_mult = 1;
  if (sintime < 0)
  {
    sign_mult = -1;
  }

  vec3 pos = p0 + p1 + p2 + norm*10*sintime4*sign_mult;

  vec4 _norm = normalMatrix * vec4(norm,1);
  te_norm = _norm.xyz;
  vec4 _pos = viewMatrix * modelMatrix * vec4(pos,1);
  te_patch_distance = gl_TessCoord;
  te_position = _pos.xyz/_pos.w;
  te_phase = phase;

  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4 (pos, 1);
}