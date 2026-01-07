#include "particle.h"
#include "../global/global.h"
#include "materials.h"

Particle::Particle(ofVec3f position, ofVec3f velocity, enum material mat, float lifespan){
    this->position = position;
    this->velocity = velocity;
    this->lifespan = lifespan;
    //this->color = color;
    this->mat = mat;
    this->time_lived = 0;
    winning_particle = false;
}

void Particle::update(float delta_time) {
    GLfloat top_river_row = global.grid->get_grid_position(global.grid->top_river_row, 0).z + global.grid->grid_size/2;
    GLfloat bottom_river_row = global.grid->get_grid_position(global.grid->bottom_river_row, 0).z - global.grid->grid_size/2;
    GLfloat left_out_of_bounds = global.grid->get_grid_position(0, -1).x + global.grid->grid_size/2;
    GLfloat right_out_of_bounds = global.grid->get_grid_position(0, 15).x - global.grid->grid_size/2;
    //GLfloat top_out_of_bounds = global.grid->get_grid_position(global.grid->top_river_row+1, 0).z - global.grid->grid_size/2;

    if(!winning_particle){
        // If the particle hits the ground
        if(position.y < 0){
            // If it falls on the road, bounce back
            if(position.z < bottom_river_row){
                position.y = 0;
                velocity.y = -velocity.y;
            }
            // If it falls in the river, just kill it
            else{
                // If it's too young, it means that it just spawned and it's the begining of a splash animation
                if(time_lived > 0.5){
                    lifespan = 0;
                }
            }
        }
        // If it hits the side or the top wall, bounce back
        if(position.x < left_out_of_bounds){
            position.x = left_out_of_bounds;
            velocity.x = -velocity.x;
        }
        else if(position.x > right_out_of_bounds){  
            position.x = right_out_of_bounds;
            velocity.x = -velocity.x;
        }
        if(position.z > top_river_row){
            position.z = top_river_row;
            velocity.z = -velocity.z;
        }
    
        // Apply gravity and velocity
        int gravity = -2.5;
        velocity.y += gravity * delta_time; 
    }
    
    position += velocity * delta_time * 200;
    lifespan -= delta_time;
    time_lived += delta_time;

    // Kill the particle
    if(lifespan < 0){
        lifespan = 0;
    }
}

void Particle::draw(){
    //position += velocity;

    load_material(mat);
    glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(ofRandom(0, 360), 1, 1, 1);
        glScalef(2*lifespan, 2*lifespan, 2*lifespan);
        //cube_unit(ofRandom(0, 1), ofRandom(0, 1), ofRandom(0, 1));
        cube_unit(1,1,1);
    glPopMatrix();
}

bool Particle::is_dead(){
    return false;
}