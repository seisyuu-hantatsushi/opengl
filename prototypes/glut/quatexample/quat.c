#include <stdio.h>
#include <math.h>

#include "quat.h"

#define SQR(x) (x*x)

quat quat_zero(){
	quat q = {
		.w = 0,
		.x = 0,
		.y = 0,
		.z = 0
	};
	return q;
}

quat quat_identity(){
	quat q = {
		.w = 1,
		.x = 0,
		.y = 0,
		.z = 0
	};
	return q;
}

quat quat_set(float w, float x, float y, float z){
	quat q = {
		.w = w,
		.x = x,
		.y = y,
		.z = z
	};
	return q;
}

quat quat_add(const quat *ql, const quat *qr){
	quat q;
	q.w = ql->w + qr->w;
	q.x = ql->x + qr->x;
	q.y = ql->y + qr->y;
	q.z = ql->z + qr->z;
	return q;
}

quat quat_sub(const quat *ql, const quat *qr){
	quat q;
	q.w = ql->w - qr->w;
	q.x = ql->x - qr->x;
	q.y = ql->y - qr->y;
	q.z = ql->z - qr->z;
	return q;
}

quat quat_mul(const quat *ql, const quat *qr){
	quat q;
	q.w = ql->w * qr->w - ql->x * qr->x - ql->y * qr->y - ql->z * qr->z;
	q.x = ql->w * qr->x + ql->x * qr->w + ql->y * qr->z - ql->z * qr->y;
	q.y = ql->w * qr->y - ql->x * qr->z + ql->y * qr->w + ql->z * qr->x;
	q.z = ql->w * qr->z + ql->x * qr->y - ql->y * qr->x + ql->z * qr->w;
	return q;
}

quat quat_mul_real(float s, const quat *q){
	quat rq;
	rq.w = s * q->w;
	rq.x = s * q->x;
	rq.y = s * q->y;
	rq.z = s * q->z;
	return rq;
}

quat quat_div_real(float s, const quat *q){
	quat rq;
	rq.w = q->w/s;
	rq.x = q->x/s;
	rq.y = q->y/s;
	rq.z = q->z/s;
	return rq;
}

float quat_norm_squar(const quat *q){
	return SQR(q->w)+SQR(q->x)+SQR(q->y)+SQR(q->z);
}

float quat_norm(const quat *q){
	return sqrt(quat_norm_squar(q));
}

void quat_to_rotation_matrix(const quat *q, matrix4x4 m){

	m[0][0] = 1.0 - 2.0 * (SQR(q->y)   + SQR(q->z));
	m[0][1] =       2.0 * (q->x * q->y - q->z * q->w);
	m[0][2] =       2.0 * (q->z * q->x + q->w * q->y);
	m[0][3] = 0.0;

	m[1][0] =       2.0 * (q->x * q->y + q->z * q->w);
	m[1][1] = 1.0 - 2.0 * (SQR(q->z)   + SQR(q->x));
	m[1][2] =       2.0 * (q->y * q->z - q->w * q->x);
	m[1][3] = 0.0;

	m[2][0] =       2.0 * (q->z * q->x - q->w * q->y);
	m[2][1] =       2.0 * (q->y * q->z + q->x * q->w);
	m[2][2] = 1.0 - 2.0 * (SQR(q->y)   + SQR(q->x));
	m[2][3] = 0.0;

	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;

	return;
}

void print_quat(const quat *q){
	printf("(%f,%f,%f,%f)\n",
		   q->w,q->x,q->y,q->z);
	return;
}

