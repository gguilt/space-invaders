/* Space Invaders | Started: 06/1/2015 05:58 - Finished: 31/1/2015 */

/* Game development log:
06/1/2015: Project created, SDL2 libraries added, general comments added, state machine created,
    code was formatted, FPS w/ cap added, window created,  renderer created,
    error & warning messages added, loadmedia/init/cleanup created, colors added.

08/1/2015: LTexture w/ text texture created, screen background color tested, cleanup upgraded,
    example text texture created, render bug found & fixed.

10/1/2015: state machine 'scoreboard' created, Started to work on STATE_INTRO:
    (created test logo, button texts, game title, background color, click-able buttons).

11/1/2015: Debug & control day. -Founded problems: FPS not stable. [VSYNC activated, so problem solved.]
13/1/2015: Moved some part of code to Intro::Intro and Intro::~Intro, FPS_OUTPUT added, window icon added.

14/1/2015: Build options upgraded, Game state 'handleEvents' upgraded, development log updated,
    Player created, scoreboard test text-textures created, FPSCAP/VSYNC switch added.

16/1/2015: 4th button added (back to menu), Scoreboard button problem fixed, gPlayer turned mPlayer,
    score system created (text-texture&variable only), health system created (sprite&variable only),
    invader class & test invaders created.

18/1/2015: Overworld invaders & their movement system created. [I slept all day, so i could not code anything]

20/1/2015: Workplan created, invaders y-axis movement created, overworld states created, pause system added,
    health system upgraded, ascii art added to code.

21/1/2015: Savescore class created, overworld states updated, overworld handleEvent updated, shoot system created.
23/1/2015: Collision for shoots created, invader death added, score system created, invaders movement updated.
24/1/2015: Invaders movement completed, brick system & brick-block created & completed, intro invader deleted,
    intro score-list created & completed (time based loading integrated).

26/1/2015: Invaders shoots created w/ little updates on the Shoot class & it's functions.
27/1/2015: Invaders shoots system updated: brick collision upgraded, random shooting added, shooting with movement.
    Health system updated, UFO system created & completed, intro updated (UFO added), some little bugs fixed.

28/1/2015: 105mm gun created & completed, started work on scoreboard & savescore (file i/o prototype created).
29/1/2015: Finishing the game - day 1: File i/o prototype deleted, started to tests for score system.
30/1/2015: Finishing the game - day 2: canceled the advanced scoreboard & started to work on simple scoreboard.
31/1/2015: Finishing the game - day 3: Simple scoreboard completed w/ score list algorithm, debug & code control completed.
-- End of the log. --
*/

/// Headers & preprocessors:
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define ERROR_MESSAGES // Wanna disable? Add // to beginning of the line.
#define WARNING_MESSAGES // Same as error messages.
//#define FPS_OUTPUT // Same as error messages.
#define SDL_RENDERER_VSYNC // FPSCAP or VSYNC switch

/// Constants:
// Screen attributes
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

#ifdef SDL_RENDERER_FPSCAP
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
#endif // SDL_RENDERER_FPSCAP

// Randomizer
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(0, 21);

// File read & write
const int TOTAL_DATA = 20;

// Game states
enum gameModes
{
    STATE_NULL,
    STATE_INTRO,
    STATE_OVERWORLD,
    STATE_SAVESCORE,
    STATE_SCOREBOARD,
    STATE_EXIT,
};

// Mouse button
const int BUTTON_WIDTH = 246;
const int BUTTON_HEIGHT = 25;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_INACTIVE = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_ACTIVE = 2,
    BUTTON_SPRITE_TOTAL = 3
};

// Player
const int HEART_WIDTH = 189;
const int HEART_HEIGHT = 208 / 8;

enum LHealthSprite
{
    HEALTH_SPRITE_MORE = 0,
    HEALTH_SPRITE_SIX = 1,
    HEALTH_SPRITE_FIVE = 2,
    HEALTH_SPRITE_FOUR = 3,
    HEALTH_SPRITE_THREE = 4,
    HEALTH_SPRITE_TWO = 5,
    HEALTH_SPRITE_ONE = 6,
    HEALTH_SPRITE_NONE = 7,
    HEALTH_SPRITE_TOTAL = 8
};

// Invader
const int INVADER_WIDTH = 64;
const int INVADER_HEIGHT = 64;

int invadersWaitTime = 30;
int invadersMoveTime = 0;
bool invadersDownTime = false;
bool invadersDirection = true;

enum LInvaderSprite
{
    INVADER_SPRITE_MOVE = 0,
    INVADER_SPRITE_WAIT = 1,
    INVADER_SPRITE_TOTAL = 2

};

// UFO
int ufoCurrentTime = 0;
int ufoMaxTime = 2000;

enum UFOSprite
{
    UFO_SPRITE_RIGHT = 0,
    UFO_SPRITE_LEFT = 1,
    UFO_SPRITE_TOTAL
};

// Shoot
const int BULLET_WIDTH = 6;
const int BULLET_HEIGHT = 25;

enum LBulletSprite
{
    BULLET_SPRITE_ONE = 0,
    BULLET_SPRITE_TWO = 1,
    BULLET_SPRITE_TOTAL = 2
};

// Brick
const int BRICK_WIDTH = 50;
const int BRICK_HEIGHT = 30;

enum LBrickSprite
{
    BRICK_SPRITE_NODAMAGE = 0,
    BRICK_SPRITE_DAMAGELOW = 1,
    BRICK_SPRITE_DAMAGEMED = 2,
    BRICK_SPRITE_DAMAGEHIGH = 3,
    BRICK_SPRITE_TOTAL = 4
};

// Overworld states
enum overworldStates
{
    OVERWORLD_STATE_NULL,
    OVERWORLD_STATE_ACTIVE,
    OVERWORLD_STATE_PAUSED,
    OVERWORLD_STATE_GAMEOVER,
    OVERWORLD_STATE_SCORELIMIT
};

/// Classes:
// Texture
class LTexture
{
    public:
        LTexture();
        ~LTexture();
        bool loadFromFile(std::string path);
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor); // Using for text textures
        void free();
        void setColor(Uint8 red, Uint8 green, Uint8 blue);
        void setBlendMode(SDL_BlendMode blending);
        void setAlpha(Uint8 alpha);
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip =  SDL_FLIP_NONE);

        int getWidth();
        int getHeight();

    private:
        SDL_Texture* mTexture;
        int mWidth;
        int mHeight;
};

// Timer
class LTimer
{
    public:
        LTimer(); // Initializes variables

        // Clock actions
        void start();
        void stop();
        void pause();
        void unpause();

        Uint32 getTicks(); // Gets the timer's time

        // Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        Uint32 mStartTicks; // Clock time when the timer starts
        Uint32 mPausedTicks; // Ticks stored when the timer was paused

        // The timer status
        bool mPaused;
        bool mStarted;
};

// Mouse button
class LButton
{
    public:
        LButton();
        void setPosition(int x, int y);
        void handleEvents(int buttonID, SDL_Event* e);
        void render();

    private:
        SDL_Point mPosition;
        LButtonSprite mCurrentSprite;
};

// Shoot
class Shoot
{
    public:
        Shoot();
        ~Shoot();
        void createFire(int x, int y, int type, bool fromPlayer);
        void resetFire();
        void move(int speed);
        void render();
        SDL_Rect mCollider;

    private:
        SDL_Point mPosition;
        bool mFromPlayer = false;

        LBulletSprite mCurrentSprite;
        LTexture mBulletSprite;
        SDL_Rect mBulletSpriteClips[BULLET_SPRITE_TOTAL];
};

// Brick
class Brick
{
    public:
        Brick();
        ~Brick();
        void setBrick(int x, int y);
        void updateBrick(SDL_Rect& rect);
        bool brickCheckCollision(SDL_Rect& rect);
        void render();

    private:
        SDL_Point mPosition;
        SDL_Rect mCollider;
        bool heded = false;

        LBrickSprite mCurrentSprite;
        LTexture mBrickSprite;
        SDL_Rect mBrickSpriteClips[BRICK_SPRITE_TOTAL];
};

// Player
class Player
{
    public:
        Player();
        ~Player();

        void handleEvents(SDL_Event* e);
        void move();
        void render();
        SDL_Rect mCollider;

        static const int PLAYER_WIDTH = 92;
        static const int PLAYER_HEIGHT = 37;
        static const int PLAYER_VEL = 20;
        int currentHealth;
        Shoot mShoot;
        Shoot mBazooka;

    private:
        int mPosX, mPosY;
        int mVelX, mVelY;
        bool activeShoot = false;

        LTexture mHeartSprite;
        LHealthSprite mCurrentSprite;
        SDL_Rect mHealthSpriteClips[HEALTH_SPRITE_TOTAL];
};

// Invader
class Invader
{
    public:
        Invader();
        ~Invader();

        void setPosition(int x, int y, int type);
        void direction(SDL_Rect& rect_1, SDL_Rect& rect_2);
        void move(int speed);
        void twingleTwingleYouLittleMotherFucker();
        void render();
        SDL_Rect mCollider;
        bool heded = false;

    private:
        SDL_Point mPosition;
        int mType;

        LInvaderSprite mCurrentSprite;
        LTexture mInvaderSprite;
        SDL_Rect mInvaderSpriteClips[INVADER_SPRITE_TOTAL];
};

// UFO - HEIL HITLER!
class UFO
{
    public:
        UFO();
        ~UFO();

        void setUFO(int x, int y, bool direction);
        void stopUFO();
        void move(int speed);
        void render();

        const int UFO_WIDTH = 60;
        const int UFO_HEIGHT = 43;
        SDL_Rect mCollider;
        bool mActivity = false;

    private:
        bool mDirection = false;

        SDL_Point mPosition;
        UFOSprite mCurrentSprite;
        LTexture mUFOTexture;
        SDL_Rect mUFOSpriteClips[UFO_SPRITE_TOTAL];
};

// Game state
class GameState
{
    public:
        virtual void handleEvents(SDL_Event* e) = 0;
        virtual void logic() = 0;
        virtual void render() = 0;
        virtual ~GameState(){};
};

class Intro : public GameState
{
    public:
        Intro();
        ~Intro();

        void handleEvents(SDL_Event* e);
        void logic();
        void render();

    private:
        // Text textures
        LTexture mSpaceText;
        LTexture mBeginText;
        LTexture mScoreText;
        LTexture mExitText;

        // Score list
        LTexture mScoreListText[4];
        Invader mInvader[3];
        UFO mUFO;
        int mScoreListTimer = 0;
};

class Overworld : public GameState
{
    public:
        Overworld(int prevState);
        ~Overworld();

        void handleEvents(SDL_Event* e);
        void logic();
        void render();

    private:
        int mCurrentState = OVERWORLD_STATE_ACTIVE; // Current overworld state

        // Game objects
        Player mPlayer; // Player
        Invader mInvader[66]; // Game invaders
        Brick mBrick[40];
        UFO heilUFO;

        // Textures & text textures
        LTexture mStateTexture; // Texture :: State background
        LTexture mStateHText; // Text texture :: Header of state
        LTexture mStateLText; // Text texture :: Info about state
        LTexture mScoreText; // Text texture :: Player score

        // Invader shoots
        int activeInvaderIDs[11];
        bool isThisShootActive[11];
        Shoot invaderShoot[11];
};

class Savescore : public GameState
{
    public:
        Savescore();
        ~Savescore();

        void handleEvents(SDL_Event* e);
        void logic();
        void render();

    private:
        bool mRenderTime = false;
        std::string inputText = "";
        Sint32 mData[TOTAL_DATA];

        LTexture mInfoText;
        LTexture mSInfoText;
        LTexture mInputText;
        LTexture mScoreText;
};

class Scoreboard : public GameState
{
    public:
        Scoreboard();
        ~Scoreboard();

        void handleEvents(SDL_Event* e);
        void logic();
        void render();

    private:
        Sint32 mData[TOTAL_DATA];

        int scoreValue = 0;
        LTexture mBackText; // Text texture
        LTexture mScoreList[TOTAL_DATA];
};

/// Functions:
void set_next_state(int newState);
void change_state();

bool checkCollision(SDL_Rect& rect_1, SDL_Rect& rect_2)
{
    if(rect_1.y + rect_1.h <= rect_2.y)
    {
        return false;
    }
    else if(rect_1.y >= rect_2.y + rect_2.h)
    {
        return false;
    }
    else if(rect_1.x + rect_1.w <= rect_2.x)
    {
        return false;
    }
    else if(rect_1.x >= rect_2.x + rect_2.w)
    {
        return false;
    }
    return true;
}

/// Globals:
// General
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* gIcon = NULL;

bool gLaserSafety = false; // Player laser cannon safety
bool gGameOverTime = false;

bool gBazookaSafety = false;
int gBazooka = 1;
int gScore = 0; // Player score
int gDeadInvaders = 0;
int gInvadersYaxis = 0;

// Event handler
SDL_Event gEvent;

// Fonts
TTF_Font* gFont = NULL;
TTF_Font* gFont_fipps = NULL;
TTF_Font* gFont_mono = NULL;

// Textures
LTexture gButtonSprite; // Mouse button
LTexture gPlayerTexture;

// Mouse button
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LButton gButtons[TOTAL_BUTTONS];

// Colors
SDL_Color gColor_blue = {15, 77, 146};
SDL_Color gColor_mono = {54, 69, 79};
SDL_Color gColor_onyx = {53, 56, 57};
SDL_Color gColor_plat = {229, 228, 226};
SDL_Color gColor_red  = {255, 50, 50};

// Game state
int stateID = STATE_NULL;
int nextState = STATE_NULL;
GameState *currentState = NULL; // Game state object

/// Class definitions:
// Game state
// - Intro
Intro::Intro()
{
    // Text textures
    gFont = gFont_mono;
    if(!mSpaceText.loadFromRenderedText("- SPACE INVADERS -", gColor_onyx))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mSpaceText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    gFont = gFont_fipps;
    if(!mBeginText.loadFromRenderedText("Begin the game", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mBeginText' texture! " << std::endl;
        #endif // ERROR_MESSAGES
    }

    if(!mScoreText.loadFromRenderedText("See high score list", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    if(!mExitText.loadFromRenderedText("Exit from game", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mExitText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    gFont = gFont_mono;
    if(!mScoreListText[0].loadFromRenderedText("=   10", gColor_plat))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText[0]' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    if(!mScoreListText[1].loadFromRenderedText("=   20", gColor_plat))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText[1]' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    if(!mScoreListText[2].loadFromRenderedText("=   40", gColor_plat))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText[2]' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    if(!mScoreListText[3].loadFromRenderedText("=  ???", gColor_red))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText[3]' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    mInvader[0].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2 - 20, (SCREEN_HEIGHT - mScoreListText[0].getHeight() - 280) / 2, 0);
    mInvader[1].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2 - 20, (SCREEN_HEIGHT - mScoreListText[1].getHeight() - 280) / 2 + 1 * INVADER_HEIGHT, 1);
    mInvader[2].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2 - 20, (SCREEN_HEIGHT - mScoreListText[2].getHeight() - 280) / 2 + 2 * INVADER_HEIGHT, 2);
    mUFO.setUFO((SCREEN_WIDTH - mScoreText.getWidth()) / 2 - 20, (SCREEN_HEIGHT - mScoreListText[3].getHeight() - 280) / 2 + 3 * INVADER_HEIGHT, true);

    gButtons[0].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight()) / 2 + 112);
    gButtons[1].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight() + mScoreText.getHeight()) / 2 + 120);
    gButtons[2].setPosition((SCREEN_WIDTH - mScoreText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight() + mScoreText.getHeight() + mExitText.getHeight()) / 2 + 128);
}

Intro::~Intro()
{
    mSpaceText.free(); // Text texture
    mBeginText.free(); // Text texture
    mScoreText.free(); // Text texture
    mExitText.free();  // Text texture

    for(int i = 0; i < 4; ++i)
    {
        mScoreListText[i].free();
    }
}

void Intro::handleEvents(SDL_Event* e)
{
    while(SDL_PollEvent(e) != 0)
    {
        if(e->type == SDL_QUIT) // Quit event
        {
            set_next_state(STATE_EXIT);
        }

        for(int i = 0; i <= 2; ++i) // Button events
        {
            gButtons[i].handleEvents(i, e);
        }
    }
}

void Intro::logic()
{
    // I have nothing to do.
}

void Intro::render()
{
    // Button
    for(int i = 0; i <= 2; ++i)
    {
        gButtons[i].render();
    }

    // Score list
    if(mScoreListTimer < 120)
    {
        mScoreListTimer++;
    }
    if(mScoreListTimer >= 30)
    {
        mInvader[0].render();
        mScoreListText[0].render((SCREEN_WIDTH + mScoreText.getWidth()) / 2 - mScoreListText[0].getWidth() + 20, (SCREEN_HEIGHT - mScoreListText[1].getHeight() - 280) / 2 + 0 * INVADER_HEIGHT + 10);
    }
    if(mScoreListTimer >= 60)
    {
        mInvader[1].render();
        mScoreListText[1].render((SCREEN_WIDTH + mScoreText.getWidth()) / 2 - mScoreListText[1].getWidth() + 20, (SCREEN_HEIGHT - mScoreListText[1].getHeight() - 280) / 2 + 1 * INVADER_HEIGHT + 10);
    }
    if(mScoreListTimer >= 90)
    {
        mInvader[2].render();
        mScoreListText[2].render((SCREEN_WIDTH + mScoreText.getWidth()) / 2 - mScoreListText[2].getWidth() + 20, (SCREEN_HEIGHT - mScoreListText[1].getHeight() - 280) / 2 + 2 * INVADER_HEIGHT + 10);
    }
    if(mScoreListTimer >= 120)
    {
        mUFO.render();
        mScoreListText[3].render((SCREEN_WIDTH + mScoreText.getWidth()) / 2 - mScoreListText[2].getWidth() + 20, (SCREEN_HEIGHT - mScoreListText[1].getHeight() - 280) / 2 + 3 * INVADER_HEIGHT + 10);
    }

    // Text textures
    mSpaceText.render((SCREEN_WIDTH - mSpaceText.getWidth()) / 2, (SCREEN_HEIGHT - mSpaceText.getHeight()) / 2 - 200);
    mBeginText.render((SCREEN_WIDTH - mBeginText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight()) / 2 + 104);
    mScoreText.render((SCREEN_WIDTH - mScoreText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight() + mScoreText.getHeight()) / 2 + 112);
    mExitText.render((SCREEN_WIDTH - mExitText.getWidth()) / 2, (SCREEN_HEIGHT - mBeginText.getHeight() + mScoreText.getHeight() + mExitText.getHeight()) / 2 + 120);
}

// - Overworld
Overworld::Overworld(int prevState)
{
    gScore = 0; // Global score reset

    // Invaders location
    int i = 0, n = 0;
    int type = 0;

    for(int y = 0; y < 6; y++) // Column
    {
        if(y == 0) type = 0;
        if(y > 0 && y <= 3) type = 1;
        if(y > 3 && y < 6) type = 2;

        for(int x = 0; x < 11; x++) // Line
        {
            if(y == 0 && n < 11)
            {
                activeInvaderIDs[n] = i;
                isThisShootActive[n] = false;
                n++;
            }
            mInvader[i].setPosition((SCREEN_WIDTH/2 + (11/2 * INVADER_WIDTH)) - (x * 64) - 32, (SCREEN_HEIGHT/2 + INVADER_HEIGHT) - (y * 64), type); // Coding w/ listening Mogwai.
            i++;
        }
    }

    // Bricks location
    i = 0; int mX = 0;
    for(int y = 0; y < 3; y++)
    {
        if(y == 0)
        {
            mX = 0;
            for(int x = 0; x < 8; x++)
            {
                if(x % 2 == 0)
                {
                    mX += BRICK_WIDTH + 140;
                }
                else
                {
                    mX += BRICK_WIDTH * 3;

                }

                mBrick[i].setBrick(mX, (SCREEN_HEIGHT - 160) - y * BRICK_HEIGHT);
                i++;
            }
        }
        else
        {
            mX = 0;
            for(int x = 0; x < 16; x++)
            {
                if(x % 4 == 0)
                {
                    mX += BRICK_WIDTH + 140;
                }
                else
                {
                    mX += BRICK_WIDTH;
                }

                mBrick[i].setBrick(mX, (SCREEN_HEIGHT - 160) - y * BRICK_HEIGHT);
                i++;
            }
        }
    }

    // Texture
    if(!mStateTexture.loadFromFile("gamedata/ow-statebg.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'ow-statebg.bmp' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        mStateTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    }
}

Overworld::~Overworld()
{
    mScoreText.free(); // Text texture
    mStateTexture.free(); // Texture :: State background
    mStateHText.free(); // Text texture :: Header of state
    mStateLText.free(); // Text texture :: Info about state

    // Global resets
    gGameOverTime = false;
    gInvadersYaxis = 0;
    invadersWaitTime = 30;
    gBazooka = 1;
    gBazookaSafety = false;
}

void Overworld::handleEvents(SDL_Event* e)
{
    while(SDL_PollEvent(e))
    {
        if(e->type == SDL_QUIT)
        {
            set_next_state(STATE_EXIT);
        }
        else if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_p)
        {
            switch(mCurrentState)
            {
                case OVERWORLD_STATE_ACTIVE: mCurrentState = OVERWORLD_STATE_PAUSED; break;
                default: mCurrentState = OVERWORLD_STATE_ACTIVE; break;
            }
        }

        if(mCurrentState == OVERWORLD_STATE_GAMEOVER || mCurrentState == OVERWORLD_STATE_PAUSED || mCurrentState == OVERWORLD_STATE_SCORELIMIT)
        {
            if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
            {
                set_next_state(STATE_INTRO);
            }
            else if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN && mCurrentState != OVERWORLD_STATE_PAUSED)
            {
                set_next_state(STATE_SAVESCORE);
            }
        }

        mPlayer.handleEvents(e);
    }
}

void Overworld::logic()
{
    if(mPlayer.currentHealth <= 0) mCurrentState = OVERWORLD_STATE_GAMEOVER, gLaserSafety = true;
    if(gScore >= 999999999) mCurrentState = OVERWORLD_STATE_SCORELIMIT, gLaserSafety = true;
    if(gGameOverTime == true) mCurrentState = OVERWORLD_STATE_GAMEOVER, gLaserSafety = true;

    if(mCurrentState == OVERWORLD_STATE_ACTIVE)
    {
        mPlayer.move();

        if(invadersMoveTime < invadersWaitTime)
        {
            invadersMoveTime++;
        }
        else
        {
            for(int i = 0; i < 66; ++i)
            {
                mInvader[i].move(11);
                invadersMoveTime = 0;
            }

            // Invaders shoots
            int dice_roll = distribution(generator);
            for(int i = 0; i < 11; ++i)
            {
                if(mInvader[activeInvaderIDs[i]].heded == false)
                {
                    if(isThisShootActive[i] == false)
                    {
                        if(i != dice_roll) continue;
                        if(dice_roll > 10) break;
                        invaderShoot[i].resetFire();
                        if(i % 2 == 0)
                        {
                            invaderShoot[i].createFire(mInvader[activeInvaderIDs[i]].mCollider.x + INVADER_WIDTH / 2, mInvader[activeInvaderIDs[i]].mCollider.y + INVADER_HEIGHT / 2, 1, false);
                        }
                        else
                        {
                            invaderShoot[i].createFire(mInvader[activeInvaderIDs[i]].mCollider.x + INVADER_WIDTH / 2, mInvader[activeInvaderIDs[i]].mCollider.y + INVADER_HEIGHT / 2, 2, false);
                        }
                        isThisShootActive[i] = true;
                    }
                }
            }
        }

        // UFO
        int dice_roll = distribution(generator);
        if(heilUFO.mActivity == true)
        {
            if(checkCollision(heilUFO.mCollider, mPlayer.mShoot.mCollider))
            {
                gScore += dice_roll * 10;
                gLaserSafety = false;
                heilUFO.stopUFO();
            }
            if(heilUFO.mCollider.x < 0 || (heilUFO.UFO_WIDTH + heilUFO.mCollider.x > SCREEN_WIDTH))
            {
                heilUFO.stopUFO();
            }
            else if(heilUFO.mCollider.y < heilUFO.UFO_HEIGHT || (heilUFO.UFO_HEIGHT + heilUFO.mCollider.y > SCREEN_HEIGHT))
            {
                heilUFO.stopUFO();
            }
            else
            {
                heilUFO.move(2);
            }
        }
        else
        {
            if(ufoCurrentTime < ufoMaxTime)
            {
                ufoCurrentTime++;
            }
            else
            {
                ufoCurrentTime = 0;
                if(dice_roll > 0) ufoMaxTime = dice_roll * 2000;
                else ufoMaxTime = 6000;

                if(dice_roll < 11)
                {
                    heilUFO.setUFO(0, heilUFO.UFO_HEIGHT, true);
                }
                else if(dice_roll > 10)
                {
                    heilUFO.setUFO(SCREEN_WIDTH - heilUFO.UFO_WIDTH, heilUFO.UFO_HEIGHT, false);
                }
            }
        }

        // Invaders shoots movement
        for(int i = 0; i < 11; ++i)
        {
            if(mInvader[activeInvaderIDs[i]].heded == false)
            {
                if(isThisShootActive[i] == true)
                {
                    if(invaderShoot[i].mCollider.x < 0 || (BULLET_WIDTH + invaderShoot[i].mCollider.x > SCREEN_WIDTH))
                    {
                        isThisShootActive[i] = false;
                    }
                    else if(invaderShoot[i].mCollider.y < BULLET_HEIGHT || (BULLET_HEIGHT + invaderShoot[i].mCollider.y > SCREEN_HEIGHT))
                    {
                        isThisShootActive[i] = false;
                    }
                    else
                    {
                        invaderShoot[i].move(8);

                        if(checkCollision(mPlayer.mCollider, invaderShoot[i].mCollider))
                        {
                            if(mPlayer.currentHealth > 0)
                            {
                                mPlayer.currentHealth--;
                                invaderShoot[i].resetFire();
                                isThisShootActive[i] = false;
                            }
                        }

                        for(int n = 0; n < 40; n++)
                        {
                            mBrick[n].updateBrick(invaderShoot[i].mCollider);

                            if(mBrick[n].brickCheckCollision(invaderShoot[i].mCollider))
                            {
                                invaderShoot[i].resetFire();
                                isThisShootActive[i] = false;
                            }
                        }
                    }
                }
            }
            else if(mInvader[activeInvaderIDs[i]].heded == true && activeInvaderIDs[i] < 55)
            {
                activeInvaderIDs[i] += 11;
            }
            else
            {
                invaderShoot[i].resetFire();
                isThisShootActive[i] = false;
            }
        }

        if(invadersDownTime == true)
        {
            invadersDownTime = false;
            if(invadersMoveTime == invadersWaitTime)
            {
                if(invadersWaitTime > 0) invadersWaitTime -= 3;
                gInvadersYaxis += 10;
            }
        }

        for(int i = 0; i < 66; ++i)
        {
            mInvader[i].direction(mPlayer.mShoot.mCollider, mPlayer.mBazooka.mCollider);
        }

        if(gDeadInvaders >= 66)
        {
            gBazooka++;
            mPlayer.currentHealth++;
            // Invaders location
            int i = 0, n = 0;
            int type = 0;
            for(int y = 0; y < 6; y++) // Column
            {
                if(y == 0) type = 0;
                if(y > 0 && y <= 3) type = 1;
                if(y > 3 && y < 6) type = 2;

                for(int x = 0; x < 11; x++) // Line
                {
                    if(y == 0 && n < 11)
                    {
                        activeInvaderIDs[n] = i;
                        isThisShootActive[n] = false;
                        n++;
                    }

                    mInvader[i].setPosition((SCREEN_WIDTH/2 + (11/2 * INVADER_WIDTH)) - (x * 64) - 32, (SCREEN_HEIGHT/2 + INVADER_HEIGHT) - (y * 64) + gInvadersYaxis, type);
                    mInvader[i].twingleTwingleYouLittleMotherFucker();
                    gDeadInvaders = 0;
                    invadersWaitTime = 30;
                    i++;
                }
            }
        }

        // Brick update
        for(int i = 0; i < 40; i++)
        {
            mBrick[i].updateBrick(mPlayer.mShoot.mCollider);
            mBrick[i].updateBrick(mPlayer.mBazooka.mCollider);

            if(mBrick[i].brickCheckCollision(mPlayer.mShoot.mCollider))
            {
                gLaserSafety = false;
            }
        }
    }
}

void Overworld::render()
{
    mPlayer.render();

    for(int i = 0; i < 66; ++i)
    {
        mInvader[i].render();
    }

    for(int i = 0; i < 40; i++)
    {
        mBrick[i].render();
    }

    for(int i = 0; i < 11; ++i)
    {
        if(isThisShootActive[i] == true)
        {
            invaderShoot[i].render();
        }
    }

    heilUFO.render();

    std::stringstream scoreText;
    scoreText.str("");
    scoreText << "Score: " << std::to_string(gScore) << " | 105mm ammo: " << std::to_string(gBazooka) << " (Press B for fire)";

    gFont = gFont_fipps;
    if(!mScoreText.loadFromRenderedText(scoreText.str().c_str(), gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    mScoreText.render(1, 0);

    if(mCurrentState == OVERWORLD_STATE_PAUSED)
    {
        mStateTexture.setAlpha(200);
        mStateTexture.render((SCREEN_WIDTH - mStateTexture.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2);

        gFont = gFont_mono;
        if(!mStateHText.loadFromRenderedText("GAME PAUSED", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }

        gFont = gFont_fipps;
        if(!mStateLText.loadFromRenderedText("ESC - Main menu | P - Return to game", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        mStateHText.render((SCREEN_WIDTH - mStateHText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight());
        mStateLText.render((SCREEN_WIDTH - mStateLText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight() + mStateLText.getHeight());
    }
    else if(mCurrentState == OVERWORLD_STATE_GAMEOVER)
    {
        mStateTexture.setAlpha(200);
        mStateTexture.render((SCREEN_WIDTH - mStateTexture.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2);

        gFont = gFont_mono;
        if(!mStateHText.loadFromRenderedText("GAME OVER", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }

        gFont = gFont_fipps;
        if(!mStateLText.loadFromRenderedText("ESC - Main menu | ENTER - Save score", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        mStateHText.render((SCREEN_WIDTH - mStateHText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight());
        mStateLText.render((SCREEN_WIDTH - mStateLText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight() + mStateLText.getHeight());
    }
    else if(mCurrentState == OVERWORLD_STATE_SCORELIMIT)
    {
        mStateTexture.setAlpha(200);
        mStateTexture.render((SCREEN_WIDTH - mStateTexture.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2);

        gFont = gFont_mono;
        if(!mStateHText.loadFromRenderedText("YOU ACHIEVE SCORE LIMIT", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }

        gFont = gFont_fipps;
        if(!mStateLText.loadFromRenderedText("ESC - Main menu | ENTER - Save score", gColor_plat))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to load 'mStateHText' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        mStateHText.render((SCREEN_WIDTH - mStateHText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight());
        mStateLText.render((SCREEN_WIDTH - mStateLText.getWidth()) / 2, (SCREEN_HEIGHT - mStateTexture.getHeight()) / 2 + mStateHText.getHeight() + mStateLText.getHeight());
    }
}

// - Savescore
Savescore::Savescore()
{
    SDL_RWops* scoreFile = SDL_RWFromFile("gamedata/ohnein.bin", "r+b");

    if(scoreFile == NULL)
    {
        #ifdef WARNING_MESSAGES
            std::cout << "[warning] File not found." << std::endl;
        #endif // WARNING_MESSAGES

        scoreFile = SDL_RWFromFile("gamedata/ohnein.bin", "w+b");

        if(scoreFile == NULL)
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Unable to create new file!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        else
        {
            #ifdef WARNING_MESSAGES
                std::cout << "[warning] New file created." << std::endl;
            #endif // WARNING_MESSAGES

            for(int i = 0; i < TOTAL_DATA; i++)
            {
                mData[i] = -1;
                SDL_RWwrite(scoreFile, &mData[i], sizeof(Sint32), 1);
            }
            SDL_RWclose(scoreFile);
        }
    }
    else
    {
        for(int i = 0; i < TOTAL_DATA; i++)
        {
             SDL_RWread(scoreFile, &mData[i], sizeof(Sint32), 1);
        }

        SDL_RWclose(scoreFile);
    }

    if(!mInfoText.loadFromRenderedText("Enter your name: ", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mInfoText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    if(!mSInfoText.loadFromRenderedText("Your score: ", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mSInfoText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }


    if(!mInputText.loadFromRenderedText(" ", gColor_plat))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mInputText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    if(!mScoreText.loadFromRenderedText(std::to_string(gScore), gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mScoreText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    SDL_StartTextInput();
}

Savescore::~Savescore()
{
    Sint32 tempData[TOTAL_DATA];
    for(int i = 0; i < TOTAL_DATA; i++)
    {
        if(gScore > mData[i] || mData[i] == 0)
        {
            for(int n = 0; n < TOTAL_DATA; n++)
            {
                tempData[n] = mData[n];
            }

            for(int n = i; n < TOTAL_DATA; n++)
            {
                if(n + 1 < TOTAL_DATA)
                {
                    mData[n + 1] = tempData[n];
                }
            }

            mData[i] = gScore;
            break;
        }
    }

    SDL_RWops* scoreFile = SDL_RWFromFile("gamedata/ohnein.bin", "w+b");

    if(scoreFile != NULL)
    {
        for(int i = 0; i < TOTAL_DATA; i++)
        {
            SDL_RWwrite(scoreFile, &mData[i], sizeof(Sint32), 1);
        }

        SDL_RWclose(scoreFile);
    }
    else
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to save score data to file!" << std:: endl;
        #endif // ERROR_MESSAGES
    }

    mInfoText.free();
    mSInfoText.free();
    mInputText.free();
    mScoreText.free();
}

void Savescore::handleEvents(SDL_Event* e)
{
    while(SDL_PollEvent(e))
    {
        if(e->type == SDL_QUIT)
        {
            set_next_state(STATE_EXIT);
        }
        else if(e->type == SDL_KEYDOWN)
        {
            if(e->key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
            {
                inputText.pop_back();
                mRenderTime = true;
            }
            else if(e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                SDL_SetClipboardText(inputText.c_str());
            }
            else if(e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
            {
                inputText = SDL_GetClipboardText();
                mRenderTime = true;
            }
            else if(e->key.keysym.sym == SDLK_RETURN)
            {
                SDL_StopTextInput();
                set_next_state(STATE_SCOREBOARD);
            }
        }
        else if(e->type == SDL_TEXTINPUT)
        {
            if(!((e->text.text[0] == 'c' || e->text.text[0] == 'C') && (e->text.text[0] == 'v' || e->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
            {
                if(inputText.length() < 12)
                {
                    inputText += e->text.text;
                    mRenderTime = true;
                }
            }
        }
    }
}

void Savescore::logic()
{
    if(mRenderTime == true)
    {
        if(inputText == "")
        {
            mInputText.loadFromRenderedText(" ", gColor_plat);
        }
        else
        {
            mInputText.loadFromRenderedText(inputText.c_str(), gColor_plat);
        }
    }
}

void Savescore::render()
{
    mInfoText.render(SCREEN_WIDTH / 2 - mInfoText.getWidth(), SCREEN_HEIGHT / 2);
    mSInfoText.render(SCREEN_WIDTH / 2 - mSInfoText.getWidth(), SCREEN_HEIGHT / 2 + mScoreText.getHeight());
    mInputText.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    mScoreText.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + mScoreText.getHeight());
}


// - Scoreboard
Scoreboard::Scoreboard()
{
    SDL_RWops* scoreFile = SDL_RWFromFile("gamedata/ohnein.bin", "r+b");

    if(scoreFile == NULL)
    {
        #ifdef WARNING_MESSAGES
            std::cout << "[warning] File not found." << std::endl;
        #endif // WARNING_MESSAGES

        scoreFile = SDL_RWFromFile("gamedata/ohnein.bin", "w+b");

        if(scoreFile == NULL)
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Unable to create new file!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        else
        {
            #ifdef WARNING_MESSAGES
                std::cout << "[warning] New file created." << std::endl;
            #endif // WARNING_MESSAGES

            for(int i = 0; i < TOTAL_DATA; i++)
            {
                mData[i] = -1;
                SDL_RWwrite(scoreFile, &mData[i], sizeof(Sint32), 1);
            }
            SDL_RWclose(scoreFile);
        }
    }
    else
    {
        for(int i = 0; i < TOTAL_DATA; i++)
        {
            SDL_RWread(scoreFile, &mData[i], sizeof(Sint32), 1);
        }

        SDL_RWclose(scoreFile);
    }

    for(int i = 0; i < TOTAL_DATA; i++)
    {
        if(mData[i] >= 0)
        {
            scoreValue++;
        }
    }

    gFont = gFont_mono;
    if(scoreValue == 0)
    {
        if(!mScoreList[0].loadFromRenderedText("NO SCORE RECORD FOUND", gColor_onyx))
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to render 'mScoreList[0]' texture!" << std::endl;
            #endif // ERROR_MESSAGES
        }
    }
    else
    {
        for(int i = 0; i < scoreValue; i++)
        {
            if(!mScoreList[i].loadFromRenderedText(std::to_string(mData[i]), gColor_onyx))
            {
                #ifdef ERROR_MESSAGES
                    std::cout << "[error] Failed to render 'mScoreList[" << i << "]' texture!" << std::endl;
                #endif // ERROR_MESSAGES
            }
        }
    }

    gFont = gFont_fipps;
    if(!mBackText.loadFromRenderedText("Back to main menu", gColor_blue))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to render 'mBackText' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    gButtons[3].setPosition(0, SCREEN_HEIGHT - (mBackText.getHeight()) + 6);
}

Scoreboard::~Scoreboard()
{
    for(int i = 0; i < TOTAL_DATA; i++)
    {
        mScoreList[i].free();
    }

    mBackText.free();
}

void Scoreboard::handleEvents(SDL_Event* e)
{
    while(SDL_PollEvent(e))
    {
        if(e->type == SDL_QUIT)
        {
            set_next_state(STATE_EXIT);
        }

        gButtons[3].handleEvents(3, e);
    }
}

void Scoreboard::logic()
{
}

void Scoreboard::render()
{
    gButtons[3].render();

    if(scoreValue == 0)
    {
        mScoreList[0].render((SCREEN_WIDTH - mScoreList[0].getWidth()) / 2, (SCREEN_HEIGHT - mScoreList[0].getHeight()) / 2);
    }
    else
    {
        for(int i = 0; i < scoreValue; i++)
        {
            mScoreList[i].render((SCREEN_WIDTH - mScoreList[i].getWidth()) / 2, (((SCREEN_HEIGHT / 2) - (scoreValue / 2 * mScoreList[i].getHeight())) + i * mScoreList[i].getHeight()));
        }
    }

    mBackText.render(0, SCREEN_HEIGHT - mBackText.getHeight());
}

void set_next_state(int newState)
{
    if(nextState != STATE_EXIT)
    {
        nextState = newState;
    }
}

void change_state()
{
    if(nextState != STATE_NULL)
    {
        if(nextState != STATE_EXIT)
        {
            delete currentState;
        }

        switch(nextState)
        {
            case STATE_INTRO:
                currentState = new Intro();
                break;

            case STATE_OVERWORLD:
                currentState = new Overworld(stateID);
                break;

            case STATE_SAVESCORE:
                currentState = new Savescore();
                break;

            case STATE_SCOREBOARD:
                currentState = new Scoreboard();
                break;
        }

        stateID = nextState;
        nextState = STATE_NULL;
    }
}

// Timer
LTimer::LTimer()
{
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    mStarted = true;
    mPaused = false;

    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    mStarted = false;
    mPaused = false;

    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    if(mStarted && !mPaused)
    {
        mPaused = true;
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    if(mStarted && mPaused)
    {
        mPaused = false;
        mStartTicks = SDL_GetTicks() - mPausedTicks;
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    Uint32 time = 0;

    if(mStarted)
    {
        if(mPaused)
        {
            time = mPausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted()
{
    return mStarted;
}

bool LTimer::isPaused()
{
    return mPaused && mStarted;
}

// Texture
LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    free();

    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] " << SDL_GetError() << "!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

        if(newTexture == NULL)
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] " << SDL_GetError() << "!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        else
        {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    free();

    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if(textSurface == NULL)
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] " << TTF_GetError() << "!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL)
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] " << SDL_GetError() << "!" << std::endl;
            #endif // ERROR_MESSAGES
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;
}

void LTexture::free()
{
    if(mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    if(clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

// Mouse button
LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
    mCurrentSprite = BUTTON_SPRITE_MOUSE_INACTIVE;
}

void LButton::setPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvents(int buttonID, SDL_Event* e)
{
    if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool inside = true;
        if(x < mPosition.x)
        {
            inside = false;
        }
        else if(x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }
        else if(y < mPosition.y)
        {
            inside = false;
        }
        else if(y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        if(!inside)
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_INACTIVE;
        }
        else
        {
            if(e->type == SDL_MOUSEMOTION)
            {
                mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
            }
            else if(e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
            {
                mCurrentSprite = BUTTON_SPRITE_MOUSE_ACTIVE;

                switch(buttonID)
                {
                    case 0: set_next_state(STATE_OVERWORLD); break;
                    case 1: set_next_state(STATE_SCOREBOARD); break;
                    case 2: set_next_state(STATE_EXIT); break;
                    case 3: set_next_state(STATE_INTRO); break;
                }
            }
        }
    }
}

void LButton::render()
{
    gButtonSprite.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}

// Player
Player::Player()
{
    mPosX = (SCREEN_WIDTH - PLAYER_WIDTH)/2, mPosY = SCREEN_HEIGHT - PLAYER_HEIGHT;
    mVelX = 0, mVelY = 0;
    activeShoot = false;

    currentHealth = 4;
    if(!mHeartSprite.loadFromFile("gamedata/heart.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'heart.bmp' sprite texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        for(int i = 0; i < HEALTH_SPRITE_TOTAL; ++i)
        {
            mHealthSpriteClips[i].x = 0;
            mHealthSpriteClips[i].y = i * HEART_HEIGHT;
            mHealthSpriteClips[i].w = HEART_WIDTH;
            mHealthSpriteClips[i].h = HEART_HEIGHT;
        }
    }
    mCurrentSprite = HEALTH_SPRITE_FOUR;

    mCollider.x = 0;
    mCollider.y = 0;
    mCollider.w = PLAYER_WIDTH;
    mCollider.h = PLAYER_HEIGHT;
}

Player::~Player()
{
    mPosX = 0, mPosY = 0;
    mVelX = 0, mVelY = 0;
    currentHealth = 0;

    mCurrentSprite = HEALTH_SPRITE_NONE;
    mHeartSprite.free();
}

void Player::handleEvents(SDL_Event* e)
{
    if(e->type == SDL_KEYDOWN && e->key.repeat == 0)
    {
        switch(e->key.keysym.sym)
        {
            case SDLK_RIGHT: mVelX += PLAYER_VEL; break;
            case SDLK_LEFT: mVelX -= PLAYER_VEL; break;
            case SDLK_SPACE:
                if(gLaserSafety == false)
                {
                    mShoot.createFire(mPosX + PLAYER_WIDTH / 2 - 4, mPosY + 5, 0, true);
                    gLaserSafety = true;
                }
                break;
            case SDLK_b:
                if(gBazooka > 0 && gBazookaSafety == false)
                {
                    gBazooka--;
                    mBazooka.createFire(mPosX + PLAYER_WIDTH / 2 + 16, mPosY + 6, 3, true);
                    gBazookaSafety = true;
                }
                break;
        }
    }
    else if(e->type == SDL_KEYUP && e->key.repeat == 0)
    {
        switch(e->key.keysym.sym)
        {
            case SDLK_RIGHT: mVelX -= PLAYER_VEL; break;
            case SDLK_LEFT: mVelX += PLAYER_VEL; break;
        }
    }
}

void Player::move()
{
    // Player movement
    mPosX += mVelX;
    if((mPosX < 0) || (PLAYER_WIDTH + mPosX > SCREEN_WIDTH))
    {
        mPosX -= mVelX;
    }

    mPosY += mVelY;
    if((mPosY < HEART_HEIGHT) || (PLAYER_HEIGHT + mPosY > SCREEN_HEIGHT))
    {
        mPosY -= mVelY;
    }

    // Collision box update
    mCollider.x = mPosX;
    mCollider.y = mPosY;

    // Shoot update
    if(mShoot.mCollider.x < 0 || (BULLET_WIDTH + mShoot.mCollider.x > SCREEN_WIDTH))
    {
        mShoot.resetFire();
        gLaserSafety = false;
    }
    else if(mShoot.mCollider.y < BULLET_HEIGHT || (BULLET_HEIGHT + mShoot.mCollider.y > SCREEN_HEIGHT))
    {
        mShoot.resetFire();
        gLaserSafety = false;
    }
    else
    {
        if(gLaserSafety == true)
        {
            mShoot.move(8);
        }
        else
        {
            mShoot.resetFire();
        }
    }

    // Bazooka update
    if(mBazooka.mCollider.x < 0 || (BULLET_WIDTH + mBazooka.mCollider.x > SCREEN_WIDTH))
    {
        mBazooka.resetFire();
        gBazookaSafety = false;
    }
    else if(mBazooka.mCollider.y < BULLET_HEIGHT || (BULLET_HEIGHT + mBazooka.mCollider.y > SCREEN_HEIGHT))
    {
        mBazooka.resetFire();
        gBazookaSafety = false;
    }
    else
    {
        if(gBazookaSafety == true)
        {
            mBazooka.move(3);
        }
        else
        {
            mBazooka.resetFire();
        }
    }
}

void Player::render()
{
    gPlayerTexture.render(mPosX, mPosY);

    switch(currentHealth)
    {
        case 6: mCurrentSprite = HEALTH_SPRITE_SIX; break;
        case 5: mCurrentSprite = HEALTH_SPRITE_FIVE; break;
        case 4: mCurrentSprite = HEALTH_SPRITE_FOUR; break;
        case 3: mCurrentSprite = HEALTH_SPRITE_THREE; break;
        case 2: mCurrentSprite = HEALTH_SPRITE_TWO; break;
        case 1: mCurrentSprite = HEALTH_SPRITE_ONE; break;
        case 0: mCurrentSprite = HEALTH_SPRITE_NONE; break;
        default: mCurrentSprite = HEALTH_SPRITE_MORE; break;
    }
    mHeartSprite.render(SCREEN_WIDTH - HEART_WIDTH, 8, &mHealthSpriteClips[mCurrentSprite]);

    if(gLaserSafety == true)
    {
        mShoot.render();
    }

    if(gBazookaSafety == true)
    {
        mBazooka.render();
    }
}

// Invader
Invader::Invader()
{
    if(!mInvaderSprite.loadFromFile("gamedata/gameinvaders.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'gameinvaders.bmp' sprite texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }

    heded = false;
    mPosition.x = 0;
    mPosition.y = 0;

    mCollider.w = INVADER_WIDTH;
    mCollider.h = INVADER_HEIGHT;
}

Invader::~Invader()
{
    mInvaderSprite.free();
}

void Invader::setPosition(int x, int y, int type)
{
    mPosition.x = x;
    mPosition.y = y;
    mType = type;

    for(int i = 0; i < INVADER_SPRITE_TOTAL; ++i)
    {
        mInvaderSpriteClips[i].x = mType * INVADER_WIDTH;
        mInvaderSpriteClips[i].y = i * INVADER_HEIGHT;
        mInvaderSpriteClips[i].w = INVADER_WIDTH;
        mInvaderSpriteClips[i].h = INVADER_HEIGHT;
    }
    mCurrentSprite = INVADER_SPRITE_WAIT;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
    heded = false;
}

void Invader::direction(SDL_Rect& rect_1, SDL_Rect& rect_2)
{
    if(heded == false)
    {
        if(mPosition.x <= 0)
        {
            invadersDirection = true;
            invadersDownTime = true;
        }
        else if(mPosition.x >= (SCREEN_WIDTH - INVADER_WIDTH))
        {
            invadersDirection = false;
            invadersDownTime = true;
        }

        if(checkCollision(mCollider, rect_1))
        {
            heded = true;
            gLaserSafety = false;
            gDeadInvaders++;

            if(mType == 0) gScore += 10;
            if(mType == 1) gScore += 20;
            if(mType == 2) gScore += 40;
        }

        if(checkCollision(mCollider, rect_2))
        {
            heded = true;
            gDeadInvaders++;

            if(mType == 0) gScore += 10;
            if(mType == 1) gScore += 20;
            if(mType == 2) gScore += 40;
        }
    }
}

void Invader::move(int speed)
{
    if(mPosition.y >= (SCREEN_HEIGHT - 280) && gGameOverTime == false) gGameOverTime = true;
    if(heded == false && gGameOverTime == false)
    {
        if(invadersDirection == true)
        {
            mPosition.x += speed;
        }
        else if(invadersDirection == false)
        {
            mPosition.x -= speed;
        }

        if(invadersDownTime == true)
        {
            mPosition.y += 10;
        }

        if(mCurrentSprite == INVADER_SPRITE_MOVE)
        {
            mCurrentSprite = INVADER_SPRITE_WAIT;
        }
        else if(mCurrentSprite == INVADER_SPRITE_WAIT)
        {
            mCurrentSprite = INVADER_SPRITE_MOVE;
        }

        mCollider.x = mPosition.x;
        mCollider.y = mPosition.y;
    }
}

void Invader::twingleTwingleYouLittleMotherFucker()
{
    heded = false;
}

void Invader::render()
{
    if(heded == false)
    {
        mInvaderSprite.render(mPosition.x, mPosition.y, &mInvaderSpriteClips[mCurrentSprite]);
    }
}

// UFO
UFO::UFO()
{
    if(!mUFOTexture.loadFromFile("gamedata/ufo.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'ufo.bmp' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        for(int i = 0; i < UFO_SPRITE_TOTAL; i++)
        {
            mUFOSpriteClips[i].x = 0;
            mUFOSpriteClips[i].y = i * UFO_HEIGHT;
            mUFOSpriteClips[i].w = UFO_WIDTH;
            mUFOSpriteClips[i].h = UFO_HEIGHT;
        }
    }
    mCurrentSprite = UFO_SPRITE_RIGHT;

    // Position
    mPosition.x = 0;
    mPosition.y = 0;

    // Collider startup settings
    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
    mCollider.w = UFO_WIDTH;
    mCollider.h = UFO_HEIGHT;
}

UFO::~UFO()
{
    mUFOTexture.free();
}

void UFO::setUFO(int x, int y, bool direction)
{
    mPosition.x = x;
    mPosition.y = y;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;

    mDirection = direction;
    mActivity = true;

    if(direction == true)
    {
        mCurrentSprite = UFO_SPRITE_LEFT;
    }
    else
    {
        mCurrentSprite = UFO_SPRITE_RIGHT;
    }
}

void UFO::stopUFO()
{
    mActivity = false;
    mPosition.x = 0;
    mPosition.y = 0;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
}

void UFO::move(int speed)
{
    if(mActivity == true)
    {
        if(mDirection == true)
        {
            mPosition.x += speed;
        }
        else
        {
            mPosition.x -= speed;
        }

        mCollider.x = mPosition.x;
        mCollider.y = mPosition.y;
    }
}

void UFO::render()
{
    if(mActivity == true)
    {
        mUFOTexture.render(mPosition.x, mPosition.y, &mUFOSpriteClips[mCurrentSprite]);
    }
}

// Shoot
Shoot::Shoot()
{
    mPosition.x = 0;
    mPosition.y = 0;
    mFromPlayer = false;
    mCurrentSprite = BULLET_SPRITE_ONE;

    mCollider.w = BULLET_WIDTH;
    mCollider.h = BULLET_HEIGHT;
    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
}

Shoot::~Shoot()
{
    mBulletSprite.free();
    mPosition.x = 0;
    mPosition.y = 0;

    mFromPlayer = false;
}

void Shoot::createFire(int x, int y, int type, bool fromPlayer)
{
    mPosition.x = x;
    mPosition.y = y;
    mFromPlayer = fromPlayer;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;

    if(!mBulletSprite.loadFromFile("gamedata/bullets.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'bullets.bmp' sprite texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        for(int i = 0; i < BULLET_SPRITE_TOTAL; ++i)
        {
            mBulletSpriteClips[i].x = type * BULLET_WIDTH;
            mBulletSpriteClips[i].y = i * BULLET_HEIGHT;
            mBulletSpriteClips[i].w = BULLET_WIDTH;
            mBulletSpriteClips[i].h = BULLET_HEIGHT;
        }
        mCurrentSprite = BULLET_SPRITE_ONE;
    }
}

void Shoot::resetFire()
{
    mPosition.x = 0;
    mPosition.y = 0;

    mCollider.x = 0;
    mCollider.y = 0;
}

void Shoot::move(int speed)
{
    if(mFromPlayer == true)
    {
        mPosition.y -= speed;
    }
    else
    {
        mPosition.y += speed;
    }

    if(mCurrentSprite == BULLET_SPRITE_ONE)
    {
        mCurrentSprite = BULLET_SPRITE_TWO;
    }
    else
    {
        mCurrentSprite = BULLET_SPRITE_ONE;
    }

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
}

void Shoot::render()
{
    mBulletSprite.render(mPosition.x, mPosition.y, &mBulletSpriteClips[mCurrentSprite]);
}

// Brick
Brick::Brick()
{
    if(!mBrickSprite.loadFromFile("gamedata/bricks.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'brick.bmp' texture!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        for(int i = 0; i < BRICK_SPRITE_TOTAL; ++i)
        {
            mBrickSpriteClips[i].x = 0;
            mBrickSpriteClips[i].y = i * BRICK_HEIGHT;
            mBrickSpriteClips[i].w = BRICK_WIDTH;
            mBrickSpriteClips[i].h = BRICK_HEIGHT;
        }
        mCurrentSprite = BRICK_SPRITE_NODAMAGE;
    }

    mPosition.x = 0;
    mPosition.y = 0;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
    mCollider.w = BRICK_WIDTH;
    mCollider.h = BRICK_HEIGHT;
}

Brick::~Brick()
{
    mBrickSprite.free();
}

void Brick::setBrick(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;

    mCollider.x = mPosition.x;
    mCollider.y = mPosition.y;
    heded = false;
}

void Brick::updateBrick(SDL_Rect& rect)
{
    if(checkCollision(mCollider, rect))
    {
        switch(mCurrentSprite)
        {
            case BRICK_SPRITE_NODAMAGE: mCurrentSprite = BRICK_SPRITE_DAMAGELOW; break;
            case BRICK_SPRITE_DAMAGELOW: mCurrentSprite = BRICK_SPRITE_DAMAGEMED; break;
            case BRICK_SPRITE_DAMAGEMED: mCurrentSprite = BRICK_SPRITE_DAMAGEHIGH; break;
            case BRICK_SPRITE_DAMAGEHIGH: heded = true;
            case BRICK_SPRITE_TOTAL: mCurrentSprite = BRICK_SPRITE_NODAMAGE; break;
        }
    }

    if(heded == true)
    {
        mCollider.x = 0;
        mCollider.y = 0;
    }
}

bool Brick::brickCheckCollision(SDL_Rect& rect)
{
    if(checkCollision(mCollider, rect))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Brick::render()
{
    if(heded == false)
    {
        mBrickSprite.render(mPosition.x, mPosition.y, &mBrickSpriteClips[mCurrentSprite]);
    }
}

/// Main game:
bool game_Init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] SDL could not initialize! " << SDL_GetError() << std::endl;
        #endif // ERROR_MESSAGES
        return false;
    }
    else
    {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            #ifdef WARNING_MESSAGES
                std::cout << "[warning] Linear texture filtering not enabled!" << std::endl;
            #endif // WARNING_MESSAGES
            return false;
        }

        gWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL)
        {
            #ifdef ERROR_MESSAGES
                std::cout << "[error] Failed to initialize window! " << SDL_GetError() << std::endl;
            #endif // ERROR_MESSAGES
            return false;
        }
        else
        {
            #ifdef SDL_RENDERER_FPSCAP
                gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            #else
                gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            #endif // SDL_RENDERER_FPSCAP

            if(gRenderer == NULL)
            {
                #ifdef ERROR_MESSAGES
                    std::cout << "[error] Renderer could not be created! " << SDL_GetError() << std::endl;
                #endif // ERROR_MESSAGES
                return false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0x11, 0x11, 0x11, 0xFF); // Background color

                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags))
                {
                    #ifdef ERROR_MESSAGES
                        std::cout << "[error] SDL_image could not initialize! " << SDL_GetError() << std::endl;
                    #endif // ERROR_MESSAGES
                    return false;
                }

                if(TTF_Init() == -1)
                {
                    #ifdef ERROR_MESSAGES
                        std::cout << "[error] SDL_ttf could not initialize! " << SDL_GetError() << std::endl;
                    #endif // ERROR_MESSAGES
                    return false;
                }
            }
        }
    }
    return true;
}

bool game_loadMedia()
{
    // Textures
    if(!gPlayerTexture.loadFromFile("gamedata/player.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'player.bmp' texture!" << std::endl;
        #endif // ERROR_MESSAGES
        return false;
    }

    // Fonts
    gFont_fipps = TTF_OpenFont("gamedata/Fipps-Regular.otf", 16);
    gFont_mono = TTF_OpenFont("gamedata/VCR_OSD_MONO.ttf", 40);

    if(gFont_fipps == NULL || gFont_mono == NULL)
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load the fonts!" << std::endl;
        #endif // ERROR_MESSAGES
        return false;
    }

    // Mouse button
    if(!gButtonSprite.loadFromFile("gamedata/button.bmp"))
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'button.bmp' sprite texture!" << std::endl;
        #endif // ERROR_MESSAGES
        return false;
    }
    else
    {
        for(int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
        {
            gSpriteClips[i].x = 0;
            gSpriteClips[i].y = i * BUTTON_HEIGHT;
            gSpriteClips[i].w = BUTTON_WIDTH;
            gSpriteClips[i].h = BUTTON_HEIGHT;
        }
    }

    // Game icon
    gIcon = IMG_Load("gamedata/icon.ico");
    if(gIcon == NULL)
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to load 'gIcon' texture image!" << std::endl;
        #endif // ERROR_MESSAGES
        return false;
    }
    else
    {
        SDL_SetWindowIcon(gWindow, gIcon);
    }
    return true;
}

void game_Cleanup()
{
    // General
    delete currentState; // Game state
    SDL_FreeSurface(gIcon); // Window icon

    // Invaders
    invadersDirection = NULL;
    invadersDownTime = NULL;
    invadersMoveTime = 0;
    invadersWaitTime = 0;

    // Textures
    gPlayerTexture.free();
    gButtonSprite.free(); // Mouse button

    // Fonts
    gFont = NULL;
    TTF_CloseFont(gFont_fipps);
    TTF_CloseFont(gFont_mono);
    gFont_fipps = NULL;
    gFont_mono = NULL;

    // Window & renderer
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;

    // Main
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    if(!game_Init())
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to initialize the game!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else if(!game_loadMedia())
    {
        #ifdef ERROR_MESSAGES
            std::cout << "[error] Failed to loading the main media!" << std::endl;
        #endif // ERROR_MESSAGES
    }
    else
    {
        // State machine
        stateID = STATE_INTRO;
        currentState = new Intro();

        // Timer
        LTimer fpsTimer;

        #ifdef SDL_RENDERER_FPSCAP
            LTimer capTimer;
        #endif // SDL_RENDERER_FPSCAP

        int countedFrames = 0;
        fpsTimer.start();

        while(stateID != STATE_EXIT)
        {
            #ifdef SDL_RENDERER_FPSCAP
                capTimer.start();
            #endif // SDL_RENDERER_FPSCAP

            float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
            if(avgFPS > 2000000)
            {
                avgFPS = 0;
            }

            #ifdef FPS_OUTPUT
                std::cout << avgFPS << "\t";
            #endif // FPS_OUTPUT

            currentState->handleEvents(&gEvent);
            currentState->logic();
            change_state();

            SDL_RenderClear(gRenderer);
            currentState->render();

            SDL_RenderPresent(gRenderer);
            ++countedFrames;

            #ifdef SDL_RENDERER_FPSCAP
                int frameTicks = capTimer.getTicks();
                if(frameTicks < SCREEN_TICKS_PER_FRAME)
                {
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
                }
            #endif // SDL_RENDERER_FPSCAP
        }
    }

    game_Cleanup();
    return 0;
}

/*                                                                                                      Some art for you, my dear future-self.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#####';;;;;;;'''''++#@@@@@@@@@@@@@@@@#'##@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'+#'';;;;;;;;;;'+######@@@@@@@@@@@@@@#+'#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'+#+;;;;;;;''+##@@@@@@@###@@@@@##+####@#'+@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@@@@@@+'+#';;;;;;;+#@@@@@#@@@@@@@@@@@@#####+++#''#@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@@@@@@+'+#';;;;;;;+#@@@@@#@@@@@@@@@@@@#####+++#''#@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+@@@@@#+'#++'';;;'#@@@@@@@@@@@@@##@@@###+'++#+;++;+#@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+##@@@@##++#+#''';;#@@##''';;;''+@##@@@@#++;;;'#+'+'+#@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+++#@@@#+#++###'';;+@#;,..........:++##@@@@@+'''+#'++##@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'+###@@+##++@@#;;;'#+.................,,;#@@#++;+#''+##@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'+#+####@@++##''''#@,....................:#####''#+'##@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'+#+@++@@#+'##'++'@+......................+@#@@''#+'#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'##+#++@@@+'+#'##+@:......................'@###''+++@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'#+####@@##;+#'##+@,......................;@###'''+#@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+####@@+#'+#+##+#.......................,@###';+#@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+####@@+#'+#+##+#.......................,@###';+#@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+++#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+#+##@@+#++#+##++........................#@##;;#@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+++#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#####@+##@@+##+@@@@#+.......................,#@+++'@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@+@@@@@@@@@@@@@@@##@@@@@@@@@@@@@#+#+##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@####+##+@@@@'##+@@@@##,......................,@#+++#@@#+##++#@@@@
@@@@+++##@@@#++#@##`'++#@@@@@@+++++;.##@@@@++#@@@#+++#@@+++#@@@@+''##@@@@@++'##@@@@@@@@@@@@@@##@+@@@#+##+@@@@##,......................:@#@@@@@#+'#+++#@@@@
@@@@'  ``#@@+  '@:: `` :@@@@@+  :. . ..#@@@. ,@@@+'++#@#`  ;@@@,   ..#@@@:  ,@@@@@@@@@@@@@@@@@#@#@@@#+@#+'@@@@#;.......................#@@@@#+#++;:;;'+@@@
@@@@@;   .#@+  '#`` `.  #@@@@, `+. .   ,@@@. ,@++'''#@@#`  ;@@:  .   .#@:  ,@@@@@@@@@@@@@@@@@@@@#@@@#+##'.###+@+......................+#'@@####':,+###@@@@
@@@@@@;   .@#  ';    .` :@@++  :@. .    ,@@. ,#'''''@@@#`  ;@:  ,@;   ,:  ,@@@@@@@@@@@###@#@@@@@#@@@@#'#'.;@@:#@;.................,,,:'.:##''+:``:'''+#@@@
@@@@@@@::  ,+  ;`  ` `.  +@,, `#@. .     ,@. ,';;;'#####`  ;;  .#@@::    .#@@@@@@@@@@@@+++###@@@#@@@##:@;`,##;:@@:............,;;+#@@#+:'###;:....,;;;#@@@
@@@@@@@@@,  .` .   '` .` :+   :@@. .   .  :. :';;;''+''+:  .  `#@@@@@.  .#@@@@@@@@@@@@+##+++##@@@@@@##'#',.,,+,;@+.....:,..:;+#@@@####@@@@@@;;;::'+''+@@@@
@@@@@@@@@,  .` .   '` .` :+   :@@. .   .  :. :';;;''+''+:  .  `#@@@@@.  .#@@@@@@@@@@@@+##+++##@@@@@@##'#',.,,+,;@+.....:,..:;+#@@@####@@@@@@;;;::'+''+@@@@
@@@@@@@++` `;`   ,,@: `. `.  `#@@. .   +:    :+''+++''''.  :.  ;@@@++` `#@@@@@@@@@@@@@#++::;'+#@@@@@##@@#@+;;;:`;',:...##'####+++++++++++++@@@@#+'+''#@@@@
@@@@@@#``  +#    ++@#` .`    :@@@. .   +@,   ,######''''`  ;@.  ;@+`` `+@@@@@@@@@@@@@@#'':,..;#@#############+'''#;#'''####+++++++++'++++++@@@@@+++++#@@@@
@@@@@#`   '@+   ,@@@@:  .  ``#@@@. .   +@#.  ,@@@@#+'''+`  ;@#`  ;`   +@@@@@@@@@@@@@@@@##',`,+++++++++++++++++##+##++++++++''''''''''''++###+;+##++##@@@@@
@@@@#.   ;@@+   +@@@@#` `` ::@@@@` .   +@@#. ,@@@@+'''+#`  ;@@+`     '@@@@@@@@@@@@@@@@@##+';:+++#++'''''''''''++#++++++#+''''''''''''''++###'+',#@###@@@@@
@@@@',,;;@@@#,,;@@@@@@',:, ##@@@@:,:,,,#@@@#`,@@@#'+++@#:,,'@@@+,,,''@@@@@@@@@@@@@@@@@@@@#++'#+++'''''''''''''';+++##++#+'''''''''''''##+##@'##'#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@+;@@@@@@@@@@@@@@@@@+;@@@+++'#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+###+#++++'''''''''''++#';##+###++''''''+##@#+::#+#@+'@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@++'###@@@@@@@@@@#@@@##########@@####@@@@@@##++####@@@@@#'''''''''#+':.''##@@@@#####@@@@@#+,,++#+,;@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+++++@@@@@@@@@@#+#@+''''+++''+##+''+@@@@@@@##+####@@@@@@#+++++####+,..,,##@@@@@@@@@@@@@@#+..+#@:,@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++###@@@@@@@@@#++#+'###'''+#+'''+##@@@@@@@#+##@##+##@@@@@@@@@@@@@#+.....+#@@@@@@@@@@@@@##;..#@@''@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++###@@@@@@@@@#++#+'###'''+#+'''+##@@@@@@@#+##@##+##@@@@@@@@@@@@@#+.....+#@@@@@@@@@@@@@##;..#@@''@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+##+#@@@@@@@@@@#'+#+'###'''+#+'''+#@@@@@@@@@@@@@@#+##@@@@@@@@@@@@@#;.....;##@@@@@@@@@@@@##:..#@#;@@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#####@@@@@@@@@@@#'#@+''''''''''''''''#@@@@@@@@@@@@@###@@@@@@@@@@@@#+:......+##@@@@@@@@@@#++...+@++@@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@@@@@@@@@@@+'#@####+''+#+''''#+'+@@@@@@@@@@@@@#++#@@@@@@@@@@##+;......,###@@@@@@@@###:...+@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##@@@@@@@@@@@@++@@@@##+''+#+''++##'#@@@@@@@@@@@@@@@@+#@@@@@@@@###++......,:++########+',....+@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@@@@@@@@@@@@++@@++++'+''+'+'''+'+#@@@@@@@@@@@@@@################;......';.,:;''''';,......+@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##@@@@@@@@@@@@@@##@#++++#@#++#@###++#@@@@@@@@@@@@@@@++:++++##+++'''',.......;:................#@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##....,,,,...;.,.........:..............,,@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,..........;:,.......,................;;@@@@@@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;..........:#@+,..,,+@+,..............++@@@@@@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;..........:#@+,..,,+@+,..............++@@@@@@@@##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+...........,;;+';;;::,..............,++@@@@@@@@###@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##@@@@@#,.............,:,...................,++@@@@@@@@###@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@@@@@:............,.......,..............,++@@@@@@@@###@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#@@@@@;..........;'';..,,;++':,......:....:##@@@@@@@#@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++@@@@@+......,,::+;,,....,::;';:....,;....,##@@@@@@#+##@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++@@@@@#,..,.:::::::;;::;;';,..:#;...::....:@@@@@@@@##++@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+++#@@@@@;,.;,...,'+#@@@@@@@@#+''#;::.:,...,#@@@@@@@@@#++##@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++'#@@@@@':.;:.......,:;;;;;:,,.......'....+@@@@@@@@@@+++++#@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+#@@@@@@#;.:+.......................:;...'@###@@@@##@+##+#@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+#@@@@@@#;.:+.......................:;...'@###@@@@##@+##+#@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+@@@@@@@@+..+,....,':......,'+,.....:,..:@@:##@@@@+#@#++##@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:`;;....,+@#++++##@+,........,#@+.++@@@#+#@####@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#,.;.....;+@@###++##+........+++:.''@@@#++@####@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#@+........:;,....,,;':......'#::..::@@#+++@###@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'@@'........................;#,,...,,@@#+'+@###@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##@@@@@@@:+@@+......................:#,.:...,,##++'#+#++##@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+@@@@@@':+#@++..................,,#:.,,.....#+#'+#++###+#@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++@@@@@@+.;'@@@'................,##;..;....;;+++'#+++++'+++#
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++#@@@@@@,,''##@;..............,#;;..;:...,##+#;+#++''''''++
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++#@@@@@@,,''##@;..............,#;;..;:...,##+#;+#++''''''++
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##########+++#@@@@@@:.:'''+#'............,#'....;,...'++#;'#+#++++'''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+''''''+++#'+#@@@@@+;..:++;:++';::::::;'+@'....:'...;+##':##+#++++'''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@####+'''''''''''#@'++###@@+;...;;'..,;'++++''+++:.....':..,+###:'#+#@++++'''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#####+++'''''''''''++@@+++####@#;.....':..................;'...++#;;:##+#@++++'''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+''''''''''''''''''''+##++++++##@@',....,':...............,,#;..;+#+,,+@++@#++##+''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'''++++'++++++++''''''''++#+++++#+#@+,.....:+:..............;;+..,+##:::##'#@#++###+'''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+''''++++++'''''+##++'''''#@@+#+++#+@@#:......:;..............;;:..;+#+,''#+;#@+'#####+''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+'''''''''''+++++''+#+##++'##@@+#+##+##@@;.......,::.................'##:,###;'##++#@@+##+'
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+''++++''''''''''''#@#++++++++@@@@###+++++#@+........,',,..............,+@+.'@@#:+@#'#@@@@###+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+''++++''''''''''''#@#++++++++@@@@###+++++#@+........,',,..............,+@+.'@@#:+@#'#@@@@###+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+''++'''''+#''''''''#@@@#+++''#@@@@###++#+'#@#.........::,.............,;#@;.+@@':#@++@@@@@@#++
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++'''''''+##+'''''''''@@@@@@##++@@@@@#+@###++'##,.......................,;+@#,:#@@:;@#'#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+++''''''#+'''++'''''''@@@@@@#+##@@@@@@+@##+#+''#+,....................;;###@+`'@##,'@#+#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++''''''''''++##+'''''''@@@@@@#+@@@@@@@@+#'''##+;'++'':............,,:'+##++@#;`+@++`+#+#@@@@@@@@#+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+''''''''+##+++#+'''''++@@@@@@++@@@@@@@@#+'':+#+';;'++#+';;;;;'''++###+';;'#@#.,#@;;`+#+@@@@@@@@@#+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@++'''''''##++####+'''''++@@@@@#+#@@@@@@@@@+##:;#+';;;;;;'''++++++++''';;;''+#@+`;@#::`++#@@@@@@@@@+'
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+'''''''+++####++++#+''##@@@@@#+#@@@@@@@@@+##;,+#+';;;;;'';;;;;;;;;''';;;''+@@;`'@#...++#@@@@@@@@@''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+'''''''+###+###@@#+'''##@@@@#+#@@@@@@@@@@#'''.'#++';;;';;;;;;;;;;;;;;;;;''+@@:`+@#``:##@@@@@@@@#'''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+''''''+#####@@@@#'''''++#@@@++@@@@@@@@@@@#'''.,##+#+''';;;;;;;;;;;;;;;''++#@#..#@+..'#@@@@@@@@@+'''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+''''''+#####@@@@#'''''++#@@@++@@@@@@@@@@@#'''.,##+#+''';;;;;;;;;;;;;;;''++#@#..#@+..'#@@@@@@@@@+'''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+'+''''+#####@@@@#+#+''++@@@#'#@@@@@@@@@@@@++':`+#+#++++++++''''''''+++++++#@+`.#@+,,+#@@@@@@##+''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'+++''####@@@@@@@++'''++@@@++#@@@@@@@@@@@@##''`'###+'''+++##########++'+++#@' ,#@+''#@@@@@@@++'''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+'+##'+##@@@@@@@@@+++''##@@#+@@@@@@@@@@@@@@@@+# :##+';;''+++''''''''++++'++@@: ,##+++@@@@@@#+'''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@++###++##@@@@@@@@@##+''##@@@@@@@@@@@@@@@@@@@@++`,+#++;;;''''''''''''''';'++@@, :@#'##@@@@@@+''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';+#++##@@@@@@@@@@@@#+'+@@@@@@@@@@@@@@@@@@@@@@#+.`+#+#;;;;;;;;;;;;;;;;;;'+##@#. ;#++@@@@@@#+'''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#`,+#++@@@@@@@@@@@@@@@#+#@@@@@@@@@@@@@@@@@@@@@@@#;`+#+#'';;;;;;;;;;;;;;;;+###@+` ;#'#@@@@@@+''''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@' .+#++@@@@@@@@@@@@@@@@#@@@@@@@@@@@@@@@@@@@@@@@@@+:'#+#'';;;;;;;;;;;;;;''++##@+``+'#@@@@@@@#+'''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@, `'####@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#++#+#'';;;;;;;;;;;;;;'''+##@' .#+#@@@@@@@##'''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.  :####@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+#;;;;;;;;;;;;;;';'+'+@@@; ;+@@@@@@@@@#+'''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.  :####@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#+#;;;;;;;;;;;;;;';'+'+@@@; ;+@@@@@@@@@#+'''''''''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@``:`+###@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+++;;;;;;;;;;;;;;;'+''#@@@;,+#@@@@@@@@@@@#+'''''+#
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@, '.:###@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+;;';;;;;;;;;;;;+';'#@@#++#@@@@@@@@@@@@+'''''+@#
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;.'+.+++@@@@@@@@@@@@@@@#@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+#''';;;;;;;;;;;'+''+#@@#'#@@@@@@@@@@@@#'''''##@+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#`'@;::+#@@@@@@@@@@@@@##@@@@@@@@@@@@@@@@@@@@@@@@@@@@#'++++';;;;;;;;';+';'+###'#@@@@@@@@@@@@+'''++#@@#'
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@: +@::+#@@@@@@@@@@@@@##@@@@@@@@@@@@@@@@@@@@@@@@@@@@++###++';;;;;;;;'+';'+#''+@@@@@@@@@@@@#'''+@@@@@+'
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'  '++'#@@##@@@@@@#@@+#@@@@@@@@@@@@@@@@@@@@@@@@@@@#'#@@@@#+';;;;;;'++';'+'##@@@@@@@@@@##+''+@@@@@@@''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'   ::++@@#+#@@@@@+@#+@@@@@@@@@@@@@@@@@@@@@@@@@@@@++@@@@@@##+'';;;+++''++#@@@@@@@@@@@@#+'+##@@@@@@#''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,     :#@@@+#@@@@##@##@@@@@@@@@@@@@@@@@@@@@@@@@@@@++@@@@@@@@#+';''+#';'+#@@@@@@@@@@#++++#@@@@@@@@#+''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:      `'@@@+####@+#@+#@@@@@@@@@@@@@@@@@@@@@@@@@@@#'#@@###@@@#+';''##'+++####@@@@@@######@@@@@@@@@#+''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:      `'@@@+####@+#@+#@@@@@@@@@@@@@@@@@@@@@@@@@@@#'#@@###@@@#+';''##'+++####@@@@@@######@@@@@@@@@#+''
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'        ;###+#@++##@#+@@@@@@@@@#+@@@@@@@@@@@@@@@@@#+#@+''++@@##+'++++#+++####@@@@@@@@@@@@@@@@@@@@@#''+
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.        :#+#+#@++##@+#@@@@@@@@@++@@@@@@@@@@@@@@@@@++##+++#'##+++#####++'+#@@@@@@#@@@@@@@@@@@@@@@@@+''#
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.        .+####@+++@#+#@@@@@@@@#++@@@@@@@@@@@@@@@@#+#@+'@@';##'+####+''''+#@###@##@@@@@@@@@@@@@@@@@+'#@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:        `'####@+++@++#@@@@@@@@#'#@@@@@@@@@@@@@@@@#+#@#+'''+@++#+''''''''+#####@##@@@@@@@@@@@@@@@@#'+@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#;+         :#@##@##@#'#@@@@@@@@#+'#@@@@@@@@@@@@@@@@++@@@#+++#@+++'''''''''+#@@@@@##@@@@@@@@@@@@@@##++@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##. ;:        .+@##@@@@'+@@@@@@@@@+++#@@@@@@@@@@@@@@@@++@@@@@@@@@'#+''''+'+''+#@@@@#+@@@@@@@@@@@@@@@++'#@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;;  `+`        ;@@+@@@#'#@@@@@@@@@+++#@@@@@@@@@@@@@@@#+#@@@@@@@@#+##''''.`,+'+##@@#++@@@@@@@@@@@@@##+++@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@..   ::        ,#@#@@@#+#@@@@@@@@#+++#@@@@@@@@@@@@@@@#+#@@@@@@@@#+##++;;   ;'++#@@#+#@@@@@@@@@@@@@#+++#@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#``  .,'        `'@##@@#@@@@@@@@@@#+++@@@@@@@@@@@@@@@#++@@@@@@@@@#+##++''   ;''+@@@#'#@@@@@@@@@@@#++#@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#``  .,'        `'@##@@#@@@@@@@@@@#+++@@@@@@@@@@@@@@@#++@@@@@@@@@#+##++''   ;''+@@@#'#@@@@@@@@@@@#++#@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#``  `'+,        :##+@@@@@@@@@@@@@#++#@@@@@@@@@@@@@@@#'#@@@@@@@@@++##+'++;.:+''#@@@+'#@@@@@@@@@@#++##@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#``   `'+        `+#+##@@@@@@@@@@@+++#@@@@@@@@@@@@@@@#'#@@@@@@@@#'+##+'''+++''+@@##'+#@@@@@@@@@#++#@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@``     ':     .:'+#+++@@@@@@@@@@@++'#@@@@@@@@@@@@@@@++@@@@@@@@@#'###+'''''''++@@##'+#@@@@@@@####@@@@@@@@@:.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@''..      ;.   :+,:#@#''#@@@@##@@@@++'#@@@@@@@@@@@@@@@+#@@@@@@@@@+'#+#+'''''''++#@##+###++';''@@@@@@@@@@@##..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@````       ;.. +;.`:@@+++@@@@##@@@@++'#@@@@@@@@@@@@@##+#@@@@@@@@@'+#+#+''''''+++#@##,.``      '@@@@@@@@@@#+..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#`          `;;.;:,:,;###+@@@@##@@@#+++#@@@@@@@@@@@@@###@@@@@@@@@#'+#+#+''''''+###@##'          '@@@@@@@@@++..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@`           ``+;:,':.:###@@@@##@@@#+++#@@@@@@@@@@@@@###@@@@@@@@@+'+#+#+'''''++###@##@'         `+@@@@@@@@++..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.         .;''#@'.....,,#@@@@##@@@#+++#@@@@@@@@@@@@####@@@@@@@##'''++#+'''''##@#####@@;         `+@@@@@@@+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;        .#:..,+@,...,;;:#@@@+#@@@#+++#@@@@@@@@@@@@####@@@@@@@##'''++#+''''+##@#####@@@;         .#@@@@@@+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;        .#:..,+@,...,;;:#@@@+#@@@#+++#@@@@@@@@@@@@####@@@@@@@##'''++#+''''+##@#####@@@;         .#@@@@@@+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#`       ;@,...:#+...:;;''@@@++@@@#++'#@@@@@@@@@@@#+++@@@@@@@@++;++###+''''###@#####@@@@;         .#@@@@#+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@':@:       ;',,,.;;#'..,,,.,#@@++@@@#+''#@@@@@@@@@@@++##@@@@@@@#'''++#+##'''+#+#@###@@@@@@@:         ,@@@@#+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#+;',       .+.::;,.:@;......:@@++@@@#+++#@@@@@@@@@@#++##@@@@@@@+'''+##'##'''####@###@@@@@@@@:         :@@@#+@..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,           ':......;@:......;@#'@@@#+++##@@@@@@@@@#++##@@@@@@#+'''+++'##'''#+#@@@##@@@@@@@@@:         :@@#+#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;+:           .+....,':'#,......'#+#@@#+++##@@@@@@@@@#+#@@@@@@@@#''''++''+#+++###@@@##@@@@@@@@@@,,        ;;##@..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#``'.           '''...,':++....,;;#++@@#+++#@@@@@@@@@@#+#@@@@@@@@+'''+++''+#+##@#@@@@++@@@@@@@@@@@@,         '@#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#  .+`          .##:....,,+;;.:::;;#+##@+++##@@@@@@@@@++@@@@@@@@#';''+++;'+##@@##@@@@++@@@@@@@@@@@@@.         +@`.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;,`,'        `'+###:......++:,:;,.+'##@#++##@@@@@@@@#+#@@@@@@@@#';''++';++##@@##@@@@+#@@@@@@@@@@@@@#.        `#..
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+;'+#+@'``     '+,;;#+:.....,,',.:::;+##@#++##@@@@@@@@#+#@@@@@@@@+';''++''++#+@@##@@@@+#@@@@@@@@@@@@@@;        ,#`.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##+;'+#+@'``     '+,;;#+:.....,,',.:::;+##@#++##@@@@@@@@#+#@@@@@@@@+';''++''++#+@@##@@@@+#@@@@@@@@@@@@@@;        ,#`.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#:``    .:''''':.  #:...:##,......,',,:::#####++#+##@@@@@@++#@@@@@@@#'''''+';'+'#+###@@@@@+#@@@@@@@@@@@@@'        .@#`.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#.             ,'+',++:..;:##....,,,,',+########+#++#@@@@@#++@@@@@@@@#';;'''';'+'#+##@@@@@@+#@@@@@@@@@@@@#`      ``+@#`.
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#`           `,,`  ``:';;;:..#'.....,.:#@#@#++######+###@@@#'#@@@@@@@@+';''''';++'++##@@@@@@##@@@@@@@@@@##.      `+++@#`.
*/
