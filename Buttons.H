#define SHADEDBTNCLASS  "GradButton"

BOOL EXPENTRY RegisterShadedButtonClass();

// Window messages
#define GBM_SETCOLOR    (WM_USER + 0)

// Presentation parameters
#define GBPP_BUTTONCOLOR (PP_USER + 0)
#define GBPP_BUTTONCOLORINDEX (PP_USER + 1)

// Window styles
#define GBT_NOLEFTLINE  0x00000001
#define GBT_LEFTROUND   0x00000002
#define GBT_NORIGHTLINE 0x00000004
#define GBT_RIGHTROUND  0x00000008
#define GBT_VERTICAL    0x00000010
#define GBT_GRAPHIC     0x00000020
#define GBT_NOPUSH      0x00000040
