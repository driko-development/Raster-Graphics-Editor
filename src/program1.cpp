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

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Testing"

// Enum for tool selection
enum tool {EYEDROPPER, CROP, PENCIL, PAINTBUCKET, RESET};

// function prototypes
static void clickCallback(int event, int x, int y, int flags, void* userdata);
static tool selectTool(int val);

// Global variables
int selected_tool_value = 0;
tool selected_tool = selectTool(selected_tool_value);
std::string toolName[5] = {"Eyedropper", "Crop", "Pencil", "Paint Bucket", "Reset"};

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
static void clickCallback(int event, int x, int y, int flags, void* userdata)
{	
    if(event == cv::EVENT_LBUTTONDOWN)
    {
        std::cout << "LEFT CLICK (" << x << ", " << y << ")" << std::endl;
    }
    else if(event == cv::EVENT_RBUTTONDOWN)
    {
        if(selected_tool_value + 1 >= 5)
		{
			selected_tool_value = 0;
		}
		else
		{
			selected_tool_value = selected_tool_value + 1;
		}

		selected_tool = selectTool(selected_tool_value);

		std::cout << "ACTIVE TOOL: " << toolName[selected_tool] << std::endl;
    }
    else if(event == cv::EVENT_MBUTTONDOWN)
    {
        std::cout << "MIDDLE CLICK (" << x << ", " << y << ")" << std::endl;
    }
    else if(event == cv::EVENT_MOUSEMOVE)
    {
        std::cout << "MOUSE OVER (" << x << ", " << y << ")" << std::endl;
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
		cv::Mat imageIn;
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
