#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

enum Type
{
	None,
	Wall,
	Pacman,
	Ghost,
	Coin
};

class MyApp : public App
{
    void load()
    {
		map<Color, int> colorToType;
		colorToType[Color(0, 0, 0)] = None;
		colorToType[Color(255, 255, 255)] = Wall;
		colorToType[Color(0, 0, 255)] = Pacman;
		colorToType[Color(255, 0, 0)] = Ghost;
		colorToType[Color(255, 255, 0)] = Coin;
		gameMap = loadMap("pacman/map.png", colorToType);

		for (int x = 0; x < gameMap.w; ++x)
		{
			for (int y = 0; y < gameMap.h; ++y)
			{
				if (gameMap[x][y] == Wall)
					walls.load("pacman/Wall.json", pixels(x, y));

				if (gameMap[x][y] == Coin)
					coins.load("pacman/Coin.json", pixels(x, y));

				if (gameMap[x][y] == Pacman)
				{
					pacman.setPos(pixels(x, y));
					pv.x = x;
					pv.y = y;
				}

				if (gameMap[x][y] == Ghost)
				{
					ghost.setPos(pixels(x, y));
					gv.x = x;
					gv.y = y;
				}
			}
		}
		pacman.anim.run("eat", 0);
		pdir.x = 1;
		pdir.y = 0;
    }

    void process(Input input)
    {
		using namespace InputKey;
		if (input.pressed(Left))
			pdir = IntVec2(-1, 0);
		if (input.pressed(Right))
			pdir = IntVec2(1, 0);
		if (input.pressed(Up))
			pdir = IntVec2(0, 1);
		if (input.pressed(Down))
			pdir = IntVec2(0, -1);
    }

    void move()
    {
		if (pacman.anim.isEmpty(1))
		{
			if (gameMap[pv + pdir] != Wall)
			{
				pv += pdir;
				if (pdir.x == -1)
					pacman.anim.run("left", 1);
				if (pdir.x == 1)
					pacman.anim.run("right", 1);
				if (pdir.y == -1)
					pacman.anim.run("down", 1);
				if (pdir.y == 1)
					pacman.anim.run("up", 1);
			}
		}

		if (ghost.anim.isEmpty(1))
		{
			auto gdir = nextDir(gv, pv);
			gv += gdir;
			if (gdir.x == -1)
				ghost.anim.run("left", 1);
			if (gdir.x == 1)
				ghost.anim.run("right", 1);
			if (gdir.y == -1)
				ghost.anim.run("down", 1);
			if (gdir.y == 1)
				ghost.anim.run("up", 1);
		}

		for (auto coin : coins.find(pacman.box()))
		{
			coins.remove(coin);
		}

		if (pacman.intersects(ghost))
			close();
    }

	Vec2 pixels(int x, int y)
	{
		return Vec2(x * 32 - gameMap.w * 16 + 16,
		            y * 32 - gameMap.h * 16 + 16);
	}

	IntVec2 nextDir(IntVec2 start, IntVec2 finish)
	{
		vector<vector<int>> map;
		for (int x = 0; x < gameMap.w; ++x)
		{
			map.push_back(vector<int>());
			for (int y = 0; y < gameMap.h; ++y)
			{
				if (gameMap[x][y] == Wall)
				{
					map[x].push_back(-1);
				}
				else
				if (x == start.x && y == start.y)
				{
					map[x].push_back(0);
				}
				else
				{
					map[x].push_back(100000);
				}
			}
		}

		deque<IntVec2> queue;
		queue.push_back(start);

		for (;;)
		{
			IntVec2 wave = queue.front();
			if (wave == finish)
				break;
			queue.pop_front();

			int dist = map[wave.x][wave.y];
			if (map[wave.x + 1][wave.y] > dist + 1)
			{
				map[wave.x + 1][wave.y] = dist + 1;
				queue.push_back(IntVec2(
					wave.x + 1, wave.y));
			}
			if (map[wave.x - 1][wave.y] > dist + 1)
			{
				map[wave.x - 1][wave.y] = dist + 1;
				queue.push_back(IntVec2(
					wave.x - 1, wave.y));
			}
			if (map[wave.x][wave.y + 1] > dist + 1)
			{
				map[wave.x][wave.y + 1] = dist + 1;
				queue.push_back(IntVec2(
					wave.x, wave.y + 1));
			}
			if (map[wave.x][wave.y - 1] > dist + 1)
			{
				map[wave.x][wave.y - 1] = dist + 1;
				queue.push_back(IntVec2(
					wave.x, wave.y - 1));
			}
		}

		IntVec2 wave = finish;
		for (;;)
		{
			int dist = map[wave.x][wave.y];
			if (map[wave.x + 1][wave.y] == dist - 1)
			{
				if (dist == 1)
					return wave - start;
				wave.x++;
			}
			if (map[wave.x - 1][wave.y] == dist - 1)
			{
				if (dist == 1)
					return wave - start;
				wave.x--;
			}
			if (map[wave.x][wave.y + 1] == dist - 1)
			{
				if (dist == 1)
					return wave - start;
				wave.y++;
			}
			if (map[wave.x][wave.y - 1] == dist - 1)
			{
				if (dist == 1)
					return wave - start;
				wave.y--;
			}
		}
	}

	LayerFromDesign(void, walls);
	LayerFromDesign(void, coins);
	FromDesign(GameObj, pacman);
	FromDesign(GameObj, ghost);
	GameMap gameMap;

	IntVec2 pv;
	IntVec2 pdir;

	IntVec2 gv;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setDesign("pacman/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
