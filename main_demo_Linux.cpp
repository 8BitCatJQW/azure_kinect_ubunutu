// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <stdio.h>
#include <stdlib.h>
#include <k4a/k4a.h>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
    int returnCode = 1;
    k4a_device_t device = NULL;
    const int32_t TIMEOUT_IN_MS = 1000;
    int captureFrameCount;
    k4a_capture_t capture = NULL;

  

   

    uint32_t device_count = k4a_device_get_installed_count();

    if (device_count == 0)
    {
        printf("No K4A devices found\n");
        return 0;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &device))
    {
        printf("Failed to open device\n");
        return 0;
    }

    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    config.color_resolution = K4A_COLOR_RESOLUTION_2160P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;

    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return 0;
    }
    cv::Mat cv_rgbImage_with_alpha;
    cv::Mat cv_rgbImage_no_alpha;
    while (1)
    {
       k4a_image_t depthImage = NULL, colorImage = NULL, irImage = NULL;
       switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture");
            continue;
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture");
            goto Exit;
        }
 
        cv::Mat color;
		colorImage = k4a_capture_get_color_image(capture);
		if(colorImage)
		{
			int width = k4a_image_get_width_pixels(colorImage);
			int height = k4a_image_get_height_pixels(colorImage);
			color = cv::Mat(cv::Size(width, height), CV_8UC4);
			memcpy(color.data, (void*)k4a_image_get_buffer(colorImage), width * height * 4);
		}
		
		cv::Mat depth;
		depthImage = k4a_capture_get_depth_image(capture);
        if (depthImage)
        {
			int width = k4a_image_get_width_pixels(depthImage);
			int height = k4a_image_get_height_pixels(depthImage);
			depth = cv::Mat(cv::Size(width, height), CV_16UC1);
			memcpy(depth.data, (void*)k4a_image_get_buffer(depthImage), width * height * sizeof(int16_t));
            
        }
		
       
		cv::Mat IR_left, IR_right;
      
        irImage = k4a_capture_get_ir_image(capture);
        if (irImage != NULL)
        {
            int width = k4a_image_get_width_pixels(irImage);
            int height = k4a_image_get_height_pixels(irImage);
            cv::Mat IR_left = cv::Mat(cv::Size(width, height), CV_16UC1);
            memcpy(IR_left.data, (void*)k4a_image_get_buffer(irImage), width * height * sizeof(int16_t));
            
        }
        
  
 
        
        if (colorImage)
            k4a_image_release(colorImage);
        if (depthImage)
            k4a_image_release(depthImage);
        if(irImage)
            k4a_image_release(irImage);
        if(capture)
            k4a_capture_release(capture);
        
        cv::waitKey(2);
    }

    returnCode = 0;
Exit: 
    if (device != NULL)
    {
        k4a_device_close(device);
    }

    return returnCode;
}

