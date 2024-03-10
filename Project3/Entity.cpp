// Entity.cpp
#include "Entity.h"

#define ACC_OF_GRAVITY -0.0981f


Entity::Entity()
{
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);
    safe_to_land = false;
    ground = false;
}

float Entity::process_input(float gravity, float fuel) {
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (fuel > 0) {
        if (key_state[SDL_SCANCODE_D]) {
            right = true;
            fuel -= 0.1;
        }
        else {
            right = false;
        }
        if (key_state[SDL_SCANCODE_A]) {
            left = true;
            fuel -= 0.1;
        }
        else {
            left = false;
        }
        if (key_state[SDL_SCANCODE_W]) {
            up = true;
            fuel -= 0.1;
        }
        else {
            up = false;
        }
    }
    else {
        right = false;
        left = false;
        up = false;
    }


    return fuel;
}



void Entity::update(float delta_time)
{
    //movement
    if (right && m_position.x < 4.5) {
        m_acceleration.x += 0.5f;
    }
    else {
        if (m_acceleration.x > 0) {
            m_acceleration.x -= 0.3f;
        }
    }
    if (left && m_position.x > -4.5) {
        m_acceleration.x -= 0.05f;
    }
    else {
        if (m_acceleration.x < 0) {
            m_acceleration.x += 0.3f;
        }
    }
    if (up && m_position.y < 3.0) {
        m_acceleration.y += (0.05f);
    }
    else {
        m_acceleration.y = ACC_OF_GRAVITY;
    }


    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y += m_movement.y * m_speed;

    //gravity
    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;

    //transformation
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

}

void Entity::render(ShaderProgram* program, float scale) {
    program->SetModelMatrix(m_model_matrix);

    float vertices[] = { -scale, -scale, scale, -scale, scale, scale, -scale, -scale, scale, scale, -scale, scale };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    draw_object(program);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::draw_object(ShaderProgram* program)
{
    program->SetModelMatrix(m_model_matrix);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}