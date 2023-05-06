#version 330

// Fragment shader

// Textures
uniform sampler2D depth;

uniform float windowWidth;
uniform float windowHeight;

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:
in vec4 interpSurfPosition;
in vec3 interpSurfNormal;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

vec2 texCoord(float xPos, float yPos) {
    return vec2((xPos) / windowWidth, (yPos) / windowHeight);
}

void main() {
    
    ivec2 Xp = ivec2(gl_FragCoord.x, gl_FragCoord.y);

    // Sobel Filter convolution
    float A = vec3(texture(depth, texCoord(Xp.x-1, Xp.y+1))).x;
    float B = vec3(texture(depth, texCoord(Xp.x, Xp.y+1))).x;
    float C = vec3(texture(depth, texCoord(Xp.x+1, Xp.y+1))).x;

    float D = vec3(texture(depth, texCoord(Xp.x-1, Xp.y))).x;
    float X = vec3(texture(depth, texCoord(Xp.x, Xp.y))).x;
    float E = vec3(texture(depth, texCoord(Xp.x+1, Xp.y))).x;

    float F = vec3(texture(depth, texCoord(Xp.x-1, Xp.y-1))).x;
    float G = vec3(texture(depth, texCoord(Xp.x, Xp.y-1))).x;
    float H = vec3(texture(depth, texCoord(Xp.x+1, Xp.y-1))).x;

    float g = (abs(A-X) + 2 * abs(B-X) + abs(C-X) + 2 * abs(D-X) + 2 * abs(E-X) + abs(F-X) + 2 * abs(G-X) + abs(H-X)) / 8.0;

    // visibility. Divide this out in the combine process
    g *= 128;
    

    // Default color of a bit blue
	vec3 finalColor = vec3(0.0, 0.2, 0.5);
    


    finalColor = vec3(g);

    
	// Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).
    fragColor.rgb = finalColor.rgb;
  

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
