#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "Constants.h"
#include "Line.h"
#include "UI_Element.h"

class ofApp : public ofBaseApp {

public:
	ofImage m_origImage; //the one we don't change
	ofImage m_editedImage; //the one we change
	string imgExtension; //our image file extension
	ofImage m_screenshot; //our screenshot
	ofxImGui::Gui m_gui; // our GUI object from ofxImGui

	//screenshot display status
	bool m_hasScreenshotted;
	time_t m_notifyTime;

	//light mode/dark mode
	bool m_isDarkMode;
	bool m_ui_isDarkMode;

	//GUI button
	UI_Element m_resetButton;
	UI_Element m_clearCanvasButton;

	//Frame buffer object
	ofFbo m_fbo;
	vector<ofTexture> m_textures;
	ofShader m_shader; 

	//font
	ofTrueTypeFont m_gui_font;
	ImFont *m_imgui_font = nullptr;
	float m_font_size;

	//changing vars
	float m_brightness;
	float m_contrast;
	float m_saturation;
	float m_red;
	float m_green;
	float m_blue;
	float m_solarisation;

	//LUT vars
	glm::vec3 m_lut[32][32][32];
	bool m_lutLoaded;

	//color picker and drawing vars
	int m_brushSize;
	ofVec2f m_mousePos;
	ofColor m_color;
	ofColor m_hoverColor;
	ImVec4 m_VecColor;
	bool  m_isHovering;
	bool m_isDrawing;
	bool  m_isColourPicking;
	bool m_imageDrawn;
	vector<Line> m_lines;
	Line m_line;

	//Drawing and related functions
	void setIsDrawing();
	void drawingHover();
	void clearCanvas();

	//Lifecycle functions
	void setup();
	void update();
	void draw();

	//Event functions
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void dragEvent(ofDragInfo dragInfo);
	//Filter functions - variable slider functions
	void applyBrightness(float& intensity);
	void applyContrast(float& intensity);
	void applySaturation(float& intensity);
	void applySolarise(float& intensity);
	void applyRGB(float& intensity, int rgb_type);
	
	//Filter functions - button enable functions
	void enableSharpenFilter();
	void enableEdgeDetectionFilter();
	void enableBevelFilter();
	void enableColourInversion();
	void enableGreyscale();	
	void enableGreyscaleInversion();
	void enableSepiaFilter();
	void enableBlur();
	void applyLUT(ofImage img);
	void loadLUT(string lutPath);
	void applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE matType);

	//utility functions
	void resetImage();
	ofImage resizeImage(ofImage img);
	void saveImageTo();
	void takeScreenshot();
	void openFileSelection(ofFileDialogResult openFileResult);


};
