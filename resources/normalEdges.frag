#version 330

// Fragment shader

// Textures
uniform sampler2D normal;

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform float windowWidth;
uniform float windowHeight;

vec2 texCoord(float xPos, float yPos) {
    return vec2((xPos) / windowWidth, (yPos) / windowHeight);
}

void main() {
    // Start with black and then add lighting to the final color as we calculate it
	vec3 finalColor = vec3(0.0, 0.0, 0.0);

    // https://gamedev.net/forums/topic/584076-glsl-edit-texture-and-save-it/4714863/
    // https://www.khronos.org/opengl/wiki/Framebuffer_Object 

    // Alternate coloring for testing 
    // fragColor.rgb = (0.5 * N + 0.5) * 0.5 + vec3(0.0, 0.0, 0.5);

    // Alternative method for silhouette drawing. 
    // if (dot(N, -L) < 0.2 && dot(N, normalize(interpSurfPosition.xyz - eye_world)) > -0.2) {
    //      fragColor.rgb = vec3(1.0);
    // }

    ivec2 Xp = ivec2(gl_FragCoord.x, gl_FragCoord.y);

    vec3 A = vec3(texture(normal, texCoord(Xp.x-1, Xp.y+1)));
    vec3 B = vec3(texture(normal, texCoord(Xp.x, Xp.y+1)));
    vec3 C = vec3(texture(normal, texCoord(Xp.x+1, Xp.y+1)));

    vec3 D = vec3(texture(normal, texCoord(Xp.x-1, Xp.y)));
    vec3 X = vec3(texture(normal, texCoord(Xp.x, Xp.y)));
    vec3 E = vec3(texture(normal, texCoord(Xp.x+1, Xp.y)));

    vec3 F = vec3(texture(normal, texCoord(Xp.x-1, Xp.y-1)));
    vec3 G = vec3(texture(normal, texCoord(Xp.x, Xp.y-1)));
    vec3 H = vec3(texture(normal, texCoord(Xp.x+1, Xp.y-1)));

    float gx = (abs(A.x-X.x) + 2 * abs(B.x-X.x) + abs(C.x-X.x) + 2 * abs(D.x-X.x) + 2 * abs(E.x-X.x) + abs(F.x-X.x) + 2 * abs(G.x-X.x) + abs(H.x-X.x)) / 8.0;
    float gy = (abs(A.y-X.y) + 2 * abs(B.y-X.y) + abs(C.y-X.y) + 2 * abs(D.y-X.y) + 2 * abs(E.y-X.y) + abs(F.y-X.y) + 2 * abs(G.y-X.y) + abs(H.y-X.y)) / 8.0;
    float gz = (abs(A.z-X.z) + 2 * abs(B.z-X.z) + abs(C.z-X.z) + 2 * abs(D.z-X.z) + 2 * abs(E.z-X.z) + abs(F.z-X.z) + 2 * abs(G.z-X.z) + abs(H.z-X.z)) / 8.0;
    
    // Multiplied by 16 for visibility. REMEMBER TO DIVIDE OUT IN PROCESSING
    fragColor.rgb = vec3(16 * (gx + gy + gz) / 3.0);
//    fragColor.rgb = 16 * vec3(gx, gy, gz); 

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
