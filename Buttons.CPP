#define INCL_WIN
#define INCL_GPI
#include <OS2.H>
#include <stdlib.H>
#include <string.H>
#include <math.H>
#include "Buttons.H"

#define HAVECAPTURE 1
#define HAVEFOCUS   2
#define INSIDERECT  4
#define SPACEDOWN   8

typedef union {
	ULONG Index;
	struct {
		UCHAR blue,green,red;
		UCHAR dummy;
	} rgb;
} CLR;

typedef struct {
  BYTE      Flags;
  HPOINTER  Bitmap;
	CLR       Colour;
	CLR       BackgroundColour;
//CLR         ForegroundColour;
//CLR         DisabledColour;
  PSZ       pszText;
} GBTN;

typedef GBTN *PGBTN;

MRESULT EXPENTRY ButtonClassProc(HWND, ULONG, MPARAM, MPARAM);

BOOL EXPENTRY RegisterShadedButtonClass() {
  return WinRegisterClass ((HAB) 0, SHADEDBTNCLASS, ButtonClassProc, CS_SIZEREDRAW, sizeof (PGBTN));
}

MRESULT EXPENTRY ButtonClassProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  POINTL        ptl;
  GBTN          *pGradBtn;
  WNDPARAMS     *pwprm;
  RECTL         rcl;

  pGradBtn = (PGBTN) WinQueryWindowPtr (hwnd, 0);

  switch(msg) {
  case WM_CREATE: {
      CREATESTRUCT  *pcrst;
      pGradBtn = (PGBTN) malloc (sizeof (GBTN));
      pGradBtn->Flags = 0;
      pcrst = (PCREATESTRUCT) PVOIDFROMMP(mp2);
      pGradBtn->pszText = (PSZ) malloc(1+strlen(pcrst->pszText));
      strcpy(pGradBtn->pszText, pcrst->pszText);
			if (WinQueryWindowULong(hwnd, QWL_STYLE) & GBT_GRAPHIC)
				pGradBtn->Bitmap = WinLoadPointer(HWND_DESKTOP, 0UL, atoi(pGradBtn->pszText));
		}
    WinSetWindowPtr(hwnd, 0, pGradBtn);

		ULONG ulID;
		if (!WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
			pGradBtn->BackgroundColour.Index = 0x00CCCCCC;
		if (!WinQueryPresParam(hwnd, GBPP_BUTTONCOLOR, GBPP_BUTTONCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
			pGradBtn->Colour.Index = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BUTTONMIDDLE, 0L);
    return 0;
	case GBM_SETCOLOR:
		pGradBtn->Colour.Index = LONGFROMMP (mp1);
    WinInvalidateRect(hwnd, NULL, FALSE);
		return MRFROMSHORT(1);
  case WM_SETWINDOWPARAMS:
    pwprm = (PWNDPARAMS) PVOIDFROMMP (mp1);
    if(pwprm->fsStatus & WPM_TEXT) {
      free(pGradBtn->pszText);
      pGradBtn->pszText = (PSZ) malloc(1+pwprm->cchText);
      strcpy(pGradBtn->pszText, pwprm->pszText);
    } else break;
		WinInvalidateRect(hwnd, NULL, FALSE);
    return MRFROMSHORT(1);
  case WM_QUERYWINDOWPARAMS:
    pwprm = (PWNDPARAMS) PVOIDFROMMP (mp1);
    if(pwprm->fsStatus & WPM_CCHTEXT)
      pwprm->cchText = strlen (pGradBtn->pszText);
    if(pwprm->fsStatus & WPM_TEXT)
      strcpy (pwprm->pszText, pGradBtn->pszText);
    if(pwprm->fsStatus & WPM_CBCTLDATA)
      pwprm->cbCtlData = 0;
    if(pwprm->fsStatus & WPM_CTLDATA)
      pwprm->pCtlData = 0;
		if(pwprm->fsStatus & (WPM_CCHTEXT | WPM_TEXT | WPM_CBCTLDATA | WPM_CTLDATA))
	    return MRFROMSHORT(1);
		break;
	case WM_PRESPARAMCHANGED:
		switch (LONGFROMMP(mp1)) {
			case 0:
				if (!WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
					pGradBtn->BackgroundColour.Index = 0x00CCCCCC;
				if (!WinQueryPresParam(hwnd, GBPP_BUTTONCOLOR, GBPP_BUTTONCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
					pGradBtn->Colour.Index = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BUTTONMIDDLE, 0L);
				return MRFROMSHORT(1);
			case PP_BACKGROUNDCOLOR:
			case PP_BACKGROUNDCOLORINDEX:
				if (!WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
					pGradBtn->BackgroundColour.Index = 0x00CCCCCC;
				return MRFROMSHORT(1);
			case GBPP_BUTTONCOLOR:
			case GBPP_BUTTONCOLORINDEX:
				if (!WinQueryPresParam(hwnd, GBPP_BUTTONCOLOR, GBPP_BUTTONCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
					pGradBtn->Colour.Index = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BUTTONMIDDLE, 0L);
				return MRFROMSHORT(1);
		}
		break;
	case WM_SYSCOLORCHANGE:
		if (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, NULL, 4UL, (PVOID)&(pGradBtn->Colour.Index), QPF_ID2COLORINDEX))
			pGradBtn->Colour.Index = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BUTTONMIDDLE, 0L);
		break;
  case WM_BUTTON1DOWN:
    if(WinQueryWindowULong(hwnd, QWL_STYLE) & GBT_NOPUSH)
      break;
    WinSetFocus(HWND_DESKTOP, hwnd);
    WinSetCapture(HWND_DESKTOP, hwnd);
    pGradBtn->Flags |= HAVECAPTURE | INSIDERECT;
    WinInvalidateRect(hwnd, NULL, FALSE);
    return 0;
  case WM_MOUSEMOVE: {
      BOOL fTestInsideRect;
      if((pGradBtn->Flags & HAVECAPTURE) == 0)
        break;
      WinQueryWindowRect(hwnd, &rcl);
      ptl.x = MOUSEMSG(&msg)->x;
      ptl.y = MOUSEMSG(&msg)->y;
      fTestInsideRect = WinPtInRect (WinQueryAnchorBlock(hwnd), &rcl, &ptl);
      if(((pGradBtn->Flags & INSIDERECT) != 0) != fTestInsideRect) {
        pGradBtn->Flags ^= INSIDERECT;
        WinInvalidateRect(hwnd, NULL, FALSE);
      }
    }
    break;
  case WM_BUTTON1UP:
    if ((pGradBtn->Flags & HAVECAPTURE == 0) || !WinIsWindowEnabled(hwnd))
      break;
    WinSetCapture(HWND_DESKTOP, NULLHANDLE);
    pGradBtn->Flags &= !(HAVECAPTURE | INSIDERECT);
    WinQueryWindowRect(hwnd, &rcl);
    ptl.x = MOUSEMSG(&msg)->x;
    ptl.y = MOUSEMSG(&msg)->y;
    if(WinPtInRect(WinQueryAnchorBlock(hwnd), &rcl, &ptl))
      WinPostMsg(WinQueryWindow(hwnd, QW_OWNER), WM_COMMAND,
          MPFROMSHORT(WinQueryWindowUShort(hwnd, QWS_ID)),
          MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
    WinInvalidateRect(hwnd, NULL, FALSE);
    return 0;
  case WM_ENABLE:
    WinInvalidateRect(hwnd, NULL, FALSE);
    return 0;
  case WM_SETFOCUS:
    pGradBtn->Flags |= SHORT1FROMMP(mp2) & HAVEFOCUS;
    WinInvalidateRect(hwnd, NULL, FALSE);
    return 0;
  case WM_CHAR:
    if(!(CHARMSG(&msg)->fs & KC_VIRTUALKEY) ||
         CHARMSG(&msg)->vkey != VK_SPACE    ||
         CHARMSG(&msg)->fs & KC_PREVDOWN    ||
         !WinIsWindowEnabled(hwnd))
      break;
    if(!(CHARMSG(&msg)->fs & KC_KEYUP))
      pGradBtn->Flags |= SPACEDOWN;
    else {
      pGradBtn->Flags &= !SPACEDOWN;
      WinPostMsg(WinQueryWindow(hwnd, QW_OWNER), WM_COMMAND,
          MPFROMSHORT(WinQueryWindowUShort(hwnd, QWS_ID)),
          MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
    }
    WinInvalidateRect(hwnd, NULL, FALSE);
    return 0;
	case WM_PAINT: {
			HPS hps;
			long height;
			CLR colr;
			POINTL aptl[TXTBOX_COUNT];
			POINTL left,right;
			ULONG WindowStyle;
// Initialize
			WinQueryWindowRect(hwnd, &rcl);
			WindowStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
			height = rcl.yTop;
			hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
			WinFillRect(hps, &rcl, CLR_PALEGRAY);
			GpiCreateLogColorTable(hps, LCOL_RESET, LCOLF_RGB, 0L, 0L, NULL);
// Draw gradient pattern
			colr.Index = 0UL;
			left.x = rcl.xLeft;
			right.x = rcl.xRight;
			for(left.y = right.y = 1; right.y < height; right.y++, left.y++) {
				if (pGradBtn->Flags & (INSIDERECT | SPACEDOWN)) {
					colr.rgb.red = pGradBtn->Colour.rgb.red * (2 * right.y + 3 * height) / 6 / height;
					colr.rgb.green = pGradBtn->Colour.rgb.green * (2 * right.y + 3 * height) / 6 / height;
					colr.rgb.blue = pGradBtn->Colour.rgb.blue * (2 * right.y + 3 * height) / 6 / height;
				} else {
					colr.rgb.red = pGradBtn->Colour.rgb.red * ( height + right.y) / 2 / height;
					colr.rgb.green = pGradBtn->Colour.rgb.green * (height + right.y) / 2 / height;
					colr.rgb.blue = pGradBtn->Colour.rgb.blue * (height + right.y) / 2 / height;
				}
				if(WindowStyle & GBT_RIGHTROUND)
					right.x = rcl.xRight - height / 2 + (LONG) sqrt((float) (right.y * (height - right.y)));
				if(WindowStyle & GBT_LEFTROUND)
					left.x = height / 2 - (LONG) sqrt((float) (left.y * (height - left.y)));
				GpiSetColor(hps, colr.Index);
				GpiMove(hps, &left);
				GpiLine(hps, &right);
			}
// Draw border
			rcl.xRight--;
			rcl.yTop--;
			ptl.y = rcl.yBottom;
			ptl.x = (WindowStyle & GBT_LEFTROUND) ? rcl.xLeft + height / 2 : rcl.xLeft;
			GpiMove(hps, &ptl);
			ptl.y = rcl.yBottom;
			ptl.x = (WindowStyle & GBT_RIGHTROUND) ? rcl.xRight - height / 2 : rcl.xRight;
			GpiSetColor(hps, 0x00888888);
			GpiLine(hps, &ptl);
			ptl.y = rcl.yTop;
			if (WindowStyle & GBT_RIGHTROUND) {
				ARCPARAMS arcparams;
				arcparams.lP = arcparams.lQ = height / 2;
				arcparams.lR = arcparams.lS = 0L;
				GpiSetArcParams(hps, &arcparams);
				right.x = rcl.xRight - height / 2;
				right.y = height / 2;
				GpiPartialArc(hps, &right, MAKEFIXED(1,0), MAKEFIXED(270,0), MAKEFIXED(90,0));
				if (!(height % 2))
					right.y--;
				GpiPartialArc(hps, &right, MAKEFIXED(1,0), MAKEFIXED(0,0), MAKEFIXED(45,0));
				GpiSetColor(hps, (pGradBtn->Flags & (INSIDERECT | SPACEDOWN) ? 0x00888888 : 0x00FFFFFF));
				GpiPartialArc(hps, &right, MAKEFIXED(1,0), MAKEFIXED(45,0), MAKEFIXED(45,0));
				GpiMove(hps, &ptl);
			} else { if (WindowStyle & GBT_NORIGHTLINE)
					GpiMove(hps, (PPOINTL) &rcl.xRight);
				else
					GpiLine(hps, (PPOINTL) &rcl.xRight);
				GpiSetColor(hps, (pGradBtn->Flags & (INSIDERECT | SPACEDOWN) ? 0x00888888 : 0x00FFFFFF));
			}
			ptl.y = rcl.yTop;
			ptl.x = (WindowStyle & GBT_LEFTROUND) ? rcl.xLeft + height / 2 : rcl.xLeft;
			GpiLine(hps, &ptl);
			rcl.yBottom = 1;
			if (WindowStyle & GBT_LEFTROUND) {
				ARCPARAMS arcparams;
				arcparams.lP = arcparams.lQ = height / 2;
				arcparams.lR = arcparams.lS = 0L;
				GpiSetArcParams(hps, &arcparams);
				ptl.y = ptl.x = height / 2;
				if (!(height % 2))
					ptl.y--;
				GpiPartialArc(hps, &ptl, MAKEFIXED(1,0), MAKEFIXED(90,0), MAKEFIXED(90,0));
				if (!(height % 2))
					ptl.y++;
				GpiPartialArc(hps, &ptl, MAKEFIXED(1,0), MAKEFIXED(180,0), MAKEFIXED(45,0));
				GpiSetColor(hps, 0x00888888);
				GpiPartialArc(hps, &ptl, MAKEFIXED(1,0), MAKEFIXED(215,0), MAKEFIXED(45,0));
			} else if (!(WindowStyle & GBT_NOLEFTLINE))
					GpiLine(hps, (PPOINTL) &rcl.xLeft);
			rcl.yBottom = 0;
// Draw text
//			GpiCreateLogColorTable(hps, LCOL_RESET, LCOLF_DEFAULT, 0L, 0L, NULL);
			ptl.x = rcl.xRight >> 1;
			ptl.y = rcl.yTop >> 1;
			if (WindowStyle & GBT_RIGHTROUND)
				ptl.x -= rcl.yTop >> 1;
			if (WindowStyle & GBT_LEFTROUND)
				ptl.x += rcl.yTop >> 1;
			if (pGradBtn->Flags & (INSIDERECT | SPACEDOWN)) {
				ptl.x++;
				ptl.y--;
			}
			if (WindowStyle & GBT_GRAPHIC) {
				LONG PointerWidth, PointerHeight;
				PointerWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXPOINTER);
				PointerHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYPOINTER);
				ptl.x -= PointerWidth >> 1;
				ptl.y -= PointerHeight >> 1;
				WinDrawPointer(hps, ptl.x, ptl.y, pGradBtn->Bitmap, WinIsWindowEnabled(hwnd) ? DP_NORMAL : DP_HALFTONED);
			} else {
				GpiQueryTextBox(hps, strlen(pGradBtn->pszText), pGradBtn->pszText, TXTBOX_COUNT, aptl);
				ptl.x -= aptl[TXTBOX_CONCAT].x >> 1;
				ptl.y -= (aptl[TXTBOX_TOPLEFT].y + aptl[TXTBOX_BOTTOMLEFT].y) >> 1;
				GpiSetColor(hps, (WinIsWindowEnabled(hwnd) ? 0x00000000 : 0x00888888));
				GpiCharStringAt (hps, &ptl, strlen(pGradBtn->pszText), pGradBtn->pszText);
			}
			WinEndPaint(hps);
			return 0;
		}
  case WM_DESTROY:
                if (WinQueryWindowULong(hwnd, QWL_STYLE) & GBT_GRAPHIC)
                        WinDestroyPointer(pGradBtn->Bitmap);
    free(pGradBtn->pszText);
    free(pGradBtn);
    return 0;
  }
  return WinDefWindowProc(hwnd, msg, mp1, mp2);
}
