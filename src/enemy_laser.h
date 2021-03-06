//
// Created by douglas on 19/1/27.
//

#ifndef INC_436D_Enemy_Laser_H
#define INC_436D_Enemy_Laser_H


#include "projectile.h"
#include "time.h"

class EnemyLaser: public Projectile
{
    static Texture texture;

public :
    bool init(float radius, float projectileSpeed = 300.f, float damage = 10.0f);

    // Releases all the associated resources
    void destroy();

    void setVariation(float variationf);

    // Renders the fish
    // projection is the 2D orthographic projection matrix
    void draw(const mat3& projection) override;

    void update(float ms) override;

    // Returns the fish' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box()const;

    clock_t timePassed;

    private:

    float variation;
};


#endif //INC_436D_Enemy_Laser_H
