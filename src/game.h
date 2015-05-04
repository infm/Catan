#ifndef __GAME_H__
#define __GAME_H__

#include "constants.h"
#include "map.h"
#include "trade.h"

#include <stdlib.h>
#include <math.h>

#include <vector>
#include <map>

const int TERRAIN_RES[7] = {-1, -1, 0, 1, 2, 3, 4};
class Player {
    public:
        Player(int);

        int get_id();
        int* get_resources();

        void AddResource(int res_id, int count);

        bool TryBuildVillage();
        bool TryBuildRoad();
        std::string to_string();
    private:
        int player_id_;

        int cities_;
        int villages_;
        int roads_;

        int resources_[5] = {0};
};

class Village {
    public:
        Village(Point* loc = NULL, Player* ownr = NULL);
    protected:
//        friend class Point;

        Point* location_;
        Player* owner_;
        int resource_delta_;
};

class City : public Village {
    public:
        City(Point* loc = NULL, Player* ownr = NULL);
};

class Game {
    public:
        Game(Map*, std::vector<Player*>, sf::RenderWindow*);

        void GenMap();

        void SetUp();
        void Update();

        void PerformTurn(Player*);
    private:
        /**
         * Defining active windows, on_click behavior etc.
         * 0: Map shown
         * 1: Trade window shown
         */
        int visual_mode_ = 0;

        int res_left_[5] = {RES_CARDS_NUM_FOREACH};
        int knights_left_ = KNIGHT_CARDS_NUM;
        int progress_left_ = PROGRESS_CARDS_NUM;
        int victory_p_left_ = VICTORY_P_CARDS_NUM;
        int build_costs_ = BUILD_COSTS_CARDS_NUM;

        Map* game_map_;
        TradeWindow* trade_win_;
        std::vector<Player*> players_;
        int curr_player_ind_;

        std::vector<Village*> villages_;

        sf::RenderWindow* window_;
        bool continued_ = true;
        std::function<void()> on_click_;

        int ThrowDice();

        void RandomSetUp();
        void ManualSetUp();
};

#endif
