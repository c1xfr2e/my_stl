
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "TreeDrawing.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace MySTL;

// CChildView

CChildView::CChildView()
: m_erase_dialog(this)
{
	srand((unsigned)time( NULL )); 
 	for (int i = 0; i < 30; i++)
 	{
		int k=rand()%100+1;
		t1.insert_unique(k);
 	}


// 	t1.erase(88);
// 	t1.erase(89);
// 	t1.erase(90);
// 	t1.erase(92);
// 	t1.erase(94);
// 	t1.erase(80);
// 	t1.erase(82);
// 	t1.erase(84);
// 	t1.erase(72);
// 	t1.erase(74);
// 	t1.erase(76);
// 	t1.erase(64);
// 	t1.erase(65);
// 	t1.erase(66);
// 	t1.erase(68);
// 	t1.erase(70);
// 	t1.erase(67);

}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	m_erase_dialog.Create(IDD_DIALOG1);

	return TRUE;
}

void CChildView::OnPaint() 
{
	MySTL::vector<draw_node> nodes;
	MySTL::vector<draw_edge> edges;
	t1.draw_tree(10, 1356, 40, 48, edges, nodes);

	CPaintDC dc(this); // device context for painting
// 	int width = 1024;
// 	int height = 1024;
// 	int unit = 32;
// 	for (int i = 0; i <= width; i += unit)
// 	{
// 		dc.MoveTo(0, i);
// 		dc.LineTo(width, i);
// 		dc.MoveTo(i, 0);
// 		dc.LineTo(i, width);
// 	}

	CBrush bkg_brh(RGB(204,232,207));	
	RECT rcClient;
	GetClientRect(&rcClient);
	dc.FillRect(&rcClient, &bkg_brh);

	CBrush brh(RGB(33,66,99));	
	dc.SelectObject(&brh);
	dc.SetTextColor(RGB(255,255,255));
	dc.SetBkMode(TRANSPARENT);


	for (size_t i = 0; i < edges.size(); ++i)
	{
		draw_edge de = edges[i];
		dc.MoveTo(de.x0, de.y0);
		dc.LineTo(de.x1, de.y1);
	}
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		draw_node dn = nodes[i];
		CRect rc(dn.x0 - 20, dn.y0 - 20, dn.x0 + 20, dn.y0 + 20);

		CBrush brh(dn.color);	
		dc.SelectObject(&brh);
		dc.Ellipse(&rc);
		dc.TextOut(dn.x0 -10, dn.y0 - 10, dn.str.c_str(), dn.str.length());
	}


	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_erase_dialog.ShowWindow(SW_SHOW);
}

void CChildView::erase_tree(int i)
{
	t1.erase(i);
	this->Invalidate(TRUE);
}