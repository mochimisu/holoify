#version 400

in vec3 vp_loc;
in vec3 vp_norm;
in float vp_phase;

out vec3 controlpoint_wor;
out vec3 controlpoint_norm;
out float controlpoint_phase;

void main(void)
{
    controlpoint_wor = vp_loc;
    controlpoint_norm = vp_norm;
    controlpoint_phase = vp_phase;
}