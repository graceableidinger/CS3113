#pragma once
#include "Scene.h"

class Level1 : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;


    const char* SPRITES[NUM_OF_ENEMIES + 1] = { "Girl.png", "Girl.png"};
    const char* MAP = "Fall_Tiles.png";
    const char* FLAG = "Winter_Flag.png";

    glm::vec3 ENDING_POS;
    glm::vec3 STARTING_POS[NUM_OF_ENEMIES + 1] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)};
    float SCALES[NUM_OF_ENEMIES + 1] = { 0.5f, 0.5f};


    // ————— CONSTRUCTOR ————— //
    ~Level1();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    void set_enemy_attributes(const char*, glm::vec3, int, AIType );
    void set_level_data(unsigned int[]);
    void set_sprites(const char*, const char*, const char*);
};