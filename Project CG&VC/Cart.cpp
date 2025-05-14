#include "Cart.h"

// Constructor
Cart::Cart(RollerCoaster* coaster, float speed)
	: m_rollerCoaster(coaster), m_shader(".\\Cart.vert", ".\\Cart.frag"), m_speed(speed), m_t(0.0f), m_currentCurveIndex(0) {

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
    float heightOffset = 0.5f; // Adjust this value to control the height above the curve
    m_position += up * heightOffset;

}

// render the cart
void Cart::Render(const glm::mat4& projection, const glm::mat4& view) {
	// calculate modelmatrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position);

	// calculate rotation 
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(up, m_direction));
	up = glm::cross(m_direction, right);
	model[0] = glm::vec4(right, 0.0f);
	model[1] = glm::vec4(up, 0.0f);
	model[2] = glm::vec4(m_direction, 0.0f);

	// init shader
	m_shader.use();
	m_shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f)); // rode kleur (uniform voor ev dmv belichting kleur veranderd??? jochen???)
	m_shader.setMat4("projection", projection);
	m_shader.setMat4("view", view);
	m_shader.setMat4("model", model);

    // Bind VAO en render
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// initialize buffers for the cart
void Cart::InitializeBuffers() {
    // Voorbeeld: een eenvoudige kubus
    float vertices[] = {
        // Posities van de kubus
        -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
   
}