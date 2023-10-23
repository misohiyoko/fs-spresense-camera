/*
 *  camera.ino - Simple camera example sketch
 *  Copyright 2018, 2022 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  This is a test app for the camera library.
 *  This library can only be used on the Spresense with the FCBGA chip package.
 */

#include <stdio.h>  /* for sprintf */

#include <Camera.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS -1 
#define TFT_RST 8 
#define TFT_DC  9 

#define BAUDRATE                (115200)
#define TOTAL_PICTURE_COUNT     (10)


int take_picture_count = 0;
uint16_t imgBuffPostProcess[320*240];
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

/**
 * Print error message
 */
uint16_t nearPow2(int n)
{
    if (n <= 0) return 0;

    if ((n & (n - 1)) == 0) return (uint)n;

    uint ret = 1;
    while (n > 0) { ret <<= 1; n >>= 1; }
    return ret;
}

void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
    {
      case CAM_ERR_NO_DEVICE:
        Serial.println("No Device");
        break;
      case CAM_ERR_ILLEGAL_DEVERR:
        Serial.println("Illegal device error");
        break;
      case CAM_ERR_ALREADY_INITIALIZED:
        Serial.println("Already initialized");
        break;
      case CAM_ERR_NOT_INITIALIZED:
        Serial.println("Not initialized");
        break;
      case CAM_ERR_NOT_STILL_INITIALIZED:
        Serial.println("Still picture not initialized");
        break;
      case CAM_ERR_CANT_CREATE_THREAD:
        Serial.println("Failed to create thread");
        break;
      case CAM_ERR_INVALID_PARAM:
        Serial.println("Invalid parameter");
        break;
      case CAM_ERR_NO_MEMORY:
        Serial.println("No memory");
        break;
      case CAM_ERR_USR_INUSED:
        Serial.println("Buffer already in use");
        break;
      case CAM_ERR_NOT_PERMITTED:
        Serial.println("Operation not permitted");
        break;
      default:
        break;
    }
}

/**
 * Callback from Camera library when video frame is captured.
 */

void CamCB(CamImage img)
{

  /* Check the img instance is available or not. */

  if (img.isAvailable())
    {

      /* If you want RGB565 data, convert image data format to RGB565 */

      img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
      
      uint16_t * imgBuff = (uint16_t *)img.getImgBuff();

      /* You can use image data directly by using getImgSize() and getImgBuff().
       * for displaying image to a display, etc. */
      uint16_t * imgBuffPostProcessPtr = imgBuffPostProcess;

      uint16_t h = 240;
      uint16_t i = 0;
      float h_ave = 0.0;
      
      float w_ave = 0.0;
      uint16_t upper_left_h = 239;
      uint16_t upper_left_w = 319;
      uint16_t lower_right_h = 0;
      uint16_t lower_right_w = 0;
      uint16_t count = 0;
      uint16_t maxs = 0;
      while(h--){
        int w = 320;
        while(w--){
          int16_t blue = (*imgBuff & 0b0000000000011111) >> 0;
          int16_t green = (*imgBuff & 0b0000011111100000) >> 6;
          int16_t red = (*imgBuff & 0b1111100000000000) >> 11;
          //*imgBuffPostProcessPtr = 0 <= red && red < 150 && 150 < green && green < 255 && 110 < blue && blue < 255 ? 0xFF : 0x00;
          int16_t A = 15;
          int16_t B = 8;
          int16_t C = 10;
          int16_t score = ((red * A - blue * B - green * C) + 32 * (B+C)) / (A+B+C);
          maxs = maxs < score ? score : maxs;
          if(score > 22){
            *imgBuffPostProcessPtr = uint16_t(score);
            int16_t h_act = 240 - h;
            int16_t w_act = 320 - w;
            h_ave += h_act ;
            w_ave += w_act;
            upper_left_h = h_act < upper_left_h ? h_act : upper_left_h;
            upper_left_w = w_act < upper_left_w ? w_act : upper_left_w;
            lower_right_h = h_act > lower_right_h ? h_act : lower_right_h;
            lower_right_w = w_act > lower_right_w ? w_act : lower_right_w;
            count++;
          }else{
            *imgBuffPostProcessPtr = 0x00;
          }
          //*imgBuffPostProcessPtr = score > 20 ? 0xFF : 0x00;
          //*imgBuffPostProcessPtr = 0b1111100000011111;
          imgBuffPostProcessPtr++;
     
          imgBuff++;
        }        
      }
      imgBuffPostProcessPtr = imgBuffPostProcess;
      imgBuff = (uint16_t *)img.getImgBuff();
      if(count > 200){
        h_ave = h_ave / count;
        w_ave = w_ave / count;
        Serial.print(String(upper_left_h));
        Serial.print(",");
        Serial.print(String(upper_left_w));
        Serial.print(",");
        Serial.print(String(lower_right_h));
        Serial.print(",");
        Serial.print(String(lower_right_w));
        Serial.print(",");
        Serial.println(String(maxs));
        

        
        for (int i = upper_left_w; i < lower_right_w; i++) {
          imgBuffPostProcessPtr[uint32_t(upper_left_h*320 + i)] = 0x00FF00;
          imgBuffPostProcessPtr[uint32_t((lower_right_h - 1)*320 + i)] = 0x00FF00;
          
        }
        
        
        for (int i = upper_left_h; i < lower_right_h; i++) {
          imgBuffPostProcessPtr[uint32_t(i*320 + upper_left_w)] = 0x00FF00;
          imgBuffPostProcessPtr[uint32_t(i*320 + (lower_right_w - 1))] = 0x00FF00;
        }
        /*
        for (int i = upper_left_w+1; i < lower_right_w - 2; i++) {
          for (int j = upper_left_h+1; j < lower_right_h - 2; j++){
               imgBuffPostProcessPtr[uint32_t(j * 320 + i)]= imgBuff[uint32_t(j * 320 + i)];
            }          
        }
        */
        CamImage resizedImage;
        uint16_t long_edge_pow2 = lower_right_w - upper_left_w > lower_right_h - upper_left_h ? lower_right_w - upper_left_w :  lower_right_h - upper_left_h;
        uint16_t center_w = (upper_left_w + lower_right_w) / 2;
        uint16_t center_h = (upper_left_h + lower_right_h) / 2;
        uint16_t upper_left_resized_w = center_w - long_edge_pow2 / 2 + 1 > 0 ? center_w - long_edge_pow2 / 2 + 1 : 0;
        uint16_t upper_left_resized_h = center_h - long_edge_pow2 / 2 + 1 > 0 ? center_h - long_edge_pow2 / 2 + 1 : 0;
        long_edge_pow2 = nearPow2(long_edge_pow2);
        img.convertPixFormat(CAM_IMAGE_PIX_FMT_YUV422);
        CamErr err =  img.clipAndResizeImageByHW(resizedImage, upper_left_w, upper_left_h, lower_right_w-1, lower_right_h-1, 64, 64);
        if (err != CAM_ERR_SUCCESS)
        {
          printError(err);
        }

        tft.drawRGBBitmap(0, 0,(uint16_t *)resizedImage.getImgBuff() ,64 ,64);
        
        
      }else{
        Serial.println("No Cone");
       
      }
      
      ///tft.drawRGBBitmap(0, 0,imgBuffPostProcessPtr, 320, 240);
      

    }
  else
    {
      Serial.println("Failed to get video stream image");
    }
}

/**
 * @brief Initialize camera
 */
void setup()
{
  CamErr err;
  tft.begin(40000000); 
  tft.setRotation(3);  

  /* Open serial communications and wait for port to open */

  Serial.begin(BAUDRATE);
  /* Initialize SD */
  /* begin() without parameters means that
   * number of buffers = 1, 30FPS, QVGA, YUV 4:2:2 format */

  Serial.println("Prepare camera");
  sleep(1);
  err = theCamera.begin();
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Start video stream.
   * If received video stream data from camera device,
   *  camera library call CamCB.
   */

  Serial.println("Start streaming");
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Auto white balance configuration */

  Serial.println("Set Auto white balance parameter");
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
 
  /* Set parameters about still picture.
   * In the following case, QUADVGA and JPEG.
   */

  Serial.println("Set still picture format");
  err = theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QUADVGA_H,
     CAM_IMGSIZE_QUADVGA_V,
     CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
    Serial.println("Set still picture HDR");
  err = theCamera.setHDR(CAM_HDR_MODE_ON);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
    



}

/**
 * @brief Take picture with format JPEG per second
 */

void loop()
{
  sleep(1); /* wait for one second to take still picture. */

}
