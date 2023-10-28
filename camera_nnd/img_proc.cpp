#ifndef IMG_PROC_CPP
#define IMG_PROC_CPP

#include "img_proc.h"

int ImgProc::addrFromWidthAndHeight(uint16_t w, uint16_t h) {
    return h * WIDTH + w;
}

ImgProcError
ImgProc::rgbToGrayWeighted(uint16_t *rgbImgBuff, uint8_t *grayImgBuff, int16_t red_weight, int16_t green_weight,
                           int16_t blue_weight) {
    uint16_t h = HEIGHT;
    while (h--){
        uint16_t w = WIDTH;
        while (w--){
            uint8_t blue = (*rgbImgBuff & 0b0000000000011111) << 1;
            uint8_t green = (*rgbImgBuff & 0b0000011111100000) >> 5;
            uint8_t red = (*rgbImgBuff & 0b1111100000000000) >> 10;
            uint8_t score = ((red * red_weight + blue * blue_weight + green * green_weight) +
                             64 * (red_weight < 0 ? red_weight : 0 + green_weight < 0 ? red_weight : 0 + blue_weight < 0 ? red_weight : 0 ))
                            / (red_weight + green_weight + blue_weight);
            *grayImgBuff = score;
            rgbImgBuff++;
            grayImgBuff++;
        }
    }
    return SUCCESS;
}

ImgProcError ImgProc::binarize(uint8_t *grayImgBuff, bool *monoImgBuff, uint8_t threshold) {
    uint16_t h = HEIGHT;
    while (h--){
        uint16_t w = WIDTH;
        while (w--){
            *monoImgBuff = *grayImgBuff > threshold;
            monoImgBuff++;
            grayImgBuff++;
        }
    }
    return SUCCESS;
}

ImgProcError
ImgProc::dilation(const bool *monoImgBuffIn, bool *monoImgBuffOut, uint8_t kernelSize, KernelType kernelType) {
    uint8_t kernelRadius = (kernelSize - 1) / 2;
    if(kernelType == CROSS){
        for(uint16_t h = kernelRadius; h < HEIGHT - kernelRadius; h++){
            for(uint16_t w = kernelRadius; w < WIDTH - kernelRadius; w++){
                bool isActivePixelAround = false;
                for(uint8_t i = 1; i < kernelRadius + 1; i++){
                    isActivePixelAround = isActivePixelAround | monoImgBuffIn[addrFromWidthAndHeight(w + i,h)];
                    isActivePixelAround = isActivePixelAround | monoImgBuffIn[addrFromWidthAndHeight(w - i,h)];
                    isActivePixelAround = isActivePixelAround | monoImgBuffIn[addrFromWidthAndHeight(w,h + i)];
                    isActivePixelAround = isActivePixelAround | monoImgBuffIn[addrFromWidthAndHeight(w,h - i)];
                    if(isActivePixelAround){
                        break;
                    }
                }
                monoImgBuffOut[addrFromWidthAndHeight(w,h)] = isActivePixelAround;
            }
        }
        return SUCCESS;
    } else if(kernelType == SQUARE){
        return NOT_YET_IMPLEMENTED;
    } else{
        return NOT_YET_IMPLEMENTED;
    }
}

ImgProcError
ImgProc::erosion(const bool *monoImgBuffIn, bool *monoImgBuffOut, uint8_t kernelSize, KernelType kernelType) {
    uint8_t kernelRadius = (kernelSize - 1) / 2;
    if(kernelType == CROSS){
        for(uint16_t h = kernelRadius; h < HEIGHT - kernelRadius; h++){
            for(uint16_t w = kernelRadius; w < WIDTH - kernelRadius; w++){
                bool isNotActivePixelAround = true;
                for(uint8_t i = 1; i < kernelRadius + 1; i++){
                    isNotActivePixelAround = isNotActivePixelAround & monoImgBuffIn[addrFromWidthAndHeight(w + i, h)];
                    isNotActivePixelAround = isNotActivePixelAround & monoImgBuffIn[addrFromWidthAndHeight(w - i, h)];
                    isNotActivePixelAround = isNotActivePixelAround & monoImgBuffIn[addrFromWidthAndHeight(w, h + i)];
                    isNotActivePixelAround = isNotActivePixelAround & monoImgBuffIn[addrFromWidthAndHeight(w, h - i)];
                    if(!isNotActivePixelAround){
                        break;
                    }
                }
                monoImgBuffOut[addrFromWidthAndHeight(w,h)] = isNotActivePixelAround;
            }
        }
        return SUCCESS;
    } else if(kernelType == SQUARE){
        return NOT_YET_IMPLEMENTED;
    } else{
        return NOT_YET_IMPLEMENTED;
    }
}

#endif