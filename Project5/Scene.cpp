#include "Scene.h"

void Scene::update_win_state() {
    if (g_state.player->get_lives() < 1) {
        g_state.win_state = Loss;
    }
}

void Scene::process_input() {
    if (g_state.player) {
       g_state.player->process_input();
    }
}