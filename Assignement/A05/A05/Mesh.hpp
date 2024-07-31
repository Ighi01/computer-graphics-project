void MakeSquare(float size, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
	
    vertices = { {-size/2.0f,0.0f,-size/2.0f} , {-size/2.0f,0.0f, size/2.0f} , { size/2.0f,0.0f,-size/2.0f} , { size/2.0f,0.0f, size/2.0f} };
	indices = { 0, 1, 2, 1, 3, 2 };
}

void MakeCube(float size, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices) {
    
    vertices = { {-size / 2.0f, -size / 2.0f, -size / 2.0f} , {-size / 2.0f, -size / 2.0f, size / 2.0f} , {-size / 2.0f, size / 2.0f, -size / 2.0f} , {-size / 2.0f, size / 2.0f, size / 2.0f} , {size / 2.0f, -size / 2.0f, -size / 2.0f} , {size / 2.0f, -size / 2.0f, size / 2.0f} , {size / 2.0f, size / 2.0f, -size / 2.0f} , {size / 2.0f, size / 2.0f, size / 2.0f} };
    indices = { 0, 1, 3, 0, 3, 2, 4, 6, 7, 4, 7, 5, 0, 4, 5, 0, 5, 1, 2, 3, 7, 2, 7, 6, 0, 2, 6, 0, 6, 4, 1, 5, 7, 1, 7, 3 };
}

void MakeCylinder(float radius, float height, int slices, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices) {

    for (int i = 0; i < slices; ++i) {
        float angle = 2.0f * M_PI / (float)(slices) * (float)(i);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(glm::vec3(x, height / 2.0f, z));
    }

    for (int i = 0; i < slices; ++i) {
        float angle = 2.0f * M_PI / (float)(slices) * (float)(i);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(glm::vec3(x, -height / 2.0f, z));
    }

    for (int i = 0; i < slices; ++i) {
        int nextIndex = (i + 1) % slices;
        indices.push_back(i);
        indices.push_back(nextIndex);
        indices.push_back(i + slices);
        indices.push_back(nextIndex);
        indices.push_back(nextIndex + slices);
        indices.push_back(i + slices);
    }

    for (int i = 0; i < slices - 1; ++i) {
        indices.push_back(0);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
    }

    for (int i = slices + 1; i < 2 * slices - 1; ++i) {
        indices.push_back(slices);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

void MakeCone(float radius, float height, int slices, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices) {

    for (int i = 0; i < slices; ++i) {
        float angle = 2.0f * M_PI / (float)(slices) * (float)(i);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(glm::vec3(x, -height / 2.0f, z));
    }

    vertices.push_back(glm::vec3(0.0f, height / 2.0f, 0.0f));

    for (int i = 0; i < slices; ++i) {
        int nextI = (i + 1) % slices;
        indices.push_back(slices);
        indices.push_back(nextI);
        indices.push_back(i);
    }

    for (int i = 1; i < slices - 1 ; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}


void MakeSphere(float radius, int rings, int slices, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices) {

    for (int i = 0; i <= rings; ++i) {
        float phi = M_PI * (float)(i) / (float)(rings);

        for (int j = 0; j < slices; ++j) {
            float theta = 2.0f * M_PI * (float)(j) / (float)(slices);

            float x = radius * cos(theta) * sin(phi);
            float y = radius * cos(phi);
            float z = radius * sin(theta) * sin(phi);

            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    for (int r = 0; r < rings; ++r) {
        for (int i = 0; i < slices; ++i) {
            int nextR = (r + 1) % (rings + 1);
            int nextI = (i + 1) % slices;

            indices.push_back(r * slices + i);
            indices.push_back(r * slices + nextI);
            indices.push_back(nextR * slices + i);

            indices.push_back(r * slices + nextI);
            indices.push_back(nextR * slices + nextI);
            indices.push_back(nextR * slices + i);
        }
    }
}

