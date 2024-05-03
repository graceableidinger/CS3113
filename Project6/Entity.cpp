// Entity.cpp
#include "Entity.h"

#define ACC_OF_GRAVITY -9.8f
#define WALL_POS 10
#define VERT_BOUNDS 5


Entity::Entity()
{
    // --- PHYSICS --- //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f);
    speed = 0.5f;
    jumping_power = 5.0f;
    collision_size_x = 0.0f,
        collision_size_y = 0.0f;
    angle = 0.0f;

    // --- BOOLS --- //
    moving_right = false;
    moving_left = false;
    moving_up = false;
    moving_down = false;
    jumping = false;
    defeated = false;
    has_gravity = false;
    rotate_left = false;
    rotate_right = false;

    // --- ANIMATION --- //
    cols = SPRITESHEET_DIMENSIONS;
    rows = SPRITESHEET_DIMENSIONS;

    num_of_items = 0;
}

void Entity::draw_object(ShaderProgram* program)
{
    program->SetModelMatrix(m_model_matrix);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

int Entity::detect_collision(Entity* other, float size = 1) {
    for (int i = 0; i < size; i++) {
        if ((!(i == item_id && type == Item)) && !(level_id == 2 && i == 0)){
            float x_diff, y_diff, x_dist, y_dist;
            x_diff = fabs(m_position.x - other[i].get_position().x);
            y_diff = fabs(m_position.y - other[i].get_position().y);

            x_dist = x_diff - (width + other[i].get_width());
            y_dist = y_diff - (height + other[i].get_height());
            if (level_id == 1) {
                other[i].collision_size_x = -2.0f;
                other[i].collision_size_y = -0.5f;
            }
            if (x_dist < other[i].collision_size_x && y_dist < other[i].collision_size_y && !other[i].get_defeated()) {
                if (y_dist < other[i].collision_size_y && level_id == 2) {
                    if (other[i].get_position().y < m_position.y) {
                        m_collide_below_item = true;
                    }
                    else {
                        m_collide_top = true;
                    }
                }
                if (x_dist < other[i].collision_size_x && level_id == 2) {
                    if (other[i].get_position().x < m_position.x) {
                        m_collide_left = true;
                    }
                    else {
                        m_collide_right = true;
                    }
                }
                if (other[i].get_type() == Item) {
                    return other[i].item_id;
                }
            }
        }
    }
    return -1;
}

bool Entity::block_below(Entity* items, int collided_id) {
    if (items[collided_id].get_position().y < m_position.y) {
        return true;
    }
    return false;
}

void Entity::detect_collision(Map* map) {
    glm::vec3 points[8] = { glm::vec3(m_position.x, m_position.y + (height / 2), m_position.z), glm::vec3(m_position.x - (width / 2), m_position.y + (height / 2), m_position.z), 
    glm::vec3(m_position.x + (width / 2), m_position.y + (height / 2), m_position.z), glm::vec3(m_position.x, m_position.y - (height / 2), m_position.z), 
    glm::vec3(m_position.x - (width / 2), m_position.y - (height / 2), m_position.z), glm::vec3(m_position.x + (width / 2), m_position.y - (height / 2), m_position.z), 
    glm::vec3(m_position.x - (width / 2), m_position.y, m_position.z), glm::vec3(m_position.x + (width / 2), m_position.y, m_position.z) };

    float penetration_x = 0;
    float penetration_y = 0;

    for (int i = 0; i <= 8; i++) {
        if (i < 3) {
            if (map->is_solid(points[i], &penetration_x, &penetration_y) && m_velocity.y > 0) {
                m_position.y -= penetration_y;
                m_velocity.y = 0;
                m_collide_top = true;
            }
        }
        else if (i < 6) {
            if (map->is_solid(points[i], &penetration_x, &penetration_y) && m_velocity.y < 0) {
                m_position.y += penetration_y;
                m_velocity.y = 0;
                m_collide_below = true;
            }
        }
        else if(i < 7) {
            if (map->is_solid(points[i], &penetration_x, &penetration_y) && m_velocity.x < 0)
            {
                m_position.x += penetration_x;
                m_velocity.x = 0;
                m_collide_left = true;
                
            }
        }
        else {
            if (map->is_solid(points[i], &penetration_x, &penetration_y) && m_velocity.x > 0)
            {
                m_position.x -= penetration_x;
                m_velocity.x = -m_velocity.x;
                m_collide_right = true;
            }
        }
    }
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
        moving_left = false;
        moving_right = false;
        moving_up = false;
        moving_down = false;
        selected = false;
        rotate_left = false;
        rotate_right = false;
        idle = true;
        const Uint8* key_state = SDL_GetKeyboardState(NULL);
        if (level_id == 0) {
            if (has_gravity) {
                if (key_state[SDL_SCANCODE_SPACE] && m_collide_below) {
                    jumping = true;
                    m_collide_below = false;
                }
            }
            if (key_state[SDL_SCANCODE_SPACE] && in_claw) {
                dropped = true;
                in_claw = false;
                set_grav(true);
            }
        }
        else {
            if (key_state[SDL_SCANCODE_D]) {
                moving_right = true;
            }
            if (key_state[SDL_SCANCODE_A]) {
                moving_left = true;
            }
            if (key_state[SDL_SCANCODE_W]) {
                moving_up = true;
            }
            if (key_state[SDL_SCANCODE_S]) {
                moving_down = true;
            }
            if (key_state[SDL_SCANCODE_SPACE]) {
                selected = true;
            }
            if (level_id == 2) {
                if (key_state[SDL_SCANCODE_SPACE]) {
                    in_air = true;
                }
                if (key_state[SDL_SCANCODE_LEFT]) {
                    rotate_left = true;
                }
                if (key_state[SDL_SCANCODE_RIGHT]) {
                    rotate_right = true;
                }
            }
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

void Entity::check_falling(Entity* claw) {
    if (m_position.y < -3.0f) {
       lives-=3;
       m_position = glm::vec3(5.0f, 0.0f, 0.0f);
       claw->m_position = glm::vec3(2.0f, 2.5f, 0.0f);
       dropped = false;
    }
}

void Entity::scene_specific_update(int collided_with, Entity* items) {
    switch (level_id) {
    case 0:
        switch (collided_with) {
        case 0: //claw
            if (!dropped) {
                in_claw = true;
                m_position = items[collided_with].m_position;
                set_grav(false);
            }
            break;
        case 1:
            items[collided_with].defeated = true;
            break;
        }
        break;
    case 1:
        if (collided_with == current_ingredient) {
            items[collided_with].defeated = true;
            current_ingredient++;
        }
        else if(collided_with>=0 && !items[collided_with].hit_player){
            lives--;
            items[collided_with].hit_player = true;
        }
        break;
    case 2:
        items[collided_with].defeated = true;
        break;
    }
}

void Entity::update(float delta_time, Entity* player, Entity* items, Map* map)
{
    int collided_with = -1;
    animate(delta_time);
    // ––––– MOVEMENT ––––– //
    switch (type) {
        // –––– PLAYER –––– //
    case Player:
        set_movement(glm::vec3(0.0f));
        if ((!in_air) && rotate_right && angle > -45) {
            angle -= 50 * delta_time;
        }
        if ((!in_air) && rotate_left && angle < 45) {
            angle += 50 * delta_time;
        }
        if (((!in_air) && moving_right && m_position.x < 15.0f) || (in_air && angle < 0)) {
            if (in_air && angle < 0) {
                m_movement.x = 10.0 * cos((90 + angle) * (M_PI / 180));
            }
            else {
                m_movement.x = 5.0;
            }
        }
        if (((!in_air) && moving_left && m_position.x > 0.0f) || (in_air && angle >= 0)) {
            if (in_air && angle >= 0) {
                m_movement.x = -10.0 * cos((90 - angle) * (M_PI / 180));
            }
            else {
                m_movement.x = -5.0;
            }
        }
        if (level_id == 2) {
            if (in_air) {
                if (angle < 0) {
                    m_movement.y = 10.0 * sin((90 + angle) * (M_PI / 180));
                }
                else {
                    m_movement.y = 10.0 * sin((90 - angle) * (M_PI / 180));
                }
            }
        }
        if (moving_down && m_position.y > -3.0f) {
            m_movement.y = -5.0f;
        }
        if (has_gravity) {
            gravity(delta_time);
            check_falling(&items[0]);
        }
        if (m_position.y >= 3.0f) {
            m_collide_top = true;
        }
        if (in_air && (m_collide_top || m_collide_below_item)) {
            m_movement.y *= -1;
        }
        if (in_air && ((m_collide_left || m_collide_right) || (m_position.x <= 0 || m_position.x >= 15))) {
            m_movement.x *= -1;
        }
        if (level_id == 2) {
            m_velocity.x = m_movement.x * speed;
            m_velocity.y = m_movement.y * speed;
            m_position += m_velocity * delta_time;
        }
        collided_with = detect_collision(items, num_of_items);
        detect_collision(map);
        if (level_id == 2 && (m_collide_below || m_position.y <= -3.0f)) {
            m_position.y += 0.3f;
            m_collide_below = false;
            m_collide_top = false;
            m_collide_right = false;
            m_collide_left = false;
            m_collide_below_item = false;
            in_air = false;
        }
        //std::cout << collided_with << std::endl;
        scene_specific_update(collided_with, items);

        if (level_id == 1) {
            m_position.y = -1.5f;
        }

        // ––––– JUMPING ––––– //
        if (jumping)
        {
            jumping = false;
            m_velocity.y += jumping_power;
        }


        break;

        // –––– ENEMY –––– //
    case Item:
        switch (ai_type) {
        case Still:
            if (has_gravity && detect_collision(items, num_of_items) == -1) {
                gravity(delta_time);
            }
            break;
        case Fly:
            set_movement(glm::vec3(1.0f));
            if (m_position.x < 0.0f || m_position.x > 15.0f) {
                speed *= -1;
            }
            m_position.x += m_movement.x * speed * 2 * delta_time;
            break;
        case Catch:
            set_movement(glm::vec3(1.0f));
            if (m_position.x < 9.0f || m_position.x > 14.0f) {
                speed *= -1.1;
            }
            m_position.x += m_movement.x * speed * 2 * delta_time;
            break;
        case Fall:
            if (defeated) {
                m_position = player->get_position();
            }
            else {
                set_movement(glm::vec3(-1.0f));
                if (m_position.y < -4.0) {
                    hit_player = false;
                    srand(time(nullptr));
                    m_position.y = 4.0f;
                    m_position.x = rand() % 15;
                }
                m_position.y += m_movement.y * speed * 2 * delta_time;
            }
            break;
        case Random:
            m_movement = glm::vec3(0.0f);
            if (curr_dir == glm::vec3(0.0f) || m_position.x < 0.0f || m_position.x>15.0f || m_position.y > 3.0f || m_position.x < 3.0f) {
                srand(time(nullptr));
                curr_dir.x = (rand() % 100);
                if (int(curr_dir.x) % 2 == 0) { curr_dir.x /= 100.0f; }
                else { curr_dir.x /= -100.0f; }
                if (int(curr_dir.y) % 2 == 0) { curr_dir.y /= 100.0f; }
                else { curr_dir.y /= -100.0f; }
            }
            m_movement = curr_dir;
            m_position += m_movement;
            break;
        case Follow:
            m_position = player->m_position;
            m_position.y = 3.0f;
            m_position.x -= 4.0f;
            break;
        }
        break;
    case Decor:
        if (level_id == 0 && item_id >1) {
            angle = 105;
        }
    }
    // ––––– TRANSFORMATIONS ––––– //
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

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

