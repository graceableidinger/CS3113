#include "Level.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 5

bool p;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int LEVEL_2_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int LEVEL_3_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3,
    4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3,
};
Level::~Level()
{
    delete[] g_state.items;
    delete    g_state.player;
    delete    g_state.map;
}

void Level::set_level_data(unsigned int ld[]) {
    for (int i = 0; i < LEVEL_WIDTH * LEVEL_HEIGHT; i++) {
        LEVEL_1_DATA[i] = ld[i];
    }
}

void Level::set_item_attributes(glm::vec3 start_pos, int index, AIType type, float scale) {
    ITEM_STARTING_POS[index] = start_pos;
    SCALES[index] = scale;

    if (g_state.scene_id == 5) {
        SCALES[index] = 0.5f;
    }
    

    // -- INDIVIDUAL ENEMY ATTRIBUTES -- //
    g_state.items[index].set_ai_type(type);
}

void Level::set_decor_attributes(glm::vec3 start_pos, int index, float scale) {
    DECOR_STARTING_POS[index] = start_pos;
    DECOR_SCALES[index] = scale;


    // -- INDIVIDUAL ENEMY ATTRIBUTES -- //
    g_state.decor[index].set_ai_type(Still);
}

void Level::initialise()
{
    g_state.next_scene_id = -1;

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    GLuint map_texture_id = g_state.util->load_texture(MAP);
    switch (g_state.scene_id) {
    case 1:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_1_DATA, map_texture_id, 1.0f, 5, 1);
        break;
    case 3:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_2_DATA, map_texture_id, 1.0f, 5, 1);
        break;
    case 5:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_3_DATA, map_texture_id, 1.0f, 5, 1);
        break;
    }
    // --- PLAYER --- //
    g_state.player = new Entity();

    if (g_state.scene_id % 2 == 1) {
        g_state.player->set_texid(g_state.util->load_texture(PLAYER[g_state.scene_id / 2]));
        g_state.player->set_width(1.5f);
        g_state.player->set_height(0.5f);
        g_state.player->set_model_matrix(glm::mat4(1.0f));
        g_state.player->set_type(Player);
        g_state.player->set_position(STARTING_POS);
        g_state.player->set_has_anim(false);
        g_state.player->set_lives(3);
        g_state.player->set_ids(0, g_state.scene_id / 2);
    }

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

void Level::initialise_items() {
    // --- items --- //
    for (int i = 0; i < g_number_of_items; i++) {
        if (g_state.scene_id == 1) {
            g_state.items[i].set_texid(g_state.util->load_texture(SPRITES[i]));
        }
        else if (g_state.scene_id == 3) {
            g_state.items[i].set_texid(g_state.util->load_texture(SPRITES[i+2]));
        }
        else if (g_state.scene_id == 5) {
            g_state.items[i].set_texid(g_state.util->load_texture(SPRITES[(i%5) + 9]));
        }
        g_state.items[i].set_width(SCALES[i]);
        g_state.items[i].set_height(SCALES[i]);

        g_state.items[i].set_model_matrix(glm::mat4(1.0f));

        g_state.items[i].set_position(ITEM_STARTING_POS[i]);

        g_state.items[i].set_type(Item);

        g_state.items[i].set_has_anim(false);
        g_state.items[i].set_ids(i, g_state.scene_id/2);
        g_state.items[i].set_num_items(g_number_of_items);
    }
    g_state.player->set_num_items(g_number_of_items);

    for (int i = 0; i < g_number_of_decor; i++) {
        if (g_state.scene_id == 1) {
            if (i < 2) {
                g_state.decor[i].set_texid(g_state.util->load_texture(DECOR[i]));
            }
            else {
                g_state.decor[i].set_texid(g_state.util->load_texture(DECOR[1+i / 2]));
            }
        }
        else if (g_state.scene_id == 3) {
            g_state.decor[i].set_texid(g_state.util->load_texture(DECOR[i+8]));
        }
        else if (g_state.scene_id == 5) {
            g_state.decor[i].set_texid(g_state.util->load_texture(DECOR[i+11]));
        }
        g_state.decor[i].set_width(DECOR_SCALES[i]);
        g_state.decor[i].set_height(DECOR_SCALES[i]);

        g_state.decor[i].set_model_matrix(glm::mat4(1.0f));

        g_state.decor[i].set_position(DECOR_STARTING_POS[i]);

        g_state.decor[i].set_type(Decor);

        g_state.decor[i].set_has_anim(false);
        g_state.decor[i].set_ids(i, g_state.scene_id / 2);
        g_state.decor[i].set_num_items(g_number_of_items);
    }
    if (g_state.scene_id == 3 || g_state.scene_id == 5) {
        BG_BLUE = 1.0f;
        BG_GREEN = 1.0f;
        BG_RED = 1.0f;
    }
}

void Level::update(float FIXED_TIMESTEP) {
    if (g_state.win_state == In_Prog) {
        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.items, g_state.map);
        for (int i = 0; i < g_number_of_items; i++) {
            if (!g_state.items[i].get_defeated() || g_state.scene_id == 3) {
                g_state.items[i].update(FIXED_TIMESTEP, g_state.player, g_state.items, g_state.map);
            }
        }
        for (int i = 0; i < g_number_of_decor; i++) {
            g_state.decor[i].update(FIXED_TIMESTEP, g_state.player, g_state.items, g_state.map);
        }
    }
}

void Level::render(ShaderProgram* g_program) {

    update_win_state();

    if (g_state.scene_id == 5) {
        if (time_left == 0) {
            g_state.win_state = Loss;
        }
        else {
            time_left--;
        }
    }

    if (g_state.win_state == Loss) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "You Lose", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else if (g_state.win_state == Win) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "You Win", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else {
        g_state.map->render(g_program);
        if (g_state.scene_id == 3) {
            for (int i = 0; i < g_number_of_decor; i++) {
                g_state.decor[i].render(g_program, g_state.decor[i].get_width());
            }
            std::string temp = "Make the following stack:";
            g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), temp, 0.4f, -0.2f, g_state.player->get_position() + glm::vec3(-4.5f, 5.0f, 0.0f));
            temp = "Attempts Left: " + std::to_string(g_state.player->get_lives());
            g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), temp, 0.4f, -0.2f, g_state.player->get_position() + glm::vec3(-4.5f, 3.5f, 0.0f));
        }
        if (g_state.scene_id == 5) {
            for (int i = 0; i < g_number_of_decor; i++) {
                g_state.decor[i].render(g_program, g_state.decor[i].get_width());
            }
            std::string temp = "Time Left: " + std::to_string(time_left / 10);
            g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), temp, 0.4f, -0.2f, glm::vec3(g_state.player->get_position().x, 0.0f, 0.0f) + glm::vec3(-4.8f, 3.0f, 0.0f));
        }
        if (g_state.scene_id == 1) {
            for (int i = 0; i < g_number_of_decor / 2; i++) {
                g_state.decor[i].render(g_program, g_state.decor[i].get_width());
            }
            for (int i = g_number_of_decor - 1; i >= g_number_of_decor / 2; i--) {
                g_state.decor[i].render(g_program, g_state.decor[i].get_width());
            }
        }
        g_state.player->render(g_program, g_state.player->get_width());
        for (int i = 0; i < g_number_of_items; i++) {
            if (!g_state.items[i].get_defeated() || g_state.scene_id == 3) {
                g_state.items[i].render(g_program, g_state.items[i].get_width());
            }
        }
        
    }
}