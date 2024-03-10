// Entity.h
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Entity
{
private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::vec3 m_movement;
    bool right = false, left = false, up = false;

    float m_speed = 0.01f,
        width = 0.5f,
        height = 0.5f;
    bool safe_to_land, ground;

    GLuint m_texture_id;

    glm::mat4 m_model_matrix;


public:
    Entity();
    // ————— GETTERS ————— //
    glm::vec3 const get_position()     const { return m_position; };
    glm::vec3 const get_velocity()     const { return m_velocity; };
    glm::vec3 const get_acceleration() const { return m_acceleration; };
    glm::vec3 const get_movement()     const { return m_movement; };
    GLuint const get_texid()     const { return m_texture_id; };
    glm::mat4 const get_model_matrix()     const { return m_model_matrix; };
    float const get_width() const { return width; }
    float const get_height() const { return height; }
    bool const is_safe_to_land() const { return safe_to_land; }
    bool const is_ground()  const { return ground; }



    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
    void const set_acceleration(glm::vec3 new_position) { m_acceleration = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
    void const set_texid(GLuint new_texture_id) { m_texture_id = new_texture_id; };
    void const set_model_matrix(glm::mat4 new_model_matrix) { m_model_matrix = new_model_matrix; };
    void const set_safety(bool safe) { safe_to_land = safe; };
    void const set_as_ground(bool new_ground) { ground = new_ground; };



    float process_input(float gravity, float fuel);
    void update(float delta_time);
    void render(ShaderProgram* program, float scale);
    void draw_object(ShaderProgram* program);
};
