
// ChildView.h : interface of the CChildView class
//


#pragma once

#include "EraseDialog.h"

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	void erase_tree(int i);

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	typedef MySTL::RB_tree<int,int,identity<int>,less<int> > tree;
	tree t1;

	EraseDialog m_erase_dialog;
};

