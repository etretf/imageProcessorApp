#pragma once

#include "ofMain.h"


namespace Constants_ImageProcessing
{

	static const int WINDOW_WIDTH = 1280;
	static const int WINDOW_HEIGHT = 720;
	static const int DESIRED_FRAMERATE = 60;
	static const char* FONT_PATH = "tt_interphases.ttf";
	static const float IMAGE_LEFT_MARGIN = 25.0f;
	static const float IMAGE_TOP_MARGIN = 45.0f;
	static const int LUT_SIZE = 32;


	enum CONVOLUTION_MAT_TYPE
	{
		SHARPEN = 0,
		EDGE_DETECTION = 1,
		BEVEL = 2,
		BLUR = 3,
	};

	static const float CONVOLUTION_MATS_3X3[] = {
		//sharpen
		0.0f,  -1.0f, 0.0f,
		-1.0f, 5.0f, -1.0f,
		0.0f,  -1.0f, 0.0f,

		//edge detection
		-1.0f, -1.0f, -1.0f,
		-1.0f,  8.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		//bevel
		0.0f, 2.0f, 0.0f,
		2.0f,  0.0f, -1.0f,
		0.0f, -2.0f, 0.0f,

		//blur
		1.0f/9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
		1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
		1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
	};

	//a way to convert between 1D and 2D array convolution matrix coordinates

	static const float CONVOLUTION_MATRIX_CONVERT_X[] = {
															-1.0f, 0.0f, 1.0f,
															-1.0f, 0.0f, 1.0f,
															-1.0f, 0.0f, 1.0f,
	};

	static const float CONVOLUTION_MATRIX_CONVERT_Y[] = {
															-1.0f, -1.0f, -1.0f,
															0.0f,   0.0f, 0.0f,
															1.0f,  1.0f, 1.0f,
	};
}

