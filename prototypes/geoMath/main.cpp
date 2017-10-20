#include <stdio.h>

#include <geoMath.hpp>

int main(int argc, char **argv){
	GeoMath::Vector<float> a(1.0,1.0,1.0);
	GeoMath::Vector<float> b(2.0,2.0,2.0);
	GeoMath::Vector<float> c;
	printf("hello world!!\n");
	c = a + b;
	printf("(%f,%f,%f)\n",c[0],c[1],c[2]);
	c = GeoMath::normalize<float>(c);
	printf("(%f,%f,%f)\n",c[0],c[1],c[2]);
	printf("%f\n",GeoMath::length<float>(c));
	printf("%f\n",GeoMath::dot(a,b));
	
	return 0;
}
