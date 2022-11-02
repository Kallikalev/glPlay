#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// for testing & printing purposes only
#include "glm/gtx/string_cast.hpp"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#include "Robot.cpp"
#include "Renderer.hpp"

class Simulation {
private:
    Robot myRobot;

    int moveDir1 = 1;
    int moveDir2 = 1;

public:
    Simulation() {
        myRobot = Robot();
    }

    void step(float deltaTime) {
        // myRobot.legs[0].segments[0].jointAngle += deltaTime / 4;

        if (!myRobot.setMotorAngle(0,1,myRobot.legs[0].segments[1].jointAngle + deltaTime * moveDir1)) {
            moveDir1 *= -1;
        }

        if (!myRobot.setMotorAngle(0,2,myRobot.legs[0].segments[2].jointAngle + deltaTime * moveDir2)) {
            moveDir2 *= -1;
        }

    }

    std::vector<shape> getShapes() {
        std::vector<shape> ret(/*1 + 6 * 3*/ 7); // one body shape, 6 legs, 3 shapes per leg

        // for each leg
        for (int i = 0; i < 1; i++) {
            Robot::leg l = myRobot.legs[i];
            int numSegments = 3;
            glm::mat4 segmentMatrices[numSegments];

            for (int j = 0; j < numSegments; j++) {

                segmentMatrices[j] = glm::mat4(1.0f);


                if (j != 0) { // no offset for first part
                    segmentMatrices[j] = segmentMatrices[j] * segmentMatrices[j - 1];
                    segmentMatrices[j] = glm::translate(segmentMatrices[j],l.segments[j-1].connectOffset);
                }

                segmentMatrices[i] = glm::rotate(segmentMatrices[j],l.segments[j].jointAngle,l.segments[j].jointAxis);


                shape newShape = {
                    l.segments[j].dimensions,
                    segmentMatrices[j] * glm::translate(glm::mat4(1.0f),l.segments[j].baseOffset),
                    glm::vec3((float)j/2.0f,1.0f,1.0f)
                };
                ret.push_back(newShape);
            }

        }



        return ret;
    }
};