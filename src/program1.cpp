//
//    Copyright 2021 Christopher D. McMurrough
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/*******************************************************************************************************************//**
 * @file program1.cpp
 * @brief C++ hello world example for OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "eyedropper.h"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Testing"

// Enum for tool selection
enum tool {EYEDROPPER, CROP, PENCIL, PAINTBUCKET, RESET};

// function prototypes
static void clickCallback(int event, int x, int y, int flags, void* userdata);
static void toolEventHandler(int event, int x, int y, int flags, void* userdata, tool currentTool);
static tool selectTool(int val);
static void updatePixelValues(int x, int y);
static void reloadImage(cv::Mat image);

// Global variables
cv::Mat imageIn;
int selected_tool_value = 0;
tool selected_tool = tool::EYEDROPPER;
std::string toolName[5] = {"Eyedropper", "Crop", "Pencil", "Paint Bucket", "Reset"};

// Default constructor invoked and bgr values set to 255,255,255
Eyedropper eyedropper;

// Points to generate point of interest 
cv::Point start;
cv::Point end;

bool drawing;

static void updatePixelValues(int x, int y)
{
	cv::Vec3b & pixel = imageIn.at<cv::Vec3b>(y,x);

	pixel[0] = eyedropper.blue;
	pixel[1] = eyedropper.green;
	pixel[2] = eyedropper.red;
}

static void reloadImage(cv::Mat image)
{
	cv::imshow(DISPLAY_WINDOW_NAME, image);
	cv::waitKey();
}

/*******************************************************************************************************************//**
 * @brief handler for image click callbacks
 * @param[in] val the selected_tool_value
 * @return return the new active tool
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
static tool selectTool(int val)
{
	switch (val)
	{
	case 0:
		return tool::EYEDROPPER;
		break;
	case 1:
		return tool::CROP;
		break;
	case 2:
		return tool::PENCIL;
		break;
	case 3:
		return tool::PAINTBUCKET;
		break;
	case 4:
		return tool::RESET;
		break;
	default:
		return tool::RESET;
		break;
	}
}

/*******************************************************************************************************************//**
 * @brief handler for image click callbacks
 * @param[in] event number of command line arguments
 * @param[in] x string array of command line arguments
 * @param[in] y string array of command line arguments
 * @param[in] flags string array of command line arguments
 * @param[in] userdata string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
static void toolEventHandler(int event, int x, int y, int flags, void* userdata, tool currentTool)
{	
    if(event == cv::EVENT_LBUTTONDOWN)
    {
        std::cout << "LEFT CLICK (" << x << ", " << y << ")" << std::endl;
		if (currentTool == tool::EYEDROPPER)
		{
			// Three color pixel
			cv::Vec3b pixel = imageIn.at<cv::Vec3b>(y,x);
			eyedropper.Update(pixel[0], pixel[1], pixel[2]);

			 std::cout << "New eyedropper value = " << eyedropper.blue << " " << eyedropper.green << " " << eyedropper.red << std::endl;
		}
		else if (currentTool == tool::CROP)
		{
			start = cv::Point{x,y};
		}
		else if (currentTool == tool::RESET)
		{
			reloadImage(imageIn);
		}
		else if (currentTool == tool::PENCIL)
		{
			updatePixelValues(x,y);

			drawing = true;
		}
		else if (currentTool == tool::PAINTBUCKET)
		{
			cv::Rect rect;
			cv::floodFill(
				imageIn, 
				cv::Point(x,y), 
				cv::Scalar(eyedropper.blue, eyedropper.green, eyedropper.red),
				&rect,
				cv::Scalar(0,0,0),
				cv::Scalar(255,255,255),
				4);

			reloadImage(imageIn);
		}
    }
    else if(event == cv::EVENT_MOUSEMOVE)
    {
        std::cout << "MOUSE OVER (" << x << ", " << y << ")" << std::endl;

		if (currentTool == tool::PENCIL && drawing)
		{
			updatePixelValues(x,y);
		}
    }
	else if (event == cv::EVENT_LBUTTONUP)
	{
		if (currentTool == tool::CROP)
		{
			end = cv::Point{x,y};
			cv::Rect region(start,end);
			cv::Mat imageROI = imageIn(region);
			cv::imshow(DISPLAY_WINDOW_NAME, imageROI);
			cv::waitKey();
		}
		if (currentTool == tool::PENCIL)
		{
			drawing = false;
			
			reloadImage(imageIn);
		}
	}
}

/*******************************************************************************************************************//**
 * @brief handler for image click callbacks
 * @param[in] event number of command line arguments
 * @param[in] x string array of command line arguments
 * @param[in] y string array of command line arguments
 * @param[in] flags string array of command line arguments
 * @param[in] userdata string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
static void clickCallback(int event, int x, int y, int flags, void* userdata)
{	
	// if the user performs a right click, switch the currently active tool
	if(event == cv::EVENT_RBUTTONDOWN)
    {
		// If the value is 4 then we are at max
		// reset the value to 0 to get the eyedropper tool
        if(selected_tool_value + 1 >= 5)
		{
			selected_tool_value = 0;
		}
		else
		{
			// Otherwise, increase the value to get to the next tool
			selected_tool_value = selected_tool_value + 1;
		}

		// Change the active tool
		selected_tool = selectTool(selected_tool_value);

		std::cout << "ACTIVE TOOL: " << toolName[selected_tool] << std::endl;
    }
	else
	{
		// Handle the event for the corresponding tool
		toolEventHandler(event, x, y, flags, userdata, selected_tool);
	}
}

/*******************************************************************************************************************//**
 * @brief program entry point
 * @param[in] argc number of command line arguments
 * @param[in] argv string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
int main(int argc, char **argv)
{
    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <image_path> \n", argv[0]);
        return 0;
    }
    else
    {
		imageIn = cv::imread(argv[1], cv::IMREAD_COLOR);
		
		// check for file error
		if(!imageIn.data)
		{
			std::cout << "Error while opening file " << argv[1] << std::endl;
			return 0;
		}
		else
		{
			cv::imshow(DISPLAY_WINDOW_NAME, imageIn);			
    		cv::setMouseCallback(DISPLAY_WINDOW_NAME, clickCallback, &imageIn);
			cv::waitKey();
		}
    }
}
