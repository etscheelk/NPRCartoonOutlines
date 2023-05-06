#ifndef SHADYBUNNY_H
#define SHADYBUNNY_H


#include <api/MinVR.h>
using namespace MinVR;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "TurntableManipulator.h"

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <BasicGraphics.h>
using namespace basicgraphics;

class ShadyBunny : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
	ShadyBunny(int argc, char** argv);
    virtual ~ShadyBunny();

    
    /** USER INTERFACE CALLBACKS **/
    virtual void onAnalogChange(const VRAnalogEvent &state);
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
	virtual void onCursorMove(const VRCursorEvent &state);
    
    /** RENDERING CALLBACKS **/
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);

    void generateFramebuffer(unsigned int& fb, std::shared_ptr<Texture>& tex, unsigned int& rb);

    void drawLight(GLSLProgram &shader);
    void renderPass(GLSLProgram &shader, const mat4 &model, const mat4 &view, const mat4 &projection, const vec3 &eyePosition);

    void setMaterialUniforms(GLSLProgram& shader);
    
    
private:
    double _lastTime;
    double _curFrameTime;
    
	virtual void reloadShaders();

    float _near = 0.1f;
    float _far = 20.0f;

	std::shared_ptr<Texture> _diffuseRamp;
	std::shared_ptr<Texture> _specularRamp;


	GLSLProgram _shader;

    unsigned int _rbo;

    // Normal Map
    GLSLProgram _normalShader;
    unsigned int _normalFB;
    std::shared_ptr<Texture> _normalTex;

    // Normal Map Edges
    GLSLProgram _normalEdgesShader;
    unsigned int _normalEdgesFB;
    std::shared_ptr<Texture> _normalEdgesTex;

    // Depth Map
    GLSLProgram _depthShader;
    unsigned int _depthFB;
    std::shared_ptr<Texture> _depthTex;

    // Depth Map Edges
    GLSLProgram _depthEdgesShader;
    unsigned int _depthEdgesFB;
    std::shared_ptr<Texture> _depthEdgesTex;

    // Edges Combined
    GLSLProgram _edgesShader;
    unsigned int _edgesFB;
    std::shared_ptr<Texture> _edgesTex;

    GLfloat _windowHeight;
    GLfloat _windowWidth;

	std::unique_ptr<Model> _modelMesh;
    std::unique_ptr<basicgraphics::Box> _box;
    std::unique_ptr<Sphere> _sphere;
	std::shared_ptr<TurntableManipulator> _turntable;

    std::unique_ptr<Model> _tree;

	glm::vec4 _lightPosition;
	bool _drawLightVector;
	float _diffuseOnOff;  // 1.0 when on, 0.0 when off
	float _specularOnOff; // 1.0 when on, 0.0 when off
	float _ambientOnOff;  // 1.0 when on, 0.0 when off

    unsigned short _displayInt = 0;
};


#endif
