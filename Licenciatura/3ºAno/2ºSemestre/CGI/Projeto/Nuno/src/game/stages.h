#pragma once

#include "ofMain.h"
#include "car.h"
#include "platform.h"
#include "../utils/cg_extras.h"
#include "../utils/cg_drawing_extras.h"
#include <vector>

inline void stage_1(std::vector<Car*> &cars, std::vector<Platform*> &platforms){
    // First row of cars 
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 1), ofVec3f(-1, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 5), ofVec3f(-1, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 9), ofVec3f(-1, 0, 0)));

    // Second row of cars
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 1), ofVec3f(1, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 9), ofVec3f(1, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 13), ofVec3f(1, 0, 0)));

    // Third row of cars
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 2), ofVec3f(-1 * 0.8, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 6), ofVec3f(-1 * 0.8, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 10), ofVec3f(-1 * 0.8, 0, 0)));

    // Fourth row of cars
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 7), ofVec3f(1 * 2.5, 0, 0)));

    // Fifth row of cars
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 3), ofVec3f(-1 * 1.5, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 8), ofVec3f(-1 * 1.5, 0, 0)));

    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 1), ofVec3f(1*1.2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 6), ofVec3f(1*1.2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 10), ofVec3f(1*1.2, 0, 0)));

    // First row of platforms
    for(int i = 0; i < 16; i++){
        if(i % 4 == 0){
            continue;
        }
        platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row, i), ofVec3f(-1 * 1, 0, 0)));

    }

    // Second row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 3), ofVec3f(1 * 0.5, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 10), ofVec3f(1 * 0.5, 0, 0)));

    // Third row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 5, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 1), ofVec3f(1 * 1.8, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 5, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 8), ofVec3f(1 * 1.8, 0, 0)));

    // Fourth row of platforms
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 7), ofVec3f(-1 * 1.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 8), ofVec3f(-1 * 1.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 9), ofVec3f(-1 * 1.5, 0, 0)));

    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 12), ofVec3f(-1 * 1.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 13), ofVec3f(-1 * 1.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 14), ofVec3f(-1 * 1.5, 0, 0)));

    // Fifth row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 2), ofVec3f(1 * 0.75, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 7), ofVec3f(1 * 0.75, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 13), ofVec3f(1 * 0.75, 0, 0)));
}

inline void stage_2(std::vector<Car*> &cars, std::vector<Platform*> &platforms){
    // First row of cars 
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 1), ofVec3f(-1 * 1.3, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 5), ofVec3f(-1 * 1.3, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 9), ofVec3f(-1 * 1.3, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 11), ofVec3f(-1 * 1.3, 0, 0)));

    // Second row of cars
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 1), ofVec3f(1 * 1.3, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 5), ofVec3f(1 * 1.3, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 9), ofVec3f(1 * 1.3, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 13), ofVec3f(1 * 1.3, 0, 0)));

    // Third row of cars
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 1), ofVec3f(-1 * 1.2, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 6), ofVec3f(-1 * 1.2, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 11), ofVec3f(-1 * 1.2, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 13), ofVec3f(-1 * 1.2, 0, 0)));

    // Fourth row of cars
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 7), ofVec3f(1 * 3, 0, 0)));
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 9), ofVec3f(1 * 3, 0, 0)));

    // Fifth row of cars
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 3), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 6), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 11), ofVec3f(-1 * 2, 0, 0)));

    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 1), ofVec3f(1 * 1.7, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 6), ofVec3f(1 * 1.7, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 10), ofVec3f(1 * 1.7, 0, 0)));

    // First row of platforms
    for(int i = 0; i < 16; i++){
        if(i % 4 == 0){
            continue;
        }
        platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row, i), ofVec3f(-1 * 1.5, 0, 0)));
    }

    // Second row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 3), ofVec3f(1 * 1, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 10), ofVec3f(1 * 1, 0, 0)));

    // Third row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 5, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 1), ofVec3f(1 * 2.3, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 5, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 8), ofVec3f(1 * 2.3, 0, 0)));

    // Fourth row of platforms
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 7), ofVec3f(-1 * 2, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 8), ofVec3f(-1 * 2, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 9), ofVec3f(-1 * 2, 0, 0)));

    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 12), ofVec3f(-1 * 2, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 13), ofVec3f(-1 * 2, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 14), ofVec3f(-1 * 2, 0, 0)));

    // Fifth row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 2), ofVec3f(1 * 1.25, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 7), ofVec3f(1 * 1.25, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 3, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 13), ofVec3f(1 * 1.25, 0, 0)));
}

inline void stage_3(std::vector<Car*> &cars, std::vector<Platform*> &platforms){
    // First row of cars 
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 1), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 5), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 9), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 11), ofVec3f(-1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(1, 13), ofVec3f(-1 * 2, 0, 0)));

    // Second row of cars
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 1), ofVec3f(1 * 1.5, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 4), ofVec3f(1 * 1.5, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 7), ofVec3f(1 * 1.5, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 10), ofVec3f(1 * 1.5, 0, 0)));
    cars.push_back(new Car(2, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(2, 13), ofVec3f(1 * 1.5, 0, 0)));

    // Third row of cars
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 1), ofVec3f(-1 * 1.4, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 5), ofVec3f(-1 * 1.4, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 9), ofVec3f(-1 * 1.4, 0, 0)));
    cars.push_back(new Car(3, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(3, 13), ofVec3f(-1 * 1.4, 0, 0)));

    // Fourth row of cars
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 3), ofVec3f(1 * 3.5, 0, 0)));
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 7), ofVec3f(1 * 3.5, 0, 0)));
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 11), ofVec3f(1 * 3.5, 0, 0)));
    cars.push_back(new Car(4, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(4, 14), ofVec3f(1 * 3.5, 0, 0)));

    // Fifth row of cars
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 2), ofVec3f(-1 * 2.5, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 5), ofVec3f(-1 * 2.5, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 8), ofVec3f(-1 * 2.5, 0, 0)));
    cars.push_back(new Car(5, ofVec3f(global.grid_size * 2, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(5, 11), ofVec3f(-1 * 2.5, 0, 0)));

    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 1), ofVec3f(1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 5), ofVec3f(1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 9), ofVec3f(1 * 2, 0, 0)));
    cars.push_back(new Car(1, ofVec3f(global.grid_size * 0.9, global.grid_size * 0.75, global.grid_size * 0.75), global.grid->get_grid_position(6, 13), ofVec3f(1 * 2, 0, 0)));

    // First row of platforms
    for(int i = 0; i < 16; i++){
        if(i % 4 == 0){
            continue;
        }
        platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row, i), ofVec3f(-1 * 2, 0, 0)));
    }

    // Second row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 2, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 3), ofVec3f(1 * 1.5, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 2, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 1, 10), ofVec3f(1 * 1.5, 0, 0)));

    // Third row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 4, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 1), ofVec3f(1 * 3, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 4, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 2, 8), ofVec3f(1 * 3, 0, 0)));

    // Fourth row of platforms
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 7), ofVec3f(-1 * 2.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 8), ofVec3f(-1 * 2.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 9), ofVec3f(-1 * 2.5, 0, 0)));

    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 12), ofVec3f(-1 * 2.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 13), ofVec3f(-1 * 2.5, 0, 0)));
    platforms.push_back(new Platform(2, ofVec3f(global.grid_size * 1, global.grid_size * 0.5, global.grid_size * 0.75), global.grid->get_grid_position(global.grid->bottom_river_row + 3, 14), ofVec3f(-1 * 2.5, 0, 0)));

    // Fifth row of platforms
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 2, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 2), ofVec3f(1 * 1.75, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 2, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 7), ofVec3f(1 * 1.75, 0, 0)));
    platforms.push_back(new Platform(1, ofVec3f(global.grid_size * 2, global.grid_size * 0.5, global.grid_size * 0.5), global.grid->get_grid_position(global.grid->bottom_river_row + 4, 13), ofVec3f(1 * 1.75, 0, 0)));
}