#ifndef OBSTACLE_H
#define OBSTACLE_H

class Obstacle : public Sprite {
public:
    Obstacle() {}
    ~Obstacle() {}

    void move() override; // Método específico do Obstacle

private:
    vec3 spawnPoint = vec3(900.0, 85.0, 0.0);
    void increasePoints();
    void resetPosition();
};

#endif