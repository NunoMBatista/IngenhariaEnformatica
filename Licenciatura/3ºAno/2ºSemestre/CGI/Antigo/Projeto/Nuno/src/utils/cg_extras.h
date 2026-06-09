/*CG_EXTRAS
collection of functions to help speed up programming in OF

Developed for the Computer Graphics course @:
Informatics Engineering Department
University of Coimbra
by
André Perrotta & Evgheni Polisciuc

Coimbra, 09/2023
*/

#ifndef CG_EXTRAS_H
#define CG_EXTRAS_H

#include "ofMain.h"

enum Direction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

//short named function to get screen width
inline float gw() {
	return ofGetWidth();
}

//short named function to get screen height
inline float gh() {
	return ofGetHeight();
}

//short named function to put Frame Rate as window title
inline void setFrWt() {
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//printa a ModelViewMatrix
inline void coutModelviewMatrix() {
	
	GLfloat Matriz[4][4];
	cout << endl << "Modelview Matrix" << endl;
	glGetFloatv(GL_MODELVIEW_MATRIX, &Matriz[0][0]);
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << Matriz[j][i] << " ";
		}
		cout << endl;
	}
	//return;
}

//print projectio matrix
inline void coutProjectionMatrix() {

	GLfloat Matriz[4][4];
	cout << endl << "Projection Matrix" << endl;
	glGetFloatv(GL_PROJECTION_MATRIX, &Matriz[0][0]);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << Matriz[j][i] << " ";
		}
		cout << endl;
	}
	//return;
}

//pega os valores de Transla��o da matriz modelview
inline ofVec3f getModelViewMatrixPos() {
	GLfloat Matriz[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, &Matriz[0][0]);
	ofVec3f aux;
	aux.x = Matriz[3][0];
	aux.y = Matriz[3][1];
	aux.z = Matriz[3][2];
	//cout << endl << aux.x<<" "<<aux.y<<" "<<aux.z;
	return aux;
}

//print de matrix 4x4
inline void printMatrix(GLfloat m[][4]) {
	cout << endl << "Matrix 4x4" << endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << m[j][i] << " ";
		}
		cout << endl;
	}
	cout << endl << "matrix" << endl;
}

inline ofVec3f cross(ofVec3f A, ofVec3f B) {
	ofVec3f aux;
	aux.x = A.y * B.z - A.z * B.y;
	aux.y = A.z * B.x - A.x * B.z;
	aux.z = A.x * B.y - A.y * B.x;

	return aux;
}

inline void getRotationFromModelView(GLfloat mIn[][4], GLfloat mOut[][4]) {
	GLfloat identity[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	copy(&identity[0][0], &identity[0][0] + 16, &mOut[0][0]);
	copy(&mIn[0][0], &mIn[0][0] + 12, &mOut[0][0]);
	printMatrix(mIn);
	printMatrix(mOut);
}

inline void getTranslationFromModelView(GLfloat mIn[][4], GLfloat mOut[][4]) {
	GLfloat identity[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	copy(&identity[0][0], &identity[0][0] + 16, &mOut[0][0]);
	copy(&mIn[0][0] + 13, &mIn[0][0] + 16, &mOut[0][0]);
	//printMatrix(mIn);
	//printMatrix(mOut);
}




#endif