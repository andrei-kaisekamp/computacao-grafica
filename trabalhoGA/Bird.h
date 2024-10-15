#ifndef BIRD_H
#define BIRD_H

class Bird : public Sprite {
public:
    Bird() {}
    ~Bird() {}

    void move() override;

private:
    float birdTime = 0.0f;
    float frequency = 1.0f;
    float amplitude = 80.0f;
    float xSpawnPoint = 850.0;
    void resetPosition();

    vec2 offset = vec2(0.0, 0.0);
};

#endif