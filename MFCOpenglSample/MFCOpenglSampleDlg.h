
// MFCOpenglSampleDlg.h : ͷ�ļ�
//

#pragma once
#include "OpenGLControl.h"

// CMFCOpenglSampleDlg �Ի���
class CMFCOpenglSampleDlg : public CDialogEx
{
// ����
public:
	CMFCOpenglSampleDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCOPENGLSAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;


	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	COpenGLControl m_oglWindow;
};
