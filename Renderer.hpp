#ifndef _RENDERER_HPP
#define _RENDERER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct shape {
    glm::vec3 dimensions; // length, width, height of rectangular prism
    glm::mat4 transformation;
    glm::vec3 color; // 0-1 rgb
};

#endif /* Renderer.hpp */