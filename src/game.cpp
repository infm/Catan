#include <SFML/Graphics.hpp>

#include "easylogging++.h"

#include "game.h"

const int HEX_SIZE = 70;
const int HEX_OUTLINE_SIZE = 2;

Game::Game() : knights_left(KNIGHT_CARDS_NUM),
               progress_left(PROGRESS_CARDS_NUM),
               victory_p_left(VICTORY_P_CARDS_NUM),
               build_costs(BUILD_COSTS_CARDS_NUM) {
  for (int i = 0; i < 5; ++i)
    res_left[i] = RES_CARDS_NUM_FOREACH;
}

Hex::Hex(){
    up_left = NULL;
    up_right = NULL;
    left = NULL;
    right = NULL;
    down_left = NULL;
    down_right = NULL;
    this -> num = -1;
    this -> type = -1;
}

Hex::Hex(Hex* ul, Hex* ur, Hex* l, Hex* r,
    Hex* dl, Hex* dr, int num, int type){
  up_left = ul;
  up_right = ur;
  left = l;
  right = r;
  down_left = dl;
  down_right = dr;
  this -> num = num;
  this -> type = type;
}

void Hex::set_num(int num){
    this -> num = num;
}

int Hex::get_num(){
    return this -> num;
}

int Hex::get_type(){
    return 1;
    //return type;
}

std::string Hex::to_string(){
    std::string res;
    if (NULL != up_left)
        res.append("up_left NON_NULL,");
    else
        res.append("up_left NULL, ");

    if (NULL != left)
        res.append("left NON_NULL, ");
    else
        res.append("left NULL, ");

    if (NULL != down_left)
        res.append("down_left NON_NULL, ");
    else
        res.append("down_left NULL, ");

    if (NULL != down_right)
        res.append("down_right NON_NULL, ");
    else
        res.append("down_right NULL, ");

    if (NULL != right)
        res.append("right NON_NULL, ");
    else
        res.append("right NULL, ");

    if (NULL != up_right)
        res.append("up_right NON_NULL; ");
    else
        res.append("up_right NULL; ");

    res.append("num ")
        .append(std::to_string(num));
    return res;
}

Point::Point(Hex* f, Hex* s, Hex* t){
  first = f;
  second = s;
  third = t;
}

Hex** Point::getHexes(Hex** res){
  res[0] = first;
  res[1] = second;
  res[2] = third;
  return res;
}

Line::Line(Hex* f, Hex* s){
  first = s;
  second = s;
}

Line::Line(Point* a, Point* b){
  if (a == b)
    return;
  Hex** ah = new Hex*[3];
  ah = a -> getHexes(ah);

  Hex** bh = new Hex*[3];
  bh = b -> getHexes(bh);

  for (int i = 0; i < 3; ++i){
    for (int j = 0; j < 3; ++j){
      if (ah[i] == bh[j]){
        if (NULL == first)
          first = ah[i];
        else
          second = ah[i];
      }
    }
  }
  delete[] ah;
}

Map::Map() {
    this -> root = new Hex();
}

Map::Map(Hex* root) {
  this -> root = root;
}

void Map::generate(){
    LOG(INFO) << "Generating map...";
    int n = GRID_SIZE;
    int dims[] = {3, 4, 5, 4, 3};

    std::vector<int> nums(&nums_arr[0], &nums_arr[0]
            + (HEXES_NUM - 1));

    Hex* curr = root;

    int desert_num = rand() % HEXES_NUM;
    int m = HEXES_NUM - 1;

    for (int i = 0; i < n; ++i){
        for (int j = 0; j < dims[i]; ++j){
            if (NULL == curr){
                LOG(ERROR) << "NULL == curr";
                return;
            }

            if (i > 0 && j > 0){
                Hex* left = curr -> up_left -> down_left;
                curr -> left = left;
                left -> right = curr;
                if (NULL != left -> down_right){
                    curr -> down_left = left -> down_right;
                    left -> down_right -> up_right = curr;
                }
            }

            if (!desert_num){
                --desert_num;
                //curr -> set_type(TYPE_DESERT);
                curr -> set_num(0);
            } else {
                --desert_num;
                //LOG(INFO) << "desert_num " << desert_num;

                int neighbor_sum = 0;
                int neighbor_count = 0;
                if (NULL != curr -> up_left){
                    neighbor_count++;
                    neighbor_sum += curr -> up_left -> get_num();
                }
                if (NULL != curr -> left){
                    neighbor_count++;
                    neighbor_sum += curr -> left -> get_num();
                }
                if (NULL != curr -> down_left){
                    neighbor_count++;
                    neighbor_sum += curr -> down_left -> get_num();
                }
                LOG(INFO) << id_map.size() <<
                    " neighbor_count is " << neighbor_count;

                int num_ind;
                if (neighbor_count == 0){
                    num_ind = rand() % m;
                } else {
                    num_ind = 0;
                    int desired = 6 + neighbor_count * 6 -
                        neighbor_sum;
                    int minabs = 6;
                    for (int ind = 0; ind < m; ++ind){
                        int currabs = abs(desired - nums[ind]);
                        if (minabs > currabs){
                            minabs = currabs;
                            num_ind = ind;
                        }
                    }
                }
                curr -> set_num(nums[num_ind]);
                //num_map[nums[num_ind]] = curr;
                nums.erase(nums.begin() + num_ind);
                --m;
            }
            LOG(INFO) << "curr -> num is " << curr -> get_num();
            id_map.push_back(curr);
            if (j < dims[i] - 1 && NULL == curr -> down_right){
                curr -> down_right = new Hex();
                curr -> down_right -> up_left = curr;
                curr = curr -> down_right;
            }
        }
        for (int j = 1; j < dims[i]; ++j){
            curr = curr -> up_left;
        }
        if (i < n / 2){
            curr -> up_right = new Hex();
            curr -> up_right -> down_left = curr;

            curr = curr -> up_right;
        } else {
            curr -> right = new Hex();
            curr -> right -> left = curr;

            curr = curr -> right;
            curr -> down_left = curr -> left -> down_right;
            curr -> down_left -> up_right = curr;
        }
    }
}

void Map::draw(sf::RenderWindow* window){
    sf::CircleShape hexagon(HEX_SIZE - HEX_OUTLINE_SIZE, 6);

    hexagon.setOutlineThickness(HEX_OUTLINE_SIZE);
    hexagon.setOutlineColor(sf::Color::Black);

    sf::Text text;
    text.setCharacterSize(HEX_SIZE / 2);
    text.setColor(sf::Color::Black);
    sf::Font font;
    if (!font.loadFromFile("black_jack.ttf")) {
        LOG(ERROR) << "Error loading font";
        return;
    }
    text.setFont(font);

    int dims[] = {GRID_SIZE - 2, GRID_SIZE - 1, GRID_SIZE,
        GRID_SIZE - 1, GRID_SIZE - 2};
    int v_dims[] = {2, 5, 8, 8, 8};
    double h_dims[] = {
        cos(30.0 * M_PI / 180.0), cos(30.0 * M_PI / 180.0),
        2 * cos(30.0 * M_PI / 180.0), 2 * cos(30.0 * M_PI / 180.0)
    };

    int wx = window -> getSize().x;
    int wy = window -> getSize().y;
    int deltax = HEX_SIZE * cos(30.0 * M_PI / 180.0);
    int deltay = HEX_SIZE * (1 + sin(30.0 * M_PI / 180.0));

    const int horizontal_start = (wx - HEX_SIZE * 10 *
            cos(30.0 * M_PI / 180.0)) / 2;
    int horizontal_offset = horizontal_start;
    int k = 0;
    Hex* curr = id_map[k++];
    for (int i = 0; i < GRID_SIZE; ++i){
        int vertical_offset = (wy - v_dims[i] * HEX_SIZE) / 2;
        horizontal_offset = horizontal_start;
        for (int j = 0; j < i; ++j)
            horizontal_offset += HEX_SIZE * h_dims[j];
        for (int j = 0; j < dims[i]; ++j){
            if (0 == curr -> get_num())
                hexagon.setFillColor(sf::Color::Yellow);
            else
                hexagon.setFillColor(sf::Color::White);

            hexagon.setPosition(horizontal_offset, vertical_offset);
            window -> draw(hexagon);

            text.setPosition(horizontal_offset + HEX_SIZE / 2, vertical_offset + HEX_SIZE / 2);
            text.setString(std::to_string(curr -> get_num()) + ", " + std::to_string(k - 1));
            window -> draw(text);

            vertical_offset += deltay;
            horizontal_offset += deltax;
            curr = id_map[k++];
        }
    }
}
