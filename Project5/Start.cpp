#include "Start.h"


void Start::initialise()
{
    g_state.next_scene_id = -1;

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

void Start::update(float delta_time) {};

void Start::render(ShaderProgram* g_program) {
    g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Season's Greetings", 1.0f, -0.6f, glm::vec3(-3.3, 0.5f, 0.0f));

    g_state.util->draw_text(g_program, g_state.util->load_texture(FONT), "Press enter to start", 1.0f, -0.6f, glm::vec3(-3.5, -0.5f, 0.0f));
}