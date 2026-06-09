#pragma once

#include "ofMain.h"
#include "cg_extras.h"

// Forward declarations
class Frog;  // Add this line

enum CameraMode {
    ORTHO_TOP_DOWN,
    PERSPECTIVE_PLAYER,
    FIRST_PERSON
};

class Camera {
    public:
        Camera(float fov = 60.0f, ofVec3f ortho_c = ofVec3f(0, 0, 0));
        
        void setup(ofVec3f player_pos, int grid_columns);
        void update(float delta_time, const ofVec3f& player_pos);
        void apply(CameraMode mode, const ofVec3f& player_pos, const Frog* frog);

        void apply_ortho_top_down(const ofVec3f& player_pos);
        void apply_perspective_player(const ofVec3f& player_pos);
        void apply_first_person(const ofVec3f& player_pos, const Frog* frog);
        
        bool is_camera_transitioning;

        GLfloat theta_fov; // Field of view angle
        GLfloat cam_dist; // Distance from the camera to the player
        ofVec3f ortho_center; // Center of the orthographic projection

        ofVec3f current_cam_position; 
        ofVec3f target_cam_position; // Target camera position for transitions
        ofVec3f current_cam_look;
        ofVec3f target_cam_look; // Target camera look-at point for transitions
        float camera_transition_speed;
};