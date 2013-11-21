// EraseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TreeDrawing.h"
#include "EraseDialog.h"
#include "ChildView.h"


// EraseDialog dialog

IMPLEMENT_DYNAMIC(EraseDialog, CDialog)

EraseDialog::EraseDialog(CChildView* pView, CWnd* pParent /*=NULL*/)
	: CDialog(EraseDialog::IDD, pParent)
{
	m_view = pView;
}

EraseDialog::~EraseDialog()
{
}

void EraseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EraseDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &EraseDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// EraseDialog message handlers

void EraseDialog::OnBnClickedButton1()
{
	int i = GetDlgItemInt(IDC_EDIT_ERASE);   
	m_view->erase_tree(i);

	// TODO: Add your control notification handler code here
}
