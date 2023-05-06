#version 330

// Fragment shader

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:
in vec3 interpSurfNormal;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

void main() {
    vec3 N = normalize(interpSurfNormal);

    // https://gamedev.net/forums/topic/584076-glsl-edit-texture-and-save-it/4714863/
    // https://www.khronos.org/opengl/wiki/Framebuffer_Object 
    fragColor.rgb = 0.5 * N + 0.5;

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
