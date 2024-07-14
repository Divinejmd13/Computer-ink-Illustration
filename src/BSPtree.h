// BSPtree.h
#ifndef BSPTREE_H
#define BSPTREE_H
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class BSPtree {
public:
    BSPtree(const std::vector<glm::vec3>& polygon, const std::vector<glm::vec2>& texCoords);
    ~BSPtree();

    void render(const glm::mat4& modelTransform, unsigned textureID);
    std::vector<glm::vec3> getVertices() const;

private:
    struct BSPNode {
        std::vector<glm::vec3> polygon;
        std::vector<glm::vec2> texCoords;

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint TBO = 0;

        BSPNode* front = nullptr;
        BSPNode* back = nullptr;
    };

    BSPNode* root;

    void destroyTree(BSPNode* node);
    void partition(BSPNode* node,const std::vector<glm::vec2>& texCoords);
    void renderNode(BSPNode* node, const glm::mat4& modelTransform, unsigned textureID);
    void collectVertices(const BSPNode* node, std::vector<glm::vec3>& vertices) const;
};

#endif // BSPTREE_H