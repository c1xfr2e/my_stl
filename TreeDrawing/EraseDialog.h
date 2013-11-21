#pragma once

class CChildView;
// EraseDialog dialog

class EraseDialog : public CDialog
{
	DECLARE_DYNAMIC(EraseDialog)

public:
	EraseDialog(CChildView* pView, CWnd* pParent = NULL);   // standard constructor
	virtual ~EraseDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CChildView* m_view;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
