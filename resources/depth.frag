#version 330

// Fragment shader

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:
in vec4 interpSurfPosition;
in vec3 interpSurfNormal;

// This is an out variable for the final color we want to render this fragment.
layout(location = 0) out vec4 fragColor;

uniform float near;
uniform float far;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    fragColor.rgb = vec3(depth);

    // Alternate non-linear depth
    //    fragColor.rgb = vec3((gl_FragCoord.z) * 2.0 - 1.0);

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
