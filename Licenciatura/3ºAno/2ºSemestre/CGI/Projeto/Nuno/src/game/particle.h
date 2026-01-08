#pragma once

#include "ofMain.h"
#include "../utils/cg_extras.h"
#include "../utils/cg_drawing_extras.h"
#include "materials.h"

class Particle{
    public:
        Particle(ofVec3f position, ofVec3f velocity, enum material mat, float lifespan);
        void update(float delta_time);
        void draw();
        bool is_dead();

        GLfloat time_lived;
        ofVec3f position;
        ofVec3f velocity;
        //ofVec3f color;
        enum material mat;
        float lifespan;
        bool winning_particle;

};