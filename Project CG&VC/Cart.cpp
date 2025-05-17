#include "Cart.h"

// Constructor
Cart::Cart(RollerCoaster* coaster, float speed)
	: m_rollerCoaster(coaster), m_model(".\\models\\cart\\coaster-train-front.fbx"), m_shader(".\\CartShader.vert", ".\\CartShader.frag"), m_speed(speed), m_t(0.0f), m_currentCurveIndex(0) {

	InitializeBuffers();
	updatePositionAndDirection();
}

// this method updates on which bezier curve segment the cart is on
void Cart::Update(float deltaTime) {
    m_t += m_speed * deltaTime;

    // einde huidige curve segment? ==> volgende segment
    if (m_t > 1.0f) {
        m_t = 0.0f; // Reset t
        m_currentCurveIndex++; // volgende curve segment

        // Einde van de achtbaan? ==> begin opnieuw
        if (m_currentCurveIndex >= m_rollerCoaster->getCurves().size()) {
            m_currentCurveIndex = 0;
        }
    }

    updatePositionAndDirection();
}

// update the position and direction of the cart
void Cart::updatePositionAndDirection() {
	// current curve
	BezierCurve& currentCurve = (m_rollerCoaster->getCurves())[m_currentCurveIndex];

	// position on the curve
	m_position = currentCurve.GetPoint(m_t);

	// recalculate direction
	glm::vec3 nextPos = currentCurve.GetPoint(m_t + 0.01f);
	m_direction = glm::normalize(nextPos - m_position);

    // calculate the "up" vector
    glm::vec3 up(0.0f, 1.0f, 0.0f); // Assuming Y-axis is up
    glm::vec3 right = glm::normalize(glm::cross(up, m_direction));
    up = glm::normalize(glm::cross(m_direction, right));

    // Apply an offset to position the cart above the curve
    float heightOffset = 0.60f; // Adjust this value to control the height above the curve
    m_position += up * heightOffset;

}

// render the cart
void Cart::Render(const glm::mat4& projection, const glm::mat4& view, std::vector<PointLight> pointLights, glm::vec3 cameraPos) {
    // Calculate orientation matrix
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, m_direction));
    up = glm::cross(m_direction, right);

    rotation[0] = glm::vec4(right, 0.0f);
    rotation[1] = glm::vec4(up, 0.0f);
    rotation[2] = glm::vec4(-m_direction, 0.0f);

	// compute the model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position)
        * rotation
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));

	// set the shader uniforms
    m_shader.use();
    m_shader.setMat4("projection", projection);

    m_shader.setInt("numLights", pointLights.size());
    for (size_t i = 0; i < pointLights.size(); ++i) {
        std::string index = std::to_string(i);
        m_shader.setVec3("lights[" + index + "].position", pointLights[i].position);
        m_shader.setVec3("lights[" + index + "].color", pointLights[i].color);
        m_shader.setFloat("lights[" + index + "].constant", pointLights[i].constant);
        m_shader.setFloat("lights[" + index + "].linear", pointLights[i].linear);
        m_shader.setFloat("lights[" + index + "].quadratic", pointLights[i].quadratic);
    }

    m_shader.setVec3("viewPos", cameraPos);

    m_shader.setMat4("view", view);
    m_shader.setMat4("model", model);

    m_model.Draw(m_shader);
}

// initialize buffers for the cart
void Cart::InitializeBuffers() {
    const int sectorCount = 36; // longitude, more = smoother
    const int stackCount = 18;  // latitude, more = smoother
    const float radius = 0.5f;

    std::vector<float> vertices;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount; // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);
        float y = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount; // 0 to 2pi

            float x = xy * cosf(sectorAngle);
            float z = xy * sinf(sectorAngle);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // Normal (normalized position for a sphere)
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    // Indices for GL_TRIANGLE_STRIP
    std::vector<unsigned int> indices;
    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j <= sectorCount; ++j) {
            int first = i * (sectorCount + 1) + j;
            int second = (i + 1) * (sectorCount + 1) + j;
            indices.push_back(first);
            indices.push_back(second);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_indexCount = static_cast<unsigned int>(indices.size());
}