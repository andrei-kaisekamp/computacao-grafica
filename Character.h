#ifndef CHARACTER_H
#define CHARACTER_H

class Character : public Sprite {
public:
    Character() {}
    ~Character() {}

    void move() override;

    int imgWidth, imgHeight, runTexture;
    int jumpimgWidth, jumpimgHeight, jumpTexture;
    float jumpVelocity = 15.0f;

private:
    vec3 spawnPoint = vec3(800.0, 300.0, 0.0);
    vec2 offset = vec2(0.0, 0.0);
    float speed = 3.0f;
    bool isJumping = false;

    void setupJump();
    void jump();
};

#endif