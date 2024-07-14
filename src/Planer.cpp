    #include "PlanarMap.h"
    #include <iostream>
    #include <GL/glew.h>

    void PlanarMap::addFace(const std::vector<glm::vec3>& polygon) {
        Face face;
        for (size_t i = 0; i < polygon.size(); ++i) {
            Edge edge;
            edge.start = polygon[i];
            edge.end = polygon[(i + 1) % polygon.size()];
            face.edges.push_back(edge);
        }
        faces.push_back(face);
    }void PlanarMap::generateOutlineStrokes(unsigned shader, unsigned textureID) {
        GLuint vao, vboVertices, vboTextureCoords; // Separate VBO for texture coordinates

        // Generate VAO and VBOs
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vboVertices);
        glGenBuffers(1, &vboTextureCoords);

        // Bind VAO and VBOs
        glBindVertexArray(vao);

        // Create vectors to store vertices and texture coordinates
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoords;

        // Populate the vertices and texture coordinates vectors
        for (const auto& face : faces) {
            for (const auto& edge : face.edges) {
                vertices.push_back(edge.start);
                vertices.push_back(edge.end);

                // Assuming you want to map the texture along the length of each line segment
                float length = glm::length(edge.end - edge.start);
                glm::vec2 texCoordStart(0.0f, 0.0f);
                glm::vec2 texCoordEnd(length, 0.0f);

                textureCoords.push_back(texCoordStart);
                textureCoords.push_back(texCoordEnd);
            }
        }

        // Buffer data for vertices
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Specify vertex attribute pointers for vertices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Buffer data for texture coordinates
        glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoords);
        glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), textureCoords.data(), GL_STATIC_DRAW);

        // Specify vertex attribute pointers for texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(1);

        // Unbind VAO and VBOs to prevent modification
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Draw lines with texture
        glUseProgram(shader); // Replace with your shader program
        glBindVertexArray(vao);

        // Activate the texture unit and bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Draw the entire buffer
        glDrawArrays(GL_LINES, 0, vertices.size());

        // Cleanup
        glDeleteBuffers(1, &vboVertices);
        glDeleteBuffers(1, &vboTextureCoords);
        glDeleteVertexArrays(1, &vao);
    }



    void PlanarMap::clear() {
        faces.clear();
    }
