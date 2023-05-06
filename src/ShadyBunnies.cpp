#include "ShadyBunnies.h"
using namespace basicgraphics;

#include <config/VRDataIndex.h>

ShadyBunny::ShadyBunny(int argc, char** argv) : VRApp(argc, argv)
{

    _turntable.reset(new TurntableManipulator(3, 0.3, 0.5)); 
    _turntable->setCenterPosition(vec3(-0.3, 0.8, 0));  

    _drawLightVector = true;
    _ambientOnOff = 1.0;
    _diffuseOnOff = 1.0;
    _specularOnOff = 1.0;
    _lastTime = 0.0;
    _curFrameTime = 0.0;

}

ShadyBunny::~ShadyBunny()
{
	shutdown(); 
}

void ShadyBunny::onAnalogChange(const VRAnalogEvent &state) {
    // This routine is called for all Analog_Change events.  Check event->getName()   
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue(). 
    
	if (state.getName() == "FrameStart") {
		_lastTime = _curFrameTime;
		_curFrameTime = state.getValue();
		// Calculate model matrix based on time
		float scaledTime = 0.5f*_curFrameTime;

		// Make the light orbit around the bunny so we can see the lighting change in response to the light position
		float radius = 5.0;
		_lightPosition = vec4(cos(scaledTime*0.6)*sin(scaledTime*0.5)*radius,
			cos(scaledTime*0.3)*sin(scaledTime*0.2)*radius,
			cos(scaledTime*0.1)*sin(scaledTime*0.4)*radius,
			1.0);
	}
}


void ShadyBunny::onButtonDown(const VRButtonEvent &event) {
    _turntable->onButtonDown(event);
    string name = event.getName();
    if (name == "KbdL_Down") {
        _drawLightVector = !_drawLightVector; // Toggle drawing the vector to the light on or off
    }
    // Press S to toggle specular lighting on/off
    else if (name == "KbdS_Down") {      
        if (_specularOnOff == 1.0) {
            _specularOnOff = 0.0;
        }
        else {
            _specularOnOff = 1.0;
        }
    }
    // Press D to toggle diffuse lighting on/off
    else if (name == "KbdD_Down") {
        if (_diffuseOnOff == 1.0) {
            _diffuseOnOff = 0.0;
        }
        else {
            _diffuseOnOff = 1.0;
        }
    }
    // Press A to toggle ambient lighting on/off
    else if (name == "KbdA_Down") {
        if (_ambientOnOff == 1.0) {
            _ambientOnOff = 0.0;
        }
        else {
            _ambientOnOff = 1.0;
        }
    }

	else if (name == "KbdF_Down") {
		_displayInt = (_displayInt + 1) % 6;
		std::cout << _displayInt << std::endl;
	}
}


void ShadyBunny::onButtonUp(const VRButtonEvent &event) {
    _turntable->onButtonUp(event);
}


void ShadyBunny::onCursorMove(const VRCursorEvent &event) {
    _turntable->onCursorMove(event);
}

void ShadyBunny::reloadShaders()
{
	// Compile Normal Map Shader
	_normalShader.compileShader("normal.vert", GLSLShader::VERTEX);
	_normalShader.compileShader("normal.frag", GLSLShader::FRAGMENT);
	_normalShader.link();

	// Compile Normal Map Edges Shader
	_normalEdgesShader.compileShader("normalEdges.vert", GLSLShader::VERTEX);
	_normalEdgesShader.compileShader("normalEdges.frag", GLSLShader::FRAGMENT);
	_normalEdgesShader.link();

	// Compile Depth Shader
	_depthShader.compileShader("depth.vert", GLSLShader::VERTEX);
	_depthShader.compileShader("depth.frag", GLSLShader::FRAGMENT);
	_depthShader.link();

	// Compile Depth Edges Shader
	_depthEdgesShader.compileShader("depthEdges.vert", GLSLShader::VERTEX);
	_depthEdgesShader.compileShader("depthEdges.frag", GLSLShader::FRAGMENT);
	_depthEdgesShader.link();

	// Compile Edges Threshold Shader
	_edgesShader.compileShader("edgesComb.vert", GLSLShader::VERTEX);
	_edgesShader.compileShader("edgesComb.frag", GLSLShader::FRAGMENT);
	_edgesShader.link();

	// Compile Regular Shader
	_shader.compileShader("BlinnPhong.vert", GLSLShader::VERTEX);
	_shader.compileShader("BlinnPhong.frag", GLSLShader::FRAGMENT);
	_shader.link();
}
    
void ShadyBunny::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame when in stereo mode.
    
    if (renderState.isInitialRenderCall()) {

#ifndef __APPLE__
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cout << "Error initializing GLEW." << std::endl;
		}
#endif     


        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();
		// This loads the model from a file and initializes an instance of the model class to store it
		_modelMesh.reset(new Model("bunny.obj", 1.0, vec4(1.0)));
		_box.reset(new basicgraphics::Box(vec3(-0.5, -0.75, -0.5), vec3(0.5, 0.25, 0.5), vec4(0.7, 0.2, 0.3, 1.0)));
		_sphere.reset(new Sphere(vec3(0.75, 0.75, 0.75), 10, vec4(0.2, 0.2, 0.7, 1.0)));

		// https://free3d.com/3d-model/tree-74556.html
		//_tree.reset(new Model("Tree.obj", 1.0, vec4(1.0)));
		

		_windowHeight = renderState.index().getValue("FramebufferHeight");
		_windowWidth = renderState.index().getValue("FramebufferWidth");     


		// Frame Buffers -- Multiple Render Passes
		/* 
			Do you want to do multiple render passes?
			Bind a framebuffer fb with `glBindFramebuffer(GL_FRAMEBUFFER, fb)`, then any render
			calls will render to fb and the texture tex associated with fb. 
			Call `glBindFramebuffer(GL_FRAMEBUFFER, 0)` when you're done / when you want that render pass drawn to screen.
		*/


		// Attach the same renderbuffer object to each framebuffer, I don't really care about it. I guess I'd get some stencil for free too?
		glGenRenderbuffers(1, &_rbo);  
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _windowWidth, _windowHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Normal Map
		_normalTex = Texture::createEmpty("normalMap", _windowWidth, _windowHeight, 1, 0, false, GL_TEXTURE_2D, GL_RGBA);
		generateFramebuffer(_normalFB, _normalTex, _rbo);
		
		// Normal Map Edges
		_normalEdgesTex = Texture::createEmpty("normalEdges", _windowWidth, _windowHeight, 1, 0, false, GL_TEXTURE_2D, GL_RGBA);
		generateFramebuffer(_normalEdgesFB, _normalEdgesTex, _rbo);

		// Depth Map
		_depthTex = Texture::createEmpty("depthMap", _windowWidth, _windowHeight, 1, 0, false, GL_TEXTURE_2D, GL_RGBA);
		generateFramebuffer(_depthFB, _depthTex, _rbo);
		
		// Depth Map Edges
		_depthEdgesTex = Texture::createEmpty("depthEdges", _windowWidth, _windowHeight, 1, 0, false, GL_TEXTURE_2D, GL_RGBA);
		generateFramebuffer(_depthEdgesFB, _depthEdgesTex, _rbo);

		_edgesTex = Texture::createEmpty("edgesTex", _windowWidth, _windowHeight, 1, 0, false, GL_TEXTURE_2D, GL_RGBA);
		generateFramebuffer(_edgesFB, _edgesTex, _rbo);
		
		/*if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;*/
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);


		// Diffuse Ramp
		//_diffuseRamp = Texture::create2DTextureFromFile("lightingNormal.jpg");  
		_diffuseRamp = Texture::create2DTextureFromFile("lightingNormalStep.jpg");            
		//_diffuseRamp = Texture::create2DTextureFromFile("lightingToon.jpg");  
		//_diffuseRamp = Texture::create2DTextureFromFile("lightingFunky.jpg");
		_diffuseRamp->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_diffuseRamp->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Specular Ramp
		//_specularRamp = Texture::create2DTextureFromFile("lightingNormal.jpg");
		//_specularRamp = Texture::create2DTextureFromFile("lightingNormalStep.jpg");
		//_specularRamp = Texture::create2DTextureFromFile("lightingToon.jpg"); 
		_specularRamp = Texture::create2DTextureFromFile("lightingFunky.jpg");
		_specularRamp->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
		_specularRamp->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }


}

void ShadyBunny::generateFramebuffer(unsigned int &fb, std::shared_ptr<Texture> &tex, unsigned int &rb) {
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadyBunny::renderPass(GLSLProgram &shader, const mat4 &model, const mat4 &view, const mat4 &projection, const vec3 &eyePosition) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	_normalTex->bind(0);
	_normalEdgesTex->bind(1);
	_depthTex->bind(2);
	_depthEdgesTex->bind(3);
	_edgesTex->bind(4);
	_diffuseRamp->bind(5);
	_specularRamp->bind(6);

	shader.use();  
	shader.setUniform("normal", 0);
	shader.setUniform("normalEdges", 1);
	shader.setUniform("depth", 2);
	shader.setUniform("depthEdges", 3);
	shader.setUniform("edges", 4); 
	shader.setUniform("diffuseRamp", 5); 
	shader.setUniform("specularRamp", 6);       

	shader.setUniform("view_mat", view); 
	shader.setUniform("projection_mat", projection);

	shader.setUniform("model_mat", model);
	shader.setUniform("normal_mat", mat3(transpose(inverse(model))));

	shader.setUniform("eye_world", eyePosition); 
	shader.setUniform("lightPosition", _lightPosition);

	setMaterialUniforms(shader);
	
	shader.setUniform("color", vec3(0.3, 0.9, 0.4));
	_modelMesh->draw(shader);

	shader.setUniform("color", vec3(0.9, 0.2, 0.1));
	_box->draw(shader, model); 

	glFrontFace(GL_CW);
	shader.setUniform("color", vec3(0x7c/255.0, 0xc0/255.0, 0xd8/255.0));
	_sphere->draw(shader, model); 
	glFrontFace(GL_CCW);

	//_tree->draw(shader);

	shader.setUniform("color", vec3(1.0));
	drawLight(shader);
} 

void ShadyBunny::setMaterialUniforms(GLSLProgram &shader) {

	// Properties of the material the model is made out of (the "K" terms in the equations discussed in class)
	// These values should make the model look like it is made out of a metal, like brass
	/*vec3 ambientReflectionCoeff = vec3(0.4125, 0.275, 0.0375);
	vec3 diffuseReflectionCoeff = vec3(0.78, 0.57, 0.11);
	vec3 specularReflectionCoeff = vec3(0.99, 0.94, 0.80); 
	float specularExponent = 27.9;*/ 

	// For toon shading, you want all the color to come from the texture, so you can just use a white bunny like this:
	vec3 ambientReflectionCoeff = vec3(1, 1, 1);
	vec3 diffuseReflectionCoeff = vec3(1, 1, 1); 
	vec3 specularReflectionCoeff = vec3(1, 1, 1); 
	float specularExponent = 50.0;

	

	shader.setUniform("ambientReflectionCoeff", ambientReflectionCoeff);
	shader.setUniform("diffuseReflectionCoeff", diffuseReflectionCoeff);
	shader.setUniform("specularReflectionCoeff", specularReflectionCoeff);
	shader.setUniform("specularExponent", specularExponent);

	// Properties of the light source (the "I" terms in the equations discussed in class)
	// These values are for a white light so the r,g,b intensities are all the same
	// Note: lightPosition is another important property of the light; it is set at the top of the method
	vec3 ambientLightIntensity = vec3(0.4, 0.4, 0.4);
	vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
	vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);   


	// Multiply these light intensities by the OnOff terms below to turn each component on the lighting on/off based on keystrokes
	ambientLightIntensity *= _ambientOnOff;
	diffuseLightIntensity *= _diffuseOnOff;
	specularLightIntensity *= _specularOnOff;

	shader.setUniform("ambientLightIntensity", ambientLightIntensity);
	shader.setUniform("diffuseLightIntensity", diffuseLightIntensity);
	shader.setUniform("specularLightIntensity", specularLightIntensity);
	 
	shader.setUniform("near", _near);
	shader.setUniform("far", _far); 

	shader.setUniform("windowWidth", _windowWidth);
	shader.setUniform("windowHeight", _windowHeight);
}

void ShadyBunny::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye.  This is the place to actually
    // draw the scene.
	                                               
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);  
    
	// Setup the camera with a good initial position and view direction to see the table
	glm::mat4 model(1.0);

    glm::mat4 view = _turntable->frame();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), _windowWidth / _windowHeight, _near, _far);
	
	vec3 eyePosition = _turntable->getPos();    
	
	// Normal
	// Render with the normal shader and save to normal frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _normalFB);
	renderPass(_normalShader, model, view, projection, eyePosition);

	// Normal Edges
	// Perform a convolution on the normal map to get edges
	glBindFramebuffer(GL_FRAMEBUFFER, _normalEdgesFB);
	renderPass(_normalEdgesShader, model, view, projection, eyePosition); 

	// Depth Map
	// Render with depth shader and save to depth frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _depthFB);
	renderPass(_depthShader, model, view, projection, eyePosition);      
	
	// Depth Edges
	// Perform a convolution on the depth map to get edges
	glBindFramebuffer(GL_FRAMEBUFFER, _depthEdgesFB);         
	renderPass(_depthEdgesShader, model, view, projection, eyePosition);

	// Edges Threshold and Combine
	glBindFramebuffer(GL_FRAMEBUFFER, _edgesFB); 
	renderPass(_edgesShader, model, view, projection, eyePosition);

	// Regular Draw
	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   
	if (_displayInt == 0) {
		renderPass(_shader, model, view, projection, eyePosition);            
	}

	else if (_displayInt == 1) {
		renderPass(_normalShader, model, view, projection, eyePosition);
	}

	else if (_displayInt == 2) {
		renderPass(_normalEdgesShader, model, view, projection, eyePosition);
	} 

	else if (_displayInt == 3) {
		renderPass(_depthShader, model, view, projection, eyePosition);
	}

	else if (_displayInt == 4) {
		renderPass(_depthEdgesShader, model, view, projection, eyePosition);      
	} 

	else {
		renderPass(_edgesShader, model, view, projection, eyePosition);                      
	}
}

void ShadyBunny::drawLight(GLSLProgram &shader) {
	// For debugging purposes, let's draw a sphere to reprsent each "light bulb" in the scene, that way
	// we can make sure the lighting on the bunny makes sense given the position of each light source.
	Sphere s(vec3(_lightPosition), 0.1f, vec4(1.0f, 1.0f, 0.0f, 1.0f));
	s.draw(shader, glm::mat4(1.0));

	// Another useful aid for debugging: draw vectors to the light sources
	if (_drawLightVector) {
		Cylinder l(vec3(-0.3, 0.8, 0), vec3(_lightPosition), 0.01f, vec4(1.0f, 1.0f, 0.0f, 1.0f));
		l.draw(shader, glm::mat4(1.0));
	}
}
