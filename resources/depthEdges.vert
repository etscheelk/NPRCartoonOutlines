#version 330

// Vertex shader

// INPUT: from the C++ program


// These are "global" variables that are set with a specific value within the application code:
uniform mat4 projection_mat, view_mat, model_mat;
uniform mat3 normal_mat;

// These variables are automatically assigned the value of each vertex and cooresponding normal and texcoord
// as they pass through the rendering pipeline. The layout locations are based on how the VAO was organized
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

// OUTPUT: to the fragment shader

// Position of the current point on the surface, interpolated across the surface.
out vec4 interpSurfPosition;

// Normal of the current point on the surface, interpolated across the surface.
out vec3 interpSurfNormal;

void main(void)
{
    // vertex_position is a variable that holds the 3D position of the current vertex.  We want to
    // pass this position on to the fragment shader because we'll need it to calculate the lighting.
    // We're also going to do one matrix multiplication at this stage in order to convert from object to world coordinates
	interpSurfPosition = model_mat * vec4 (vertex_position, 1.0);
    
    // We also need the normal to calculate lighting.  So, we will similarly pass it on to the fragment
    // program as an "out" variable, and we'll do the same type of matrix multiplication.  However,
    // it turns out you have to use a slightly different matrix for normals because they transform a
    // bit differently than points.
    interpSurfNormal = normal_mat * vertex_normal;

    // This is the last line of almost every vertex shader program.  We don't need this for our lighting
    // calculations, but it is required by OpenGl.  Whereas a fragment program must output a color
    // as its final result, a vertex program must output a vertex position that has been projected into the
    // 2D screen space as its final result.  The line below does this.  This is exactly what OpenGL
    // would do by default for us if we didn't write our own custom vertex program.
    gl_Position = projection_mat * view_mat * interpSurfPosition;
}
