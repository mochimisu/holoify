#version 120

// Pass through things to our tess shader, which does the projections

attribute vec3 vp_loc;
attribute vec3 vp_norm;
attribute float vp_phase;

varying vec3 v_position;
varying vec3 v_norm;
varying float v_phase;

// We do our MVP mult in here, so we're interpolating in model space
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform float time;

void main(void)
{
    v_norm = vp_norm;
    v_phase = vp_phase;

    // Powers of sin(t+p)
    float sintime = sin(time+vp_phase);
    float sintime2 = sintime*sintime;
    float sintime4 = sintime2*sintime2;
    //float sintime8 = sintime4*sintime4;

    // clamp_time used for the implode/explode animations
    float clamp_time = -cos(clamp(time*5.f,0.f,3.14f));

    // Distance to displace along the normal
    float dist = 2*sin(20*vp_phase)*sintime2*sintime + (1.f-clamp_time)*40.f;


    vec3 disp_pos = vp_loc + dist*vp_norm;

    vec4 _pos = projectionMatrix * viewMatrix * modelMatrix * vec4(disp_pos,1);
    vec3 pos = _pos.xyz/_pos.w;
    v_position = pos;
    gl_Position = vec4(pos,1);
}
