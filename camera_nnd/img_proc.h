#ifndef IMG_PROC_H
#define IMG_PROC_H

#define WIDTH 320
#define HEIGHT 240

#include <cstdint>

enum ImgProcError{
    SUCCESS,
    NOT_YET_IMPLEMENTED,
};

enum KernelType{
    CROSS,
    SQUARE
};

class ImgProc {
protected:
    static int addrFromWidthAndHeight(uint16_t w, uint16_t h);
public:
    static ImgProcError rgbToGrayWeighted(uint16_t * rgbImgBuff, uint8_t * grayImgBuff,
                                          int16_t red_weight, int16_t green_weight, int16_t blue_weight);
    static ImgProcError binarize(uint8_t * grayImgBuff, bool * monoImgBuff, uint8_t threshold);
    static ImgProcError dilation(const bool * monoImgBuffIn, bool * monoImgBuffOut, uint8_t kernelSize, KernelType kernelType);
    static ImgProcError erosion(const bool * monoImgBuffIn, bool * monoImgBuffOut, uint8_t kernelSize, KernelType kernelType);
};

#endif
