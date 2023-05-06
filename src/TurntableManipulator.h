#ifndef TurntableManipulator_h
#define TurntableManipulator_h

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include <api/MinVR.h>
using namespace MinVR;

namespace basicgraphics{
    
class TurntableManipulator {
public:
	TurntableManipulator(double d=1.5, double a=0.3, double u=0.5);
    virtual glm::mat4 frame() const;
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
    virtual void onCursorMove(const VRCursorEvent &state);
    void setCenterPosition(glm::vec3 position);
	void bump(double ar, double u);
    glm::vec3 getPos() const;

protected:
	double around;
	double up;
	double distance;
    glm::vec3 center;
    
    bool mouseDown;
    glm::vec2 lastMousePos;
};
}

#endif
