/*

Just some example menus. File...->Quit is the only thing that works.

*/


#include "screen.h"
#include "menu.h"
#include <cbm.h>


unsigned char MainMenu();
unsigned char FileMenu();
unsigned char Menu2();


void main(void) {
  InitScreen("Simple example menu                     ", COLOR_BLACK, COLOR_BLACK, COLOR_LIGHTGREEN);
  SetMenuColors(COLOR_WHITE, COLOR_YELLOW, COLOR_LIGHTGREEN, COLOR_GREEN);
  MainMenu();
  ExitScreen();
}



unsigned char MainMenu() {
  Menu *menu;
  unsigned char state = 0;

  enum {
    MM_FILE = 1,
    MM_MENU2,
    MM_HELLO,
    MM_TOGGLE
  };

  menu = NewMenu();
  SetMenuTitle(menu, "Main Menu");
  AddMenuItem(menu, MM_FILE, ISTATE_NORMAL, "File Menu...");
  AddMenuItem(menu, MM_MENU2, ISTATE_NORMAL, "Go to menu 2...");
  AddMenuItem(menu, MM_HELLO, ISTATE_INACTIVE, "Hello");
  AddMenuItem(menu, MM_TOGGLE, ISTATE_NORMAL, "Toggle previous");
  ShowMenu(menu);

  while (1) {
    switch (SelectMenu(menu)) {
    case MM_FILE:
      HideMenu(menu);
      if (FileMenu()) {
	DestroyMenu(menu);
	return(0);
      } else {
	ShowMenu(menu);
      }
      break;
    case MM_MENU2:
      HideMenu(menu);
      Menu2();
      ShowMenu(menu);
      break;
    case MM_HELLO:
      DisplayMessage("Hello, world!");
      break;
    case MM_TOGGLE:
      SetItemState(menu, MM_HELLO, (state = 1 - state) ? ISTATE_NORMAL : ISTATE_INACTIVE);
      DrawMenu(menu);
      break;
    }
  }
 
}



unsigned char FileMenu() {
  Menu *menu;

  enum {
    FM_BACK = 1,
    FM_QUIT
  };

  menu = NewMenu();
  SetMenuTitle(menu, "File Menu");
  AddMenuItem(menu, FM_BACK, ISTATE_NORMAL, "Back...");
  AddMenuItem(menu, FM_QUIT, ISTATE_NORMAL, "Quit");
  ShowMenu(menu);

  while (1) {
    switch (SelectMenu(menu)) {
    case MENU_CANCEL:
    case FM_BACK:
      HideMenu(menu);
      DestroyMenu(menu);
      return(0);
      break;
    case FM_QUIT:
      if (AskAreYouSure(menu)) {
	HideMenu(menu);
	DestroyMenu(menu);
	return(1);
      } else {
	DrawMenu(menu);
      }
      break;
    }
  }
}



unsigned char Menu2() {
  Menu *menu;

  enum {
    M2_BACK = 1,
    M2_TEST1,
    M2_TEST2
  };

  menu = NewMenu();
  SetMenuTitle(menu, "Section Menu");
  AddMenuItem(menu, M2_BACK,ISTATE_NORMAL, "Back...");
  AddMenuItem(menu, M2_TEST1,ISTATE_INACTIVE, "Test 1");
  AddMenuItem(menu, M2_TEST2,ISTATE_INACTIVE, "Test 2");
  ShowMenu(menu);

  while (1) {
    switch (SelectMenu(menu)) {
    case MENU_CANCEL:
    case M2_BACK:
      HideMenu(menu);
      DestroyMenu(menu);
      return(0);
      break;
    case M2_TEST1:
      break;
    case M2_TEST2:
      break;
    }
  }
}
