#ifndef __QUAT_H_
#define __QUAT_H_

typedef struct QUAT {
	float w, x, y, z;
} quat;

quat quat_zero();
quat quat_identity();
quat quat_set(float w, float x, float y, float z);

quat quat_add(const quat *ql, const quat *qr);
quat quat_sub(const quat *ql, const quat *qr);
quat quat_mul(const quat *ql, const quat *qr);

quat quat_mul_real(float s, const quat *q);
quat quat_div_real(float s, const quat *q);
float quat_norm_squar(const quat *q);
float quat_norm(const quat *q);

void print_quat(const quat *q);

#endif /* __QUAT_H_ */
