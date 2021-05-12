/**
 * Professor: Min He
 * Class: CECS 461
 * Title: Speed Snake
 * By: Naomi George and Anthony Won
 */

#include "PmodJSTK2.h"
#include "sleep.h"
#include "xil_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "zybo_z7_hdmi/display_ctrl.h"

#ifdef __MICROBLAZE__
#define CPU_CLOCK_FREQ_HZ (XPAR_CPU_CORE_CLOCK_FREQ_HZ)
#else
#define CPU_CLOCK_FREQ_HZ (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ)
#endif

// Frame size (based on 1440x900 resolution, 32 bits per pixel)
#define MAX_FRAME (1440*900)
#define FRAME_STRIDE (1440*4)

PmodJSTK2 joystick;

void JoyStickInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();
void srand(unsigned int seed);
void numberzero(int, int, int, u32 * frame);
void numberone(int, int, int, u32 * frame);
void numbertwo(int, int, int, u32 * frame);
void numberthree(int, int, int, u32 * frame);
void numberfour(int, int, int, u32 * frame);
void numberfive(int, int, int, u32 * frame);
void numbersix(int, int, int, u32 * frame);
void numberseven(int, int, int, u32 * frame);
void numbereight(int, int, int, u32 * frame);
void numbernine(int, int, int, u32 * frame);
void face(int, int, int, int, int, int,int,int,int,int,u32 * frame);

DisplayCtrl dispCtrl; // Display driver struct
u32 frameBuf[DISPLAY_NUM_FRAMES][MAX_FRAME] __attribute__((aligned(0x20))); // Frame buffers for video data
void *pFrames[DISPLAY_NUM_FRAMES]; // Array of pointers to the frame buffers

int * MoveRock(int, int, int, int);

int main(void) {
	JoyStickInitialize();
	JSTK2_Position position;
	JSTK2_DataPacket rawdata;
	// Initialize an array of pointers to the 2 frame buffers
	int i;
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
		pFrames[i] = frameBuf[i];

	// Initialize the display controller
	DisplayInitialize(&dispCtrl, XPAR_AXIVDMA_0_DEVICE_ID, XPAR_VTC_0_DEVICE_ID, XPAR_HDMI_AXI_DYNCLK_0_BASEADDR, pFrames, FRAME_STRIDE);

	// Start with the first frame buffer (of two)
	DisplayChangeFrame(&dispCtrl, 0);

	// Set the display resolution
	DisplaySetMode(&dispCtrl, &VMODE_1440x900);

	// Enable video output
	DisplayStart(&dispCtrl);

	printf("\n\r");
	printf("HDMI output enabled\n\r");
	printf("Current Resolution: %s\n\r", dispCtrl.vMode.label);
	printf("Pixel Clock Frequency: %.3fMHz\n\r", dispCtrl.pxlFreq);
	printf("Starting animation loop...\n\r");

	// Get parameters from display controller struct
	int x, y;
	u32 stride = dispCtrl.stride / 4;
	u32 width = dispCtrl.vMode.width;
	u32 height = dispCtrl.vMode.height;

	u32 *frame;
	int right = 1;
	int down = 0;
	int left = 0;
	int up = 0;
	int xpos = 0;
	int ypos = 0;
	int applex = 800;
	int appley = 300;

	int score_oneslot_x = 1320;
	int score_oneslot_y = 25;

	int score_twoslot_x = 1380;
	int score_twoslot_y = 25;
	int scorecounter = 0;



	int delay = 6000000;

	int trailArrayX[] = {260, 195, 130, 65, 0};
	int trailArrayY[] = {0, 0, 0, 0, 0};

	u32 buff = dispCtrl.curFrame;

	int joyStickPos = 1;

	int left_eyex = (width/2)-200;
	int eyey = 300;
	int right_eyex = (width/2)+200;
	int smilebarx = (width/2)-500;
	int smilebary = 600;
	int smilexleft = (width/2)-500;
	int smilexright = (width/2)+500;
	int smilecorny = 600;

	while (1) {
			// Get joystick x and y coordinate values
		    position = JSTK2_getPosition(&joystick);
		    // Get button states
		    rawdata = JSTK2_getDataPacket(&joystick);
		    // Delay to read the joystick
		    usleep(50000);

		    //An if statement to figure out if the joystick is facing up, down, left, or right
		    if((position.XData)> 128 && (position.YData)>35 && (position.YData)<215){
		    	joyStickPos = 0; // Up
		    }
		    if((position.XData)>= 35 && (position.XData)<202 && (position.YData)<128){
		    	  joyStickPos = 1; // Right
		    }
		    if((position.XData)< 206 && (position.XData)>40 && (position.YData)>128){
		    	  joyStickPos = 3; // Left
		    }
		    if((position.XData)< 128 && (position.YData)>35 && (position.YData)<215){
		    	  joyStickPos = 2; // Down
		    }
		    if((position.XData)==128 && (position.YData)==128){
		    	  joyStickPos = joyStickPos;
		    }
			// Switch the frame we're modifying to be the back buffer (1 to 0, or 0 to 1)
			buff = !buff;
			frame = (u32 *)dispCtrl.framePtr[buff];

			// Clear the entire frame to white (inefficient, but it works)
			memset(frame, 0xFF, MAX_FRAME*4);

			//An if statement to set the variable and check to ensure the snake cannot move into itself
			if ((joyStickPos==0) && (down !=1)){
				up = 1;
				down = 0;
				right = 0;
				left = 0;

			}
			else if ((joyStickPos==1) && (left !=1)){
				up = 0;
				down = 0;
				right = 1;
				left = 0;

			}
			else if ((joyStickPos==2) && (up !=1)){
				up = 0;
				down = 1;
				right = 0;
				left = 0;

			}
			else if ((joyStickPos==3) && (right !=1)){
				up = 0;
				down = 0;
				right = 0;
				left = 1;

			}

			// Adjust the position of the head of the snake, the rest of the body is moved later.
			if (right) {
				xpos=xpos+65;

			}
			if (left) {
				xpos=xpos-65;

			}
			if (down) {
				ypos=ypos+65;

			}
			if (up) {
				ypos=ypos-65;

			}



			//A check to see if the snake collides with a wall
			if (xpos > width-64 || xpos < 0 ||(ypos > height)||ypos < 0){
				// Switch the frame we're modifying to be the back buffer (1 to 0, or 0 to 1)
				buff = !buff;
				frame = (u32 *)dispCtrl.framePtr[buff];

				// Clear the entire frame to white (inefficient, but it works)
				memset(frame, 0xFF, MAX_FRAME*4);

				// Draws the entire frame as Red for the game over screen
				for (x = 0; x < width; x++) {
					for (y = 0; y < height; y++) {
						frame[y*stride + x] = ((0xFF << BIT_DISPLAY_RED)) ;
					}
				}

				//A function that will draw the face for a game over screen
				face(left_eyex, right_eyex, eyey, smilebarx, smilebary,stride,width,smilexleft, smilexright,smilecorny, frame);

				// Flush everything out to DDR from the cache
				Xil_DCacheFlush();

				// Switch active frame to the back buffer (will take place during next vertical blanking period)
				DisplayChangeFrame(&dispCtrl, buff);

				// Wait for the frame to switch before continuing (so after current frame has been drawn)
				DisplayWaitForSync(&dispCtrl);

				// Gets the position of the joystick once more
			    position = JSTK2_getPosition(&joystick);

			    // A delay for how long the losing screen is displayed.
			    for(int c = 1; c <= 32767; c++)
			        for (int d = 1; d <= 3000; d++);

				xpos=0;
				ypos=0;
				up = 0;
				down = 0;
				right = 1;
				left = 0;
				joyStickPos=1;
				//Our snake is consistent of an array, each one entry represents one square in the body
				trailArrayX[0] = 260;
				trailArrayX[1] = 195;
				trailArrayX[2] = 130;
				trailArrayX[3] = 65;
				trailArrayX[4] = 0;
				trailArrayY[0] = 0;
				trailArrayY[1] = 0;
				trailArrayY[2] = 0;
				trailArrayY[3] = 0;
				trailArrayY[4] = 0;
				scorecounter = 0;
				delay = 6000000;

				// Switch the frame we're modifying to be the back buffer (1 to 0, or 0 to 1)
				buff = !buff;
				frame = (u32 *)dispCtrl.framePtr[buff];

				// Clear the entire frame to white (inefficient, but it works)
				memset(frame, 0xFF, MAX_FRAME*4);
				//Draws the snake and the snake body as blue.
				for(int j = 0; j<(sizeof trailArrayX / sizeof *trailArrayX); j++){
					for (x = trailArrayX[j]; x < trailArrayX[j]+64; x++) {
						for (y = trailArrayY[j]; y < trailArrayY[j]+64; y++) {
							frame[y*stride + x] = ((0xFF << BIT_DISPLAY_BLUE)) ;
						}
					}
				}
				// Flush everything out to DDR from the cache
				Xil_DCacheFlush();

				// Switch active frame to the back buffer (will take place during next vertical blanking period)
				DisplayChangeFrame(&dispCtrl, buff);

				// Wait for the frame to switch before continuing (so after current frame has been drawn)
				DisplayWaitForSync(&dispCtrl);
			    position = JSTK2_getPosition(&joystick);
			    // A check that waits for a joystick input before moving the snake.
			    while ((position.XData)==128 && (position.YData)==128){
				    position = JSTK2_getPosition(&joystick);
			    }
			}



			//Moves the rest of the snake body after the head has moved
			int nX = sizeof trailArrayX / sizeof *trailArrayX;
				for(int k=nX-1;k>0;k--)
					{
						trailArrayX[k]=trailArrayX[k-1];
					}
				trailArrayX[0]=xpos;
				xpos=trailArrayX[0];

			//Does the same but in the Y coordinates.
			int nY = sizeof trailArrayY / sizeof *trailArrayY;
				for(int k=nY-1;k>0;k--)
					{
						trailArrayY[k]=trailArrayY[k-1];
					}
						trailArrayY[0]=ypos;
						ypos=trailArrayY[0];

			// The if statement that checks the four sides of the square to see if it has collided with the snake.
			if((((ypos+64)>=appley) && ((ypos+64)<=appley+64) && ((xpos+64)>=applex) && ((xpos+64)<=applex+64)) ||
				(((ypos+64)>=appley) && ((ypos+64)<=appley+64) && ((xpos)>=applex) && ((xpos)<=applex+64)) ||
				(((ypos)>=appley) && ((ypos)<=appley+64) && ((xpos)>=applex) && ((xpos)<=applex+64)) ||
				(((ypos)>=appley) && ((ypos)<=appley+64) && ((xpos+64)>=applex) && ((xpos+64)<=applex+64))) {

				//Creates a new set of random X and Y coordinates for the apple to spawn.
				applex = (rand() % (width-64) + 64) - 64;
				appley = (rand() % (height-64) + 64) - 64;

				scorecounter ++;

				//Decreases the delay by 500000 ticks if possible
				if (delay >=500000){
				delay = delay-500000;
				}
			}

			//Delays the snake
			for (int c = 0; c <= delay; c++);


			//Now begins drawing the rest of the snake body.
			for(int j = 0; j<(sizeof trailArrayX / sizeof *trailArrayX); j++){
				for (x = trailArrayX[j]; x < trailArrayX[j]+64; x++) {
					for (y = trailArrayY[j]; y < trailArrayY[j]+64; y++) {
						frame[y*stride + x] = ((0xFF << BIT_DISPLAY_BLUE)) ;
					}
				}
			}

			//Draws the apple
			for (x = applex; x < (applex)+64; x++) {
				for (y = appley; y < (appley)+64; y++) {
					frame[y*stride + x] = ((0xFF << BIT_DISPLAY_RED)) ;
				}
			}


			//The if statement that increments the first 9 points from 0 through a switch statement and a function.
			if (scorecounter < 10 ){
				switch (scorecounter) {
				 case 1:
					numberone(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 2:
					numbertwo(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 3:
					numberthree(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 4:
					numberfour(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 5:
					numberfive(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 6:
					numbersix(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 7:
					numberseven(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 8:
					numbereight(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 9:
					numbernine(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				default:
					numberzero(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
			}

			}
			else {
				// Once the case statement hits the tens we use two switch statements that display the numbers next to
				// each other

				//Finds the tenths number and the ones number.
				int tenths = floor(scorecounter/10);
				int tenth = tenths % 10;
				int ones = floor(scorecounter);
				int one = ones % 10;

				switch (one) {
	            case 1:
	            	numberone(score_twoslot_x, score_twoslot_y,stride,frame);
	                break;
	            case 2:
	            	numbertwo(score_twoslot_x, score_twoslot_y,stride,frame);
	                break;
	            case 3:
	            	numberthree(score_twoslot_x, score_twoslot_y,stride,frame);
	                break;
	            case 4:
	            	numberfour(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
	            case 5:
	            	numberfive(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
	            case 6:
	            	numbersix(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
	            case 7:
	            	numberseven(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
	            case 8:
	            	numbereight(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
	            case 9:
	            	numbernine(score_twoslot_x, score_twoslot_y,stride,frame);
	            	break;
				case 0:
					numberzero(score_twoslot_x, score_twoslot_y,stride,frame);
					break;
				default:
					numberzero(score_twoslot_x, score_twoslot_y,stride,frame);
					break;
	        }
				switch (tenth) {
				 case 1:
					numberone(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 2:
					numbertwo(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 3:
					numberthree(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 4:
					numberfour(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 5:
					numberfive(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 6:
					numbersix(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 7:
					numberseven(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 8:
					numbereight(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 9:
					numbernine(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				case 0:
					numberzero(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
				default:
					numberzero(score_oneslot_x, score_oneslot_y,stride,frame);
					break;
			}

		}

			// Flush everything out to DDR from the cache
			Xil_DCacheFlush();

			// Switch active frame to the back buffer (will take place during next vertical blanking period)
			DisplayChangeFrame(&dispCtrl, buff);

			// Wait for the frame to switch before continuing (so after current frame has been drawn)
			DisplayWaitForSync(&dispCtrl);
	}
	DemoCleanup();
	return 0;
}


void JoyStickInitialize() {
   EnableCaches();

   // Initialize the joystick device
   JSTK2_begin(
      &joystick,
      XPAR_PMODJSTK2_1_AXI_LITE_SPI_BASEADDR,
      XPAR_PMODJSTK2_1_AXI_LITE_GPIO_BASEADDR
   );

   // Set inversion register to invert only the Y axis
   JSTK2_setInversion(&joystick, 0, 1);
}



void DemoCleanup() {
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}


void numberzero(int scorex, int scorey, int stride, u32 * frame){
	int y, x;

	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+16; x++) { //E
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void numberone(int scorex, int scorey, int stride, u32 * frame){
	int y, x;
	for (x = scorex; x < scorex+16; x++) { //E
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

}

void numbertwo(int scorex, int scorey, int stride, u32 * frame){
	int y, x;
	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+16; x++) { //E
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}


}

void numberthree(int scorex, int scorey, int stride, u32 * frame){
	int y, x;
	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}


}

void numberfour(int scorex, int scorey, int stride, u32 * frame){
	int y, x;


	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void numberfive(int scorex, int scorey, int stride, u32 * frame){
	int y, x;
	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void numbersix(int scorex, int scorey, int stride, u32 * frame){
		int y, x;

		for (x = scorex+32; x < scorex+32+16; x++) { //C
						for (y = scorey+32; y < scorey+64; y++) {
							frame[y*stride + x] = 0;
						}
					}

		for (x = scorex; x < scorex+48; x++) { //D
						for (y = (scorey+48); y < (scorey+64); y++) {
							frame[y*stride + x] = 0;
						}
					}
		for (x = scorex; x < scorex+16; x++) { //E
						for (y = scorey+32; y < scorey+64; y++) {
							frame[y*stride + x] = 0;
						}
					}

		for (x = scorex; x < scorex+16; x++) { //F
						for (y = scorey; y < scorey+32; y++) {
							frame[y*stride + x] = 0;
						}
					}

		for (x = scorex; x < scorex+48; x++) { //G
						for (y = scorey+24; y < scorey+40; y++) {
							frame[y*stride + x] = 0;
						}
					}
	}

void numberseven(int scorex, int scorey, int stride, u32 * frame){
	int y, x;

	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void numbereight(int scorex, int scorey, int stride, u32 * frame){
	int y, x;

	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+16; x++) { //E
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void numbernine(int scorex, int scorey, int stride, u32 * frame){
	int y, x;

	for (x = scorex; x < scorex+48; x++) { //A
					for (y = scorey; y < scorey+16; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //B
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex+32; x < scorex+32+16; x++) { //C
					for (y = scorey+32; y < scorey+64; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //D
					for (y = (scorey+48); y < (scorey+64); y++) {
						frame[y*stride + x] = 0;
					}
				}
	for (x = scorex; x < scorex+16; x++) { //F
					for (y = scorey; y < scorey+32; y++) {
						frame[y*stride + x] = 0;
					}
				}

	for (x = scorex; x < scorex+48; x++) { //G
					for (y = scorey+24; y < scorey+40; y++) {
						frame[y*stride + x] = 0;
					}
				}
}

void face(int left_eyex, int right_eyex, int eyey, int smile_barx, int smile_bary,int stride, int width, int smileleftx, int smilerightx, int smiley, u32 * frame){
    int y, x;
    //left eye
    for (x = left_eyex; x < left_eyex+32; x++) {
                    for (y = eyey; y < eyey+94; y++) {
                        frame[y*stride + x] = 0;
                    }
                }
    //right eye
    for (x = right_eyex; x < right_eyex+32; x++) {
                    for (y = eyey; y < eyey+94; y++) {
                        frame[y*stride + x] = 0;
                    }
                }
    //sad bar face
    for (x = smile_barx; x < (width/2)+500; x++) {
                    for (y = smile_bary; y < smile_bary+60; y++) {
                        frame[y*stride + x] = 0;
                    }
                }
    //sad bar corner left
    for (x = smileleftx; x < smileleftx+60; x++) {
                    for (y = smiley; y < smiley+94+94/3; y++) {
                        frame[y*stride + x] = 0;
                    }
                }
    //sad bar corner right
    for (x = smilerightx; x < smilerightx+60; x++) {
                    for (y = smiley; y < smiley+94+94/3; y++) {
                        frame[y*stride + x] = 0;
                    }
                }

}
