//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// - Example 08: Radio control
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <libgen.h>       // For path parsing


// Defines for resources
#define MAX_PATH  255
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_GRAD_BACK   "/res/grad-blue1.bmp"
#define IMG_GRADBAR_TOP "/res/gradbar-purple-top.bmp"
#define IMG_GRADBAR_BOT "/res/gradbar-purple-bot.bmp"
char  m_strImgGradBack[MAX_PATH];
char  m_strImgGradBarTop[MAX_PATH];
char  m_strImgGradBarBot[MAX_PATH];

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_IMG_BACK,E_IMG_BAR_TOP,E_IMG_BAR_BOT,
      E_BOX_CURRENT,E_BOX_PRESETS,
      E_BTN1,E_BTN2,E_BTN3,E_BTN4,E_BTN5,E_BTN6,E_BTN7,E_BTN8,
      E_BTN_P1,E_BTN_P2,E_BTN_P3,E_BTN_P4,E_BTN_P5,
      E_BTN_QUIT};

enum {E_FONT_BTN,E_FONT_TXT,E_FONT_HEAD,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            10
#define MAX_ELEM_PG_MAIN    30

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];


// Configure environment variables suitable for display
// - These may need modification to match your system
//   environment and display type
// - Defaults for GSLC_DEV_FB and GSLC_DEV_TOUCH are in GUIslice_config.h
// - Note that the environment variable settings can
//   also be set directly within the shell via export
//   (or init script).
//   - eg. export TSLIB_FBDEVICE=/dev/fb1
void UserInitEnv()
{
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)
  setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);
  setenv((char*)"SDL_FBDEV",GSLC_DEV_FB,1);
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
#endif

#if defined(DRV_TOUCH_TSLIB)
  setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1);
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
#endif
}

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create page elements
bool InitOverlays(char *strBasePath)
{
  gslc_tsElem*  pElem = NULL;
  gslc_tsElem*  pElemSrc = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background gradient
  strncpy(m_strImgGradBack,strBasePath,MAX_PATH);
  strncat(m_strImgGradBack,IMG_GRAD_BACK,MAX_PATH);
  gslc_SetBkgndImage(&m_gui,gslc_GetImageFromFile(&m_gui,m_strImgGradBack,GSLC_IMGREF_FMT_BMP16));

  // Top & Bottom gradient bars
  strncpy(m_strImgGradBarTop,strBasePath,MAX_PATH);
  strncat(m_strImgGradBarTop,IMG_GRADBAR_TOP,MAX_PATH);
  pElem = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,0,320,28},
          gslc_GetImageFromFile(&m_gui,m_strImgGradBarTop,GSLC_IMGREF_FMT_BMP16));

  strncpy(m_strImgGradBarBot,strBasePath,MAX_PATH);
  strncat(m_strImgGradBarBot,IMG_GRADBAR_BOT,MAX_PATH);
  pElem = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,240-28,320,28},
          gslc_GetImageFromFile(&m_gui,m_strImgGradBarBot,GSLC_IMGREF_FMT_BMP16));

  // Status bar
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,28,320,20});
  gslc_ElemSetCol(pElem,GSLC_COL_BLACK,(gslc_tsColor){59,58,120},GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pElem,false);

  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){9,28,50,20},
          "RADIO",0,E_FONT_HEAD);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetFrameEn(pElem,false);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);

  // Create Quit button with text label
#if(1)
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,28,50,20},"QUIT",0,E_FONT_BTN,&CbBtnQuit);
    //xxx (gslc_tsRect){250,28,50,20},"",E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetFrameEn(pElem,false);
  gslc_ElemSetGlowEn(pElem,true);
  //xxx gslc_ElemSetTxtCol(pElem,GSLC_COL_BLUE_LT1);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_RED);
  gslc_ElemSetGlowCol(pElem,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_WHITE);
#else
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,28,50,20},"QUIT",0,E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetCol(pElem,(gslc_tsColor){0,0,192},(gslc_tsColor){0,0,128},(gslc_tsColor){0,0,224});
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
#endif

  // Currently playing background
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){9,57,177,85});
  gslc_ElemSetCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){59,58,120},GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pElem,true);

  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){9,80,177,1});
  gslc_ElemSetCol(pElem,GSLC_COL_BLACK,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
  gslc_ElemSetFrameEn(pElem,false);

  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){9,57,177,80-57},
          "Now Playing",0,E_FONT_HEAD);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetFrameEn(pElem,false);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_BLUE_LT3);
  gslc_ElemSetTxtMargin(pElem,5);


  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){9,80,177,85-(80-57)},
          "93.5 FM",0,E_FONT_TITLE);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetFrameEn(pElem,false);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);

  // Presets background
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){190,57,117,115});
  //xxx gslc_ElemSetCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){59,58,120},GSLC_COL_BLACK);
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_DK4,GSLC_COL_BLACK);
  gslc_ElemSetFrameEn(pElem,true);
  gslc_ElemSetFillEn(pElem,false);

  // Control buttons
  int nCtrlStartY = 148;
  int nCtrlStartX = 9;
  int nCtrlW = 56;
  int nCtrlH = 24;
  int nCtrlGapW = 5;
  int nCtrlGapH = 6;
  int nCtrlX = nCtrlStartX;
  int nCtrlY = nCtrlStartY;

  nCtrlX = nCtrlStartX;
  nCtrlY = nCtrlStartY;


  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN1,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"<<",0,E_FONT_BTN,NULL);
  //xxx gslc_ElemSetCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){59,58,120},(gslc_tsColor){0,0,224});
  //xxx gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  //xxx gslc_ElemSetGlowCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){0,0,224},GSLC_COL_YELLOW);

  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  gslc_ElemSetGlowCol(pElem,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_DK1,GSLC_COL_YELLOW);

  nCtrlX += (nCtrlW + nCtrlGapW);
  pElemSrc = pElem;

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN2,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"BAND",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN3,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},">>",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  nCtrlX = nCtrlStartX;
  nCtrlY += (nCtrlH + nCtrlGapH);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN4,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"Search",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN5,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"EQ",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN6,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"Bal",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN7,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"Source",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN8,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"Connect",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlX += (nCtrlW + nCtrlGapW);

  // Preset Buttons
  nCtrlStartX = 190+4;
  nCtrlStartY = 57+4;
  nCtrlW = 117-8;
  nCtrlH = 16;
  nCtrlGapH = 6;
  nCtrlX = nCtrlStartX;
  nCtrlY = nCtrlStartY;

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_P1,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"88.1",0,E_FONT_BTN,NULL);
  gslc_ElemSetFrameEn(pElem,false);
  //xxx gslc_ElemSetCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){59,58,120},(gslc_tsColor){0,0,224});

  //xxx gslc_ElemSetCol(pElem,(gslc_tsColor){120,129,182},(gslc_tsColor){59,58,120},(gslc_tsColor){0,0,224});
  //xxx gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  //xxx gslc_ElemSetGlowCol(pElem,(gslc_tsColor){181,200,245},(gslc_tsColor){0,0,224},GSLC_COL_YELLOW);

  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  gslc_ElemSetGlowCol(pElem,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_DK1,GSLC_COL_YELLOW);


  nCtrlY += (nCtrlH + nCtrlGapH);
  pElemSrc = pElem;

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_P2,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"93.5",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  gslc_ElemSetGlow(pElem,true); //xxx HACK: Show selected
  nCtrlY += (nCtrlH + nCtrlGapH);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_P3,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"97.3",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlY += (nCtrlH + nCtrlGapH);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_P4,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"102.7",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlY += (nCtrlH + nCtrlGapH);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_P5,E_PG_MAIN,
    (gslc_tsRect){nCtrlX,nCtrlY,nCtrlW,nCtrlH},"105.9",0,E_FONT_BTN,NULL);
  gslc_ElemSetStyleFrom(pElemSrc,pElem);
  nCtrlY += (nCtrlH + nCtrlGapH);

  return true;
}


int main( int argc, char* args[] )
{
  bool                bOk = true;

  // -----------------------------------
  // Initialize
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - Normally we would select a number of different fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT_DROID_SANS,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_HEAD,GSLC_FONTREF_FNAME,FONT_DROID_SANS,20);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_FNAME,FONT_DROID_SANS,36);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

  // -----------------------------------
  // Start display
  InitOverlays(dirname(args[0]));

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

