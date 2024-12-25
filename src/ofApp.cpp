//Emma Souannhaphanh
//IMD3005A: Sensor-based Interaction
//A2: Playing with Pixels

#include "ofApp.h"
#include "Constants.h"
#include "ofxImGui.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//set window/renderer properties
	ofSetWindowShape(Constants_ImageProcessing::WINDOW_WIDTH, Constants_ImageProcessing::WINDOW_HEIGHT);
	ofSetFrameRate(Constants_ImageProcessing::DESIRED_FRAMERATE);

	//init vars;
	m_brightness = 0;
	m_contrast = 0;
	m_saturation = 0;
	m_solarisation = 0;
	m_red = 0;
	m_green = 0;
	m_blue = 0;

	m_lutLoaded = false;

	m_brushSize = 5;
	m_mousePos.x = ofGetMouseX();
	m_mousePos.y = ofGetMouseY();
	m_color = 255;
	m_hoverColor = m_color;
	m_VecColor = ofFloatColor(m_color);
	m_isHovering = false;
	m_isDrawing = false;
	m_isColourPicking = false;
	m_hasScreenshotted = false;
	m_isDarkMode = false;
	m_font_size = 18.0f;

	//load all images
	imgExtension = "JPG";
	m_origImage.load("penguins.jpg");
	m_origImage = resizeImage(m_origImage);
	//copy image to m_editedImage
	m_editedImage = m_origImage;

	//Setup the ImGui object
	m_gui.setup();
	
	// Load Fonts
	m_imgui_font = m_gui.addFont("Roboto-Regular.ttf", m_font_size, nullptr);
	m_gui_font.load("Roboto-Regular.ttf", 12);

	
	//Custom GUI
	
	//set "reset image" button
	m_resetButton.m_ui_setup();
	static const int xpos = m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN + 50.0f;
	static const int xremain = ofGetWindowWidth() - xpos;
	int setXpos = xpos + xremain - 210.0f;
	static const int btnHeight = 35.0f;
	m_resetButton.set(setXpos, Constants_ImageProcessing::IMAGE_TOP_MARGIN,
		m_editedImage.getWidth()/4.0f, btnHeight, 8.0f
	);

	//set "clear canvas" button
	m_clearCanvasButton.m_ui_setup();
	m_clearCanvasButton.set(setXpos, Constants_ImageProcessing::IMAGE_TOP_MARGIN + m_resetButton.height + 8.0f,
		m_editedImage.getWidth() / 4.0f, btnHeight, 8.0f
	);

}

//--------------------------------------------------------------
void ofApp::update() {

	//update the mouse position variable
	m_mousePos.set(ofGetMouseX(), ofGetMouseY());

	//clear the screenshot notification after seven seconds 
	if (m_hasScreenshotted && m_notifyTime)
	{
		if (time(NULL) - m_notifyTime > 7)
		{
			m_hasScreenshotted = false;
		}
	}

	//Custom GUI event catching
	
	//reset image to unedited version if pressed
	if(m_resetButton.isMousePressed())
	{
		resetImage();
	}
	//clear all drawn lines from canvas if pressed
	if (m_clearCanvasButton.isMousePressed())
	{
		clearCanvas();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	//draw the background according to the colour preference
	if (m_isDarkMode)
		ofSetBackgroundColor(ofColor(10, 15, 30));
	else
		ofSetBackgroundColor(ofColor(200));

	//edited image
	m_editedImage.draw(25.0f, 45.0f);

	//draw the original image to the right of the edited image and make sure it is not too tall so that it intersects with the ImGui window
	if (m_origImage.getHeight() / 4.0f < 125.0f)
		m_origImage.draw(ofGetWindowWidth()*0.6 + 10.0f, Constants_ImageProcessing::IMAGE_TOP_MARGIN, m_editedImage.getWidth() / 4.0f, m_editedImage.getHeight() / 4.0f);
	else
		m_origImage.draw(ofGetWindowWidth() * 0.6 + 10.0f, Constants_ImageProcessing::IMAGE_TOP_MARGIN, m_editedImage.getWidth() / 5.0f, m_editedImage.getHeight() / 5.0f);


	//draw hovering brush while in drawing mode
	drawingHover();

	//notify user of screenshot in bottom left of screen
	if (m_hasScreenshotted)
	{
		if(!m_isDarkMode)
			ofSetColor(ofColor::black);
		m_gui_font.drawString("Screenshot saved to bin/data folder.", Constants_ImageProcessing::IMAGE_LEFT_MARGIN, ofGetWindowHeight() - 20.0f);
		ofSetColor(255);
	}

	//draw the current line
	ofSetLineWidth(m_brushSize);
	ofSetColor(m_color);
	m_line.m_currentPath.draw();	
	
	//drawing all lines previously drawn
	for (int i = 0; i < m_lines.size(); i++)
	{
		ofSetLineWidth(m_lines[i].m_strokeRadius);
		ofSetColor(m_lines[i].m_currentColour);
		m_lines[i].m_currentPath.draw();
	}
	ofSetColor(255);
	ofSetLineWidth(1.0f);


	//Custom GUI
	
	//reset image button
	m_resetButton.m_ui_draw();
	m_gui_font.drawString("Reset Image", m_resetButton.x + 45.0f, m_resetButton.y + m_resetButton.height / 2.0f + 5.0f);

	//clear canvas
	m_clearCanvasButton.m_ui_draw();
	m_gui_font.drawString("Clear Canvas", m_clearCanvasButton.x + 45.0f, m_clearCanvasButton.y + m_clearCanvasButton.height / 2.0f + 5.0f);



	//ImGui
	m_gui.begin();

	//ImGui styling
	ImGuiStyle& style = ImGui::GetStyle();
	style.GrabRounding = style.FrameRounding = 4.0f;
	ImGui::StyleColorsDark();
	style.ItemSpacing = ImVec2(8.0f, 8.0f);
	style.ScrollbarRounding = 8.0f;
	style.WindowRounding = 8.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);

	//Add Roboto font
	ImGui::PushFont(m_imgui_font);

	//top window menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Image"))
			{
				ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a jpg or png");
				if (openFileResult.bSuccess)
				{
					openFileSelection(openFileResult);
				}
			}
			if (ImGui::BeginMenu("Save Image"))
			{
				if (ImGui::MenuItem("Save image as.."))
				{
					saveImageTo();
				}
				if (ImGui::MenuItem("Save image to bin/data"))
				{
					takeScreenshot();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		//checkbox to switch from light mode to dark mode
		(ImGui::Checkbox("Dark Mode", &m_isDarkMode));
		ImGui::EndMainMenuBar();
	}

	//filter and drawing settings window
	ImGui::SetNextWindowPos(ofVec2f(ofGetWindowWidth() * 0.6 + 10.0f, m_editedImage.getHeight() / 4.0f + 60.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ofVec2f(ofGetWindowWidth() - m_editedImage.getWidth() - 75.0f, ofGetWindowHeight() - m_editedImage.getHeight() / 4.0f - 75.0f), ImGuiCond_Once);

	ImGui::Begin("Control Panel");

	ImGui::Text("Application Average %.3f ms/frame (%.1f)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::SliderFloat("Brightness", &m_brightness, -255.0f, 255.0f))
	{
		applyBrightness(m_brightness);
	}
	if (ImGui::SliderFloat("Contrast", &m_contrast, -255.0f, 255.0f))
	{
		applyContrast(m_contrast);
	}
	if (ImGui::SliderFloat("Saturation", &m_saturation, -180.0f, 180.0f))
	{
		applySaturation(m_saturation);
	}
	ImGui::SeparatorText("Colour Balance");
	if (ImGui::SliderFloat("Green - Red", &m_red, -100.0f, 100.0f))
	{
		applyRGB(m_red, 1);
	}
	if (ImGui::SliderFloat("Magenta - Green", &m_green, -100.0f, 100.0f))
	{
		applyRGB(m_green, 2);
	}
	if (ImGui::SliderFloat("Yellow - Blue", &m_blue, -100.0f, 100.0f))
	{
		applyRGB(m_blue, 3);
	}
	ImGui::Separator();
	if (ImGui::SliderFloat("Solarise", &m_solarisation, -255.0f, 255.0f))
	{
		applySolarise(m_solarisation);
	}
	ImGui::Spacing();
	ImGui::SeparatorText("Filters");
	ImGui::Spacing();

	if (ImGui::Button("Enable Sharpen", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableSharpenFilter();
	}
	if (ImGui::Button("Enable Blur", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableBlur();
	}
	if (ImGui::Button("Enable Edge Detection", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableEdgeDetectionFilter();
	}
	if (ImGui::Button("Enable Bevel", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableBevelFilter();
	}
	if (ImGui::Button("Enable Colour Inversion", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableColourInversion();
	}
	if (ImGui::Button("Enable Greyscale", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableGreyscale();
	}
	if (ImGui::Button("Enable Greyscale Inversion", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableGreyscaleInversion();
	}
	if (ImGui::Button("Enable Sepia Filter", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		enableSepiaFilter();
	}
	ImGui::SeparatorText("LUT Filters");
	if (ImGui::Button("Enable Good Morning", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		loadLUT("LUT\\Morning.CUBE");
		if (m_lutLoaded)
			applyLUT(m_origImage);
	}
	if (ImGui::Button("Enable Chill Spring", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		loadLUT("LUT\\Chill.CUBE");
		if (m_lutLoaded)
			applyLUT(m_origImage);
	}
	if (ImGui::Button("Enable Moody", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		loadLUT("LUT\\Moody.CUBE");
		if(m_lutLoaded)
			applyLUT(m_origImage);
	}
	if (ImGui::Button("Enable Oil", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		loadLUT("LUT\\Oil.CUBE");
		if (m_lutLoaded)
			applyLUT(m_origImage);
	}
	if (ImGui::Button("Enable Red Moon", ofVec2f(ImGui::GetWindowSize().x - 50.0f, 30.0f)))
	{
		loadLUT("LUT\\RedMoon.CUBE");
		if (m_lutLoaded)
			applyLUT(m_origImage);
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if(ImGui::CollapsingHeader("Art Station"))
	{
		ImGui::TextWrapped("Draw on the image using a brush.");
		if (ImGui::SliderInt("Brush size", &m_brushSize, 1, 10))
		{

		}
		if (ImGui::Button("Pick a colour"))
		{
			m_isColourPicking = true;
			m_isHovering = false;
		}
		ImGui::Spacing();
		ImGui::Separator();		
		ImGui::Text("Brush Size");
		if (ImGui::RadioButton("Drawing", m_isHovering))
		{
			setIsDrawing();
		}
		if (ImGui::ColorPicker3("Colour Picker", (float*)(&m_VecColor)))
		{
			//convert from ImVec4 to ofFloatColor to ofColor
			ofFloatColor newCol;
			newCol.r = m_VecColor.x;
			newCol.g = m_VecColor.y;
			newCol.b = m_VecColor.z;
			newCol.a = 1.0f;
			m_color = ofColor(newCol);
		}
	}


	ImGui::PopFont();
	ImGui::End();

	m_gui.end();

	//While color picking...
	//draw a floating box that is zoomed into the picture with a border that shows the color you are hovering over
	if (ofGetMouseX() > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		ofGetMouseX() < Constants_ImageProcessing::IMAGE_LEFT_MARGIN + m_editedImage.getWidth() &&
		ofGetMouseY() > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		ofGetMouseY() < Constants_ImageProcessing::IMAGE_TOP_MARGIN + m_editedImage.getHeight() &&
		m_isColourPicking)
	{
		ofPushMatrix();
			//draw image subsection and get hovered over color
			ofTranslate(m_mousePos.x, m_mousePos.y);
			m_screenshot.grabScreen(Constants_ImageProcessing::IMAGE_LEFT_MARGIN, Constants_ImageProcessing::IMAGE_TOP_MARGIN, m_editedImage.getWidth(), m_editedImage.getHeight());
			m_hoverColor = m_screenshot.getColor(m_mousePos.x - Constants_ImageProcessing::IMAGE_LEFT_MARGIN, m_mousePos.y - Constants_ImageProcessing::IMAGE_TOP_MARGIN);
			ofSetColor(m_hoverColor);
			ofDrawRectangle(-2, -2, 104, 104);
			ofSetColor(255);
			m_screenshot.drawSubsection(0, 0, 100, 100, mouseX - (Constants_ImageProcessing::IMAGE_LEFT_MARGIN + 30.0f), mouseY - (Constants_ImageProcessing::IMAGE_TOP_MARGIN + 20.0f), 50, 50);
			//draw hovered over color
			ofSetColor(m_hoverColor);
			ofDrawRectangle(0, 70, 100, 30);
			ofSetColor(ofColor::black);
			ofSetColor(255);
		ofPopMatrix();
	}
}



//--------------------------------------------------------------
//function sets colour picking state to false
void ofApp::setIsDrawing()
{
	m_isHovering = !m_isHovering;
	if (m_isColourPicking)
	{
		m_isColourPicking = false;
	}
}



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isHovering 
		)
	{
		m_isDrawing = true;
	}
	//if drawing, add points to a vertex array
	if (x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isDrawing
		)
	{
		ofPoint pt; 
		pt.set(x, y);
		m_line.m_currentPath.addVertex(pt);
	}
	//take a screenshot of the image while colour picking to ensure that brush strokes are colour-pickable
	if (x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isColourPicking
		)
	{
		m_color = m_screenshot.getColor(x - Constants_ImageProcessing::IMAGE_LEFT_MARGIN, y - Constants_ImageProcessing::IMAGE_TOP_MARGIN);
		m_color = ofFloatColor(m_color); //must convert from ofColor to ofFloatColor to ImVec4 for ImGui parameter restraint
		m_VecColor = m_color;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	//if hovering over the canvas and in the hovering state, drawing state is true (able to draw)
	if (x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isHovering
		)
	{
		m_isDrawing = true;
	}
	//if hovering over the canvas and colour picking is true, get the color at the coordinate in the image
	if (x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isColourPicking
		)
	{
		m_color = m_screenshot.getColor(x - Constants_ImageProcessing::IMAGE_LEFT_MARGIN, y - Constants_ImageProcessing::IMAGE_TOP_MARGIN);		m_color = ofFloatColor(m_color);
		m_VecColor = m_color;
	}
}

void ofApp::mouseReleased(int x, int y, int button)
{
	//creates a new line and pushes to the lines vector array
	if (m_isDrawing)
	{
		m_isDrawing = false;
		Line m_newLine = m_line;
		m_newLine.m_currentColour = m_color;
		m_newLine.m_strokeRadius = m_brushSize;
		m_lines.push_back(m_newLine);
		m_line.m_currentPath.clear();
	}
	//if colour picking, user is no longer colour picking after clicking and releasing
	if (m_isColourPicking)
	{
		m_isColourPicking = false;
	}
}


void ofApp::dragEvent(ofDragInfo dragInfo)
{
	//if an image is dragged over and dropped into the current edited image box, set the image to the dropped image
	if (dragInfo.position.x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN && dragInfo.position.x < Constants_ImageProcessing::IMAGE_LEFT_MARGIN + m_origImage.getWidth()
		&& dragInfo.position.y > Constants_ImageProcessing::IMAGE_TOP_MARGIN
		&& dragInfo.position.y < Constants_ImageProcessing::IMAGE_TOP_MARGIN + m_origImage.getHeight())
	{
		ofImage newImg;
		newImg.load(dragInfo.files[0]);
		newImg = resizeImage(newImg);
		m_origImage = newImg;
		m_editedImage = newImg;
		resetImage();
		clearCanvas();
	}
}


//function takes a screenshot of the screen where the image is and saves it to the bin/data folder
void ofApp::takeScreenshot()
{
	m_hasScreenshotted = true;
	m_notifyTime = time(NULL);
	m_screenshot.grabScreen(Constants_ImageProcessing::IMAGE_LEFT_MARGIN, Constants_ImageProcessing::IMAGE_TOP_MARGIN, m_editedImage.getWidth(), m_editedImage.getHeight());
	string fileName = "screenshot_" + ofGetTimestampString() + ".png";
	m_screenshot.save(fileName);
}


//function draws a circle that depicts the brush radius while the user is in the drawing state and is hovering over the image
void ofApp::drawingHover()
{
	if (m_mousePos.x > Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		m_mousePos.x < m_editedImage.getWidth() + Constants_ImageProcessing::IMAGE_LEFT_MARGIN &&
		m_mousePos.y > Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_mousePos.y < m_editedImage.getHeight() + Constants_ImageProcessing::IMAGE_TOP_MARGIN &&
		m_isHovering
		)
	{
		ofPushMatrix();
		ofNoFill();
		ofTranslate(m_mousePos.x, m_mousePos.y);
		ofDrawCircle(0, 0, m_brushSize);
		ofFill();
		ofPopMatrix();
	}
}

//clear the current line and the previously drawn lines
void ofApp::clearCanvas()
{
	m_lines.clear();
	m_line.m_currentPath.clear();
}


void ofApp::resetImage()
{
	m_editedImage = m_origImage;
	m_brightness = 0.0f;
	m_contrast = 0.0f;
	m_saturation = 0.0f;
	m_solarisation = 0.0f;
	m_red = 0.0f;
	m_green = 0.0f;
	m_blue = 0.0f;
}

void ofApp::applyBrightness(float& intensity)
{
	ofColor tempCol;
	ofColor newCol;

	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_origImage.getColor(x, y);
			//change colour brightness
			newCol.set(MIN(MAX(tempCol.r + intensity, 0.0f), 255.0f),
				MIN(MAX(tempCol.g + intensity, 0.0f), 255.0f),
				MIN(MAX(tempCol.b + intensity, 0.0f), 255.0f)
			);
			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	
	m_editedImage.update();
}

//function changes the contrast of the image depending on the intensity
void ofApp::applyContrast(float& intensity)
{
	ofColor tempCol;
	ofColor newCol;

	float contrastFactor = (259.0f * (intensity + 255.0f) / (255.0f * (259.0f - intensity)));

	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_origImage.getColor(x, y);

			//change contrast
			newCol.set(MIN(MAX(contrastFactor * (tempCol.r - 128.0f) + 128.0f, 0.0f), 255.0f),
				MIN(MAX(contrastFactor * (tempCol.g - 128.0f) + 128.0f, 0.0f), 255.0f),
				MIN(MAX(contrastFactor * (tempCol.b - 128.0f) + 128.0f, 0.0f), 255.0f)
			);

			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	
	m_editedImage.update();

}

//function changes the saturation of the image depending on the intensity
void ofApp::applySaturation(float& intensity)
{
	ofColor tempCol;
	ofColor newCol;

	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_origImage.getColor(x, y);

			int hue = tempCol.getHue();
			int saturation = tempCol.getSaturation();
			int brightness = tempCol.getBrightness();

			newCol = ofColor::fromHsb(hue, MIN(MAX(saturation + intensity, 0.0f), 255.0f), brightness);

			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	
	m_editedImage.update();
}

//function applies a solarisation effect depending on the intensity (tone reversal effect / overexposure)
void ofApp::applySolarise(float& intensity)
{
	ofColor tempCol;
	ofColor newCol;


	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_origImage.getColor(x, y);
			if (tempCol.r < intensity)
			{
				newCol.r = 255 - tempCol.r;
			}
			else
			{
				newCol.r = tempCol.r;
			}
			if (tempCol.g < intensity)
			{
				newCol.g = 255 - tempCol.g;
			}
			else
			{
				newCol.g = tempCol.g;
			}
			if (tempCol.b < intensity)
			{
				newCol.b = 255 - tempCol.b;
			}
			else
			{
				newCol.b = tempCol.b;
			}
			newCol.b = 255 - tempCol.b;


			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}
	m_editedImage.update();
}

//function changes the RGB values in an image depending on the hue selected and the intensity
void ofApp::applyRGB(float& intensity, int rgb_type) {
	ofColor tempCol;
	ofColor newCol;
	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_origImage.getColor(x, y);
			switch (rgb_type)
			{
				//cyan - red
			case 1:
				newCol.r = (MIN(MAX(tempCol.r - intensity *0.05f, 0.0f), 255.0f));				
				newCol.g = (MIN(MAX(tempCol.g - intensity, 0.0f), 255.0f));
				newCol.b = (MIN(MAX(tempCol.b - intensity, 0.3f), 255.0f));
				break;
				//magenta - green
			case 2:
				newCol.r = (MIN(MAX(tempCol.r - intensity, 0.0f), 255.0f));
				newCol.g = (MIN(MAX(tempCol.g - intensity * 0.05f, 0.0f), 255.0f));
				newCol.b = (MIN(MAX(tempCol.b - intensity, 0.3f), 255.0f));
				break;
				//yellow - blue
			case 3:
				newCol.r = (MIN(MAX(tempCol.r - intensity, 0.0f), 255.0f));
				newCol.g = (MIN(MAX(tempCol.g - intensity, 0.0f), 255.0f));
				newCol.b = (MIN(MAX(tempCol.b - intensity *0.05f, 0.3f), 255.0f));
				break;
			}
			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	
	m_editedImage.update();
}

//function makes image sharper
void ofApp::enableSharpenFilter()
{
	applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE::SHARPEN);
}

//function makes edges more defined
void ofApp::enableEdgeDetectionFilter()
{
	applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE::EDGE_DETECTION);
}

//function makes images have a bevel/ridged 3D look
void ofApp::enableBevelFilter()
{
	applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE::BEVEL);
}

//function inverts the RGB colour values
void ofApp::enableColourInversion()
{
	ofColor tempCol;
	ofColor newCol;


	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_editedImage.getColor(x, y);

			newCol.r = 255 - tempCol.r;
			newCol.g = 255 - tempCol.g;
			newCol.b = 255 - tempCol.b;
			

			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	
	m_editedImage.update();
}

//function changes the image to grayscale by setting saturation to 0
void ofApp::enableGreyscale()
{
	ofColor tempCol;
	ofColor newCol;


	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_editedImage.getColor(x, y);

			newCol = tempCol;
			newCol.setSaturation(0);

			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}

	m_editedImage.update();
}

//function changes image to grayscale then inverts the RGB values
void ofApp::enableGreyscaleInversion()
{
	ofColor tempCol;
	ofColor newCol;

	enableGreyscale();
	enableColourInversion();
}

//function creates a sepia tone for the image
void ofApp::enableSepiaFilter()
{
	ofColor tempCol;
	ofColor newCol;


	for (int y = 0; y < m_origImage.getHeight(); y++)
	{
		for (int x = 0; x < m_origImage.getWidth(); x++)
		{
			//get current colour
			tempCol = m_editedImage.getColor(x, y);

			//adjust colour to fit sepia tone
			newCol.r = 0.4 * tempCol.r + 0.45 * tempCol.g + 0.1 * tempCol.b;
			newCol.g = 0.35 * tempCol.r + 0.35 * tempCol.g + 0.1 * tempCol.b;
			newCol.b = 0.2 * tempCol.r + 0.25 * tempCol.g + 0.2 * tempCol.b;

			//set colour
			m_editedImage.setColor(x, y, newCol);
		}
	}
	m_editedImage.update();
}

//function blurs the image
void ofApp::enableBlur()
{
	applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE::BLUR);
}

//function loads the LUT CUBE file and stores the coordinates in a vec3 array
void ofApp::loadLUT(string lutPath)
{
	m_lutLoaded = false;

	ofFile file(lutPath);
	string line;
	//skip the first four lines of metadata
	for (int i = 0; i < 5; i++) {
		getline(file, line);
	}
	for (int z = 0; z < 32; z++) {
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 32; x++) {
				glm::vec3 cur;
				file >> cur.x >> cur.y >> cur.z;
				m_lut[x] [y] [z] = cur;
			}
		}
	}

	m_lutLoaded = true;
}

//I referenced the lutFilterExample code
//function applies the LUT grid points to the image
void ofApp::applyLUT(ofImage img)
{
	ofPixelsRef pix = img.getPixels();

		for (size_t y = 0; y < pix.getHeight(); y++) {
			for (size_t x = 0; x < pix.getWidth(); x++) {

				ofColor color = pix.getColor(x, y);

				int lutPos[3];
				for (int m = 0; m < 3; m++) {
					lutPos[m] = color[m] / 8;
					if (lutPos[m] == 31) {
						lutPos[m] = 30;
					}
				}

				glm::vec3 start = m_lut[lutPos[0]][lutPos[1]][lutPos[2]];
				glm::vec3 end = m_lut[lutPos[0] + 1][lutPos[1] + 1][lutPos[2] + 1];

				for (int k = 0; k < 3; k++) {
					float amount = (color[k] % 8) / 8.0f;
					color[k] = (start[k] + amount * (end[k] - start[k])) * 255;
				}

				m_editedImage.setColor(x, y, color);

			}
		}

		m_editedImage.update();
}

//function handles the application of different convolution matrices to the image
void ofApp::applyConvolution3x3(Constants_ImageProcessing::CONVOLUTION_MAT_TYPE matType)
{
	ofColor tempCol;
	ofColor newCol;
	ofVec3f sum;
	int neighbourX = 0;
	int neighbourY = 0;
	int startIndex = (int)matType * 9; // this gives us the starting position within the CONVOLUTION_MATS_3X3 array
	int imgWidth = m_origImage.getWidth();
	int imgHeight = m_origImage.getHeight();

	for (int y = 0; y < imgHeight; y++)
	{
		for (int x = 0; x < imgWidth; x++)
		{
			sum.set(0.0f, 0.0f, 0.0f);

			//loop though our neighbouring pixels and sum their factors - the chosen convolution matrix
			for (int k = startIndex; k < startIndex + 9; k++)
			{
				//get neighbour's x coordinate and "clamp" to image size
				neighbourX = x + Constants_ImageProcessing::CONVOLUTION_MATRIX_CONVERT_X[k % 9];
				if (neighbourX < 0)
				{
					neighbourX = 0;
				}
				else if (neighbourX > imgWidth - 1)
				{
					neighbourX = imgWidth - 1;
				}
				//get neighbour's Y coordinate and "clamp" to image size
				neighbourY = y + Constants_ImageProcessing::CONVOLUTION_MATRIX_CONVERT_Y[k % 9];
				if (neighbourY < 0)
				{
					neighbourY = 0;
				}
				else if (neighbourY > imgHeight - 1)
				{
					neighbourY = imgHeight - 1;
				}

				tempCol = m_origImage.getColor(neighbourX, neighbourY);

				//sum the kernel factors

				sum.x += tempCol.r * Constants_ImageProcessing::CONVOLUTION_MATS_3X3[k];
				sum.y += tempCol.g * Constants_ImageProcessing::CONVOLUTION_MATS_3X3[k];
				sum.z += tempCol.b * Constants_ImageProcessing::CONVOLUTION_MATS_3X3[k];
			}
			//factor loop done
			newCol.set(
				MIN(MAX(sum.x, 0.0f), 255.0f),
				MIN(MAX(sum.y, 0.0f), 255.0f),
				MIN(MAX(sum.z, 0.0f), 255.0f)
			);
			m_editedImage.setColor(x, y, newCol);
		}
	}

	//update
	m_editedImage.update();
}

//this function is called when loading images to make sure the image sizes fits the screen
ofImage ofApp::resizeImage(ofImage img)
{
	int testImgResizeX = img.getWidth();
	int testImgResizeY = img.getHeight();
	//decrease the image size until it is less than 70% of window width and height
	while (testImgResizeX > ofGetWindowWidth() * 0.6 || testImgResizeY > ofGetWindowHeight() * 0.9 )
	{
		testImgResizeX *= 0.9;
		testImgResizeY *= 0.9;
	}
	//resize image
	img.resize(testImgResizeX, testImgResizeY);
	return img;
}

//I referenced the input_output fileOpenSaveDialogExample for this function
//function opens image file at the specified path
void ofApp::openFileSelection(ofFileDialogResult openFileResult)
{
	ofLogVerbose getName = ofLogVerbose("getName(): " + openFileResult.getName());
	ofLogVerbose getPath = ofLogVerbose("getPath(): " + openFileResult.getPath());

	ofFile file(openFileResult.getPath());

	if (file.exists())
	{
		m_editedImage.clear();
		m_origImage.clear();
	
		string fileExtension = ofToUpper(file.getExtension());

		if (fileExtension == "JPG" || fileExtension == "PNG")
		{
			imgExtension = fileExtension;
			m_origImage.load(openFileResult.getPath());
			m_origImage = resizeImage(m_origImage);
			m_editedImage = m_origImage;
			resetImage();
			clearCanvas();
		}
	}
}
//function saves image to file path
void ofApp::saveImageTo()
{
	ofFileDialogResult saveFileResult = ofSystemSaveDialog("screenshot.png", "Saving image");
	if (saveFileResult.bSuccess)
	{
		m_editedImage.save(saveFileResult.filePath);
	}
}
