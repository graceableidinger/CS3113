// Entity.h

#define NUM_OF_PLATFORMS 14
#define NUM_OF_ENEMIES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

enum EntityType{ Player, Enemy, Flag };
enum AIType{ Fall, Fly, Wait };
enum Anim{ Block_Left, Block_Right, Left, Right};

class Entity
{
private:
    EntityType type;
    AIType ai_type;

    GLuint m_texture_id;
    glm::mat4 m_model_matrix;

    // --- PHYSICS --- //
    glm::vec3 m_position, 
        m_movement, 
        m_velocity, 
        m_acceleration;

    float speed,
        jumping_power;

    bool moving_right, 
        moving_left, 
        jumping, 
        defended, 
        defeated,
        has_animation;

    int lives;

    // --- COLLISIONS --- //

    bool m_collide_below = false,
        m_collide_left = false,
        m_collide_right = false,
        m_collide_top = false;

    float width, 
        height;

    // --- ANIMATION --- //
    bool idle = true, change_anim = false;
    const int SPRITESHEET_DIMENSIONS = 4;
    const int FRAMES_PER_SECOND = 4;
    int g_animation_frames = SPRITESHEET_DIMENSIONS;
    float cols, rows;
    Anim anim_direction;
    int g_animation_index = 0, anim_indicator = 1; // tells which animation we are using, range (0,SPRITESHEET_DIMENSIONS-1)

    float g_animation_time = 0.0f;

public:

    Entity();


    // ————— GETTERS ————— //
    glm::vec3 const get_position()     const { return m_position; };
    glm::vec3 const get_movement()     const { return m_movement; };
    glm::vec3 const get_velocity()     const { return m_velocity; };
    glm::vec3 const get_acceleration()     const { return m_acceleration; };
    GLuint const get_texid()     const { return m_texture_id; };
    glm::mat4 const get_model_matrix()     const { return m_model_matrix; };
    float const get_width() const { return width; }
    float const get_height() const { return height; }
    bool const get_defeated() const { return defeated; }
    bool const get_defended() const { return defended; }
    EntityType get_type() const { return type; }
    int const get_lives() const { return lives; }


    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
    void const set_velocity(glm::vec3 new_velocity) { m_position = new_velocity; };
    void const set_acceleration(glm::vec3 new_acceleration) { m_position = new_acceleration; };
    void const set_texid(GLuint new_texture_id) { m_texture_id = new_texture_id; };
    void const set_height(float new_height) { height = new_height; };
    void const set_width(float new_width) { width = new_width; };
    void const set_model_matrix(glm::mat4 new_model_matrix) { m_model_matrix = new_model_matrix; };
    void const set_type(EntityType new_type) { type = new_type; }
    void const set_ai_type(AIType new_type) { ai_type = new_type; }
    void const set_defended(bool new_def) { defended = new_def; }
    void const set_defeated(bool new_def) { defeated = new_def; }
    void const set_has_anim(bool new_anim) { has_animation = new_anim; }
    void const set_anim_dir(Anim new_dir) { anim_direction = new_dir; }
    void const adj_speed(float new_speed) { speed *= new_speed;  }
    void const set_lives(int new_lives) { lives = new_lives; }


    // ————— HELPERS ————— //
    void gravity(float);
    void draw_object(ShaderProgram*);
    bool detect_collision(Entity*, float);
    void detect_collision(Map*);
    void adjust_overlap(Entity*);
    void animate(float);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void check_which_animation();
    void check_falling();


    // ————— BASIC ————— //
    void process_input();
    void update(float, Entity*, Entity*, Map*, Entity*);
    void render(ShaderProgram*, float);
};
