#version 400

in vec3 g_normal;
in vec3 g_patch_distance;
in vec3 g_tri_distance;
in vec4 g_position;
in float g_primitive;
 
out vec4 frag_color;
 
float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}
void main () {
/*
	vec3 light_pos = vec3(0,0,5);
	vec3 ambient = vec3(0.6);

	vec3 direction = normalize(light_pos-g_position.xyz);

	float ndl = max(dot(g_normal, direction),0);

	float d1 = min(min(g_tri_distance.x, g_tri_distance.y), g_tri_distance.z);
	float d2 = min(min(g_patch_distance.x, g_patch_distance.y), g_patch_distance.z);


	vec3 color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * (vec3(ndl) + ambient);

	frag_color = vec4(color, 1.0);
    */
    frag_color = vec4(0,0,0,1);
}