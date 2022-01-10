#define FPS_LIMIT 60

#include <iostream>
#include <fstream>
#include <thread>

#include "MinGL2/include/mingl/mingl.h"
#include "MinGL2/include/mingl/gui/sprite.h"
#include "MinGL2/include/mingl/graphics/vec2d.h"
#include "MinGL2/include/mingl/shape/rectangle.h"
#include "MinGL2/include/mingl/gui/text.h"
#include "MinGL2/include/mingl/shape/line.h"
#include "MinGL2/include/mingl/audio/audioengine.h"
#include "MinGL2/include/mingl/gui/glut_font.h"
#include "MinGL2/include/mingl/transition/transition_engine.h"

#include "bgtext.h"
#include "menu.h"


using namespace std;
using namespace nsGraphics;
using namespace nsGui;
using namespace chrono;
using namespace nsAudio;



//// Selects the theme and moves the cursor
void selectTheme(MinGL &window, Sprite &image)
{
        if (window.isPressed({'a', false})) {
            Vec2D position = image.getPosition();
            int arrowX = 188;
            int arrowY = position.getY();
            Vec2D positionF (arrowX, arrowY);
            image.setPosition(positionF);
        }
        else if (window.isPressed({'z', false})) {
            Vec2D position = image.getPosition();
            int arrowX = 435;
            int arrowY = position.getY();
            Vec2D positionF (arrowX, arrowY);
            image.setPosition(positionF);
        }
}

//Change theme to theme select
unsigned chooseTheme(MinGL &window, Sprite &image, unsigned &baseTheme)
{

    if (window.isPressed({13, false})) {
        Vec2D position = image.getPosition();
        int arrowX = position.getX();
        if (arrowX == 188) {
            return 1;
        }
        else {
            return 0;
        }
    }
    return baseTheme;
}

//Change the place of the cursor in the menu

void menu(MinGL &window, nsGui::Sprite &image) {
    if (window.isPressed({'s', false})) {
        window.resetKey({'s', false});
        Vec2D position = image.getPosition();
        if (position.getY()==575) {
            int mugX = position.getX();
            int mugY = 305;
            Vec2D positionF (mugX, mugY);
            image.setPosition(positionF);
        }
        else {
            int mugX = position.getX();
            int mugY = position.getY()+90;
            Vec2D positionF (mugX, mugY);
            image.setPosition(positionF);
        }

    }
    if (window.isPressed({'z', false})) {
        window.resetKey({'z', false});
        Vec2D position = image.getPosition();
        if (position.getY()==305) {
            int mugX = position.getX();
            int mugY = 575;
            Vec2D positionF (mugX, mugY);
            image.setPosition(positionF);
        }
        else {
            int mugX = position.getX();
            int mugY = position.getY()-90;
            Vec2D positionF (mugX, mugY);
            image.setPosition(positionF);
        }

    }
}

// Choice of menu
unsigned entrerMenu(MinGL &window, nsGui::Sprite &image)
{
    if (window.isPressed({13, false})) {
        window.resetKey({13, false});
            Vec2D position = image.getPosition();
            int mugY = position.getY();
            if (mugY == 575) {
                exit(0); // exit
            }
            else if (mugY == 485) {
                return 3; // menu 3
            }
            else if (mugY == 395) {
                return 2; // menu 2
            }else if(mugY == 305){
                return 1; // menu 1
            }
    }
    return 0;
}

//Applies the selected theme
void choixLightDark (MinGL &window, unsigned &choixpsgom,Sprite &themelight, Sprite &themedark){
    if (choixpsgom == 0) {
        window << themelight;
    }
    else {
        window << themedark;
    }
}

bool isBetter (const playersStruct & joueur1, const playersStruct & joueur2){ // Bool used to sort player
    return joueur1.point >= joueur2.point;
}

void showScore(MinGL & window) //Function which shows the scoreboard
{
    ifstream data_file("score.txt");
    vector<playersStruct> playerScore;
    playersStruct playerStruc;
    while (data_file >> playerStruc.player >> playerStruc.point) { //Place every elements of the file in the structure
        playerScore.emplace_back(playerStruc);
    }
    sort (playerScore.begin (), playerScore.end(), isBetter); // Sort all players by their score
    vector<string> stringScore;
    if (playerScore.size()>10) { //Make that the structure only retains the 10 best players
        playerScore.resize(10);
    }
    for (size_t i = 0; i<playerScore.size(); ++i) { //Place every score and player on the screen
        stringScore.push_back(to_string(playerScore[i].point)); //Transform the score of the players into String so i can use nsGui::Text to show them
        window << nsGui::Text(Vec2D(190, 294+(30*i)), playerScore[i].player, KWhite, nsGui::GlutFont::BITMAP_9_BY_15)
               << nsGui::Text(Vec2D(525, 294+(30*i)), stringScore[i], KWhite, nsGui::GlutFont::BITMAP_9_BY_15);
    }
}
