
#include <stdio.h>
#include "quat.h"

int main(int argc, char **argv){
	quat q;
	quat id;
	
	q = quat_zero();
	print_quat(&q);
	id = quat_identity();
	print_quat(&id);
	q = quat_set(1.0, 1.0, 1.0, 1.0);
	print_quat(&q);
	q = quat_mul(&q, &id);
	print_quat(&q);
	q = quat_mul(&q, &q);
	print_quat(&q);

	return 0;
}
