#pragma once 

#include "ofMain.h"
#include "../utils/cg_extras.h"
#include "../utils/cg_drawing_extras.h"
#include "materials.h"

class Car{
    public: 
        Car(int car_type, ofVec3f dimensions, ofVec3f position, ofVec3f velocity);

        void draw();
        void draw_generic_car(enum material mat);
        void draw_wheel(ofVec3f w_pos);

        void draw_car_type_1();
        void draw_car_type_2();
        void draw_car_type_3();
        void draw_car_type_4();
        void draw_car_type_5();      

        void update(GLfloat delta_time);


        ofVec3f velocity;
        ofVec3f dimensions;
        ofVec3f position;
        ofVec3f base_velocity;

        ofVec3f w_dim; // Wheel dimensions
        GLfloat w_rot; // Current wheel rotation

        int car_type;
        int direction_sign;
};