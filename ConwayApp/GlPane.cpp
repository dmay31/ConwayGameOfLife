#include "wx/wx.h"
#include "wx/sizer.h"
#include <GL/glew.h>
#include "wx/glcanvas.h"
#include "GlPane.h"
#include "Conway.h"
#include "config.h"

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/ext/matrix_transform.hpp> 
#include <iostream>
#include <fstream>

#include <stdlib.h>

BEGIN_EVENT_TABLE(GlPane, wxGLCanvas)
EVT_MOTION(GlPane::mouseMoved)
EVT_LEFT_DOWN(GlPane::mouseDown)
EVT_LEFT_UP(GlPane::mouseReleased)
EVT_RIGHT_DOWN(GlPane::rightClick)
EVT_LEAVE_WINDOW(GlPane::mouseLeftWindow)
EVT_SIZE(GlPane::resized)
EVT_KEY_DOWN(GlPane::keyPressed)
EVT_KEY_UP(GlPane::keyReleased)
EVT_MOUSEWHEEL(GlPane::mouseWheelMoved)
EVT_PAINT(GlPane::render)
END_EVENT_TABLE()


// some useful events to use
void GlPane::mouseMoved(wxMouseEvent& event) {}
void GlPane::mouseDown(wxMouseEvent& event) {}
void GlPane::mouseWheelMoved(wxMouseEvent& event) {}
void GlPane::mouseReleased(wxMouseEvent& event) {}
void GlPane::rightClick(wxMouseEvent& event) {}
void GlPane::mouseLeftWindow(wxMouseEvent& event) {}
void GlPane::keyPressed(wxKeyEvent& event) {}
void GlPane::keyReleased(wxKeyEvent& event) {}

glm::mat4 modelMatrix;

// Vertices and faces of a simple cube to demonstrate 3D render
// source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
GLfloat v[8][3];
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
	{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
	while (GLenum aError = glGetError()) {
		std::cout << "[OpenGL Error] (" << aError << ")" << std::endl;
	}
}


unsigned int GlPane::compileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char * src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	int length;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (GL_FALSE == result) {
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char * message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		printf(message);

	}

	return id;
}

const std::string GlPane::loadShader(std::string filename) {

	std::ifstream fileStream(filename, std::ios::in);

	std::string line = "";
	std::string content;
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();

	return content;
}


GlPane::GlPane(wxFrame* parent, int* args) :
	wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	mConwayEngine = new ConwayEngine();

	m_context = new wxGLContext(this);
	// prepare a simple cube to demonstrate 3D render
	// source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	//float positions[6] = {
	//	 50.0f, 50.0f,
	//	 100.0f,  150.0f,
	//	 150.0f,  50.0f
	//};

	float positions[] = {
	 10.0f, 10.0f, // 0
	 15.0f,  10.0f,
	 15.0f,  15.0f,
	 10.0f, 15.0f //3
	};

	GLuint indices[]{ 0, 1, 2, 2, 3, 0 };

	// To avoid flashing on MSW
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	this->SetCurrent(*m_context);

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		printf("%d\n", err);
	}
	float lines[TOTAL_SQUARES*2];
	GLuint j = 0;

	for (auto i = 0; i < (HEIGHT / SPACING) + 1; i++) {
		lines[j++] = -5.0f;
		lines[j++] = SPACING*i;
		lines[j++] = WIDTH;
		lines[j++] = SPACING * i;
	}

	for (auto i = 0; i < (WIDTH / SPACING) + 1; i++) {
		lines[j++] = SPACING * i;
		lines[j++] = 0;
		lines[j++] = SPACING * i;
		lines[j++] = HEIGHT;
	}

	auto content = loadShader(std::string("vertex.glsl"));
	auto vs = compileShader(GL_VERTEX_SHADER, content);
	content = loadShader(std::string("fragment.glsl"));
	auto fs = compileShader(GL_FRAGMENT_SHADER, content);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	glUseProgram(program);;

	modelMatrix = glm::ortho(0.0f, 700.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	GLint aMVP = glGetUniformLocation(program, "MVP");
	glUniformMatrix4fv(aMVP, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mColor = glGetUniformLocation(program, "setColor");

	glGenBuffers(2, mVBO);
	glGenVertexArrays(2, mVAO);
	glGenBuffers(1, &mIBO);
	
	// Grid
	glBindVertexArray(mVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, TOTAL_SQUARES * 2 * sizeof(float), lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(0);
	
	// Triangle
	GLClearError();
	glBindVertexArray(mVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
	glVertexAttribPointer(0, 2, GL_UNSIGNED_INT, FALSE, sizeof(GLuint) * 2, 0);
	glEnableVertexAttribArray(0);
	GLCheckError();
	
}

void GlPane::drawSquare(int x, int y)
{
	unsigned int xpos = x * SPACING;
	unsigned int ypos = y * SPACING;

	int x1 = xpos;
	int y1 = ypos;
	int x2 = xpos + SPACING;
	int y2 = ypos;
	int x3 = xpos + SPACING;
	int y3 = ypos + SPACING;

}

GlPane::~GlPane()
{
	delete m_context;
}

void GlPane::resized(wxSizeEvent& evt)
{
	//	wxGLCanvas::OnSize(evt);

	Refresh();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void GlPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glEnable(GL_TEXTURE_2D);   // textures
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(topleft_x, topleft_y, bottomrigth_x - topleft_x, bottomrigth_y - topleft_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

int GlPane::getWidth()
{
	return GetSize().x;
}

int GlPane::getHeight()
{
	return GetSize().y;
}


void GlPane::render(wxPaintEvent& evt)
{
	std::vector<std::pair<int, int>> currentSquares;

	if (!IsShown()) return;

	currentSquares = mConwayEngine->GetCoordinates();
	GLuint triangleCount = currentSquares.size() * 2;
	GLuint memSize = triangleCount * 3 * 2 * sizeof(GLuint);
	GLuint * vertexArray = (GLuint*)malloc(memSize);
	GLuint vertexArrayOffset = 0;
	memset(vertexArray, 0x00, memSize);

	for (std::pair<int,int> aSquare : currentSquares) {
		GLuint x = std::get<0>(aSquare) * SPACING;
		GLuint y = std::get<1>(aSquare) * SPACING;

		vertexArray[vertexArrayOffset++] = x;
		vertexArray[vertexArrayOffset++] = y;
		vertexArray[vertexArrayOffset++] = x + SPACING;
		vertexArray[vertexArrayOffset++] = y;
		vertexArray[vertexArrayOffset++] = x + SPACING;
		vertexArray[vertexArrayOffset++] = y + SPACING;

		vertexArray[vertexArrayOffset++] = x;
		vertexArray[vertexArrayOffset++] = y;
		vertexArray[vertexArrayOffset++] = x;
		vertexArray[vertexArrayOffset++] = y + SPACING;
		vertexArray[vertexArrayOffset++] = x + SPACING;
		vertexArray[vertexArrayOffset++] = y + SPACING;

	}

	mConwayEngine->Update();

	glBindVertexArray(mVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexArrayOffset * sizeof(GLuint), vertexArray, GL_DYNAMIC_DRAW);

	free(vertexArray);

	wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ------------- draw some 2D ----------------
	prepare2DViewport(0, 0, getWidth(), getHeight());
	glLoadIdentity();

	// red square
	//glColor4f(.5, .5, .5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(mVAO[0]);
	glUniform4fv(mColor, 1, glm::value_ptr(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)));
	glDrawArrays(GL_LINES, 0, GRID_LINES);
	glBindVertexArray(mVAO[1]);
	glUniform4fv(mColor, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
	GLClearError();
	glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	GLCheckError();

	glFlush();
	SwapBuffers();

	Refresh();
}