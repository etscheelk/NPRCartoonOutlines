#version 330

// Fragment shader

// Textures
uniform sampler2D diffuseRamp;
uniform sampler2D specularRamp;
uniform sampler2D edges;

uniform vec3 eye_world;

uniform vec4 lightPosition;

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:
in vec4 interpSurfPosition;
in vec3 interpSurfNormal;

in vec4 glPos;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform vec3 color;

uniform vec3 ambientReflectionCoeff;
uniform vec3 diffuseReflectionCoeff;
uniform vec3 specularReflectionCoeff;
uniform float specularExponent;

uniform vec3 ambientLightIntensity;
uniform vec3 diffuseLightIntensity;
uniform vec3 specularLightIntensity; 

uniform float near;
uniform float far;

uniform float windowWidth;
uniform float windowHeight;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec2 texCoord(float xPos, float yPos) {
    return vec2((xPos) / windowWidth, (yPos) / windowHeight);
}

void mapP(float x, float y, float p, vec3 color) {
    if (pow(pow(abs(x), p) + pow(abs(y), p), 1.0/p) <= 1)
        fragColor.rgb = vec3(color);
}

void main() {
    vec3 E = normalize(eye_world - vec3(interpSurfPosition));
    vec3 N = normalize(interpSurfNormal);
    vec3 L = normalize(vec3(lightPosition - interpSurfPosition));
    vec3 H = normalize(L + E);

    float NdotL = max(dot(N, L), 0.0);

    float EdotL = max(dot(E, L), 0.0);

    float HdotN = max(dot(H, N), 0.0);


    // Start with black and then add lighting to the final color as we calculate it
	vec3 finalColor = vec3(0.0, 0.0, 0.0);

    // Calculate ambient, diffuse, and specular lighting for this pixel based on its position, normal, etc.
    vec3 ambient = ambientLightIntensity * color; 

    vec3 diffuse = diffuseLightIntensity * color * vec3(texture(diffuseRamp, vec2(NdotL, 0.0)));
	
    vec3 specular = specularLightIntensity * color * vec3(texture(specularRamp, vec2(pow(HdotN, specularExponent), 0.0)));
    
    
    finalColor = ambient + diffuse + specular;
    
    ivec2 Xp = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    
    vec3 edgeColor = texture(edges, texCoord(Xp.x, Xp.y)).xyz;
    
	// Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).
    fragColor.rgb = finalColor.rgb;

    // If edge color is bright enough, use it. 
    if (edgeColor.x > 0.5) {
        fragColor.rgb = vec3(0.0);
    }


	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
