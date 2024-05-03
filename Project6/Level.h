#pragma once
#include "Scene.h"

class Level : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    const char* PLAYER[3] = { "Bear.png", "Pole.png", "Dart.png" };
    const char* MAP = "Fall_Tiles.png";
    const char* SPRITES[14] = { "Claw.png", "Basket.png", "Blue_Ring.png", "Purple_Ring.png", "Pink_Ring.png",
        "Red_Ring.png", "Orange_Ring.png", "Yellow_Ring.png", "Rings.png", "Blue_Balloon.png", 
        "Green_Balloon.png", "Yellow_Balloon.png", "Red_Balloon.png", "Purple_Balloon.png"};
    const char* DECOR[14] = { "Left_Curtain.png", "Right_Curtain.png", "Toy1.png", "Toy2.png", "Toy3.png", 
        "Toy4.png", "Toy5.png", "Toy6.png", "Level2_BG.png", "Level2_Counter.png", "Level2_Roof.png", "Level3_BG.png", "Level3_Counter.png", "Level3_Roof.png"};

    float* SCALES;
    float* DECOR_SCALES;

    float time_left = 5000.0f;

    glm::vec3 STARTING_POS = { glm::vec3(5.0f, 0.0f, 0.0f)};
    glm::vec3* ITEM_STARTING_POS;
    glm::vec3* DECOR_STARTING_POS;

    // ————— CONSTRUCTOR ————— //
    ~Level();

    // ————— METHODS ————— //
    void initialise() override;
    void initialise_items();
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    void set_item_attributes(glm::vec3, int, AIType, float);
    void set_decor_attributes(glm::vec3, int, float);
    void set_level_data(unsigned int[]);
};