#pragma once
#include "Scene.h"

class Start : public Scene {
public:
    bool enter_pressed = false;
    Entity design;

    const char* DESIGNS[4] = { "StartScreen1.png", "StartScreen2.png", "StartScreen3.png", "StartScreen4.png"};

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

};