#ifndef PLANAR_MAP_H
#define PLANAR_MAP_H

#include <vector>
#include <glm/glm.hpp>

struct Edge {
    glm::vec3 start;
    glm::vec3 end;
};

struct Face {
    std::vector<Edge> edges;
};

class PlanarMap {
public:
    void addFace(const std::vector<glm::vec3>& polygon);
    void generateOutlineStrokes(unsigned shader, unsigned textureID);
    void clear();

private:
    std::vector<Face> faces;
};

#endif // PLANAR_MAP_H
