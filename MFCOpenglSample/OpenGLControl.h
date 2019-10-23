#pragma once
#include "afxwin.h"

//egl module
// #include "GL/glew.h"
// #include "GL/wglew.h"

// #include "esUtil.h"
// 



#include <stdint.h>

//---- OpenGL ------

#include "GL/glew.h"
#include "GL/wglew.h"
#pragma comment(lib, "glew32.lib")

#		pragma comment(lib, "glu32.lib")    /* link OpenGL Utility lib     */
#       pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#       pragma comment (lib, "gdi32.lib")    /* link Windows GDI lib        */
#       pragma comment (lib, "winmm.lib")    /* link Windows MultiMedia lib */
#       pragma comment (lib, "user32.lib")   /* link Windows user lib       */

//----------------


class COpenGLControl :
	public CWnd
{
public:
	COpenGLControl();
	virtual ~COpenGLControl();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	

protected:

	void oglInitialize(void);
	void onDraw(CDC *pDC);
	void oglDrawScene(void);

	bool CreateGLContext(CDC *pDC);
	void PrepareScence();


public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void oglCreate(CRect rect, CWnd *parent);

public:
	UINT_PTR m_unpTimer;


protected:

	HGLRC m_hrc;			// OpenGL Rendering Context
	GLuint m_programObject;
	GLuint m_attrPos;
	GLuint m_attrUv;

	GLuint m_sam_Y;
	GLuint m_sam_U;
	GLuint m_sam_V;
	
	GLuint m_textures[3];

	int m_SRCW = 352;
	int m_SRCH = 288;

};

