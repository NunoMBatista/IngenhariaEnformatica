#include "grid.h"
#include "../utils/cg_extras.h"
#include "../utils/cg_drawing_extras.h"
#include "materials.h"

Grid::Grid(int rows, int columns, float size) {
    grid_rows = rows;
    grid_columns = columns;
    grid_size = size;

    top_river_row = grid_rows - 3;
    bottom_river_row = top_river_row - 4;

    top_road_row = 6;
    bottom_road_row = 1;

    water_offset = 0.0f;
}

void Grid::draw(){
    glPushMatrix();
        glTranslatef(0, -grid_size/2, 0);
        for(int i = 0; i < grid_rows; i++){
            for(int j = -1; j <= grid_columns; j++){
                ofVec3f position = get_grid_position(i, j);
                glPushMatrix();
                    glTranslatef(position.x, position.y, position.z);
                    // Draw the road
                    if((i <= top_road_row) && (i >= bottom_road_row)){
                        
                            glRotatef(90, 1, 0, 0);
                            glScalef(grid_size, grid_size, 1);
                            load_material(ROAD);
                            glEnable(GL_TEXTURE_2D);

                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            global.cobble.bind();

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                            rect_texture_unit(2, 2);

                        global.cobble.unbind();
                        glDisable(GL_TEXTURE_2D);
                    }
                    // Draw the river
                    else if((i <= top_river_row) && (i >= bottom_river_row)){
                        // Update texture offset
                        //water_offset += 0.01f; 
                        water_offset += 0.005f * ofGetLastFrameTime();

                        glRotatef(90, 1, 0, 0);
                        glScalef(grid_size, grid_size, 1);
                        glMatrixMode(GL_TEXTURE);
                        glPushMatrix();
                            glTranslatef(water_offset, 0.0f, 0.0f); 


                            glPushMatrix();
                                load_material(WATER);
                                glEnable(GL_TEXTURE_2D);
                                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                global.water_tex.bind();

                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                                cube_texture_unit(1, 1);
                            glPopMatrix();

                            global.water_tex.unbind();
                            glDisable(GL_TEXTURE_2D);

                        glPopMatrix();
                        glMatrixMode(GL_MODELVIEW);
                    }
                    // Draw the final row (green)
                    else if(i == grid_rows - 1){
                        glScalef(grid_size, grid_size*4, grid_size);
                        //cube_unit(0, 0.6, 0);
                        glPushMatrix();
                            load_material(GREEN_GRASS);
                            // Add texture
                            glEnable(GL_TEXTURE_2D);
                            

                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            global.green_grass.bind();

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                            //glScalef(grid_size, grid_size, grid_size);
                            cube_texture_unit(1, 4);
                        glPopMatrix();
                        global.green_grass.unbind();
                        glDisable(GL_TEXTURE_2D);
                    }
                    // Draw the goal row (green and yellow)
                    else if(i == grid_rows - 2 && j >= 0 && j < grid_columns){
                        if((j - 1) % 3 == 0){
                            glScalef(grid_size, 1, grid_size);
                        
                            if(global.filled_slots[j]){
                                load_material(GOLD_PARTICLE);
                                cube_unit(1, 1, 0);
                            }
                            else{
                                glPushMatrix();
                                    load_material(TURTLE_SKIN);
                                    glEnable(GL_TEXTURE_2D);
                                    

                                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                    global.green_grass.bind();

                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                                    cube_texture_unit(2, 2);
                                glPopMatrix();
                                global.green_grass.unbind();
                                glDisable(GL_TEXTURE_2D);
                            }
                        }
                        else{
                            glScalef(grid_size, grid_size*4, grid_size);
                            glPushMatrix();
                                load_material(GREEN_GRASS);
                                // Add texture
                                glEnable(GL_TEXTURE_2D);
                                
                                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                global.green_grass.bind();

                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                                //glScalef(grid_size, grid_size, grid_size);
                                cube_texture_unit(1, 4);
                            glPopMatrix();
                            global.green_grass.unbind();
                            glDisable(GL_TEXTURE_2D);
                        }
                    }
                    else if(i == grid_rows - 2 && (j == -1 || j == grid_columns)){
                        glScalef(grid_size, grid_size, grid_size);
                        glPushMatrix();
                            load_material(TUNNEL);
                            // Add texture
                            glEnable(GL_TEXTURE_2D);
                            glEnable(GL_LIGHTING);
                            

                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            global.tunnel.bind();

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                            //glScalef(grid_size, grid_size, grid_size);
                            cube_texture_unit(1, 1);
                        glPopMatrix();
                        global.tunnel.unbind();
                        glDisable(GL_TEXTURE_2D);
                        glEnable(GL_LIGHTING);
                    }
                    // Draw the grass (purple)
                    else{
                        //load_material(PURPLE_GRASS);
                        load_material(BRICK);

                        // Add texture
                        glEnable(GL_TEXTURE_2D);
                        glEnable(GL_LIGHTING);
                        

                        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        global.brick.bind();

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                        //glRotatef(90, 1, 0, 0);
                        glScalef(grid_size, 0.2, grid_size);
                        cube_texture_unit(1, 1);
                        //rect_texture_unit(2);
                    
                        global.brick.unbind();
                        glDisable(GL_TEXTURE_2D);
                        glEnable(GL_LIGHTING);
                    }
                glPopMatrix();
            }
        }
    glPopMatrix();

    // Draw the barrier around the grid
    glPushMatrix();
        for (int i = 0; i <= grid_rows + 2; i++) {
            for (int j = -6; j <= grid_columns + 5; j++) {

                // Skip the inner grid
                if (!(i < 0 || i >= grid_rows || j < 0 || j >= grid_columns)) 
                    continue;
    
                ofVec3f position = get_grid_position(i, j);
                glPushMatrix();
                    glTranslatef(position.x, position.y, position.z);
                    
                    glScalef(grid_size, grid_size, grid_size);
                    if(i < bottom_river_row-1 && i > 0){
                        glPushMatrix();
                            glTranslatef(0, -2, 0);
                            load_material(PUPIL);
                            cube_unit(0, 0, 0);
                        glPopMatrix();
                    }  

                    else if(i >= bottom_river_row && i <= top_river_row){
                        glTranslatef(0, -0.5, 0);
                        glRotatef(90, 1, 0, 0);
                        glMatrixMode(GL_TEXTURE);
                        glPushMatrix();
                            glTranslatef(water_offset, 0.0f, 0.0f); 

                            glPushMatrix();
                                load_material(WATER);
                                glEnable(GL_TEXTURE_2D);
                                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                global.water_tex.bind();

                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                                cube_texture_unit(1, 1);
                            glPopMatrix();

                            global.water_tex.unbind();
                            glDisable(GL_TEXTURE_2D);

                        glPopMatrix();
                        glMatrixMode(GL_MODELVIEW);


                    }
                    else if(i >= top_river_row && j >= 0 && j < grid_columns){
                        load_material(GREEN_GRASS);
                        cube_unit(0, 0.6, 0);
                    }
                    else{
                        glPushMatrix();
                            load_material(TUNNEL);
                            // Add texture
                            glEnable(GL_TEXTURE_2D);
                            glEnable(GL_LIGHTING);
                            

                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            global.tunnel.bind();

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                            //glScalef(grid_size, grid_size, grid_size);
                            cube_texture_unit(1, 1);
                        glPopMatrix();
                        global.tunnel.unbind();
                        glDisable(GL_TEXTURE_2D);
                        glEnable(GL_LIGHTING);
                    }

                glPopMatrix();
            }
        }
    glPopMatrix();
    
    // Add grey blocks on top of the blue and black barrier blocks
    glPushMatrix();
        for (int i = 0; i <= grid_rows + 2; i++) {
            for (int j = -6; j <= grid_columns + 5; j++) {

                // Skip the inner grid
                if (!(i < 0 || i >= grid_rows || j < 0 || j >= grid_columns)){
                    continue;
                }
                
                if ((i <= top_road_row && i >= bottom_road_row) || (i <= top_river_row && i >= bottom_river_row)) {
                    ofVec3f position = get_grid_position(i, j);
                    glPushMatrix();
                        glTranslatef(position.x, position.y + grid_size, position.z);
                        glScalef(grid_size, grid_size, grid_size);

                        glPushMatrix();
                            load_material(TUNNEL);
                            // Add texture
                            glEnable(GL_TEXTURE_2D);
                            glEnable(GL_LIGHTING);
                            

                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                            global.tunnel.bind();

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                            //glScalef(grid_size, grid_size, grid_size);
                            cube_texture_unit(1, 1);
                        glPopMatrix();
                        global.tunnel.unbind();
                        glDisable(GL_TEXTURE_2D);
                        glEnable(GL_LIGHTING);
                    glPopMatrix();
                }

                // Draw the tunnel entrances one block off
                if((i <= top_road_row) && (i >= bottom_road_row) && (j < -1 || j > grid_columns)){
                    ofVec3f position = get_grid_position(i, j);
                    glPushMatrix();
                        glTranslatef(position.x, position.y, position.z);
                        glScalef(grid_size, grid_size, grid_size);
                        load_material(PUPIL);
                        cube_unit(0, 0, 0);
                    glPopMatrix();
                }

                // Draw the waterfall entrances one block off
                if((i <= top_river_row) && (i >= bottom_river_row) && (j < -1 || j > grid_columns)){
                    ofVec3f position = get_grid_position(i, j);
                    glPushMatrix();
                        glTranslatef(position.x, position.y, position.z);
                        glRotatef(90, 0, 1, 0);
                        glScalef(grid_size, grid_size, 1);
                        glMatrixMode(GL_TEXTURE);
                        glPushMatrix();
                            glTranslatef(0, -water_offset, 0); 
                            glPushMatrix();
                                load_material(WATER);
                                glEnable(GL_TEXTURE_2D);
                                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                global.water_tex.bind();

                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                                cube_texture_unit(1, 1);
                            glPopMatrix();

                            global.water_tex.unbind();
                            glDisable(GL_TEXTURE_2D);

                        glPopMatrix();
                        glMatrixMode(GL_MODELVIEW);
                    glPopMatrix();
                }
            }
        }
    glPopMatrix();
    
}


// Get the world position from a grid row and column
ofVec3f Grid::get_grid_position(int row, int column) {
    GLfloat x = column * grid_size;
    GLfloat z = row * grid_size;
    return ofVec3f(x, 0, z);
}


// Get the grid row and column from a world position
ofVec2f Grid::get_grid_row_column(ofVec3f position){
    int row = ceil(position.z / global.grid_size);
    int column = ceil(position.x / global.grid_size);
    return ofVec2f(row, column);
}

bool Grid::is_valid(int row, int column) {
    if (row == global.grid->top_river_row + 1){
        if((column - 1) % 3 == 0){
            return true;
        }
        return false;
    }

    return row >= 0 && row < grid_rows && column >= 0 && column < grid_columns;
}

int Grid::closest_column(ofVec3f position){
    return round(position.x / grid_size);
}