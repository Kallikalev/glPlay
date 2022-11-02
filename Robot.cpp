#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Robot {
private:
    float PI = glm::pi<float>();
public:
    // each part is represented as a line between it's origin and joint at the end/contact point
    // (0,0,0) is origin
    struct legPart {
        glm::vec3 jointAxis; // axis of the previous motor joint
        float minJointAngle;
        float maxJointAngle;
        float jointAngle;
        glm::vec3 connectOffset; // where the next part connects to this one
        glm::vec3 dimensions; // the shape of the part
        glm::vec3 baseOffset; // the offset needed to make the model dimensions line up with the joints
    };
    // a leg is just a series of parts
    struct leg {
        legPart segments[3];
        glm::vec3 baseRotationAxis;
        float baseRotationAngle;
        glm::vec3 baseOffset;
    };

    leg legs[1];

    Robot() {
        legPart p0 = {glm::vec3(0,1,0),-PI,PI,0,glm::vec3(1.0f,0,0),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.5f,0.0f,0.0f)};
        legPart p1 = {glm::vec3(0,0,1),-PI/2,PI/2,0,glm::vec3(3.0f,0,0),glm::vec3(3.0f,1.0f,1.0f),glm::vec3(1.5f,0.0f,0.0f)};
        legPart p2 = {glm::vec3(0,0,1),-PI/4,PI/4,0,glm::vec3(3.0f,0,0),glm::vec3(3.0f,1.0f,1.0f),glm::vec3(1.5f,0.0f,0.0f)};

        leg l0 = {{p0,p1,p2},glm::vec3(0,1,0),0,glm::vec3(0,0,0)};

        legs[0] = l0;
    }

    
    leg getLeg(int index) {
        return legs[index];
    }

    // returns true if it was successful, false if out of bounds
    bool setMotorAngle(int legIndex, int motorIndex, float newAngle) {
        if (newAngle > legs[legIndex].segments[motorIndex].maxJointAngle) {
            legs[legIndex].segments[motorIndex].jointAngle = legs[legIndex].segments[motorIndex].maxJointAngle;
            return false;
        }
        if (newAngle < legs[legIndex].segments[motorIndex].minJointAngle) {
            legs[legIndex].segments[motorIndex].jointAngle = legs[legIndex].segments[motorIndex].minJointAngle;
            return false;
        }
        legs[legIndex].segments[motorIndex].jointAngle = newAngle;
        return true;
    }

};