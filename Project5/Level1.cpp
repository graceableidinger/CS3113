#include "Level1.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 5

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0,
    3, 3, 0, 2, 3, 0, 0, 0, 2, 3, 3, 3, 3, 3,
    4, 4, 3, 3, 4, 0, 3, 3, 3, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4
};

unsigned int LEVEL_2_DATA[] =
{
    0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 3, 3, 3, 0, 0, 2, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4,
    1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1
};

unsigned int LEVEL_3_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0,
    3, 0, 3, 0, 4, 0, 3, 3, 3, 4, 3, 4, 4, 4,
    4, 0, 4, 0, 0, 0, 3, 3, 4, 4, 3, 3, 3, 3,
    3, 1, 3, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3
};
Level1::~Level1()
{
    delete[] g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
}

void Level1::set_level_data(unsigned int ld[]) {
    for (int i = 0; i < LEVEL_WIDTH * LEVEL_HEIGHT; i++) {
        LEVEL_1_DATA[i] = ld[i];
    }
}

void Level1::set_enemy_attributes(const char* file, glm::vec3 start_pos, int index, AIType type) {
    SPRITES[index + 1] = file;
    STARTING_POS[index + 1] = start_pos;

    // -- INDIVIDUAL ENEMY ATTRIBUTES -- //
    g_state.enemies[index].set_ai_type(type);
}

void Level1::set_sprites(const char* player, const char* tiles, const char* flag) {
    SPRITES[0] = player;
    MAP = tiles;
    FLAG = flag;
}

void Level1::initialise()
{
    g_state.next_scene_id = -1;

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    GLuint map_texture_id = g_state.util->load_texture(MAP);
    switch (g_state.scene_id) {
    case 1:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_1_DATA, map_texture_id, 1.0f, 5, 1);       
        break;
    case 2:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_2_DATA, map_texture_id, 1.0f, 5, 1);
        break;
    case 3:
        g_state.map = new Map(14.0f, 5.0f, LEVEL_3_DATA, map_texture_id, 1.0f, 5, 1);
        break;
}

    g_state.flag = new Entity();

    g_state.flag->set_texid(g_state.util->load_texture(FLAG));
    g_state.flag->set_width(0.5f);
    g_state.flag->set_height(0.5f);
    g_state.flag->set_model_matrix(glm::mat4(1.0f));
    g_state.flag->set_type(Flag);
    g_state.flag->set_position(ENDING_POS);
    g_state.flag->set_has_anim(false);

    // --- PLAYER --- //
    g_state.player = new Entity();

    g_state.player->set_texid(g_state.util->load_texture(SPRITES[0]));
    g_state.player->set_width(SCALES[0]);
    g_state.player->set_height(SCALES[0]);
    g_state.player->set_model_matrix(glm::mat4(1.0f));
    g_state.player->set_type(Player);
    g_state.player->set_position(STARTING_POS[0]);
    g_state.player->set_has_anim(true);

    // --- ENEMIES --- //
    g_state.enemies = new Entity[NUM_OF_ENEMIES];

    for (int i = 0; i < NUM_OF_ENEMIES; i++) {
        g_state.enemies[i].set_texid(g_state.util->load_texture(SPRITES[1 + i]));

        g_state.enemies[i].set_width(SCALES[i + 1]);
        g_state.enemies[i].set_height(SCALES[i + 1]);

        g_state.enemies[i].set_model_matrix(glm::mat4(1.0f));

        g_state.enemies[i].set_position(STARTING_POS[i + 1]);

        g_state.enemies[i].set_type(Enemy);

        g_state.enemies[i].set_has_anim(false);
        
        if (g_state.scene_id == 1) {
            g_state.enemies[i].set_ai_type(Wait);
        }
        else if (g_state.scene_id == 2) {
            g_state.enemies[i].set_ai_type(Fall);
            g_state.enemies[i].adj_speed(-1);
            BG_BLUE = 0.6f;
            BG_GREEN = 0.6f;
            BG_RED = 0.6f;
        }
        else if (g_state.scene_id == 3) {
            g_state.enemies[i].set_ai_type(Fly);
            BG_BLUE = 0.75f;
            BG_GREEN = 0.6f;
            BG_RED = 0.75f;
        }
    }
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

void Level1::update(float FIXED_TIMESTEP) {

    if (g_state.win_state == In_Prog) {
        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map, g_state.flag);
        g_state.flag->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map, g_state.flag);
        for (int i = 0; i < NUM_OF_ENEMIES; i++) {
            if (!g_state.enemies[i].get_defeated()) {
                g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map, g_state.flag);
            }
        }
        if (g_state.flag->get_defeated()) {
            g_state.next_scene_id = g_state.scene_id + 1;
            if (g_state.next_scene_id > 3) {
                g_state.next_scene_id = 0;
            }
        }
    }
}

void Level1::render(ShaderProgram* g_program) {

    update_win_state();

    if (g_state.win_state == Loss) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "You Lose", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else if (g_state.win_state == Win) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "You Win", 1.0f, -0.6f, glm::vec3(-1.7, 0.0f, 0.0f));
    }
    else {
        g_state.map->render(g_program);
        g_state.player->render(g_program, SCALES[0]);
        g_state.flag->render(g_program, g_state.flag->get_height()*2);
        for (int i = 0; i < NUM_OF_ENEMIES; i++) {
            if (!g_state.enemies[i].get_defeated()) {
                g_state.enemies[i].render(g_program, SCALES[i + 1]);
            }
        }
    }
}