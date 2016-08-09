#define OLED_FPS 30

/* Код  */
enum shifting {
  wait_shifting_left,
  shifting_left,
  wait_shifting_right,
  shifting_right
};

short currentWidthOffset = 0;
const short shiftDelay = 40;
short currentShiftDelay = shiftDelay;
shifting overlayMode = wait_shifting_left;

void ipOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  String text;
  if(currentSSID == "" && currentIp =="")
  {
      text = String("Connecting..");
  }
  else
  {   
      text = String(currentSSID + ": " + currentIp);
  }
  short width = display->getStringWidth(text);
  if(width <= OLED_WIDTH)
  {
    display->drawString(0, 0, text);
    currentWidthOffset = 0;
    currentShiftDelay = shiftDelay;
  }
  else
  {
    short maxShift = OLED_WIDTH - width;
    short x = getXOffset(maxShift);
    display->drawString(x, 0, text);
  }
}

short getXOffset(short maxShift)
{
    if(overlayMode == wait_shifting_left || overlayMode == wait_shifting_right)
    {
      currentShiftDelay--;
      if(currentShiftDelay <= 0)
      {
        if(overlayMode == wait_shifting_left)
          overlayMode = shifting_left;
        else if(overlayMode == wait_shifting_right)
          overlayMode = shifting_right;
        currentShiftDelay = shiftDelay;
      }
    }
    else if(overlayMode == shifting_left)
    {
      if(currentWidthOffset > maxShift)
      {
        currentWidthOffset--;
      }
      else
        overlayMode = wait_shifting_right;
    }
    else if(overlayMode == shifting_right)
    {
      if(currentWidthOffset <= 0)
      {
        currentWidthOffset++;
      }
      else
        overlayMode = wait_shifting_left;
    }
    return currentWidthOffset;
}

byte activePoint = 0;
const short activeWait = 20;
short currentActiveWait = activeWait;
void wiFiLogoFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  ui.disableIndicator();
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y
  display->drawXbm(x + 34, y + 16, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);

  for(int i = 0; i < 4; i++)
  {
    display->drawXbm((x + 34 + 16*i), y + 54, 8, 8, i == activePoint ? activeSymbol : inactiveSymbol);
  }
  if(currentActiveWait > 0)
  {
    currentActiveWait--;
  }
  else
  {
    currentActiveWait = activeWait;
    activePoint++;
    if(activePoint > 3)
      activePoint = 0;
  }
}

void wiFiClientFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setFont(ArialMT_Plain_16);

  display->drawString(0, 16, "PPM:");
  
  display->drawString(50, 26, String(lastAIQ));
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { wiFiLogoFrame, wiFiClientFrame };
// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { ipOverlay };
int overlaysCount = 1;

void init_oled()
{
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(OLED_FPS);
  
  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);
  
  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  //disable auto-transition
  ui.disableAutoTransition();
  
  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();  
}

