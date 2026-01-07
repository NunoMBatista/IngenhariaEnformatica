#include "car.h"
#include "../grid/grid.h"
#include "../global/global.h"
#include "materials.h"

Car::Car(int car_type, ofVec3f dimensions, ofVec3f position, ofVec3f velocity){
    this->dimensions = dimensions;
    this->position = position;
    this->base_velocity = velocity;
    this->car_type = car_type;
    
    
    // -1 if going left, 1 if going right
    this->direction_sign = velocity.x/abs(velocity.x);

    w_dim = ofVec3f(10, 10, 3);
    w_rot = ofRandom(0, 90);
}

void Car::update(GLfloat delta_time){
    this->velocity = base_velocity * global.base_element_speed;
    // position += velocity;
    position += velocity * delta_time;

    if(position.x < global.left_out_of_bounds){
        position.x = global.right_out_of_bounds;
    }
    else if(position.x > global.right_out_of_bounds){
        position.x = global.left_out_of_bounds;
    }

    // Rotate the wheel
    if(direction_sign > 0) w_rot -= 10;
    else w_rot += 10;
    if(w_rot > 90){
        w_rot = 0;
    }

} 


void Car::draw(){
    if(car_type == 1){
        draw_car_type_1();
    }
    if(car_type == 2){
        draw_car_type_2();
    }
    if(car_type == 3){
        draw_car_type_3();
    }
    if(car_type == 4){
        draw_car_type_4();
    }
    if(car_type == 5){
        draw_car_type_5();
    }
}

void Car::draw_generic_car(enum material mat){
    glPushMatrix();
        // Main frame
        glTranslatef(position.x, position.y - dimensions.y*0.4, position.z);
        glPushMatrix();
            load_material(mat);
            glPushMatrix(); // Main frame
                glScalef(dimensions.x, dimensions.y*0.3, dimensions.z*0.8);
                cube_unit(1,1,1);
            glPopMatrix();


            glPushMatrix(); // Front
                glTranslatef(direction_sign*dimensions.x*0.5, 0, 0);

                glPushMatrix(); // Head lights
                    glTranslatef(0, dimensions.y*0.15, 0);
                    load_material(HEAD_LIGHT);
                    glPushMatrix(); // Left head light
                        glTranslatef(0, 0, dimensions.z * 0.3);
                        glScalef(2, dimensions.y*0.2, dimensions.z*0.2);
                        cube_unit(1, 1, 0);
                    glPopMatrix();
                
                    load_material(HEAD_LIGHT);
                    glPushMatrix(); // Right head light
                        glTranslatef(0, 0, -dimensions.z * 0.3);
                        glScalef(2, dimensions.y*0.2, dimensions.z*0.2);
                        cube_unit(1, 1, 0);
                    glPopMatrix();
                glPopMatrix();

                load_material(RADIATOR);
                glPushMatrix(); // Radiator
                    glTranslatef(0, -dimensions.y*0.05, 0);
                    glScalef(2, dimensions.y*0.1, dimensions.z*0.6);
                    cube_unit(0.5, 0.5, 0.5);
                glPopMatrix(); // Head lights pop

            glPopMatrix(); // Front pop

            glPushMatrix(); // Back lights
                glTranslatef(-direction_sign*dimensions.x*0.5, dimensions.y*0.07, 0);

                load_material(HEAD_LIGHT);
                glPushMatrix(); // Left head light
                    glTranslatef(0, 0, dimensions.z * 0.3);
                    glScalef(2, dimensions.y*0.1, dimensions.z*0.15);
                    cube_unit(1, 1, 0);
                glPopMatrix();

                load_material(HEAD_LIGHT);
                glPushMatrix(); // Right head light
                    glTranslatef(0, 0, -dimensions.z * 0.3);
                    glScalef(2, dimensions.y*0.1, dimensions.z*0.15);
                    cube_unit(1, 1, 0);
                glPopMatrix();
            glPopMatrix(); // Back lights pop

            load_material(TIRE); // Black
            glPushMatrix(); // Door detail left
                glTranslatef(0, 0, -dimensions.z*0.4);
                glScalef(0.5, dimensions.y*0.3, 1);
                cube_unit(0, 0, 0);
            glPopMatrix();

            load_material(TIRE); // Black
            glPushMatrix(); // Door detail right
                glTranslatef(0, 0, dimensions.z*0.4);
                glScalef(0.5, dimensions.y*0.3, 1);
                cube_unit(0, 0, 0);
            glPopMatrix();

        glPopMatrix();

        // Top 
        glPushMatrix();
            glTranslatef(0, dimensions.y*0.3, 0);

            load_material(mat);            
            glPushMatrix(); // Top Frame
                glScalef(dimensions.x*0.8, dimensions.y*0.2, dimensions.z*0.6);
                cube_unit(1,1,1);
            glPopMatrix();
        
            load_material(WINDOW);
            glPushMatrix();// Side windows
                glScalef(dimensions.x*0.7, dimensions.y*0.15, dimensions.z*0.6+2);
                cube_unit(0, 1, 1);         
            glPopMatrix();

            load_material(WINDOW);
            glPushMatrix();// Front and back windows
                glScalef(dimensions.x*0.8+1.5, dimensions.y*0.15, dimensions.z*0.5);
                cube_unit(0, 1, 1);         
            glPopMatrix();
        glPopMatrix();

        // Wheels
        glPushMatrix();
            // Front wheels
            draw_wheel(ofVec3f(dimensions.x*0.4, -dimensions.y*0.1, dimensions.z*0.4));
            draw_wheel(ofVec3f(dimensions.x*0.4, -dimensions.y*0.1, -dimensions.z*0.4));
            
            // Back wheels
            draw_wheel(ofVec3f(-dimensions.x*0.4, -dimensions.y*0.1, dimensions.z*0.4));
            draw_wheel(ofVec3f(-dimensions.x*0.4, -dimensions.y*0.1, -dimensions.z*0.4));
        glPopMatrix();


    glPopMatrix();
}

void Car::draw_wheel(ofVec3f w_pos){
    glPushMatrix();
        glTranslatef(w_pos.x, w_pos.y, w_pos.z);
        
        // Tire
        load_material(TIRE);
        glPushMatrix();
            glScalef(w_dim.x, w_dim.y, w_dim.z);
            glRotatef(w_rot, 0, 0, 1);
            cube_unit(0, 0, 0);
        glPopMatrix();

        // Rim
        load_material(RIM);
        glPushMatrix();
            glScalef(w_dim.x/2, w_dim.y/2, w_dim.z+2);
            glRotatef(w_rot, 0, 0, 1);
            cube_unit(1, 1, 1);
        glPopMatrix();
    glPopMatrix();
}

void Car::draw_car_type_1(){
    draw_generic_car(CAR_BODY_1);
}

void Car::draw_car_type_2(){
    draw_generic_car(CAR_BODY_2);
}

void Car::draw_car_type_3(){
    draw_generic_car(CAR_BODY_3);
}

void Car::draw_car_type_4(){
    draw_generic_car(CAR_BODY_4);
}

void Car::draw_car_type_5(){
    draw_generic_car(CAR_BODY_5);
}
