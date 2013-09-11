#version 400

// The actual tessellation!
// This is a simple averaging tessellation of the 3 original CPs
// Because we use triangles, gl_TessCoord is barycentric (will be UV for quads)

// The stuff we get from the control shader
layout (triangles, equal_spacing, ccw) in;
in vec3 evaluationpoint_wor[];
in vec3 evaluationpoint_norm[];
in float evaluationpoint_phase[];

// Stuff we pass to the geometry shader
out vec3 te_position;
out vec3 te_norm;
out float te_phase;
out float te_dist;

uniform float time;

// We do our MVP mult in here, so we're interpolating in model space
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

void main () {
    vec3 p0 = gl_TessCoord.x * evaluationpoint_wor[0];
    vec3 p1 = gl_TessCoord.y * evaluationpoint_wor[1];
    vec3 p2 = gl_TessCoord.z * evaluationpoint_wor[2];

    vec3 n0 = gl_TessCoord.x * evaluationpoint_norm[0];
    vec3 n1 = gl_TessCoord.y * evaluationpoint_norm[1];
    vec3 n2 = gl_TessCoord.z * evaluationpoint_norm[2];

    float ph0 = gl_TessCoord.x * evaluationpoint_phase[0];
    float ph1 = gl_TessCoord.y * evaluationpoint_phase[1];
    float ph2 = gl_TessCoord.z * evaluationpoint_phase[2];

    vec3 norm = normalize(n0 + n1 + n2);
    float phase = (ph0 + ph1 + ph2)/3.f;

    // Powers of sin(t+p)
    float sintime = sin(time+phase);
    float sintime2 = sintime*sintime;
    float sintime4 = sintime2*sintime2;
    //float sintime8 = sintime4*sintime4;

    // clamp_time used for the implode/explode animations
    float clamp_time = -cos(clamp(time*5.f,0.f,3.14f));

    // Distance to displace along the normal
    float dist = 2*sin(20*phase)*sintime2*sintime + (1.f-clamp_time)*40.f;

    // Our final, displaced position
    vec3 pos = p0 + p1 + p2 + dist*norm;

    // ...and projections, etc
    vec4 _norm = normalMatrix * vec4(norm,1);
    te_norm = _norm.xyz;
    vec4 _pos = viewMatrix * modelMatrix * vec4(pos,1);
    te_position = _pos.xyz/_pos.w;
    te_phase = phase;
    te_dist = dist;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4 (pos, 1);
}