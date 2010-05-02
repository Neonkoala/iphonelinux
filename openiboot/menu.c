#ifndef SMALL
#ifndef NO_STBIMAGE

#include "openiboot.h"
#include "lcd.h"
#include "util.h"
#include "framebuffer.h"
#include "buttons.h"
#include "timer.h"
#include "images/ConsolePNG.h"
#include "images/iPhoneOSPNG.h"
#include "images/iDroidPNG.h"
#include "images/ConsoleSelectedPNG.h"
#include "images/iPhoneOSSelectedPNG.h"
#include "images/iDroidSelectedPNG.h"
#include "images/HeaderPNG.h"
#include "images.h"
#include "actions.h"
#include "stb_image.h"
#include "pmu.h"
#include "nand.h"
#include "radio.h"
#include "hfs/fs.h"
#include "ftl.h"
#include "scripting.h"

int globalFtlHasBeenRestored = 0; /* global variable to tell wether a ftl_restore has been done*/

static uint32_t FBWidth;
static uint32_t FBHeight;

static int ToggleUp   = 1;
static int ToggleDown = 2;

static uint32_t* imgiPhoneOS;
static int imgiPhoneOSWidth;
static int imgiPhoneOSHeight;
static int imgiPhoneOSX;
static int imgiPhoneOSY;

static uint32_t* imgConsole;
static int imgConsoleWidth;
static int imgConsoleHeight;
static int imgConsoleX;
static int imgConsoleY;

static uint32_t* imgiDroid;
static int imgiDroidWidth;
static int imgiDroidHeight;
static int imgiDroidX;
static int imgiDroidY;

static uint32_t* imgiPhoneOSSelected;
static uint32_t* imgConsoleSelected;
static uint32_t* imgiDroidSelected;

static uint32_t* imgHeader;
static int imgHeaderWidth;
static int imgHeaderHeight;
static int imgHeaderX;
static int imgHeaderY;

typedef enum MenuSelection {
	MenuSelectioniPhoneOS,
	MenuSelectioniDroid,
	MenuSelectionConsole
} MenuSelection;

static MenuSelection Selection;

static void drawSelectionBox() {
	if(Selection == MenuSelectioniPhoneOS) {
		framebuffer_draw_image(imgiPhoneOSSelected, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgiDroid, imgiDroidX, imgiDroidY, imgiDroidWidth, imgiDroidHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
	}

	if(Selection == MenuSelectioniDroid) {
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgiDroidSelected, imgiDroidX, imgiDroidY, imgiDroidWidth, imgiDroidHeight);
		framebuffer_draw_image(imgConsole, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
	}

	if(Selection == MenuSelectionConsole) {
		framebuffer_draw_image(imgiPhoneOS, imgiPhoneOSX, imgiPhoneOSY, imgiPhoneOSWidth, imgiPhoneOSHeight);
		framebuffer_draw_image(imgiDroid, imgiDroidX, imgiDroidY, imgiDroidWidth, imgiDroidHeight);
		framebuffer_draw_image(imgConsoleSelected, imgConsoleX, imgConsoleY, imgConsoleWidth, imgConsoleHeight);
	}
}

static void toggle(int direction) {
	if(direction == ToggleDown) {
		if(Selection == MenuSelectioniPhoneOS) {
			Selection = MenuSelectioniDroid;
		} else if(Selection == MenuSelectioniDroid) {
			Selection = MenuSelectionConsole;
		} else if(Selection == MenuSelectionConsole) {
			Selection = MenuSelectioniPhoneOS;
		}
	} 
	if(direction == ToggleUp) {
		if(Selection == MenuSelectioniPhoneOS) {
			Selection = MenuSelectionConsole;
		} else if(Selection == MenuSelectioniDroid) {
			Selection = MenuSelectioniPhoneOS;
		} else if(Selection == MenuSelectionConsole) {
			Selection = MenuSelectioniDroid;
		}
	}
	drawSelectionBox();
}

int menu_setup(int timeout, int defaultOS) {
	FBWidth = currentWindow->framebuffer.width;
	FBHeight = currentWindow->framebuffer.height;	

	imgiPhoneOS = framebuffer_load_image(dataiPhoneOSPNG, dataiPhoneOSPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgiPhoneOSSelected = framebuffer_load_image(dataiPhoneOSSelectedPNG, dataiPhoneOSSelectedPNG_size, &imgiPhoneOSWidth, &imgiPhoneOSHeight, TRUE);
	imgiDroid = framebuffer_load_image(dataiDroidPNG, dataiDroidPNG_size, &imgiDroidWidth, &imgiDroidHeight, TRUE);
	imgiDroidSelected = framebuffer_load_image(dataiDroidSelectedPNG, dataiDroidSelectedPNG_size, &imgiDroidWidth, &imgiDroidHeight, TRUE);
	imgConsole = framebuffer_load_image(dataConsolePNG, dataConsolePNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgConsoleSelected = framebuffer_load_image(dataConsoleSelectedPNG, dataConsoleSelectedPNG_size, &imgConsoleWidth, &imgConsoleHeight, TRUE);
	imgHeader = framebuffer_load_image(dataHeaderPNG, dataHeaderPNG_size, &imgHeaderWidth, &imgHeaderHeight, TRUE);

	bufferPrintf("menu: images loaded\r\n");

	imgiPhoneOSX = (FBWidth - imgiPhoneOSWidth) / 2;
	imgiPhoneOSY = 57;

	imgiDroidX = (FBWidth - imgiDroidWidth) / 2;
	imgiDroidY = 192;

	imgConsoleX = (FBWidth - imgConsoleWidth) / 2;
	imgConsoleY = 327;

	imgHeaderX = (FBWidth - imgHeaderWidth) / 2;
	imgHeaderY = 10;

	framebuffer_draw_image(imgHeader, imgHeaderX, imgHeaderY, imgHeaderWidth, imgHeaderHeight);

	framebuffer_setloc(0, 47);
	framebuffer_setcolors(COLOR_WHITE, 0x222222);
	framebuffer_print_force(OPENIBOOT_VERSION_STR);
	framebuffer_setcolors(COLOR_WHITE, COLOR_BLACK);
	framebuffer_setloc(0, 0);


	switch(defaultOS){
		case 0:
			Selection = MenuSelectioniPhoneOS;
			break;
		case 1:
			Selection = MenuSelectionConsole;
			break;
		case 2:
			Selection = MenuSelectionConsole;
			break;
		default:
			Selection = MenuSelectioniPhoneOS;
	}
	drawSelectionBox();

	pmu_set_iboot_stage(0);

	uint64_t startTime = timer_get_system_microtime();
	uint64_t holdStartTime = timer_get_system_microtime();
	int timeoutLeft = timeout / 1000;
	int timeoutLeftb = timeout / 1000;

	while(TRUE) {
		if(timeout > 0){
			if(has_elapsed(startTime, (uint64_t)(timeout - (timeoutLeft * 1000)) * 1000)){
				char timeoutstr[5] = "";
				timeoutLeft -= 1;
				if(timeoutLeft != timeoutLeftb){
					sprintf(timeoutstr, "%d ", timeoutLeft + 1);
					framebuffer_setloc(49,47);
					framebuffer_print_force(timeoutstr);
					framebuffer_setloc(0,0);
					timeoutLeftb -= 1;
				}
			}
		}   // timeout print code here ^^
		else if(timeout == -1){
			framebuffer_setloc(50,47);
			framebuffer_print_force("  ");
			framebuffer_setloc(0,0);
		}
		if(buttons_is_pushed(BUTTONS_HOLD)) {
			if(has_elapsed(holdStartTime, (uint64_t)10 * 1000)) {
				toggle(ToggleDown);
			} 
			if(has_elapsed(holdStartTime, (uint64_t)2000 * 1000)) {
				pmu_poweroff();
			}
			timeout = -1;
			defaultOS = -1;
			udelay(200000);
		} else {
			holdStartTime = timer_get_system_microtime();
			udelay(200000);
		}
#ifndef CONFIG_IPOD
		if(!buttons_is_pushed(BUTTONS_VOLUP)) {
			toggle(ToggleUp);
			startTime = timer_get_system_microtime();
			timeout = -1;
			defaultOS = -1;
			udelay(200000);
		}
		if(!buttons_is_pushed(BUTTONS_VOLDOWN)) {
			toggle(ToggleDown);
			startTime = timer_get_system_microtime();
			timeout = -1;
			defaultOS = -1;
			udelay(200000);
		}
#endif
		if(buttons_is_pushed(BUTTONS_HOME)) {
			timeout = -1;
			defaultOS = -1;
			break;
		}
		if(timeout > 0 && has_elapsed(startTime, (uint64_t)timeout * 1000)) {
			bufferPrintf("menu: timed out, selecting current item\r\n");
			break;
		}
		udelay(10000);
	}

	if(Selection == MenuSelectioniPhoneOS) {
		Image* image = images_get(fourcc("ibox"));
		if(image == NULL)
			image = images_get(fourcc("ibot"));
		void* imageData;
		images_read(image, &imageData);
		chainload((uint32_t)imageData);
	}

	if(Selection == MenuSelectionConsole) {
		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
	}

	if(Selection == MenuSelectioniDroid) {
#ifndef NO_HFS
		startTime = timer_get_system_microtime();
		while(TRUE) {
			if(!buttons_is_pushed(BUTTONS_HOME))
				break;

			if(has_elapsed(startTime, (uint64_t)2000 * 1000) || defaultOS == 1) {
				framebuffer_setdisplaytext(TRUE);
				framebuffer_clear();
				radio_setup();
				nand_setup();
				fs_setup();
				if(globalFtlHasBeenRestored) /* if ftl has been restored, sync it, so kernel doesn't have to do a ftl_restore again */
				{
					if(ftl_sync())
					{
						bufferPrintf("ftl synced successfully");
					}
					else
					{
						bufferPrintf("error syncing ftl");
					}
				}

				pmu_set_iboot_stage(0);
				startScripting("linux"); //start script mode if there is a script file
				boot_linux_from_files();
			}
		}

		pmu_set_iboot_stage(0);
		startScripting("linux"); //start script mode if there is a script file
		boot_linux_from_files();
#endif

		framebuffer_setdisplaytext(TRUE);
		framebuffer_clear();
	}

	return 0;
}

#endif
#endif
