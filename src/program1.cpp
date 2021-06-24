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
 * @brief Basic Raster Graphrics Editor with 5 tools
 * @author Edrik Aguilera
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "eyedropper.h"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Raster Graphics Editor"

// Enum for tool selection
enum tool {EYEDROPPER, CROP, PENCIL, PAINTBUCKET, RESET};

// function prototypes
static void clickCallback(int event, int x, int y, int flags, void* userdata);
static void toolEventHandler(int event, int x, int y, int flags, void* userdata, tool currentTool);
static tool activeTool(int val);
static void updatePixelValues(int x, int y);
static void reloadImage();

// Original image provided via command line args
cv::Mat imageIn;

// Deep copy, clone, of the original image
cv::Mat processedImage;

// Index used to retrieve tool name
int selected_tool_value = 0;

// Active tool
tool selected_tool = tool::EYEDROPPER;

// String names corresponding to the tool enums
std::string toolName[5] = {"Eyedropper", "Crop", "Pencil", "Paint Bucket", "Reset"};

// Default constructor invoked and bgr values set to 255,255,255
Eyedropper eyedropper;

// Points to generate point of interest, used in Crop tool
cv::Point point1;
cv::Point point2;

// State boolean to determine if the pencil is actively being dragged
bool drawing;


/*******************************************************************************************************************//**
 * @brief Updates the BGR values of a pixel at location (x,y)
 * @return return 0, updates the pixel value
 * @author Edrik Aguilera
 **********************************************************************************************************************/
static void updatePixelValues(int x, int y)
{
	// Grab a reference to the pixel at position (x,y)
	// we pass y,x because mat access is row column y -> row and x -> col
	cv::Vec3b & pixel = processedImage.at<cv::Vec3b>(y,x);

	// Assign the values stored in the eyedropper
	pixel[0] = eyedropper.blue;
	pixel[1] = eyedropper.green;
	pixel[2] = eyedropper.red;
}

/*******************************************************************************************************************//**
 * @brief Reloads the processed image
 * @return return 0
 * @author Edrik Aguilera
 **********************************************************************************************************************/
static void reloadImage()
{
	cv::imshow(DISPLAY_WINDOW_NAME, processedImage);
	cv::waitKey();
}

/*******************************************************************************************************************//**
 * @brief Returns the new active tool
 * @return return the new active tool
 * @author Edrik Aguilera
 **********************************************************************************************************************/
static tool activeTool()
{
	switch (selected_tool_value)
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
	// On a left double click with the reset tool active, reset back to original image
	if (event == cv::EVENT_LBUTTONDBLCLK  && currentTool == tool::RESET)
	{
		// Reset the original image
		processedImage = imageIn.clone();

		// Reload the window
		reloadImage();
	}

	else if(event == cv::EVENT_LBUTTONDOWN && currentTool == tool::EYEDROPPER)
	{
		// Three color pixel located at the position (x,y)
		cv::Vec3b pixel = processedImage.at<cv::Vec3b>(y,x);

		// Update the eyedropper values to the values in the current pixel
		eyedropper.Update(pixel[0], pixel[1], pixel[2]);

		// Display the updated values
		std::cout << "New eyedropper value = " << eyedropper.blue << " " << eyedropper.green << " " << eyedropper.red << std::endl;
	}

	else if(event == cv::EVENT_LBUTTONDOWN && currentTool == tool::CROP)
	{
		// Set the first point in the region of interest for crop tool
		point1 = cv::Point{x,y};
	}

	else if(event == cv::EVENT_LBUTTONDOWN && currentTool == tool::PENCIL)
	{
		// Update the pixel values at the starting position
		updatePixelValues(x,y);

		// Set the drawing flag to register mouse movements for the pencil
		drawing = true;
	}

    else if(event == cv::EVENT_LBUTTONDOWN && currentTool == tool::PAINTBUCKET)
    {
		cv::Rect rect;

		cv::floodFill(
			processedImage, 
			cv::Point(x,y), 
			cv::Scalar(eyedropper.blue, eyedropper.green, eyedropper.red),
			&rect,
			cv::Scalar(0,0,0),
			cv::Scalar(0,0,0),
			4
		);

		reloadImage();
    }

    else if(event == cv::EVENT_MOUSEMOVE && currentTool == tool::PENCIL && drawing)
    {		
		// when the pencil tool is active and drawing,, moving the mouse will change pixel values
		updatePixelValues(x,y);
    }

	
	else if (event == cv::EVENT_LBUTTONUP && currentTool == tool::CROP)
	{
		// Once the button has been released, set the second point
		point2 = cv::Point{x,y};

		// Create the region of interest
		cv::Rect region(point1,point2);

		// Clone the resulting image into the processed image to update
		processedImage = processedImage(region).clone();
		reloadImage();
	}

	else if (event == cv::EVENT_LBUTTONUP && currentTool == tool::PENCIL)
	{
		// Turn off drawing flag
		drawing = false;
			
		// Display the markings created by the pencil
		reloadImage();
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
 * @author Edrik Aguilera
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
		selected_tool = activeTool();

		// Display the newly selected tool
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
		// Read the file into the original imageIn
		imageIn = cv::imread(argv[1], cv::IMREAD_COLOR);

		// check for file error
		if(!imageIn.data)
		{
			std::cout << "Error while opening file " << argv[1] << std::endl;
			return 0;
		}
		else
		{
			// Create a deep copy of the image
			// all operations will be done on this image
			processedImage = imageIn.clone();

			// Display the image window
			cv::imshow(DISPLAY_WINDOW_NAME, processedImage);

			// Click call back's main function is to switch tools and delegate toolEventHandler			
    		cv::setMouseCallback(DISPLAY_WINDOW_NAME, clickCallback, &processedImage);
			cv::waitKey();
		}
    }
}
