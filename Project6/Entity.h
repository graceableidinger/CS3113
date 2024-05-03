// Entity.h

#define NUM_OF_PLATFORMS 14

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdlib>
#include <iostream>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"


enum EntityType{ Player, Item, Decor};
enum AIType{ Random, Fly, Spin, Still, Fall, Bubble, Follow, Catch};
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
        jumping_power,
        collision_size_x,
        collision_size_y,
        angle;

    bool moving_right, 
        moving_left, 
        moving_up,
        moving_down,
        jumping, 
        has_gravity, 
        defeated,
        has_animation,
        selected,
        rotate_left,
        rotate_right;

    int lives, num_of_items, item_id, level_id;

    // --- COLLISIONS --- //

    bool m_collide_below = false,
        m_collide_left = false,
        m_collide_right = false,
        m_collide_top = false,
        m_collide_below_item = false,
        hit_player = false;

    float width, 
        height;

    //Scene 1
    bool in_claw = false,
        dropped = false;

    //Scene 2
    int current_ingredient = 0;
    glm::vec3 curr_dir = glm::vec3(0.0f);

    //Scene 3
    bool pouring;

    // --- ANIMATION --- //
    bool idle = true, change_anim = false, in_air = false;
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
    EntityType get_type() const { return type; }
    int const get_lives() const { return lives; }
    int const get_num_items() const { return num_of_items; };


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
    void const set_defeated(bool new_def) { defeated = new_def; }
    void const set_has_anim(bool new_anim) { has_animation = new_anim; }
    void const set_anim_dir(Anim new_dir) { anim_direction = new_dir; }
    void const adj_speed(float new_speed) { speed *= new_speed;  }
    void const set_lives(int new_lives) { lives = new_lives; }
    void const set_grav(bool grav) { has_gravity = grav; }
    void const set_ids(int i_id, int l_id) {
        item_id = i_id;
        level_id = l_id;
    }
    void const set_num_items(int num) { num_of_items = num; }


    // ————— HELPERS ————— //
    void gravity(float);
    void draw_object(ShaderProgram*);
    int detect_collision(Entity*, float);
    bool block_below(Entity*, int);
    void detect_collision(Map*);
    void adjust_overlap(Entity*);
    void animate(float);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void check_which_animation();
    void check_falling(Entity*);


    // ————— BASIC ————— //
    void process_input();
    void update(float, Entity*, Entity*, Map*);
    void scene_specific_update(int, Entity*);
    void render(ShaderProgram*, float);
};
