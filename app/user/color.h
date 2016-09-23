#ifndef __COLOR_H__
#define __COLOR_H__

void ICACHE_FLASH_ATTR RGB2HSV( uint16_t * h, uint16_t * s, uint16_t * v, uint8_t r, uint8_t g, uint8_t b);
void ICACHE_FLASH_ATTR HSV2RGB( uint8_t *r, uint8_t *g, uint8_t *b, uint16_t h, uint16_t s, uint16_t v);
void ICACHE_FLASH_ATTR RGB2HLS( double *h, double *l, double *s, uint8_t r, uint8_t g, uint8_t b);
void ICACHE_FLASH_ATTR HLS2RGB( uint8_t *r, uint8_t *g, uint8_t *b, double h, double l, double s);
double ICACHE_FLASH_ATTR HLS2RGBvalue(double n1,double n2, double hue);

#endif /* __COLOR_H__ */
