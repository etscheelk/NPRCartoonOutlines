#version 330

// Fragment shader

// Textures
uniform sampler2D normalEdges;
uniform sampler2D depthEdges;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

uniform float windowWidth;
uniform float windowHeight;

float Knormal = 0.0125;
float Kdepth = 0.003;

// Test
//float Knormal = 0.005;
//float Kdepth = 0.005;

vec2 texCoord(float xPos, float yPos) {
    return vec2((xPos) / windowWidth, (yPos) / windowHeight);
}

float threshold(sampler2D tex, ivec2 Xp, float K, float divide) {
    float gmax = 0.0;
    float gmin = 999999;

    float[9]a;

    float A = texture(tex, texCoord(Xp.x-1, Xp.y+1)).x;         a[0] = A / divide;
    float B = texture(tex, texCoord(Xp.x,   Xp.y+1)).x;         a[1] = B / divide;
    float C = texture(tex, texCoord(Xp.x+1, Xp.y+1)).x;         a[2] = C / divide;

    float D = texture(tex, texCoord(Xp.x-1, Xp.y)).x;           a[3] = D / divide;
    float X = texture(tex, texCoord(Xp.x,   Xp.y)).x;           a[4] = X / divide;
    float E = texture(tex, texCoord(Xp.x+1, Xp.y)).x;           a[5] = E / divide;
    
    float F = texture(tex, texCoord(Xp.x-1, Xp.y-1)).x;         a[6] = F / divide;
    float G = texture(tex, texCoord(Xp.x,   Xp.y-1)).x;         a[7] = G / divide;
    float H = texture(tex, texCoord(Xp.x+1, Xp.y-1)).x;         a[8] = H / divide;

    for (int i = 0; i < 9; ++i) {
        float ai = a[i];
        
        if (ai > gmax) {
            gmax = ai;
        }

        if (ai < gmin) {
            gmin = ai;
        }
    }

    float p = min(pow((gmax - gmin) / K, 2), 1);

    return p;
}

void main() {
    // Start with black and then add lighting to the final color as we calculate it
	vec3 finalColor = vec3(0.5, 0.0, 0.0);
    
    
	// Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).

    ivec2 Xp = ivec2(gl_FragCoord.x, gl_FragCoord.y);

    float pNormal = threshold(normalEdges, Xp, Knormal, 16.0);
    float pDepth = threshold(depthEdges, Xp, Kdepth, 128.0);


    finalColor = vec3(pNormal);
    finalColor = vec3((pNormal + pDepth) / 1.0);
    

    // If the final added color isn't bright about, discard it
    if (finalColor.x < 0.75) {
        finalColor = vec3(0.0);
    }
    
  
    fragColor.rgb = finalColor.rgb;

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
