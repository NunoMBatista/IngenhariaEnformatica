#include "platform.h"
#include "../global/global.h"
#include "materials.h"

Platform::Platform(int platform_type, ofVec3f dimensions, ofVec3f position, ofVec3f velocity){
    this->dimensions = dimensions;
    
    // Adjust position to center of the platform
    position.x += (dimensions.x / 2.0f) - (global.grid_size / 2.0f);
    this->position = position;

    this->base_velocity = velocity;
    this->platform_type = platform_type;

    turtle_shell_dimensions = dimensions * 0.6;
    turtle_head_dimensions = dimensions * 0.4;
    turtle_leg_dimensions = dimensions * 0.1;
}

void Platform::draw(){
    if(platform_type == 1){
        draw_log();
    }
    if(platform_type == 2){
        draw_turtle();
    }
}

void Platform::update(GLfloat delta_time){
    velocity = base_velocity * global.base_element_speed;
    position += velocity * delta_time;

    if(position.x < global.left_out_of_bounds){
        position.x = global.right_out_of_bounds;
    }
    else if(position.x > global.right_out_of_bounds){
        position.x = global.left_out_of_bounds;
    }
}

void Platform::draw_log(){
    load_material(LOG);
    glEnable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.log_tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPushMatrix();
        glTranslatef(position.x, -global.grid_size/2, position.z);
        glScalef(dimensions.x, dimensions.y, dimensions.z);
        cube_texture_unit(4, 1);
    glPopMatrix();

    global.log_tex.unbind();
    glDisable(GL_TEXTURE_2D);

    load_material(LOG);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.log_tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPushMatrix();
        glTranslatef(position.x, -global.grid_size/2, position.z);
        glScalef(dimensions.x*1.03, dimensions.y*0.9, dimensions.z*0.9);
        cube_texture_unit(1, 1);
    glPopMatrix();

    global.log_tex.unbind();
    glDisable(GL_TEXTURE_2D);
}

void Platform::draw_turtle(){
    // Red shell
    glPushMatrix();
        glTranslatef(position.x, -global.grid_size/2, position.z);
        draw_shell();

        // Legs and tail
        glPushMatrix();
            glTranslatef(0, turtle_shell_dimensions.y * 0.2, 0);
            draw_legs();
        glPopMatrix();

        // In front of the shell
        glPushMatrix();
            glTranslatef(-turtle_shell_dimensions.x * 0.5, 0, 0);
            draw_head();
        glPopMatrix();
        
    glPopMatrix();


}

void Platform::draw_shell(){
    load_material(TURTLE_SHELL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.turtle_tex.bind();
    glPushMatrix();
        glScalef(turtle_shell_dimensions.x*0.9, turtle_shell_dimensions.y*0.9, turtle_shell_dimensions.z);
        cube_texture_unit(1, 1);
    glPopMatrix();
    global.turtle_tex.unbind();
    glDisable(GL_TEXTURE_2D);

    load_material(TURTLE_SHELL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.turtle_tex.bind();
    glPushMatrix();
        glScalef(turtle_shell_dimensions.x, turtle_shell_dimensions.y*0.9, turtle_shell_dimensions.z*0.9);
        cube_texture_unit(1, 1);
    glPopMatrix();
    global.turtle_tex.unbind();
    glDisable(GL_TEXTURE_2D);

    load_material(TURTLE_SHELL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.turtle_tex.bind();
    glPushMatrix();
        glScalef(turtle_shell_dimensions.x*0.8, turtle_shell_dimensions.y*0.9, turtle_shell_dimensions.z*0.8);
        cube_texture_unit(1, 1);
    glPopMatrix();
    global.turtle_tex.unbind();    
    glDisable(GL_TEXTURE_2D);

    load_material(TURTLE_SHELL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    global.turtle_tex.bind();
    glPushMatrix();
        glScalef(turtle_shell_dimensions.x*0.6, turtle_shell_dimensions.y, turtle_shell_dimensions.z*0.6);
        cube_texture_unit(1, 1);
    glPopMatrix();
    global.turtle_tex.unbind();

    glDisable(GL_TEXTURE_2D);

}

void Platform::draw_head(){
    load_material(TURTLE_SKIN);
    glPushMatrix();
        glScalef(turtle_head_dimensions.x, turtle_head_dimensions.y, turtle_head_dimensions.z);
        cube_unit(0, 0.7, 0);
    glPopMatrix();

    // Eyes
    load_material(EYE);
    glPushMatrix();
        glTranslatef(-turtle_head_dimensions.x * 0.5, turtle_head_dimensions.y * 0.5, turtle_head_dimensions.z * 0.3);
        glScalef(turtle_head_dimensions.x * 0.3, turtle_head_dimensions.y * 0.6, turtle_head_dimensions.z * 0.35);
        cube_unit(1, 1, 1);
    glPopMatrix();
    
    load_material(EYE);
    glPushMatrix();
        glTranslatef(-turtle_head_dimensions.x * 0.5, turtle_head_dimensions.y * 0.5, -turtle_head_dimensions.z * 0.3);
        glScalef(turtle_head_dimensions.x * 0.3, turtle_head_dimensions.y * 0.6, turtle_head_dimensions.z * 0.35);
        cube_unit(1, 1, 1);
    glPopMatrix();

    // Pupils
    load_material(PUPIL);
    glPushMatrix();
        glTranslatef(-turtle_head_dimensions.x * 0.6, turtle_head_dimensions.y * 0.3, turtle_head_dimensions.z * 0.3);
        glScalef(turtle_head_dimensions.x * 0.3, turtle_head_dimensions.y * 0.3, turtle_head_dimensions.z * 0.3);
        cube_unit(0, 0, 0);
    glPopMatrix();

    load_material(PUPIL);
    glPushMatrix();
        glTranslatef(-turtle_head_dimensions.x * 0.6, turtle_head_dimensions.y * 0.3, -turtle_head_dimensions.z * 0.3);
        glScalef(turtle_head_dimensions.x * 0.3, turtle_head_dimensions.y * 0.3, turtle_head_dimensions.z * 0.3);
        cube_unit(0, 0, 0);
    glPopMatrix();

}

void Platform::draw_legs(){
    glPushMatrix();
        glTranslatef(-turtle_shell_dimensions.x * 0.5, 0, turtle_shell_dimensions.z * 0.5);
        draw_leg();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-turtle_shell_dimensions.x * 0.5, 0, -turtle_shell_dimensions.z * 0.5);
        draw_leg();
    glPopMatrix();

    glPushMatrix(); 
        glTranslatef(turtle_shell_dimensions.x * 0.5, 0, turtle_shell_dimensions.z * 0.5);
        draw_leg();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(turtle_shell_dimensions.x * 0.5, 0, -turtle_shell_dimensions.z * 0.5);
        draw_leg();
    glPopMatrix();

    // Tail
    glPushMatrix();
        glTranslatef(turtle_shell_dimensions.x * 0.5, 0, 0);
        glScalef(1.5, 1, 1.5);
        draw_leg();
    glPopMatrix();


}

void Platform::draw_leg(){
    load_material(TURTLE_SKIN);
    glPushMatrix();
        glScalef(turtle_leg_dimensions.x, turtle_leg_dimensions.y, turtle_leg_dimensions.z);
        cube_unit(0, 0.7, 0);
    glPopMatrix();

}