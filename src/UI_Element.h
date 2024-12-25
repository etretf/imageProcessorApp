#pragma once
#include "ofxMSAInteractiveObject.h"

static const ofColor IDLE_DARK_COL = ofColor(42, 76, 116);
static const ofColor HOVER_DARK_COL = ofColor(66, 150, 250);
static const ofColor PRESS_DARK_COL = ofColor(15, 125, 250);

class UI_Element : public ofxMSAInteractiveObject
{
public:

	ofVec2f m_ui_pos;
	ofColor m_base_col;
	ofColor m_pressed_col;
	ofColor m_hover_col;
	std::string m_ui_text;
	float m_radius;

	void m_ui_setup();
	void set(float px, float py, float w, float h, float radius);
	void m_ui_draw();
};

