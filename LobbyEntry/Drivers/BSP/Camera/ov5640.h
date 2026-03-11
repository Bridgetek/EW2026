#ifndef OV5640_H
#define OV5640_H

#include "main.h"
#include "camera.h"

int ov5640_init(framesize_t framesize, pixformat_t format);
int ov5640_reset();

#endif
