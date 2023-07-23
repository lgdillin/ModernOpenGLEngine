#version 330
#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 4

in vec4 vertexColor;
in vec2 texCoords;
in vec3 normals;
in vec3 fragmentPosition;
in vec4 directionalLightSpacePos;

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

struct OmniShadowMap {
	samplerCube shadowMap;
	float farPlane;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform sampler2D diffuseTexture;
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];
uniform Material material;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 eyePosition;

float calcOmniShadowFactor(PointLight light, int shadowIndex) {
	// we want to know what the distance between our light source and that frag 
	// so we can then compare it to the closest position 
	vec3 fragToLight = fragmentPosition - light.position;
	
	// so if our fragToLight is equal to closest  then it is in the light
	float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight).r;
	closest *= omniShadowMaps[shadowIndex].farPlane;
	
	// if current has a larger value than closest, then it is in the shadow 
	float current = length(fragToLight);

	float bias = 0.05;
	float shadow = current - bias > closest ? 1.0 : 0.0;

	return shadow;
}

vec4 depthMapVisual(PointLight light, int shadowIndex) {
	vec3 fragToLight = fragmentPosition - light.position;
	float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight).r;
	closest *= omniShadowMaps[shadowIndex].farPlane;
	return vec4(vec3(closest/omniShadowMaps[shadowIndex].farPlane), 1.0);
}

float calcDirectionalShadowFactor(DirectionalLight dLight) {
	
	// by dividing by w our coords will be [-1,1]
	vec3 projectionCoords = directionalLightSpacePos.xyz 
		/ directionalLightSpacePos.w;

	projectionCoords = (projectionCoords * 0.5) + 0.5;

	// how far away the point is from the actual light
	float currentDepth = projectionCoords.z;
	// now -1 is being mapped to 0.0. (values now fall between [0,1])

	// we now know where the light says the closest point is in that direction (xy)
	// however its not necessarily the point we're looking at.
	// the actual point we're looking at is the entire 


	vec3 normal = normalize(normals);
	vec3 lDirection = normalize(directionalLight.direction);
	float bias = max(0.005 * (1.0 - dot(normal, lDirection)), 0.0005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(
				directionalShadowMap,
				projectionCoords.xy + vec2(x,y) * texelSize).r;

			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	// we iterated over 9 pixels, so we are dividing it by the number 
	// of pixels to compute an average, and scale by that
	shadow /= 9.0;

	if(projectionCoords.z > 1.0) {
		shadow = 0.0;
	}	

	return shadow;
}


float calcDirectionalShadowFactor1(DirectionalLight dLight) {
	vec3 projCoords = directionalLightSpacePos.xyz / directionalLightSpacePos.w;

	projCoords = (projCoords * 0.5) + 0.5;
	float closest = texture(directionalShadowMap, projCoords.xy).r;
	float current = projCoords.z;

	float shadow = current > closest ? 1.0 : 0.0;
	return shadow;
}


vec4 calculateLightUsingDirVec(
	Light _basicLight, 
	vec3 _direction, 
	float _shadowFactor
) {


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

	return 
		(ambientColor + (1.0 - _shadowFactor) 
		* (diffuseColor + specularColor));
}

vec4 calculatePointLight(PointLight pLight, int shadowIndex) {
	// Get the vector pointing from the pointlight to the fragment
	// ie. getting the Direction of the fragment from the pointLight
	vec3 direction =  pLight.position - fragmentPosition;

	float _distance = length(direction); // 'distance' is reserved
	direction = normalize(direction);

	float shadowFactor = calcOmniShadowFactor(pLight, shadowIndex);

	vec4 lightDirection = calculateLightUsingDirVec(
		pLight.basicLight, direction, shadowFactor);

	float attenuation = 
		pLight.quadratic * _distance * _distance // ax^2
		+ pLight.linear * _distance // bx 
		+ pLight.constant;

	return (lightDirection / attenuation);
}

vec4 calculateSpotLight(SpotLight sLight, int shadowIndex) {
	vec3 rayDirection = normalize(
		fragmentPosition - sLight.basePointLight.position);

	// Calculate what the spot light factor is 
	// factor which decides whether or not to light the frag in question 
	float spotLightFactor = dot(rayDirection, normalize(sLight.direction));

	// the larger the factor gets, the more we are in cone of influence
	if(spotLightFactor > sLight.edge) {
		vec4 pLightColor = calculatePointLight(sLight.basePointLight, shadowIndex);
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
	float shadowFactor = calcDirectionalShadowFactor(directionalLight);

	return calculateLightUsingDirVec(
		directionalLight.basicLight,
		directionalLight.direction,
		shadowFactor);
}

vec4 calculateTotalSpotLights() {
	vec4 spotLightColor = vec4(0, 0, 0, 0);

	for(int i = 0; i < spotLightCount; ++i) {
		// Get the vector pointing from the spot light to the fragment
		// ie. getting the Direction of the fragment from the spotLight
		spotLightColor += calculateSpotLight(spotLights[i], i + MAX_POINT_LIGHTS);
	}
	return spotLightColor;
}

vec4 calculateTotalPointLights() {
	vec4 pointLightColor = vec4(0, 0, 0, 0);

	for(int i = 0; i < pointLightCount; ++i) {
		// Get the vector pointing from the pointlight to the fragment
		// ie. getting the Direction of the fragment from the pointLight
		pointLightColor += calculatePointLight(pointLights[i], i);
		//depthMapVisual(pointLights[i], 
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

	color = texture(diffuseTexture, texCoords) * computedLightColor;

	//color = vec4(vec3(closestDepth / farPlane), 1.0);


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