
#include <cstdint>

#define WIDTH 320
#define HEIGHT 240

enum ImgProcError{
    SUCCESS,
    NOT_YET_IMPLEMENTED,

};

class ImgProc{
    int addrFromWidthAndHeight(uint16_t w, uint16_t h){
        return h * WIDTH + HEIGHT;
    }
    ImgProcError rgbToGrayWeighted(uint16_t * rgbImgBuff, uint16_t * grayImgBuff,
                                   int16_t red_weight, int16_t green_weight, int16_t blue_weight){
        uint16_t h = HEIGHT;
        while (h--){
            uint16_t w = WIDTH;
            while (w--){
                uint8_t blue = (*rgbImgBuff & 0b0000000000011111) << 1;
                uint8_t green = (*rgbImgBuff & 0b0000011111100000) >> 5;
                uint8_t red = (*rgbImgBuff & 0b1111100000000000) >> 10;
                uint8_t score = ((red * red_weight + blue * blue_weight + green * green_weight) +
                        64 * (red_weight < 0 ? red_weight : 0 +green_weight < 0 ? red_weight : 0 +blue_weight < 0 ? red_weight : 0 ))
                        / (red_weight+green_weight+blue_weight);
                *grayImgBuff = score;
                rgbImgBuff++;
                grayImgBuff++;
            }
        }
    }
};