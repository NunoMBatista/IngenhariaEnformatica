#pragma once

// #include "ofMain.h"
// #include "cg_extras.h"
#include "ofApp.h"
#include "materials.h"

//desenha ponto 3D na origem
inline void drawPoint() {
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();
}

//função que desenha quadrado unitário 
//centrado na origem e preenchido
inline void rectFill_unit() {
	glBegin(GL_QUADS);
	glVertex3d(-0.5, -0.5, 0.);
	glVertex3d(-0.5, 0.5, 0.);
	glVertex3d(0.5, 0.5, 0.);
	glVertex3d(0.5, -0.5, 0.);
	glEnd();
}

inline void rect_texture_unit_aux(int N, int M) {
	// Bottom left - start of tiling
	glTexCoord2f(0, M);
	glVertex3f(-0.5, -0.5, 0);
	
	// Top left - M tiles up
	glTexCoord2f(0, 0);
	glVertex3f(-0.5, 0.5, 0);
	
	// Top right - N tiles across and M tiles up  
	glTexCoord2f(N, 0);
	glVertex3f(0.5, 0.5, 0);

	// Bottom right - N tiles across and M tiles down
	glTexCoord2f(N, M); 
	glVertex3f(0.5, -0.5, 0);
}

inline void rect_texture_unit(int N, int M) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		rect_texture_unit_aux(N, M);
	glEnd();
}

inline void cube_texture_unit(int N, int M) {
	glEnable(GL_NORMALIZE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPushMatrix();
		glTranslatef(0, 0, 0.5);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 0, -0.5);
		glRotatef(180, 0, 1, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 0.5, 0);
		glRotatef(90, 1, 0, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, -0.5, 0);
		glRotatef(270, 1, 0, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.5, 0, 0);
		glRotatef(90, 0, 1, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-0.5, 0, 0);
		glRotatef(270, 0, 1, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			rect_texture_unit_aux(N, M);
		glEnd();
	glPopMatrix();
}

inline void cube_unit_outline() {
	GLfloat p = 0.5;
    glColor3f(0, 0, 0);
	load_material(PUPIL); // Black
	// Draw filled cube
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
		// Front
		glNormal3f(0, 0, 1);
		glVertex3f(-p, -p, p);
		glVertex3f(-p, p, p);
		glVertex3f(p, p, p);
		glVertex3f(p, -p, p);

		// Back
		glNormal3f(0, 0, -1);	
		glVertex3f(-p, -p, -p);
		glVertex3f(p, -p, -p);
		glVertex3f(p, p, -p);
		glVertex3f(-p, p, -p);

		// Top
		glNormal3f(0, 1, 0);
		glVertex3f(-p, -p, -p);
		glVertex3f(-p, -p, p);
		glVertex3f(p, -p, p);
		glVertex3f(p, -p, -p);

		// Bottom
		glNormal3f(0, -1, 0);
		glVertex3f(-p, p, p);
		glVertex3f(-p, p, -p);
		glVertex3f(p, p, -p);
		glVertex3f(p, p, p);

		// Left
		glNormal3f(-1, 0, 0);
		glVertex3f(-p, -p, p);
		glVertex3f(-p, -p, -p);
		glVertex3f(-p, p, -p);
		glVertex3f(-p, p, p);

		// Right
		glNormal3f(1, 0, 0);
		glVertex3f(p, -p, p);
		glVertex3f(p, p, p);
		glVertex3f(p, p, -p);
		glVertex3f(p, -p, -p);
	glEnd();

}

inline void cube_unit(GLfloat R = 0, GLfloat G = 1, GLfloat B = 0) {
	GLfloat p = 0.5;
	//glColor3f(R, G, B);
	//glColor3f(1, 1, 1);


	// Draw filled cube
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
		// Front
		glNormal3f(0, 0, 1);
		glVertex3f(-p, -p, p);
		glVertex3f(-p, p, p);
		glVertex3f(p, p, p);
		glVertex3f(p, -p, p);

		// Back
		glNormal3f(0, 0, -1);
		glVertex3f(-p, -p, -p);
		glVertex3f(p, -p, -p);
		glVertex3f(p, p, -p);
		glVertex3f(-p, p, -p);

		// Top
		glNormal3f(0, 1, 0);
		glVertex3f(-p, -p, -p);
		glVertex3f(-p, -p, p);
		glVertex3f(p, -p, p);
		glVertex3f(p, -p, -p);

		// Bottom
		glNormal3f(0, -1, 0);
		glVertex3f(-p, p, p);
		glVertex3f(-p, p, -p);
		glVertex3f(p, p, -p);
		glVertex3f(p, p, p);

		// Left
		glNormal3f(-1, 0, 0);
		glVertex3f(-p, -p, p);
		glVertex3f(-p, -p, -p);
		glVertex3f(-p, p, -p);
		glVertex3f(-p, p, p);

		// Right
		glNormal3f(1, 0, 0);
		glVertex3f(p, -p, p);
		glVertex3f(p, p, p);
		glVertex3f(p, p, -p);
		glVertex3f(p, -p, -p);
	glEnd();

	//cube_unit_outline();
}

