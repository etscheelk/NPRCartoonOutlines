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
//    vec3 ambient = ambientLightIntensity * ambientReflectionCoeff; 
    vec3 ambient = ambientLightIntensity * color; 

//    vec3 diffuse = diffuseLightIntensity * diffuseReflectionCoeff * vec3(texture(diffuseRamp, vec2(NdotL, 0.0)));
    vec3 diffuse = diffuseLightIntensity * color * vec3(texture(diffuseRamp, vec2(NdotL, 0.0)));
	
//    vec3 specular = specularLightIntensity * specularReflectionCoeff * vec3(texture(specularRamp, vec2(pow(HdotN, specularExponent), 0.0)));
    vec3 specular = specularLightIntensity * color * vec3(texture(specularRamp, vec2(pow(HdotN, specularExponent), 0.0)));
    
    
    finalColor = ambient + diffuse + specular;
    
    ivec2 Xp = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    
    vec3 edgeColor = texture(edges, texCoord(Xp.x, Xp.y)).xyz;
    
	// Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).
    fragColor.rgb = finalColor.rgb;

    if (edgeColor.x > 0.5) {
        fragColor.rgb = vec3(0.0);
    }

    // Math Visualization Stuff. Please don't leave it enabled for bunny stuff
//    fragColor.rgb = vec3(0.0);
//    float plotWidth = 2.0;
//    float x = (Xp.x / windowWidth)  * plotWidth - plotWidth / 2.0;
//    float y = (Xp.y / windowHeight) * plotWidth - plotWidth / 2.0;

//    vec2 u = vec2(x, y);
//    vec2 v = vec2(0.5, 0.9);
//
//    // bray curtis
//    vec2 diff = abs(u - v);
//    vec2 sum = abs(u + v);
//
//    float dist = (diff.x + diff.y) / (sum.x + sum.y);
//    if (dist <= 1.0)
//        fragColor.rgb = vec3(1.0);
    
//    mapP(x, y, 2, vec3(1.0));

    
//    mapP(x, y, 0.5, vec3(1.0));
//
//    float p = 10000;
//    mapP(x, y, p, vec3(1.0));
//
//    p = 5;
//    mapP(x, y, p, vec3(1.0, 0.0, 0.0));
//
//    p = 3;
//    mapP(x, y, p, vec3(1.0, 0.7, 0.0));
//
//    p = 2.5;
//    mapP(x, y, p, vec3(1.0, 1.0, 0.0));
//
//    p = 2;
//    mapP(x, y, p, vec3(0.0, 0.5, 0.0));
//
//    p = 1.5;
//    mapP(x, y, p, vec3(0.0, 0.0, 1.0));
//
//    p = 1;
//    mapP(x, y, p, vec3(0.25, 0.0, 0.5));
//
//    p = 0.75;
//    mapP(x, y, p, vec3(0.9, 0.5, 0.9));
//
//    p = 0.5;
//    mapP(x, y, p, vec3(242/255.0, 218/255.0, 227/255.0));

    

//     fragColor.rgb = edgeColor;

    // https://gamedev.net/forums/topic/584076-glsl-edit-texture-and-save-it/4714863/
    // https://www.khronos.org/opengl/wiki/Framebuffer_Object 
//    fragColor.rgb = 0.5 * N + 0.5;

//    float depth = LinearizeDepth(gl_FragCoord.z) / far;
//    fragColor.rgb = vec3(depth);

    
  

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
