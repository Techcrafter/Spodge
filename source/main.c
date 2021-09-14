// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SPRITES   26
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = 1;

int waitingCount;
int score;
int dead;

int playerX = 200;
int playerY = 120;
int playerW = 23;
int playerH = 23;
int colorCount;
int playerColor = 2;

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
	//size_t numImages = C2D_SpriteSheetCount(spriteSheet);
	srand(time(NULL));

	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];
		
		if(i <= 1)
		{
		    C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		    C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		    C2D_SpriteSetPos(&sprite->spr, 200, 120);
		}
		else if(i > 1 && i < 6)
		{
		    C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		    C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		    C2D_SpriteSetPos(&sprite->spr, playerX, playerY);
		}
		else
		{
		    // Random image, position, rotation and speed
		    C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		    C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		    C2D_SpriteSetPos(&sprite->spr, 0, 0);
		    C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand()/(float)RAND_MAX));
		    sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		    sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
		}
	}
}

//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------
	for (size_t i = 0; i < numSprites; i++)
	{
		Sprite* sprite = &sprites[i];
		
		if(i >= 6)
		{
		    C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);
		    C2D_SpriteRotateDegrees(&sprite->spr, 1.0f);

		    // Check for collision with the screen boundaries
		    if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			    (sprite->spr.params.pos.x > (SCREEN_WIDTH-(sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			    sprite->dx = -sprite->dx;

		    if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			    (sprite->spr.params.pos.y > (SCREEN_HEIGHT-(sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			    sprite->dy = -sprite->dy;
		    
		    if(sprite->spr.params.pos.x + sprite->spr.params.pos.w / 4.0f > playerX - playerW && sprite->spr.params.pos.x - sprite->spr.params.pos.w / 4.0f < playerX + playerW && sprite->spr.params.pos.y + sprite->spr.params.pos.h / 4.0f > playerY - playerH && sprite->spr.params.pos.y - sprite->spr.params.pos.h / 4.0f < playerY + playerH)
		    {
		        dead = 1;
		    }
		}
		else if(i > 1)
		{
		    C2D_SpriteRotateDegrees(&sprite->spr, -2.0f);
		    if(i == playerColor)
		    {
		        C2D_SpriteSetPos(&sprite->spr, playerX, playerY);
		    }
		    else
		    {
		        C2D_SpriteSetPos(&sprite->spr, 500, 340);
		    }
		}
		else if(i == 1)
		{
		    C2D_SpriteSetPos(&sprite->spr, 200, 120);
		}
		else if(i == 0 && numSprites > 1)
		{
		    C2D_SpriteSetPos(&sprite->spr, 500, 340);
		}
	}
}

void wait(int cycles)
{
    int temp = numSprites;
    numSprites = 1;
    while(cycles > 0)
    {
        cycles--;
        waitingCount = 0;
	    while(waitingCount < 99999)
	    {
	        waitingCount++;
	        moveSprites();
	    }
	}
	numSprites = temp;
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	initSprites();

	printf("\x1b[7;1HSpodge by Techcrafter");
	printf("\x1b[9;1HAvoid the flying objects!");
	printf("\x1b[10;1HPress [Start] to exit.");
	
	printf("\x1b[14;1HVersion 1.1");
	
	printf("\x1b[24;1HFollow on GitHub!");

	// Main loop
	while (aptMainLoop())
	{
	    run:
	    
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		u32 kHeld = hidKeysHeld();
		if ((kHeld & KEY_UP) && playerY > 23)
			playerY -= numSprites - 6;
		if ((kHeld & KEY_DOWN) && playerY < 217)
			playerY += numSprites - 6;
	    if ((kHeld & KEY_LEFT) && playerX > 23)
			playerX -= numSprites - 6;
		if ((kHeld & KEY_RIGHT) && playerX < 377)
			playerX += numSprites - 6;
			
		if(playerY < 23)
		{
		    playerY = 23;
		}
		else if(playerY > 217)
		{
		    playerY = 217;
		}
		
		if(playerX < 23)
		{
		    playerX = 23;
		}
		else if(playerX > 377)
		{
		    playerX = 377;
		}

		moveSprites();
		
		if(dead == 1)
		{
		    goto dead;
		}
		
		waitingCount++;
		
		if(numSprites > 1)
		{
		    colorCount++;
		    if(colorCount == 25)
		    {
		        if(playerColor < 5)
	            {
	                playerColor++;
	            }
	            else
	            {
	                playerColor = 2;
	            }
	            score++;
	            printf("\x1b[14;1HScore: %zu\x1b[K", score);
	            colorCount = 0;
		    }
		}
		
		if(waitingCount == 200 && numSprites == 1)
		{
		    printf("\x1b[14;1H           ");
		    numSprites = 7;
		    waitingCount = 0;
		}
		
		if(waitingCount == 500)
		{
		    if(numSprites < MAX_SPRITES)
		    {
		        numSprites++;
		    }
		    waitingCount = 0;
		}
		
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		for (size_t i = 0; i < numSprites; i ++)
			C2D_DrawSprite(&sprites[i].spr);
		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
	
	dead:
	printf("\x1b[19;1HGAME OVER!");
    
    // Render the scene
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
	C2D_SceneBegin(top);
	for (size_t i = 0; i < numSprites; i ++)
		C2D_DrawSprite(&sprites[i].spr);
	C3D_FrameEnd(0);
	
	wait(100);
	
	dead = 0;
	waitingCount = 0;
	score = 0;
	playerX = 200;
    playerY = 120;
    numSprites = 7;
    colorCount = 0;
    playerColor = 2;
    
    for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];
		
		if(i >= 6)
		{
		    // Random image, position, rotation and speed
		    C2D_SpriteSetPos(&sprite->spr, 0, 0);
		    C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand()/(float)RAND_MAX));
		    sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		    sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
		}
	}
    
    printf("\x1b[19;1H          ");
    
	goto run;
}
