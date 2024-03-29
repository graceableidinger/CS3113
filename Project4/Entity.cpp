// Entity.cpp
#include "Entity.h"

#define ACC_OF_GRAVITY -9.8f
#define WALL_POS 4.5


Entity::Entity()
{
    // --- PHYSICS --- //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f);
    speed = 0.5f;
    jumping_power = 3.0f;

    // --- BOOLS --- //
    moving_right = false;
    moving_left = false;
    jumping = false;
    defended = false;
    defeated = false;

    // --- COLLISION --- //
    m_collide_below = false;
    m_collide_left = false;
    m_collide_right = false;

    // --- ANIMATION --- //
    cols = SPRITESHEET_DIMENSIONS;
    rows = SPRITESHEET_DIMENSIONS;
}

void Entity::draw_object(ShaderProgram* program)
{
    program->SetModelMatrix(m_model_matrix);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

bool Entity::detect_collision(Entity* other, float size) {
    for (int i = 0; i < size; i++) {
        if (!other[i].defeated) {
            float x_diff, y_diff, x_dist, y_dist;
            x_diff = fabs(m_position.x - other[i].get_position().x);
            y_diff = fabs(m_position.y - other[i].get_position().y);

            x_dist = x_diff - (width + other[i].get_width());
            y_dist = y_diff - (height + other[i].get_height());
            if (x_dist < 0 && y_dist < 0) {
                if (other[i].get_type() == Enemy) {
                    if (defended) {
                        other[i].set_defeated(true);
                    }
                    else {
                        defeated = true;
                    }
                    return true;
                }
                else if (other[i].get_type() == Platform) {
                    adjust_overlap(&other[i]);
                    return true;
                }
            }
        }
    }
    return false;

}

void Entity::gravity(float delta_time){
    m_velocity.x = m_movement.x  * speed;
    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;
}

void Entity::adjust_overlap(Entity* collided_with) {
    float y_distance = fabs(m_position.y - collided_with->m_position.y);
    float y_overlap = fabs(y_distance - (height) - (collided_with->get_height()));

    if (m_velocity.y > 0) {
        m_position.y -= y_overlap;
        //m_velocity.y = 0;
    }
    else if (m_velocity.y < 0) {
        m_position.y += y_overlap;
        m_velocity.y = 0;
    }
}

void Entity::check_which_animation() {
    switch (anim_direction) {
    case Block_Left:
        anim_indicator = 0;
        break;
    case Block_Right:
        anim_indicator = 2;
        break;
    case Left:
        anim_indicator = 1;
        break;
    case Right:
        anim_indicator = 3;
        break;
    }
}

void Entity::animate(float delta_time) {
    if (!idle) {
        g_animation_time += delta_time;
        float seconds_per_frame = (float)1 / FRAMES_PER_SECOND;

        // If we've reached the beginning of a frame span...
        if (g_animation_time >= seconds_per_frame || change_anim)
        {
            check_which_animation();
            // Reset the animation time, change the index
            g_animation_time = 0.0f;
            g_animation_index += cols;

            // And reset the index if we've reached the last animation index
            if (g_animation_index >= g_animation_frames * cols || change_anim)
            {
                g_animation_index = anim_indicator;
            }
            change_anim = false;

        }
    }
    else {
        g_animation_index = anim_indicator;
    }
}

void Entity::process_input() {
    if (type == Player) {
        defended = false;
        moving_left = false;
        moving_right = false;
        idle = true;
        const Uint8* key_state = SDL_GetKeyboardState(NULL);
        if (key_state[SDL_SCANCODE_Q]) {
            defended = true;
        }
        if (key_state[SDL_SCANCODE_D]) {
            idle = false;
            if (defended) {
                if (anim_direction != Block_Right) {
                    change_anim = true;
                }
                anim_direction = Block_Right;
            }
            else {
                if (anim_direction != Right) {
                    change_anim = true;
                }
                anim_direction = Right;
            }
            moving_right = true;
        }
        if (key_state[SDL_SCANCODE_A]) {
            idle = false;
            if (defended) {
                if (anim_direction != Block_Left) {
                    change_anim = true;
                }
                anim_direction = Block_Left;
            }
            else {
                if (anim_direction != Left) {
                    change_anim = true;
                }
                anim_direction = Left;
            }
            moving_left = true;
        }
        if (key_state[SDL_SCANCODE_SPACE] && m_collide_below) {
            jumping = true;
        }
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % SPRITESHEET_DIMENSIONS) / (float)SPRITESHEET_DIMENSIONS;
    float v_coord = (float)(index / SPRITESHEET_DIMENSIONS) / (float)SPRITESHEET_DIMENSIONS;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)SPRITESHEET_DIMENSIONS;
    float height = 1.0f / (float)SPRITESHEET_DIMENSIONS;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->GetPositionAttribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->GetPositionAttribute());

    glVertexAttribPointer(program->GetTexCoordAttribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->GetTexCoordAttribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->GetPositionAttribute());
    glDisableVertexAttribArray(program->GetTexCoordAttribute());
}

void Entity::update(float delta_time, Entity* player, Entity* enemies, Entity* platforms)
{
    animate(delta_time);
    // ––––– MOVEMENT ––––– //
    switch (type) {
    // –––– PLAYER –––– //
    case Player:
        set_movement(glm::vec3(0.0f));
        if (moving_right && m_position.x < WALL_POS) {
            m_movement.x = 5.0f;
        }
        if (moving_left && m_position.x > -WALL_POS) {
            m_movement.x = -5.0f;
        }
        gravity(delta_time);
        m_collide_below = detect_collision(platforms, NUM_OF_PLATFORMS);
        detect_collision(enemies, NUM_OF_ENEMIES);

        // ––––– JUMPING ––––– //
        if (jumping)
        {

            jumping = false;
            m_velocity.y += jumping_power;
        }
        break;
    // –––– ENEMY –––– //
    case Enemy:
        switch (ai_type) {
        case Walk:
            idle = false;
            set_movement(glm::vec3(-1.0f, 0.0f, 0.0f));
            if (m_position.x < -WALL_POS || m_position.x>WALL_POS) {
                speed *= -1;
                switch (anim_direction) {
                case Left:
                    anim_direction = Right;
                    break;
                case Right:
                    anim_direction = Left;
                    break;
                }
            }
            if (!detect_collision(platforms, NUM_OF_PLATFORMS)) {
                gravity(delta_time);
            }
            break;
        case Fly:
            set_movement(glm::vec3(1.0f));
            if (m_position.y < -WALL_POS + 1 || m_position.y > WALL_POS - 1 || detect_collision(platforms, NUM_OF_PLATFORMS)) {
                speed *= -1;
            }
            m_position.y += m_movement.y * speed * delta_time;
            break;
        case Wait:
            set_movement(glm::vec3(0.0f));
            if (glm::distance(m_position, player->get_position()) < 3.0f) {
                idle = false;
                if (m_position.x > player->get_position().x) {
                    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
                    anim_direction = Left;
                }
                else {
                    m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                    anim_direction = Right;
                }
            }
            else {
                idle = true;
            }
            if (!detect_collision(platforms, NUM_OF_PLATFORMS)) {
                gravity(delta_time);
            }
            break;
        }
        break;
    }
    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);

}

void Entity::render(ShaderProgram* program, float scale) {
    program->SetModelMatrix(m_model_matrix);

    if (has_animation)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, g_animation_index);
        return;
    }

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

