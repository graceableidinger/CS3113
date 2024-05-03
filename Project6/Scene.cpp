#include "Scene.h"

void Scene::update_win_state() {
    if (g_state.player->get_lives() < 1) {
        g_state.win_state = Loss;
    }
    if (g_state.scene_id == 1) {
        if (g_state.items[1].get_defeated()) {
            g_state.next_scene_id = g_state.scene_id +1;
        }
    }
    if (g_state.scene_id == 3) {
        bool all_gone = true;
        for (int i = 0; i < g_state.player->get_num_items(); i++) {
            if (!g_state.items[i].get_defeated()) {
                all_gone = false;
            }
        }
        if (all_gone) {
            g_state.next_scene_id = g_state.scene_id + 1;
        }
    }
    if (g_state.scene_id == 5) {
        bool all_gone = true;
        for (int i = 1; i < g_state.player->get_num_items(); i++) {
            if (!g_state.items[i].get_defeated()) {
                all_gone = false;
            }
        }
        if (all_gone) {
            g_state.next_scene_id = g_state.scene_id + 1;
        }

    }

}

void Scene::process_input() {
    if (g_state.player) {
       g_state.player->process_input();
    }
}