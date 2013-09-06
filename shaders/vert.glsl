#version 400

in vec3 vp_loc;
in vec3 vp_norm;

out vec3 controlpoint_wor;
out vec3 controlpoint_norm;

void main(void)
{
     controlpoint_wor = vp_loc;
	 controlpoint_norm = vp_norm;
}