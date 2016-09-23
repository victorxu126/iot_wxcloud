#include "c_types.h"
#include "osbean.h"

ICACHE_FLASH_ATTR double floor(double x)
{
    return (double) (x < 0.f ? (((int) x) - 1) : ((int) x));
}

ICACHE_FLASH_ATTR double pow(double x, double y){
	double ret=1;
	int i;
	for(i=1; i<(int)y; i++){
		ret *= x;
	}
	return ret;
}
ICACHE_FLASH_ATTR double  fabs(double x){
	if(x<0)
		return (-1*x);
	else
		return x;
}
