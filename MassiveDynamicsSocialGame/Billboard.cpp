#include <math.h>
#include "gl\glut.h"
#include "maths.h"

/* --------------------------------------------------------
Simplest form of Sperical Billboarding. It is not real billboarding
since the object won't face the camera in the general case.
The object will face a plane perpendicular to the cameras
"look at" vector. It is the fastest of them all though.
---------------------------------------------------------*/

void BillboardCheatSphericalBegin() {

	float modelview[16];
	int i, j;

	// save the current modelview matrix
	glPushMatrix();

	// get the current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	// undo all rotations
	// beware all scaling is lost as well 
	for (i = 0; i<3; i++)
	for (j = 0; j<3; j++) {
		if (i == j)
			modelview[i * 4 + j] = 1.0;
		else
			modelview[i * 4 + j] = 0.0;
	}

	// set the modelview with no rotations
	glLoadMatrixf(modelview);
}
