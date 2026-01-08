#pragma once 

#include "ofMain.h"
#include "../utils/cg_drawing_extras.h"
#include "../utils/cg_cam_extras.h"
#include "particle.h"

class Frog {
    public:
        Frog(ofVec3f dimensions, ofVec3f position);
        ~Frog();
        
        /*
        
        */
        void draw();
        void update(float delta_time);
        void turn(Direction direction);
        void start_move(const ofVec3f& start_pos, const ofVec3f& target_pos);
        bool is_moving;

        void update_movement(float delta_time);
        void update_jump(float delta_time);
        void update_rotation(float delta_time);
        void update_explosion(float delta_time);
        void update_drowning(float delta_time);

        void explosion();
        void burst_effect();
        
        void drown();
        void splash_effect();

        void winning_effect();

        void start_scale_animation();
        bool is_scaling;
        float scale_progress;
        float scale_duration;

        // Public variables
        ofVec3f eye_vector;        
        Direction direction;
        GLfloat rotation;
        ofVec3f dimensions;
        ofVec3f position;

        GLfloat f_scale;

        bool is_jumping;
        float jump_progress;
        float jump_duration;
        float jump_height;

        bool is_exploding;
        GLfloat explosion_timer;
        GLfloat explosion_duration;
        GLfloat explosion_rotation_speed;
        GLfloat explosion_jump_height;

        float target_rotation;
        float rotation_speed;
        bool is_rotating;

        bool is_alive;
        bool is_bursting;

        bool on_plat;
        ofVec3f plat_velocity;

        bool is_drowning;
        bool is_splashing;
        bool is_winning; 
        GLfloat drowning_timer;
        GLfloat drowning_duration;
        GLfloat drowning_jump_height; 

    private:
        // Drawing methods
        void draw_body();
        void draw_neck();
        void draw_head();
        void draw_eye(GLfloat x);
        void draw_eyes();
        void draw_tongue();
        void draw_leg(GLfloat x, GLfloat y, GLfloat z);
        void draw_legs();

        // Body dimensions
        ofVec3f body_dim;
        ofVec3f leg_dim;
        ofVec3f neck_dim;
        ofVec3f head_dim;
        ofVec3f eye_dim;

        // Movement variables
        ofVec3f start_position;
        ofVec3f target_position;
        float movement_timer;
        float movement_duration;

        std::vector<Particle*> particles;

};