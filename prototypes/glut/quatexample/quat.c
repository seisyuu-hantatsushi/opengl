#include <stdio.h>
#include <math.h>

#include "quat.h"

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
	return q->w*q->w+q->x*q->x+q->y*q->y+q->z*q->z;
}

float quat_norm(const quat *q){
	return sqrt(quat_norm_squar(q));
}

void print_quat(const quat *q){
	printf("(%f,%f,%f,%f)\n",
		   q->w,q->x,q->y,q->z);
	return;
}
