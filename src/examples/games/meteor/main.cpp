#include <gamebase/Gamebase.h>

using namespace std;
using namespace gamebase;

class MyApp : public App
{
public:
    void load()
    {
        randomize();
        sun.anim.run("rotate");
        earth.anim.run("rotate");

        connect(design.child<Button>("restart"), restart);
        connect(design.child<Button>("fixcam"), switchCameraMode);
        connect(timer, createMeteor);
        focusCameraOnFighter = true;

        w = game.gameBox().width();
        h = game.gameBox().height();

        /* ToDo: need preload
		loadObj<GameObj>("meteor\\Meteor0.json");
        loadObj<GameObj>("meteor\\Meteor1.json");
        loadObj<GameObj>("meteor\\Meteor2.json");
        loadObj<GameObj>("meteor\\Meteor3.json");
        loadObj<GameObj>("meteor\\Meteor4.json");*/

        restart();
    }

    void restart()
    {
        gameover = false;
        gameoverLabel.hide();
        fireTimer.start();
        
        earth.setPos(800, 0);
        fighter.setAngle(0);
        fighter.setPos(700, -100);
        velo = Vec2(0, 0);

        missiles.clear();
        meteors.clear();
        meteorMarks.clear();

        timer.repeat(3);
    }

    void switchCameraMode()
    {
        focusCameraOnFighter = !focusCameraOnFighter;
    }

    void process(Input input)
    {
		using namespace InputKey;
        float fangle = fighter.angle();
        if (input.pressed(Left))
            fangle = fighter.rotate(3.14 * timeDelta());
        if (input.pressed(Right))
            fangle = fighter.rotate(-3.14 * timeDelta());
        if (input.justPressed(Up))
        {
            fighter.anim.reset(0);
            fighter.anim.run("turnon");
            fighter.anim.run("move");
        }
        if (input.justOutpressed(Up))
        {
            fighter.anim.reset(0);
            fighter.anim.run("turnoff");
        }
        if (input.pressed(Up))
        {
			velo += polarVec(20.0 * timeDelta(), fangle);
        }
        fighterMark.setAngle(fangle);

        if (!focusCameraOnFighter)
        {
            auto cpos = game.view();
            if (input.pressed('a'))
                cpos.x -= 400 * timeDelta();
            if (input.pressed('d'))
                cpos.x += 400 * timeDelta();
            if (input.pressed('s'))
                cpos.y -= 400 * timeDelta();
            if (input.pressed('w'))
                cpos.y += 400 * timeDelta();
            game.setView(cpos);

            if (minimap.isMouseOn() && input.pressed(MouseLeft))
                game.setView(minimap.mousePos() * 20);
        }

        if (fireTimer.time() > 0.3 && game.isMouseOn() && input.justPressed(MouseLeft))
        {
            auto mpos = game.mousePos();
            auto fpos = fighter.pos();
			auto laser = missiles.load("meteor\\Laser.json", fpos);
            laser.setAngle((mpos - fpos).angle());
            fireTimer.start();
        }
    }

    void move()
    {
        if (gameover)
            return;

        Vec2 epos = earth.pos();
        epos.rotate(6.28 / 120 * timeDelta());
        earth.setPos(epos);
        earthMark.setPos(epos / 20);

        Vec2 fpos = fighter.pos();
        float fangle = fighter.angle();

        if (fpos.x < game.gameBox().l)
            velo.x += 100.0 * timeDelta();
        if (fpos.x > game.gameBox().r)
            velo.x -= 100.0 * timeDelta();
        if (fpos.y < game.gameBox().b)
            velo.y += 100.0 * timeDelta();
        if (fpos.y > game.gameBox().t)
            velo.y -= 100.0 * timeDelta();

        fpos += velo * timeDelta();

        fighter.setPos(fpos);
        fighterMark.setPos(fpos / 20);

        if (focusCameraOnFighter)
        {
            game.setView(fpos);
        }
        windowMark.setPos(game.view() / 20);

        for (auto meteor : meteors.all())
        {
            auto mpos = meteor.pos();
            auto d = epos - mpos;
            d.normalize();
            mpos = meteor.move(d * 150 * timeDelta());
            auto mark = meteorMarks.get(meteor.id());
            mark.setPos(mpos / 20);

            if (dist(mpos, epos) < 50) {
                gameover = true;
                timer.stop();
                gameoverLabel.show();
            }
        }

        for (auto laser : missiles.all())
        {
			auto lpos = laser.move(polarVec(600, laser.angle()) * timeDelta());

            for (auto meteor : meteors.find(laser.box()))
            {
                auto mpos = meteor.pos();
                if (dist(mpos, lpos) < 30)
                {
                    meteorMarks.remove(meteor.id());
                    meteor.anim.reset();
                    meteor.anim.run("explode");
                }
            }

            if (!game.gameBox().contains(lpos))
                missiles.remove(laser);
        }
    }

    void createMeteor()
    {
        int index = rand() % 5;
        Vec2 mpos(w / 2, 0);
        auto meteor = meteors.load("meteor\\Meteor" + toString(index) + ".json", mpos);
        mpos.setAngle(randomFloat() * 6.28);
        meteor.setPos(mpos);
        meteor.anim.run("rotate");

        meteorMarks.load(meteor.id(), "meteor\\MeteorMark.json");
    }


    FromDesign(GameObj, fighter);
    FromDesign(GameObj, sun);
    FromDesign(GameObj, earth);
    
    FromDesign(GameView, minimap);
    FromDesign(GameObj, fighterMark);
    FromDesign(GameObj, earthMark);
    FromDesign(GameObj, windowMark);

    FromDesign(GameView, game);
    LayerFromDesign(void, meteors);
	LayerFromDesign(void, missiles);
	LayerFromDesign(void, meteorMarks);

    FromDesign(Label, gameoverLabel);

    Vec2 velo;

    bool gameover;
    Timer timer;
    Timer fireTimer;
    bool focusCameraOnFighter;

    float w, h;
};

int main(int argc, char** argv)
{
    MyApp app;
	app.setDesign("meteor\\Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
