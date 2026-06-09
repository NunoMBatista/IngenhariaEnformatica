
#pragma once

#include "ofMain.h"
#include "grid.h"

class Global {
public:
    Global();

    int grid_rows;
    int grid_columns;
    float grid_size;

    Grid* grid;

    GLfloat left_out_of_bounds;
    GLfloat right_out_of_bounds;

    GLfloat base_element_speed;    
    GLfloat platform_offset_y;

    std::bitset<20> filled_slots;

    void load_textures();
    ofImage life;
    ofImage brick;
    ofImage cobble;
    ofImage tunnel;
    ofImage green_grass;
    ofImage water_tex;
    ofImage log_tex;
    ofImage turtle_tex;
};


extern Global global;