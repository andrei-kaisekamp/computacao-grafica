#ifndef BACKGROUND_H
#define BACKGROUND_H

class Background : public Sprite {
public:
    Background() {}
    ~Background() {}

    void move() override; // Método específico do Background

private:
    vec3 spawnPoint = vec3(800.0, 300.0, 0.0);
    void resetPosition();
};

#endif