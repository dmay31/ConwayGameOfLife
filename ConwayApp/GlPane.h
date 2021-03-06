#pragma once

#define GLEW_STATIC
#include "wx/wx.h"
#include "wx/glcanvas.h"

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "Conway.h"

class GlPane :
	public wxGLCanvas
{
	wxGLContext*	m_context;

public:
	GlPane(wxFrame* parent, int* args);
	virtual ~GlPane();

	void resized(wxSizeEvent& evt);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	void drawSquare(int x, int y);

	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()

private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	const std::string loadShader(std::string filename);

	//position and color data handles
	GLuint mVBO[2];
	GLuint mVAO[2];
	GLuint mIBO;

	GLuint  mColor;

	ConwayEngine * mConwayEngine;
	//GLuint vertexArray[100];
};
