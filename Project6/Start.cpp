#include "Start.h"


void Start::initialise()
{
    g_state.next_scene_id = -1;

    design.set_texid(g_state.util->load_texture(DESIGNS[g_state.scene_id/2]));
    
    design.set_width(3.5f);
    design.set_height(3.5f);

    design.set_model_matrix(glm::mat4(1.0f));

    design.set_position(glm::vec3(0.0f, 0.0f, 0.0f));

    design.set_type(Item);

    design.set_has_anim(false);
    design.set_ids(g_state.scene_id, g_state.scene_id / 2 );
    design.set_num_items(1);

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

void Start::update(float delta_time) {};

void Start::render(ShaderProgram* g_program) {

    design.render(g_program, design.get_height());
    /*if (g_state.scene_id == 0) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Claw Machine!", 1.0f, -0.6f, glm::vec3(-3.3, 0.5f, 0.0f));
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Press enter to start", 1.0f, -0.6f, glm::vec3(-3.5, -0.5f, 0.0f));
    }
    else if (g_state.scene_id == 2) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Ring Catch!", 1.0f, -0.6f, glm::vec3(-3.3, 0.5f, 0.0f));
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Press enter to start", 1.0f, -0.6f, glm::vec3(-3.5, -0.5f, 0.0f));
    }
    else if (g_state.scene_id == 4) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Darts!", 1.0f, -0.6f, glm::vec3(-3.3, 0.5f, 0.0f));
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Press enter to start", 1.0f, -0.6f, glm::vec3(-3.5, -0.5f, 0.0f));
    }
    if (g_state.scene_id == 6) {
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Congrats!", 1.0f, -0.6f, glm::vec3(-3.3, 0.5f, 0.0f));
        g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "The end.", 1.0f, -0.6f, glm::vec3(-3.5, -0.5f, 0.0f));
    }*/
}