#version 150

out vec4 out_Color;

uniform  mat4 rotMatrix_x;
uniform  mat4 rotMatrix_y;
uniform  mat4 worldToView;
uniform  float osillator;
uniform  vec3 cameraCoord;

float distance_estimator1(vec3 z){
		float Scale = 2.0;
		vec3 a1 = vec3(rotMatrix_x * vec4(1,1,1,1));
		vec3 a2 = vec3(rotMatrix_x * vec4(-1,-1,1,1));
		vec3 a3 = vec3(rotMatrix_x * vec4(1,-1,-1,1));
		vec3 a4 = vec3(rotMatrix_x * vec4(-1,1,-1,1));
		vec3 c;
		int n = 0;
		float dist, d;
		while (n < 16) {
			 c = a1; dist = length(z-a1);
			d = length(z-a2); if (d < dist) { c = a2; dist=d; }
			 d = length(z-a3); if (d < dist) { c = a3; dist=d; }
			 d = length(z-a4); if (d < dist) { c = a4; dist=d; }
			z = Scale*z-c*(Scale-1.0);
			n++;
		}

		return length(z) * pow(Scale, float(-n));
}

float distance_estimator2(vec3 position){
	vec3 z = vec3(rotMatrix_x * rotMatrix_y * vec4(position, 1.0));
	//z = mod(z + 1, 1.0) -1;
	float dr = 1.0;
	float r = 0.0;
	const int Iterations = 8;
	const int Bailout= 5;
	float Power = 20*osillator;
	for (int i = 0; i < Iterations ; i++) {
		r = length(z);
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=position;
	}
	return 0.5*log(r)*r/dr;
}

void sphereFold(inout vec3 z, inout float dz) {
	float minRadius = 0.001;
	float fixedRadius = 0.6 + 0.8*osillator;
	float r = dot(z,z);
	if (r<minRadius) { 
		// linear inner scaling
		float temp = (fixedRadius/minRadius);
		z *= temp;
		dz*= temp;
	} else if (r<fixedRadius) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius/r);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz) {
	float foldingLimit = 0.70;
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}

float distance_estimator(vec3 position)
{
	//vec3 z = vec3(rotMatrix_x * rotMatrix_y * vec4(position, 1.0));
	vec3 z = position;
	vec3 offset = z;
	float dr = 1.0;
	const int Iterations = 10;
	float Scale = 2.4;
	for (int n = 0; n < Iterations; n++) {
		boxFold(z,dr);       // Reflect
		sphereFold(z, dr);    // Sphere Inversion
        z=Scale*z + offset;  // Scale & Translate
        dr = dr*abs(Scale)+1.0;
	}
	float r = length(z);
	return r/abs(dr);
}

vec4 rayMarch(vec3 start, vec3 direction){

	float traveled_distance = 0;
	const float AMBIENT_OCC = 64;
	const int MAX_ITERATIONS = 64;
	const float MIN_DISTANCE = 0.0004;
	const float MAX_DISTANCE = 1000;

	for (int i = 0; i < MAX_ITERATIONS; ++i){

		vec3 current_position = start + direction * traveled_distance;

		float closes_distance = distance_estimator(current_position);

		if (closes_distance < MIN_DISTANCE){
			return max(0.3, (AMBIENT_OCC-i)/AMBIENT_OCC) * vec4(1.0, 0.05, 0.05, 1.0);
		}

		if (closes_distance > MAX_DISTANCE){
		    return 0.1 * vec4(1.0, 0.05, 0.05, 1.0);
		}

		traveled_distance += closes_distance;

	}
		
	return 0.1 * vec4(1.0, 0.05, 0.05, 1.0);
}

void main(void)
{
	vec2 uv;
	uv.x = gl_FragCoord.x/1800 - 0.5;
	uv.y = gl_FragCoord.y/1200 - 0.5;
	vec3 camera = vec3(cameraCoord.x, cameraCoord.y, cameraCoord.z);
	vec3 ray_direction = normalize(vec3( rotMatrix_y * rotMatrix_x  * vec4(uv.x, uv.y, -1.0, 1.0))); 
	out_Color = rayMarch(camera, ray_direction);
}
