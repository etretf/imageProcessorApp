#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "Constants.h"



class Line {
public:
	Line();
	~Line();

	ofPolyline m_currentPath;
	ofColor m_currentColour;
	float m_strokeRadius;
};

