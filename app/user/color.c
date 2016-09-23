#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "color.h"

#define SCALE   1
#define GETA    1
#define H_SCALE  1
#define H_GETA  1
#ifndef MAX
#define MAX(x,y) (__extension__({typeof(x) __X=(x);typeof(y) __Y=(y);__X>__Y?__X:__Y;}))
#endif
#ifndef MIN
#define MIN(x,y) (__extension__({typeof(x) __X=(x);typeof(y) __Y=(y);__X<__Y?__X:__Y;}))
#endif

void ICACHE_FLASH_ATTR RGB2HSV( uint16_t * h, uint16_t * s, uint16_t * v, uint8_t r, uint8_t g, uint8_t b)
{
    double rr, gg, bb;
    double hh, ss, vv;
    double cmax, cmin, cdes;
 
    rr = r;
    gg = g;
    bb = b;
 
    cmax = (rr > gg) ? rr : gg;
    if (bb > cmax) {
        cmax = bb;
    }
 
    cmin = (rr < gg) ? rr : gg;
    if (bb < cmin) {
        cmin = bb;
    }
 
    cdes = cmax - cmin;
    vv = cmax;
    if (cdes != 0) {
        ss = cdes * SCALE / cmax;
        if (cmax == rr) {
            hh = (gg - bb) * SCALE / cdes;
        }else if (cmax == gg) {
            hh = (bb - rr) * SCALE / cdes + 2 * H_SCALE;
        }else {
            hh = (rr - gg) * SCALE / cdes + 4 * H_SCALE;
        }
    }else if (cmax != 0) {
        ss = cdes * SCALE / cmax;
        hh = 0;
    }else {
        ss = 0;
        hh = 0;
    }
    if (hh < 0) {
        hh += 6 * H_SCALE;
    }
 
    *h = hh * H_GETA;
    *s = ss * H_GETA;
    *v = vv * H_GETA;
}
 
void ICACHE_FLASH_ATTR HSV2RGB( uint8_t *r, uint8_t *g, uint8_t *b, uint16_t h, uint16_t s, uint16_t v)
{
    double rr = 0, gg = 0, bb = 0;
    double hh, ss, vv;
 
    if (h == 6 * H_GETA * H_SCALE) {
        h = 0;
    }
    hh = (double)h / H_GETA;
    ss = (double)s / GETA;
    vv = (double)v / GETA;
 
    switch((int)(hh / H_SCALE)) {
        case 0:
            rr = SCALE;
            gg = hh;
            bb = 0;
            break;
        case 1:
            rr = 2 * H_SCALE - hh;
            gg = SCALE;
            bb = 0;
            break;
        case 2:
            rr = 0;
            gg = SCALE;
            bb = hh - 2 * H_SCALE;
            break;
        case 3:
            rr = 0;
            gg = 4 * H_SCALE - hh;
            bb = SCALE;
            break;
        case 4:
            rr = hh - 4 * H_SCALE;
            gg = 0;
            bb = SCALE;
            break;
        case 5:
            rr = SCALE;
            gg = 0;
            bb = 6 * H_SCALE - hh;
            break;
    }
 
    rr = (rr + (SCALE - rr) * (SCALE - ss) / SCALE) * vv / SCALE;
    gg = (gg + (SCALE - gg) * (SCALE - ss) / SCALE) * vv / SCALE;
    bb = (bb + (SCALE - bb) * (SCALE - ss) / SCALE) * vv / SCALE;
 
    *r = rr;
    *g = gg;
    *b = bb;
    if (*r > 255)*r = 255;
    if (*g > 255)*g = 255;
    if (*b > 255)*b = 255;
}
 
void ICACHE_FLASH_ATTR RGB2HLS( double *h, double *l, double *s, uint8_t r, uint8_t g, uint8_t b)
{
    double dr = (double)r/255;
    double dg = (double)g/255;
    double db = (double)b/255;
    double cmax = MAX(dr, MAX(dg, db));
    double cmin = MIN(dr, MIN(dg, db));
    double cdes = cmax - cmin;
    double hh, ll, ss;
 
    ll = (cmax+cmin)/2;
    if(cdes){
        if(ll <= 0.5)
            ss = (cmax-cmin)/(cmax+cmin);
        else
            ss = (cmax-cmin)/(2-cmax-cmin);
 
        if(cmax == dr)
            hh = (0+(dg-db)/cdes)*60;
        else if(cmax == dg)
            hh = (2+(db-dr)/cdes)*60;
        else// if(cmax == b)
            hh = (4+(dr-dg)/cdes)*60;
        if(hh<0)
            hh+=360;
    }else
        hh = ss = 0;
 
    *h = hh;
    *l = ll;
    *s = ss;
}
 
void ICACHE_FLASH_ATTR HLS2RGB( uint8_t *r, uint8_t *g, uint8_t *b, double h, double l, double s)
{
    double cmax,cmin;
 
    if(l <= 0.5)
        cmax = l*(1+s);
    else
        cmax = l*(1-s)+s;
    cmin = 2*l-cmax;
 
    if(s == 0){
        *r = *g = *b = l*255;
    }else{
        *r = HLS2RGBvalue(cmin,cmax,h+120)*255;
        *g = HLS2RGBvalue(cmin,cmax,h)*255;
        *b = HLS2RGBvalue(cmin,cmax,h-120)*255;
    }
}
 
double ICACHE_FLASH_ATTR HLS2RGBvalue(double n1,double n2, double hue)
{
    if(hue > 360)
        hue -= 360;
    else if(hue < 0)
        hue += 360;
    if(hue < 60)
        return n1+(n2-n1)*hue/60;
    else if(hue < 180)
        return n2;
    else if(hue < 240)
        return n1+(n2-n1)*(240-hue)/60;
    else
        return n1;
}