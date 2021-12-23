#define FPS_LIMIT 60

#include <iostream>
#include <thread>
#include <chrono>
#include<unistd.h>

//CHANGER LES CHEMINS !!!

#include "MinGL/include/mingl/gui/text.h"

#include "MinGL/include/mingl/shape/line.h"

#include "MinGL/include/mingl/shape/rectangle.h"

#include "MinGL/include/mingl/mingl.h"

#include "MinGL/include/mingl/gui/sprite.h"

#include <MinGL/include/mingl/graphics/vec2d.h>

#include "MinGL/include/mingl/shape/rectangle.h"

//CHANGER LES CHEMINS !!

using namespace std;
using namespace nsGraphics;
using namespace nsGui;
using namespace chrono;

Vec2D misPos;
Vec2D torPos;

struct mugStruct {
    vector<Sprite> vecMug;
    unsigned index;
};

struct enemy {
    vector<Sprite> vecSprite;
    int rightOrLeft;
    vector<bool> state;
    void update(MinGL &window){
        for (unsigned i = 0; i < vecSprite.size(); ++i) {
            if (state[i]){
                window << vecSprite[i];
            }
        }
    }
};

// Si on appuie sur une touche, le mug bouge
void keyboard(MinGL &window, Sprite &mug)
{
    if (window.isPressed({'q', false}) && mug.getPosition().getX() > 50 ) {
        Vec2D position = mug.getPosition();
        int mugX = position.getX();
        int mugY = position.getY();
        Vec2D positionF (mugX-5, mugY);
        mug.setPosition(positionF);
    }
    if (window.isPressed({'d', false}) && mug.getPosition().getX() < (600-64+50)) {
        Vec2D position = mug.getPosition();
        int mugX = position.getX();
        int mugY = position.getY();
        Vec2D positionF (mugX+5, mugY);
        mug.setPosition(positionF);
    }
}
//Verify if an ennmy is still alive
bool allDead(const enemy & PPs) {
    for (size_t i = 0; i < PPs.state.size(); ++i) {
        //If the ennemy is alive
        if (PPs.state[i] == true) {
            return false;
        }
    }
    //There is no enemy alive anymore
    return true;
}

bool isTouching (const Vec2D firstCorner, const Vec2D secondCorner, const Vec2D test){
    return ((test.getX() <= secondCorner.getX() && test.getY() <= secondCorner.getY()) && (test.getX() >= firstCorner.getX() && test.getY() >= firstCorner.getY()));
}

bool colision(const Vec2D misPos, enemy &vecSprite){
    for (size_t i = 0; i < vecSprite.vecSprite.size(); ++i) {
        Vec2D a = vecSprite.vecSprite[i].getPosition();
        int bX = vecSprite.vecSprite[i].getPosition().getX()+55;
        int bY = vecSprite.vecSprite[i].getPosition().getY()+50;
        Vec2D b {bX,bY};
        if(isTouching(a,b,misPos) && (vecSprite.state[i] == true)) {
            vecSprite.state[i] = false;
            return true;
        }
    }
    return false;
}


bool missile(MinGL &window, Sprite &mug, enemy &IPPs, enemy &KPPs, enemy &JPPs, unsigned &playerLifeUnsigned, bool &firstShootM, bool &isPressed){
    if (window.isPressed({'x', false})){
        isPressed = true;
    }
    if (isPressed == true){
        if(firstShootM == true){//Si première apparition/clique
            Vec2D position = mug.getPosition();
            int mugX = position.getX();
            int mugY = position.getY();
            misPos.setX(mugX + 16);
            misPos.setY(mugY);
        }//Test si il y a colision avec la fenètre ou avec un enemi
        if (misPos.getY() <= 150){
            firstShootM = true;
            return isPressed = false;
        }
            else if (colision(misPos, IPPs) || colision(misPos, KPPs) || colision(misPos, JPPs)){
                ++playerLifeUnsigned;
                firstShootM = true;
                return isPressed = false;
            }
        firstShootM = false;
        misPos.setY(misPos.getY() - 16);
        return isPressed;
    }
    return false;
}

bool torpedo(mugStruct &mug, enemy &IPPs, bool &firstShootT){

    srand (time(NULL));
    int n = rand() % IPPs.vecSprite.size();

    if((firstShootT == true) && (IPPs.state[n] == true)){//Si première apparition et qu'il n'est pas détruit
        Vec2D position = IPPs.vecSprite[n].getPosition();
        int IPPsX = position.getX();
        int IPPsY = position.getY();
        torPos.setX(IPPsX + 16);
        torPos.setY(IPPsY);
    }//Test si il y a colision avec la fenètre ou le joueur

    Vec2D pos = mug.vecMug[mug.index].getPosition();
    int X = pos.getX() + 50;
    int Y = pos.getY() + 55;
    Vec2D pos2 = {X,Y};

    if(torPos.getY() >= 696){
        firstShootT = true;
        return false;
    }else if(isTouching(pos,pos2,torPos)){
        firstShootT = true;
        if (mug.index < 4){
            int posX = mug.vecMug[mug.index].getPosition().getX();
            int posY = mug.vecMug[mug.index].getPosition().getY();
            mug.index += 1;
            mug.vecMug[mug.index].setPosition({posX,posY});
        }
        else{
            exit(0);
        }

        return false;
    }
    firstShootT = false;
    torPos.setY(torPos.getY() + 16);
    return true;
}

void move(Sprite &position, const int &x, const int &y) {
    position.setPosition(Vec2D(position.getPosition().getX() + x, position.getPosition().getY() + y));
}

void moveOpen(enemy & open) {
    if (open.vecSprite[0].getPosition().getX() < (600 - 470) || open.vecSprite[open.vecSprite.size() - 1].getPosition().getX() > 50) {
        for(Sprite & letter : open.vecSprite) {
            move(letter, open.rightOrLeft*5, 0);
        }
    }
    //Marche
    if (open.vecSprite[open.vecSprite.size() - 1].getPosition().getX() > (600 - 99) && open.rightOrLeft == 1) {
        open.rightOrLeft = -1;
        for (Sprite & letter : open.vecSprite) {
            move(letter, 0, 10);
        }
    }
    else if(open.vecSprite[0].getPosition().getX() <  (600 - 470) && open.rightOrLeft == -1) {
        open.rightOrLeft = 1;
        for (Sprite & letter : open.vecSprite) {
            move(letter, 0, 10);
        }
    }
    if (open.vecSprite[0].getPosition().getY() > 600) exit(0);
}

void moveVecSprite(enemy &vecSprite){
    // Si les sprites au extrémité ne touches pas les bords, bouger tout les sprites en même temps
    if (vecSprite.vecSprite[0].getPosition().getX() < (600-64+50) ||
        vecSprite.vecSprite[vecSprite.vecSprite.size() - 1].getPosition().getX() > 0+50){
        for(Sprite &sprite : vecSprite.vecSprite){
            move(sprite, vecSprite.rightOrLeft *5, 0);
        }
    }
    // Si les sprites au extrémité touches les bords, changer de direction et dessendre les sprites de 10 pixels
    if(vecSprite.vecSprite[vecSprite.vecSprite.size() - 1].getPosition().getX() > (600-64+50) && vecSprite.rightOrLeft == 1){
        vecSprite.rightOrLeft = -1;
        for(Sprite &sprite : vecSprite.vecSprite){
            move(sprite, 0, 10);
        }
    }else if(vecSprite.vecSprite[0].getPosition().getX() < 0+50 && vecSprite.rightOrLeft == -1){
        vecSprite.rightOrLeft = 1;
        for(Sprite &sprite : vecSprite.vecSprite){
            move(sprite, 0, 10);
        }
    }
    if(vecSprite.vecSprite[0].getPosition().getY()>(600))exit(0);
}

void generateVecSprite(enemy &IPPs, const int posY, const string pathSprite){
    // liste de sprite
    for (size_t i = 0; i < 5; ++i) {
        Vec2D ipp;
        ipp.setX(50+124+72*i);
        ipp.setY(138+posY);
        Sprite sprite(pathSprite, ipp);
        IPPs.vecSprite.push_back(sprite);
        IPPs.state.push_back(true);
    }
}

void generateOPEN(enemy & open, const int posY) {
    //Generate letters one by one
    Vec2D firstO;
    firstO.setX(280);
    firstO.setY(138+posY);

    Vec2D p;
    p.setX(firstO.getX()+33);
    p.setY(138+posY);

    Vec2D e;
    e.setX(p.getX()+33);
    e.setY(138+posY);

    Vec2D n;
    n.setX(e.getX()+33);
    n.setY(138 + posY);

    Sprite spriteO("spritesi2/first-O.si2", firstO);
    Sprite spriteP("spritesi2/P.si2", p);
    Sprite spriteE("spritesi2/E.si2", e);
    Sprite spriteN("spritesi2/N.si2", n);

    //Gathers very sprite in a vector so we can add them to OPEN
    vector<Sprite> vecOPEN = {spriteO, spriteP, spriteP, spriteE, spriteN};
    for (size_t i = 0; i < vecOPEN.size(); ++i) {
        open.vecSprite.push_back(vecOPEN[i]);
        open.state.push_back(true);
    }
}

void generateCLASSROOM(enemy & classroom, const int posY) {
    //Generate all the letters one by one

    //Genretate C
    Vec2D c;
    c.setX(200);
    c.setY(105+posY);

    //Generate L
    Vec2D l;
    l.setX(c.getX()+33);
    //We use the .getX() of the previous letter
    l.setY(105+posY);

    //Generate A
    Vec2D a;
    a.setX(l.getX()+33);
    a.setY(105+posY);

    //Generate S
    Vec2D s;
    s.setX(a.getX()+33);
    s.setY(105+posY);

    //Generate the second S
    Vec2D sTwo;
    sTwo.setX(s.getX()+33);
    sTwo.setY(105+posY);

    //Generate R
    Vec2D r;
    r.setX(sTwo.getX()+33);
    r.setY(105+posY);

    //Generate O
    Vec2D o;
    o.setX(r.getX()+33);
    o.setY(105+posY);

    //Generate the second O
    Vec2D oTwo;
    oTwo.setX(o.getX()+33);
    oTwo.setY(105+posY);

    //Generate M
    Vec2D m;
    m.setX(oTwo.getX()+33);
    m.setY(105+posY);

    //Create sprites with the Vec2D
    Sprite spriteC("spritesi2/C.si2", c);
    Sprite spriteL("spritesi2/L.si2", l);
    Sprite spriteA("spritesi2/A.si2", a);
    Sprite spriteS("spritesi2/S.si2", s);
    Sprite spriteSTwo("spritesi2/S.si2", sTwo);
    Sprite spriteR("spritesi2/R.si2", r);
    Sprite spriteO("spritesi2/O.si2", o);
    Sprite spriteOTwo("spritesi2/O.si2", oTwo);
    Sprite spriteM("spritesi2/M.si2", m);

    //Gather all the sprites in a vector
    vector<Sprite> vecClassroom = {spriteC, spriteL, spriteA, spriteS, spriteSTwo, spriteR, spriteO, spriteOTwo, spriteM};

    //Adding all the sprite in the CLASSROOM vecSprite
    for (size_t i = 0; i < vecClassroom.size(); ++i) {
        classroom.vecSprite.push_back(vecClassroom[i]);
        classroom.state.push_back(true);
    }

}

void generateVecMug(mugStruct &mug){
    Sprite mug3("spritesi2/mug-full-vie.si2");
    Sprite mug2("spritesi2/mug-2-vies.si2");
    Sprite mug1("spritesi2/mug-1-vies.si2");
    Sprite mug0("spritesi2/mug-is-finito.si2");
    mug.vecMug.push_back(mug3);
    mug.vecMug.push_back(mug2);
    mug.vecMug.push_back(mug1);
    mug.vecMug.push_back(mug0);
}

void win(MinGL &window){
    window.clearScreen();
    exit(0);
}


int main()
{

    Sprite back("spritesi2/back.si2", Vec2D(0, 0));

    enemy open;
    open.rightOrLeft = 1;
    generateOPEN(open, 50);

    enemy classroom;
    classroom.rightOrLeft = 1;
    generateCLASSROOM(classroom, 125);

    enemy IPPs;
    IPPs.rightOrLeft = 1;
    generateVecSprite(IPPs, 50, "spritesi2/i++.si2");

    enemy JPPs;
    JPPs.rightOrLeft = 1;
    generateVecSprite(JPPs, 100, "spritesi2/j++.si2");

    enemy KPPs;
    KPPs.rightOrLeft = 1;
    generateVecSprite(KPPs, 150, "spritesi2/k++.si2");

    mugStruct mug;
    generateVecMug(mug);
    mug.index = 0;
    mug.vecMug[mug.index].setPosition(Vec2D(50+284, 138+500));

    // Initialise le système
    MinGL window("CasaliShooter", Vec2D(700, 1000), Vec2D(128, 128), KBlack);
    window.initGlut();
    window.initGraphic();


    // Variable qui tient le temps de frame
    chrono::microseconds frameTime = chrono::microseconds::zero();

    unsigned playerLifeUnsigned = 0;
    bool firstShootM = true;
    bool isPressed = false;
    bool firstShootT = true;


    // On fait tourner la boucle tant que la fenêtre est ouverte
    while (window.isOpen()){
        // Récupère l'heure actuelle
        chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

        // On efface la fenêtre
        window.clearScreen();

        // On fait tourner les procédures
        window << back;
        window << mug.vecMug[mug.index];

        IPPs.update(window);
        JPPs.update(window);
        KPPs.update(window);

        keyboard(window, mug.vecMug[mug.index]);

        moveVecSprite(IPPs);
        moveVecSprite(KPPs);
        moveVecSprite(JPPs);

        isPressed = missile(window, mug.vecMug[mug.index], IPPs, KPPs, JPPs, playerLifeUnsigned, firstShootM, isPressed);
        if(isPressed == true) window << nsShape::Rectangle(misPos, misPos + Vec2D(2, 10), KCyan);
        string playerLifeString = to_string(playerLifeUnsigned);
        //Points generator
        window << Text(nsGraphics::Vec2D(60, 160), "Pts:", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);
        window << Text(nsGraphics::Vec2D(100, 160), playerLifeString, nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);

        if (torpedo(mug, IPPs, firstShootT)) window << nsShape::Rectangle(torPos, torPos + Vec2D(5, 10), KGreen);

        //if (playerLifeUnsigned == 15) win(window);


        // On finit la frame en cours
        window.finishFrame();

        // On vide la queue d'évènements
        window.getEventManager().clearEvents();

        // On attend un peu pour limiter le framerate et soulager le CPU
        this_thread::sleep_for(chrono::milliseconds(3000 / FPS_LIMIT) - chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start));

        // On récupère le temps de frame
        frameTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start);

        //Verify if enemies are still alive
        if (allDead(IPPs) && allDead(JPPs) && allDead(KPPs)) {
            while (!allDead(open)) {
                // Récupère l'heure actuelle
                chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

                window.clearScreen();

                //On fait tourner les procédures

                window << back;
                window << mug.vecMug[mug.index];

                keyboard(window, mug.vecMug[mug.index]);


                open.update(window);
                classroom.update(window);

                //OPEN and CLASSROOM movements

                moveVecSprite(classroom);
                moveOpen(open);

                isPressed = missile(window, mug.vecMug[mug.index], IPPs, KPPs, JPPs, playerLifeUnsigned, firstShootM, isPressed);
                if(isPressed == true) window << nsShape::Rectangle(misPos, misPos + Vec2D(2, 10), KCyan);
                string playerLifeString = to_string(playerLifeUnsigned);
                //Points generator
                window << Text(nsGraphics::Vec2D(60, 160), "Pts:", nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);
                window << Text(nsGraphics::Vec2D(100, 160), playerLifeString, nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);

                if (torpedo(mug, open, firstShootT)) window << nsShape::Rectangle(torPos, torPos + Vec2D(5, 10), KGreen);


                window.finishFrame();

                window.getEventManager().clearEvents();

                // On attend un peu pour limiter le framerate et soulager le CPU
                this_thread::sleep_for(chrono::milliseconds(3000 / FPS_LIMIT) - chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start));

                // On récupère le temps de frame
                frameTime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start);

            }
        }
    }
    return 0;
}
