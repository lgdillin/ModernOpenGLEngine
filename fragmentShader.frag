#version 330
#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 4

in vec4 vertexColor;
in vec2 texCoords;
in vec3 normals;
in vec3 fragmentPosition;

out vec4 color;

struct Light {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight {
	Light basicLight;
	// Position of the light relative to every object (maintains the 
	// same relative position to every object in the scene regardless 
	// of that object's global position)
	vec3 direction; 
};

struct PointLight {
	Light basicLight;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	PointLight basePointLight;
	vec3 direction;
	float edge;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform sampler2D texture0;
uniform Material material;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 eyePosition;

vec4 calculateLightUsingDirVec(Light _basicLight, vec3 _direction) {


	vec4 ambientColor = vec4(_basicLight.color, 1.0) 
		* _basicLight.ambientIntensity;


	// produces the cos() of the angle between them.
	// Remember:
	// dot(v1,v2) = length(v1) * length(v2) * cos(theta)
	// if the vectors have a length of 1, then its only cos(theta)
	float diffuseFactor = max(dot(
		// Since the dot product is a measure of how parallel two
		// vectors are, we can use it to get a measure of the angle 
		// between them.  Since they are both normalized, then we get 
		// precisely the parallel-ness of the two vectors.  
		// Then we pick the max of dot(v1, v2) and 0.  Meaning, 
		// If the angle between them is larger than 90 degrees, dont
		// bother rendering the light 
		normalize(normals), 
		normalize(_direction)),0.0);

	vec4 diffuseColor = 
		vec4(_basicLight.color 
		* _basicLight.diffuseIntensity 
		* diffuseFactor, 1.0);

	// Initialize the specular to zero
	vec4 specularColor = vec4(0, 0, 0, 0);
	// if diffuse factor is > 0 (we actually have diffuse lighting)
	// then calculate specular
	// If we cannot even get diffuse lighting, then it is unlikely 
	// that the conditions exist to see specular lighting 
	if(diffuseFactor > 0.0) {
		// We want to get a vector that points at the camera 
		// for calculating the bouncing light off an object 
		// the magnitude is irrelevent, we just need the direction 
		vec3 fragmentToEye = normalize(eyePosition - fragmentPosition);
		
		// find where the light ray is effectively reflected around the 
		// normal (think like angle of incidence/reflection)
		vec3 reflectedVertex = normalize(reflect(
			-_direction, normalize(normals)));

		//vec3 reflectedVertex = 
		//	normalize(directionalLight.direction - 2.0 
		//	* dot(directionalLight.direction, normalize(normals)) 
		//	* normalize(normals));

		// So now we have the vertex from us to the fragment (fragmentToEye)
		// and now we have the vertex that is the reflected line (reflectedVertex)
		// so if fragmentToEye == reflectedVertex, then we are looking  
		// directly at the reflection, and will see brightest light
		// As we move away from the vertex (or as the angle increases),
		// Then it will dim out until it disappears completely

		// Now we want to try and find the angle between the two vectors 
		// fragToEye and reflectedVertex
		// Similar to how we use the dot product of two unit vectors 
		// to find the diffuse lighting angle, we do the same technique here
		// to find the specularFactor
		float specularFactor = dot(fragmentToEye, reflectedVertex);

		// we need to apply the shininess value (property of the material)
		// but we want to threshold the specularFactor before doing the 
		// Calculation 
		if(specularFactor > 0.0) {
			// usually shininess is based on a power of 2. (32 is most common)
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(
				_basicLight.color 
				* material.specularIntensity // How much specular light to apply
				* specularFactor, // how powerful that light is (shininess)
			1.0);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 calculatePointLight(PointLight pLight) {
	// Get the vector pointing from the pointlight to the fragment
	// ie. getting the Direction of the fragment from the pointLight
	vec3 direction =  fragmentPosition - pLight.position;

	float _distance = length(direction); // 'distance' is reserved
	direction = normalize(direction);

	vec4 lightDirection = calculateLightUsingDirVec(
		pLight.basicLight, direction);

	float attenuation = 
		pLight.quadratic * _distance * _distance // ax^2
		+ pLight.linear * _distance // bx 
		+ pLight.constant;

	return (lightDirection / attenuation);
}

vec4 calculateSpotLight(SpotLight sLight) {
	vec3 rayDirection = normalize(
		fragmentPosition - sLight.basePointLight.position);

	// Calculate what the spot light factor is 
	// factor which decides whether or not to light the frag in question 
	float spotLightFactor = dot(rayDirection, normalize(sLight.direction));

	// the larger the factor gets, the more we are in cone of influence
	if(spotLightFactor > sLight.edge) {
		vec4 pLightColor = calculatePointLight(sLight.basePointLight);
		//return (pLightColor * (1.0 - spotLightFactor) * (1.0 / (1.0 - sLight.edge)));
		
		// Attenuation function 
		//return (pLightColor / sLight.edge);
		return pLightColor 
			* (1.0 - (1.0 - spotLightFactor) * (1.0 / (1.0 - sLight.edge)));
	} else {
		return vec4(0, 0, 0, 0);
	}
}

vec4 calculateDirectionalLight() {
	return calculateLightUsingDirVec(
		directionalLight.basicLight,
		directionalLight.direction);
}

vec4 calculateTotalSpotLights() {
	vec4 spotLightColor = vec4(0, 0, 0, 0);

	for(int i = 0; i < spotLightCount; ++i) {
		// Get the vector pointing from the spot light to the fragment
		// ie. getting the Direction of the fragment from the spotLight
		spotLightColor += calculateSpotLight(spotLights[i]);
	}
	return spotLightColor;
}

vec4 calculateTotalPointLights() {
	vec4 pointLightColor = vec4(0, 0, 0, 0);

	for(int i = 0; i < pointLightCount; ++i) {
		// Get the vector pointing from the pointlight to the fragment
		// ie. getting the Direction of the fragment from the pointLight
		pointLightColor += calculatePointLight(pointLights[i]);
	}
	return pointLightColor;
}

void main() {

	///////////////////////
	// Lighting modes /////
	//////////////////////

	vec4 computedLightColor = 
		calculateDirectionalLight() 
		+ calculateTotalPointLights()
		+ calculateTotalSpotLights();

	color = texture(texture0, texCoords) * computedLightColor;

	// Texture, ambient, directional, pointLight

	// Color only mode
	//color = vertexColor; 

	// Texture only mode
	//color = texture(texture0, texCoords);

	// Texture and ambient color only
	//color = texture(texture0, texCoords) * ambientColor;

	// Texture and vertex color only
	//color = texture(texture0, texCoords) * vertexColor;
}