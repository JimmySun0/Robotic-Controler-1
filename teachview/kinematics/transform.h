#ifndef KINEMATICS_H
#define KINEMATICS_H

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include <Eigen/Dense>
using namespace Eigen;

typedef Vector3d point;
typedef Vector3d direction;

/* the transformation between Homogeneous transformation matrix and Rotation Matrix */
Matrix3d tr2ro(Matrix4d Trans);

/* the transformtion between Homogeneous transformstion matrix and eul angle(ZYZ) or rpy angle,
	the default units is rad - mode = 'r'; degree - mode = 'd' */
Vector3d tr2eul(Matrix4d Trans, char mode = 'r');
Vector3d ro2eul(Matrix3d Rotation, char mode = 'r');

Vector3d tr2rpy(Matrix4d Trans, char mode = 'r');
Vector3d ro2rpy(Matrix3d Rotation, char mode = 'r');

Matrix3d eul2ro(Vector3d eul, char mode = 'r');
Matrix3d rpy2ro(Vector3d rpy, char mode = 'r');

#endif // !KINEMATICS_H