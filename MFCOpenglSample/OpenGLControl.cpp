#include "stdafx.h"
#include "OpenGLControl.h"


//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//
GLuint esLoadShader(GLenum type, const char *shaderSrc)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
	{
		return 0;
	}

	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char *infoLog = (char*)malloc(sizeof(char) * infoLen);

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			//			esLogMessage("Error compiling shader:\n%s\n", infoLog);

			free(infoLog);
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;

}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
//         Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//
GLuint esLoadProgram(const char *vertShaderSrc, const char *fragShaderSrc)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programObject;
	GLint linked;

	// Load the vertex/fragment shaders
	vertexShader = esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);

	if (vertexShader == 0)
	{
		return 0;
	}

	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);

	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		return 0;
	}

	// Create the program object
	programObject = glCreateProgram();

	if (programObject == 0)
	{
		return 0;
	}

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);

	// Link the program
	glLinkProgram(programObject);

	// Check the link status
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;

		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char *infoLog = (char*)malloc(sizeof(char) * infoLen);

			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			//			esLogMessage("Error linking program:\n%s\n", infoLog);

			free(infoLog);
		}

		glDeleteProgram(programObject);
		return 0;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return programObject;
}


BEGIN_MESSAGE_MAP(COpenGLControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

COpenGLControl::COpenGLControl()
{
}


COpenGLControl::~COpenGLControl()
{
}


void COpenGLControl::oglCreate(CRect rect, CWnd *parent)
{
	CString className = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL,
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL);

	CreateEx(0, className, L"OpenGL", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);//这里会触发OnCreate事件

	// Setinitial variables' values
	//ShowWindow(SW_SHOW);
}



void COpenGLControl::OnPaint()
{
//	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()

	ValidateRect(NULL);
}


//?? 窗口创建成功之前还是之后创建?
int COpenGLControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	oglInitialize();

	return 0;
}


void COpenGLControl::oglInitialize()
{
	CreateGLContext(GetDC());
	PrepareScence();
}

bool COpenGLControl::CreateGLContext(CDC *pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		return false;
	}

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
		return false;
	}

	HGLRC tempContext = wglCreateContext(pDC->m_hDC);
	wglMakeCurrent(pDC->m_hDC, tempContext);


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		AfxMessageBox(_T("GLEW is not initialized!"));
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	m_hrc = wglCreateContextAttribsARB(pDC->m_hDC, 0, attribs);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	//set new current
	wglMakeCurrent(pDC->m_hDC, m_hrc);

	return true;
}


void COpenGLControl::PrepareScence()
{
	const char *vertexShaderStr =

		"attribute vec3 position;                       \n"
		"attribute vec2 texcoord;                       \n"

		"varying vec2  v_texcoord;                      \n"

		"void main()                                    \n"
		"{                                              \n"
		"  v_texcoord =  texcoord ;                     \n"
		"  gl_Position = vec4(position , 1.0) ;         \n"
		"}                                              \n"

		;

	const char *fragShaderStr =

		"precision highp float;							\n"
		"varying vec2 v_texcoord;                       \n"
		"uniform sampler2D  y_sampler;                  \n"
		"uniform sampler2D  u_sampler;                  \n"
		"uniform sampler2D  v_sampler;                  \n"
		"void main()                                    \n"
		"{                                              \n"
		"  float y,u,v,r,g,b;                               \n"
		"  y = texture2D(y_sampler,v_texcoord).r;           \n"
		"  u = texture2D(u_sampler,v_texcoord).r;           \n"
		"  v = texture2D(v_sampler,v_texcoord).r;           \n"
		"  y = 1.16438355 * (y - 0.0625);                   \n"
		"  u = u - 0.5;                                     \n"
		"  v = v - 0.5;                                     \n"
		"  r = clamp(y + 1.596 * v, 0.0, 1.0);              \n"
		"  g = clamp(y - 0.391 * u - 0.813 * v, 0.0, 1.0);              \n"
		"  b = clamp(y + 2.018 * u, 0.0, 1.0);              \n"
		"  gl_FragColor = vec4(r,g,b,1.0) ;                 \n"
		"}                                                  \n"
		;

	m_programObject = esLoadProgram(vertexShaderStr, fragShaderStr);
	m_attrPos = glGetAttribLocation(m_programObject, "position");
	m_attrUv = glGetAttribLocation(m_programObject, "texcoord");

	m_sam_Y = glGetUniformLocation(m_programObject, "y_sampler");
	m_sam_U = glGetUniformLocation(m_programObject, "u_sampler");
	m_sam_V = glGetUniformLocation(m_programObject, "v_sampler");


	uint8_t * pbuf = new uint8_t [m_SRCW * m_SRCH * 3  / 2] ;
	FILE *fs = fopen("d://akiyo_cif.yuv" , "rb+");
	fread(pbuf,m_SRCW * m_SRCH * 3  / 2 , 1,fs);
	fclose(fs);

	glGenTextures(3, m_textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_SRCW, m_SRCH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pbuf);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textures[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_SRCW >> 1, m_SRCH >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pbuf + m_SRCW * m_SRCH);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textures[2]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_SRCW >> 1, m_SRCH >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pbuf + m_SRCW * m_SRCH + m_SRCW * m_SRCH / 4);

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	delete[] pbuf;
	pbuf = NULL;

}



void COpenGLControl::onDraw(CDC *pDC)
{
}

void COpenGLControl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	oglDrawScene();

	SwapBuffers(GetDC()->m_hDC);

	CWnd::OnTimer(nIDEvent);
}


void COpenGLControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED) return;

	// Map the OpenGL coordinates.
	glViewport(0, 0, cx, cy);

}


void COpenGLControl::oglDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glUseProgram(m_programObject);

	GLfloat arrays[6 * 5] =
	{
		//第一个三角形 -- 逆时针
		-1.0f,-1.0f,0.0f , 0.0f,1.0f , // bottom left
		1.0f ,-1.0f,0.0f , 1.0f,1.0f , // bottom right
		1.0f,1.0f,0.0f,    1.0f,0.0f , // top right

		 // 第二个三角形
		1.0f,1.0f,0.0f,    1.0f,0.0f , // top right
		-1.0f,1.0f,0.0f,    0.0f,0.0f ,// top left
		-1.0f,-1.0f,0.0f , 0.0f,1.0f , // bottom left							   							   
	};

	glEnableVertexAttribArray(m_attrPos);
	glVertexAttribPointer(m_attrPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), arrays);

	glEnableVertexAttribArray(m_attrUv);
	glVertexAttribPointer(m_attrUv, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), &arrays[3]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textures[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textures[2]);

	glUniform1i(m_sam_Y, 0);
	glUniform1i(m_sam_U, 1);
	glUniform1i(m_sam_V, 2);

	glDrawArrays(GL_TRIANGLES, 0, 6);

}