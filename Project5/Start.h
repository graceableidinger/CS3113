#pragma once
#include "Scene.h"

class Start : public Scene {
public:
    bool enter_pressed = false;

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

};