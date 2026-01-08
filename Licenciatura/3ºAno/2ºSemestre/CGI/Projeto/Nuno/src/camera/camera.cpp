#include "camera.h"
#include "../utils/cg_cam_extras.h"
#include "../game/frog.h"


Camera::Camera(float fov, ofVec3f ortho_c) {
    theta_fov = fov;
    camera_transition_speed = 5.0f;
    ortho_center = ortho_c;

    cam_dist = (gh()/2)/tan((theta_fov/2)*(PI/180));
}


void Camera::setup(ofVec3f player_pos, int grid_columns) {
    current_cam_position = ofVec3f(0, cam_dist, 0);
    current_cam_look = ofVec3f(0, 0, 0);
    target_cam_position = current_cam_position;
    target_cam_look = current_cam_look;
    is_camera_transitioning = false;
}


void Camera::update(float delta_time, const ofVec3f& player_pos) {
    // Camera animation
    if (is_camera_transitioning) {
        float step = camera_transition_speed * delta_time;

        /*
            Interpolate camera position and look-at point with a smooth step

            current_cam_position = current_cam_position + (target_cam_position - current_cam_position) * step;
            current_cam_look = current_cam_look + (target_cam_look - current_cam_look) * step;
        */
        current_cam_position = current_cam_position.getInterpolated(target_cam_position, step);
        current_cam_look = current_cam_look.getInterpolated(target_cam_look, step);

        // Check if the camera has reached the target position
        if (current_cam_position.distance(target_cam_position) < 0.1f &&
            current_cam_look.distance(target_cam_look) < 0.1f) {
            
            current_cam_position = target_cam_position;
            current_cam_look = target_cam_look;

            is_camera_transitioning = false;
        }
    }
}


void Camera::apply(CameraMode mode, const ofVec3f& player_pos, const Frog* frog) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch(mode) {
        case ORTHO_TOP_DOWN:
            apply_ortho_top_down(player_pos);
            break;
        case PERSPECTIVE_PLAYER:
            apply_perspective_player(player_pos);
            break;
        case FIRST_PERSON:
            apply_first_person(player_pos, frog);
            break;
    }
}


void Camera::apply_ortho_top_down(const ofVec3f& player_pos) {
    glOrtho(gw()/2, -gw()/2, -gh()/2, gh()/2, 10, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    lookat(
        ortho_center.x,   cam_dist,     ortho_center.z, 
        ortho_center.x,          0,     ortho_center.z,
                     0,          0,                  1
    );

}


void Camera::apply_perspective_player(const ofVec3f& player_pos) {
    perspective(theta_fov, 100, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    target_cam_position = ofVec3f(player_pos.x, player_pos.y, player_pos.z - 300);
    target_cam_look = player_pos;
    is_camera_transitioning = true;
    
    lookat(
        current_cam_position.x,     current_cam_position.y + 200,     current_cam_position.z,
            current_cam_look.x,               current_cam_look.y,         current_cam_look.z,
                             0,                                1,                          0
    );
}


void Camera::apply_first_person(const ofVec3f& player_pos, const Frog* frog) {
    perspective(theta_fov*1.5, 100.0f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ofVec3f offset = ofVec3f(0.0f, 0.0f, 0.0f);
    switch(frog->direction) {
        case UP:    offset = ofVec3f(   0.0f, 0.0f,   100.0f); break;
        case DOWN:  offset = ofVec3f(   0.0f, 0.0f,  -100.0f); break;
        case LEFT:  offset = ofVec3f(-100.0f, 0.0f,     0.0f); break;
        case RIGHT: offset = ofVec3f( 100.0f, 0.0f,     0.0f); break;
    }
    
    target_cam_position = ofVec3f(player_pos.x, player_pos.y + 50, player_pos.z);
    target_cam_look = target_cam_position + offset;
    is_camera_transitioning = true;
    
    lookat(
        current_cam_position.x,         current_cam_position.y,     current_cam_position.z,
            current_cam_look.x,             current_cam_look.y,         current_cam_look.z,
                             0,                              1,                          0
    );
}