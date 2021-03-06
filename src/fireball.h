//
// Created by douglas on 19/1/27.
//

#ifndef INC_436D_Fireball_H
#define INC_436D_Fireball_H


#include "projectile.h"

class Fireball: public Projectile
{
    static Texture texture;

public :

    Fireball(float radius, float projectileSpeed = 400.f, float damage = 20.f)
    {
        init(radius,projectileSpeed, damage);
    }
    bool init(float radius, float projectileSpeed, float damage);

    // Renders the fish
    // projection is the 2D orthographic projection matrix
    void draw(const mat3& projection) override;

    // Returns the fish' bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box()const;

};

#endif //INC_436D_Fireball_H
