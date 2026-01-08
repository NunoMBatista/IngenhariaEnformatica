#pragma once 

#include "ofMain.h"
#include "../utils/cg_extras.h"
#include "../utils/cg_drawing_extras.h"

/*
    Platform types:
    1 - Log
    2 - Turtle
*/
class Platform{
    public: 
        Platform(int platform_type, ofVec3f dimensions, ofVec3f position, ofVec3f velocity);

        void draw();

        void update(GLfloat delta_time);

        void draw_log();

        void draw_turtle();
        void draw_shell();
        void draw_head();
        void draw_legs();
        void draw_leg();

        ofVec3f velocity;
        ofVec3f dimensions;
        ofVec3f position;
        ofVec3f base_velocity;

        int platform_type;

    private: 
        ofVec3f turtle_shell_dimensions;
        ofVec3f turtle_head_dimensions;
        ofVec3f turtle_leg_dimensions;
};
