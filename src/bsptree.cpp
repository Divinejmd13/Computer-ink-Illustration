// BSPtree.cpp
#include "BSPtree.h"
#include <iostream>
#include <algorithm>

BSPtree::BSPtree(const std::vector<glm::vec3>& polygon, const std::vector<glm::vec2>& texCoords) {
    root = new BSPNode;
    root->polygon = polygon;
    root->texCoords = texCoords;
    partition(root, texCoords);
}

BSPtree::~BSPtree() {
    destroyTree(root);
}

void BSPtree::destroyTree(BSPNode* node) {
    if (node) {
        destroyTree(node->front);
        destroyTree(node->back);
        glDeleteVertexArrays(1, &node->VAO);
        glDeleteBuffers(1, &node->VBO);
        glDeleteBuffers(1, &node->TBO);
        delete node;
    }
}

void BSPtree::render(const glm::mat4& modelTransform, unsigned textureID) {
    renderNode(root, modelTransform, textureID);
}
void BSPtree::renderNode(BSPNode* node, const glm::mat4& modelTransform, unsigned textureID) {
    if (node) {
        if (node->VAO == 0) {
            glGenVertexArrays(1, &node->VAO);
            glGenBuffers(1, &node->VBO);
            glGenBuffers(1, &node->TBO);

            glBindVertexArray(node->VAO);

            // Transform vertices by model matrix
            std::vector<glm::vec3> transformedVertices;
            transformedVertices.reserve(node->polygon.size());
            for (const glm::vec3& vertex : node->polygon) {
                glm::vec4 transformedVertex = modelTransform * glm::vec4(vertex, 1.0f);
                transformedVertices.push_back(glm::vec3(transformedVertex));
            }

            glBindBuffer(GL_ARRAY_BUFFER, node->VBO);
            glBufferData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(glm::vec3), transformedVertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, node->TBO);
            glBufferData(GL_ARRAY_BUFFER, node->texCoords.size() * sizeof(glm::vec2), node->texCoords.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }

        glBindVertexArray(node->VAO);

        // Bind the texture before drawing
        glBindTexture(GL_TEXTURE_2D, textureID);

        glDrawArrays(GL_TRIANGLE_FAN, 0, node->polygon.size());

        glBindVertexArray(0);

        // Recursively render child nodes
        renderNode(node->front, modelTransform, textureID);
        renderNode(node->back, modelTransform, textureID);
    }
}

std::vector<glm::vec3> BSPtree::getVertices() const {
    std::vector<glm::vec3> allVertices;
    collectVertices(root, allVertices);
    return allVertices;
}

void BSPtree::collectVertices(const BSPNode* node, std::vector<glm::vec3>& vertices) const {
    if (node) {
        vertices.insert(vertices.end(), node->polygon.begin(), node->polygon.end());
        collectVertices(node->front, vertices);
        collectVertices(node->back, vertices);
    }
}
void BSPtree::partition(BSPNode* node, const std::vector<glm::vec2>& texCoords) {
    if (node && node->polygon.size() > 3) {
        // Choose a splitting plane (you can customize this based on your needs)
        glm::vec3 planeNormal = glm::normalize(glm::cross(node->polygon[1] - node->polygon[0], node->polygon[2] - node->polygon[0]));
        glm::vec3 planePoint = node->polygon[0];

        std::vector<glm::vec3> frontPolygon;
        std::vector<glm::vec3> backPolygon;

        std::vector<glm::vec2> frontTexCoords;
        std::vector<glm::vec2> backTexCoords;

        // Classify each vertex of the polygon
        for (size_t i = 0; i < node->polygon.size(); ++i) {
            float distance = glm::dot(planeNormal, node->polygon[i] - planePoint);
            if (distance > 0) {
                frontPolygon.push_back(node->polygon[i]);
                frontTexCoords.push_back(texCoords[i]);
            } else {
                backPolygon.push_back(node->polygon[i]);
                backTexCoords.push_back(texCoords[i]);
            }
        }

        // Check if the split is meaningful
        if (!frontPolygon.empty() && !backPolygon.empty()) {
            // Ensure winding order for frontPolygon
            if (glm::dot(glm::cross(frontPolygon[1] - frontPolygon[0], frontPolygon[2] - frontPolygon[0]), planeNormal) < 0) {
                std::reverse(frontPolygon.begin(), frontPolygon.end());
                std::reverse(frontTexCoords.begin(), frontTexCoords.end());
            }

            // Ensure winding order for backPolygon
            if (glm::dot(glm::cross(backPolygon[1] - backPolygon[0], backPolygon[2] - backPolygon[0]), planeNormal) < 0) {
                std::reverse(backPolygon.begin(), backPolygon.end());
                std::reverse(backTexCoords.begin(), backTexCoords.end());
            }

            // Create front and back child nodes
            node->front = new BSPNode;
            node->back = new BSPNode;

            // Assign the split polygons and texture coordinates to the child nodes
            node->front->polygon = frontPolygon;
            node->back->polygon = backPolygon;

            node->front->texCoords = frontTexCoords;
            node->back->texCoords = backTexCoords;

            // Recursively partition the child nodes
            partition(node->front, frontTexCoords);
            partition(node->back, backTexCoords);
        }
    }
}
