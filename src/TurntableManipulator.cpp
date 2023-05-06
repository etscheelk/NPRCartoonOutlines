#include "TurntableManipulator.h"

using namespace std;
using namespace glm;

namespace basicgraphics{

const double X_SCALE = 0.005;
const double Y_SCALE = 0.005;

TurntableManipulator::TurntableManipulator(double d, double a, double u) {
	distance = d;
	around = a;
	up = u;
    
    mouseDown = false;
}

void TurntableManipulator::bump(double ar, double u) {
	around += ar;
	up += u;
}
    
void TurntableManipulator::onButtonDown(const VRButtonEvent &event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.
    
    //std::cout << "ButtonDown: " << event.getName() << std::endl;
    // Dolly the camera closer or farther away from the earth
    
    string name = event.getName();
    if (name == "KbdUp_Down") {
        distance -= 0.01;
    }
    else if (name == "KbdDown_Down") {
        distance += 0.01;
    }
    
    // Rotate the earth when the user clicks and drags the mouse
    else if (name == "MouseBtnLeft_Down") {
        mouseDown = true;
    }
}

void TurntableManipulator::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.
    
    //std::cout << "ButtonUp: " << event.getName() << std::endl;
    if(event.getName() == "MouseBtnLeft_Up"){
        mouseDown = false;
    }
}

void TurntableManipulator::onCursorMove(const VRCursorEvent &event) {
    // This routine is called for all mouse move events. You can get the absolute position
    // or the relative position within the window scaled 0--1.
    
    //std::cout << "MouseMove: "<< event.getName() << " " << event.getPos()[0] << " " << event.getPos()[1] << std::endl;
    if (mouseDown){
        vec2 dxy = vec2(event.getPos()[0], event.getPos()[1]) - lastMousePos;
        if(length(dxy) != 0.0){
            around = around + dxy.x * X_SCALE;
            up = clamp(up + dxy.y * Y_SCALE, -pi<float>()/2 + 0.2, pi<float>()/2 - 0.2);
        }
        
    }
    lastMousePos = vec2(event.getPos()[0], event.getPos()[1]);
}

glm::vec3 TurntableManipulator::getPos() const {
    glm::vec3 direction(cos(around)*cos(up)*distance,
                  sin(up)*distance,
                  sin(around)*cos(up)*distance);
    return center + direction;
}

glm::mat4 TurntableManipulator::frame() const {
    glm::vec3 pos = getPos();
    glm::mat4 cf = glm::lookAt(pos, center, glm::vec3(0, 1, 0));
	return cf;
}


void TurntableManipulator::setCenterPosition(glm::vec3 position) {
	center = position;
}
}//namespace
