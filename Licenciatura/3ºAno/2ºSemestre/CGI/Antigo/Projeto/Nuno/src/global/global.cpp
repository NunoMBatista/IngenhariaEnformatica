
#include "global.h"

Global global; 

Global::Global() {
    grid_rows = 15;
    grid_columns = 15;
    grid_size = 50.0f;

    grid = new Grid(grid_rows, grid_columns, grid_size);

    left_out_of_bounds = grid->get_grid_position(0, -3).x;
    right_out_of_bounds = grid->get_grid_position(1, grid_columns+3).x;

    base_element_speed = 50.0f;
    platform_offset_y = 10.0f;
}

void Global::load_textures(){
    // Load the textures
    life.load("life.png");
    life.setImageType(OF_IMAGE_COLOR_ALPHA);

    brick.load("brick.png");

    tunnel.load("tunnel.png");

    green_grass.load("green_grass.jpg");

    cobble.load("cobble.png");

    water_tex.load("water.png");

    log_tex.load("log.jpg");

    turtle_tex.load("turtle_shell.png");
}