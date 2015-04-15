#include "map.h"

#include "game.h"
#include "util.h"

#include <algorithm>

#include "easylogging++.h"

const int HEX_SIZE = 60;
const int HEX_OUTLINE_SIZE = 4;
const int HEX_PRECISION = 6;

const int MOUSE_POINTER_SIZE = 10;
const int MOUSE_POINTER_PRECISION = 17;

const int POINT_SIZE = 10;
const int POINT_OUTLINE_SIZE = 1;
const int POINT_PRECISION = 72;

const int DICE_BUTTON_SIZE = 20;
const int DICE_BUTTON_PRECISION = 36;

const sf::Vector2f ACTION_PANEL_POS = sf::Vector2f(10, 600);
const sf::Vector2f ACTION_PANEL_SIZE = sf::Vector2f(300, 50);

OnClickListener::OnClickListener(Map* pmap)
    : pmap_(pmap) {}

OnClickListener::~OnClickListener() {}

MapObject::MapObject()
    : pos_() {}

MapObject::~MapObject() {}

sf::Vector2f MapObject::get_pos() const {
    return pos_;
}

void MapObject::set_pos(sf::Vector2f new_pos) {
    pos_ = new_pos;
}

void MapObject::set_on_click_listener(OnClickListener* ocl) {
    on_click_listener_ = ocl;
}

void MapObject::Click() {
    on_click_listener_ -> OnClick();
}

void MapObject::Draw(sf::RenderWindow* window) {}

Hex::Hex(Hex* ul, Hex* ur, Hex* l, Hex* r,
    Hex* dl, Hex* dr, int num, int type)
    : up_left(ul),
      up_right(ur),
      left(l),
      right(r),
      down_left(dl),
      down_right(dr),
      num_(num),
      type_(type) {}

//TODO : carefully review it
Hex::~Hex() {
}

void Hex::set_num(int num) {
    this -> num_ = num;
}

int Hex::get_num() const {
    return this -> num_;
}

void Hex::set_type(int type) {
    this -> type_ = type;
}

int Hex::get_type() const {
    return type_;
}

std::string Hex::to_string() const {
    std::string res;
//    if (NULL != up_left)
//        res.append("up_left NON_NULL,");
//    else
//        res.append("up_left NULL, ");
//
//    if (NULL != left)
//        res.append("left NON_NULL, ");
//    else
//        res.append("left NULL, ");
//
//    if (NULL != down_left)
//        res.append("down_left NON_NULL, ");
//    else
//        res.append("down_left NULL, ");
//
//    if (NULL != down_right)
//        res.append("down_right NON_NULL, ");
//    else
//        res.append("down_right NULL, ");
//
//    if (NULL != right)
//        res.append("right NON_NULL, ");
//    else
//        res.append("right NULL, ");
//
//    if (NULL != up_right)
//        res.append("up_right NON_NULL; ");
//    else
//        res.append("up_right NULL; ");

    res.append("type ")
        .append(std::to_string(type_))
        .append(", num ")
        .append(std::to_string(num_));
    return res;
}

void Hex::Click() {
    LOG(INFO) << to_string() << " clicked";
}

bool Hex::OnMouse(sf::Vector2i point) const {
    double sqrt2 = sqrt(2.0);
    sf::Vector2f center = pos_ +
        sf::Vector2f(HEX_SIZE * sqrt2 * .5, HEX_SIZE * sqrt2 * .5);

    double radius = (double) HEX_SIZE;
    return sqrt((center.x - point.x) * (center.x - point.x) +
            (center.y - point.y) * (center.y - point.y)) - radius < EPS + 20;
}

bool Hex::CanBeAdded(Point* point) {
    int index = GetIndexForPoint(point);
    return !points_[(index + 5) % 6] &&
        !points_[(index + 7) % 6];
}

void Hex::AddPoint(Point* point) {
    AddPoint(point, GetIndexForPoint(point));
}

void Hex::AddPoint(Point* point, int index) {
    points_[index] = point;
}

int Hex::GetIndexForPoint(Point* point) {
    Hex* point_hexes[3];
    point -> get_hexes(point_hexes);
    bool matches[6] = {false};
    for (int i = 0; i < 3; ++i) {
        Hex* curr = point_hexes[i];
        if (this == curr)
            continue;
        if (curr == up_left)
            matches[0] = true;
        if (curr == up_right)
            matches[1] = true;
        if (curr == right)
            matches[2] = true;
        if (curr == down_right)
            matches[3] = true;
        if (curr == down_left)
            matches[4] = true;
        if (curr == left)
            matches[5] = true;
    }
    int index = -1;
    for (int i = 0; index == -1 && i < 6; ++i) {
        if (matches[i] && matches[(i + 1) % 6])
            index = i;
    }
    return index;
}

std::vector<int> Hex::GetBoundPlayersIds() const {
    std::vector<int> result;
    for (int i = 0; i < 6; ++i) {
        if (points_[i]) {
            result.push_back(points_[i] -> get_owner_id());
        }
    }
    return result;
}

Point::Point(Hex* f, Hex* s, Hex* t) {
    hexes_.insert(f);
    hexes_.insert(s);
    hexes_.insert(t);
}

Point::~Point() {
    hexes_.clear();
}

Hex** Point::get_hexes(Hex** res) const {
    for (int i = 0; i < 3; ++i)
        res[i] = NULL;
    int k = 0;
    for (std::set<Hex*>::iterator it = hexes_.begin();
            it != hexes_.end(); ++it)
        res[k++] = *it;
    return res;
}

void Point::Click() {
//    std::string log;
//    log.append(std::to_string(first_ -> get_num()))
//        .append(" ")
//        .append(std::to_string(second_ -> get_num()))
//        .append(" ")
//        .append(std::to_string(third_ -> get_num()))
//        .append(" clicked");
//    LOG(INFO) << log;
}

bool Point::OnMouse(sf::Vector2i cursor) const {
    bool res = true;
    for (std::set<Hex*>::iterator it = hexes_.begin();
            it != hexes_.end() && res; ++it)
        res = (*it) -> OnMouse(cursor);
    return res;
}

int Point::get_owner_id() const {
    return this -> owner_id_;
}

void Point::set_owner_id(int owner_id) {
    this -> owner_id_ = owner_id;
}

std::set<Hex*> Point::get_hexes() {
    return hexes_;
}

Line::Line(Point* f, Point* s, std::set<Hex*> h)
    : first_(f),
      second_(s),
      hexes_(h) {}

Line::~Line() {
    delete first_;
    delete second_;
    hexes_.clear();
}

Line* Line::FromPoints(Point* first, Point* second) {
    std::set<Hex*> first_hexes_ = first -> get_hexes();
    std::set<Hex*> second_hexes_ = second -> get_hexes();
    std::set<Hex*> hexes_;

    std::set_intersection(first_hexes_.begin(), first_hexes_.end(),
            second_hexes_.begin(), second_hexes_.end(),
            std::inserter(hexes_, hexes_.begin()));
    Line* line = NULL;
    if (hexes_.size() == 2)
        line = new Line(first, second, hexes_);
    return line;
}

void Line::Click() {
//    std::string log;
//    log.append(std::to_string(first_ -> get_num()))
//        .append(" ")
//        .append(std::to_string(second_ -> get_num()))
//        .append(" clicked");
//    LOG(INFO) << log;
}

bool Line::OnMouse(sf::Vector2i cursor) const {
    bool res = true;
    for (std::set<Hex*>::iterator it = hexes_.begin();
            it != hexes_.end() && res; ++it)
        res = (*it) -> OnMouse(cursor);
    return res;
}

/**
 * TODO: check in rules if this is correct
 */
bool Line::CheckOwnership(int owner_id) const {
    int fid = first_ -> get_owner_id();
    int sid = second_ -> get_owner_id();
    return (fid == -1 && sid == owner_id) ||
        (fid == owner_id && sid == -1) ||
        (fid == owner_id && sid == owner_id);
}

void Line::set_owner_id(int owner_id) {
    this -> owner_id_ = owner_id;
}

int Line::get_owner_id() const {
    return this -> owner_id_;
}

Point** Line::get_points(Point** container) const {
    container[0] = first_;
    container[1] = second_;
    return container;
}

NotificationArea*
        NotificationArea::CreateInstance(std::string font_path) {
    NotificationArea *instance = new NotificationArea;
    if (!instance -> notification_text_font_.loadFromFile(
                font_path)) {
        LOG(ERROR) << "Error loading font";
        return NULL;
    }
    instance -> notification_text_
        .setFont(instance -> notification_text_font_);
    instance -> notification_text_.setCharacterSize(20);
    instance -> notification_text_.setColor(sf::Color::White);
    return instance;
}

NotificationArea::NotificationArea()
    : notification_text_(),
      notification_text_font_() {
    pos_ = sf::Vector2f(10.0f, 10.0f);
}

void NotificationArea::Click() {}

bool NotificationArea::OnMouse(sf::Vector2i cursor) const {
    return false;
}

void NotificationArea::MakeVisible() {
    MakeVisible(INF_DURATION);
}

void NotificationArea::MakeVisible(int duration) {
    visible_ = true;
    duration_ = duration;
}

void NotificationArea::MakeInvisible() {
    visible_ = false;
    duration_ = 0;
}

void NotificationArea::Draw(sf::RenderWindow* const window) {
    if (visible_) {
        notification_text_.setPosition(pos_);
        window -> draw(notification_text_);
    }
}

void NotificationArea::SetContent(std::string text) {
    notification_text_.setString(text);
}

void NotificationArea::Update() {
    if (visible_ && duration_ != INF_DURATION)
        visible_ = --duration_ != 0;
}

DiceButton* DiceButton::CreateInstance() {
    DiceButton* instance = new DiceButton;
    instance -> pos_ = sf::Vector2f(10, 100);
    instance -> dice_circle_.setPosition(instance -> pos_);
    instance -> dice_circle_.setFillColor(sf::Color::Yellow);
    return instance;
}

DiceButton::DiceButton()
    : dice_circle_(sf::CircleShape(DICE_BUTTON_SIZE,
              DICE_BUTTON_PRECISION)) {}

void DiceButton::Click() {
    LOG(INFO) << "Dice button clicked";
}

bool DiceButton::OnMouse(sf::Vector2i cursor) const {
    const double sqrt2 = sqrt(2.0);
    sf::Vector2f center = pos_ +
        sf::Vector2f(DICE_BUTTON_SIZE * sqrt2 * .5,
            DICE_BUTTON_SIZE * sqrt2 * .5);

    double radius = (double) DICE_BUTTON_SIZE;
    return sqrt((center.x - cursor.x) * (center.x - cursor.x) +
            (center.y - cursor.y) * (center.y - cursor.y)) - radius < EPS + 20;
}

void DiceButton::Draw(sf::RenderWindow* window) {
    if (OnMouse(sf::Mouse::getPosition(*window)))
        dice_circle_.setFillColor(color_focused_);
    else
        dice_circle_.setFillColor(color_idle_);
    window -> draw(dice_circle_);
}

NewVillageButton::NewVillageButton(Map* pmap)
    : shape_(sf::Vector2f(30, 30)) {
    shape_.setPosition(ACTION_PANEL_POS +
            sf::Vector2f(10, 10));
    on_click_listener_ = new NewVillageOCL(pmap);
}

void NewVillageButton::Draw(sf::RenderWindow* window) {
    if (OnMouse(sf::Mouse::getPosition(*window)))
        shape_.setFillColor(color_focused_);
    else
        shape_.setFillColor(color_idle_);
    window -> draw(shape_);
}

NewVillageButton::NewVillageOCL::NewVillageOCL(Map* pmap)
    : OnClickListener(pmap) {}

void NewVillageButton::NewVillageOCL::OnClick() {
    pmap_ -> AddVillage();
}

bool NewVillageButton::OnMouse(sf::Vector2i cursor) const {
    return
        shape_.getGlobalBounds()
            .contains((float) cursor.x,
                      (float) cursor.y);
}

ActionPanel* ActionPanel::CreateInstance() {
    ActionPanel* instance = new ActionPanel;
    instance -> pos_ = ACTION_PANEL_POS;
    instance -> panel_shape_
            .setFillColor(instance -> panel_color_);
    instance -> panel_shape_
            .setPosition(instance -> pos_.x, instance -> pos_.y);
    return instance;
}

ActionPanel::ActionPanel()
    : panel_shape_(ACTION_PANEL_SIZE),
      panel_color_(sf::Color(212, 193, 131, 255)),
      components_() {}

void ActionPanel::Click() {
    for (size_t i = 0; i < components_.size(); ++i)
        components_[i] -> Click();
}

bool ActionPanel::OnMouse(sf::Vector2i cursor) const {
    return panel_shape_
            .getGlobalBounds()
            .contains((float) cursor.x, (float) cursor.y);
}

void ActionPanel::Draw(sf::RenderWindow* window) {
    window -> draw(panel_shape_);
    for (size_t i = 0; i < components_.size(); ++i)
        components_[i] -> Draw(window);
}

void ActionPanel::AddComponent(MapObject* pcomponent) {
    components_.push_back(pcomponent);
}

Map::Map(sf::RenderWindow* window)
    : Map(new Hex, window) {}

Map::Map(Hex* root, sf::RenderWindow* window)
    : root_(root),
      window_(window),
      notifications_(NotificationArea::
              CreateInstance("black_jack.ttf")),
      dice_button_(DiceButton::CreateInstance()),
      action_panel_(ActionPanel::CreateInstance()),
      hexagon_(sf::CircleShape(HEX_SIZE - HEX_OUTLINE_SIZE,
              HEX_PRECISION)),
      mouse_circle_(sf::CircleShape(MOUSE_POINTER_SIZE,
              MOUSE_POINTER_PRECISION)),
      point_circle_(sf::CircleShape(POINT_SIZE - POINT_OUTLINE_SIZE,
              POINT_PRECISION)),
      line_array_(sf::VertexArray(sf::Lines, 2)) {
    Init();
}

Map::~Map() {
    map_objects_.clear();
    points_.clear();
    lines_.clear();

    delete root_;
    delete notifications_;
    delete dice_button_;
    delete action_panel_;
}

void Map::Init() {
    hexagon_.setOutlineThickness(HEX_OUTLINE_SIZE);
    hexagon_.setOutlineColor(sf::Color::Black);

    mouse_circle_.setFillColor(sf::Color(200, 200, 200, 200));
    mouse_circle_.setOrigin(.5 * sqrt(2.0) * MOUSE_POINTER_SIZE,
            .5 * sqrt(2.0) * MOUSE_POINTER_SIZE);

    point_circle_.setFillColor(sf::Color::Red);
    point_circle_.setOutlineColor(sf::Color::Black);
    point_circle_.setOutlineThickness(POINT_OUTLINE_SIZE);

    hex_text_.setCharacterSize(HEX_SIZE / 2);
    hex_text_.setColor(sf::Color::Black);
    if (!hex_font_.loadFromFile("mplus-1m-regular.ttf")) {
        LOG(ERROR) << "Error loading font";
        return;
    }
    hex_text_.setFont(hex_font_);

}

void Map::Generate() {
    LOG(INFO) << "Generating map...";
    int v_dims[] = {2, 5, 8, 11, 11, 11, 11};
    double h_dims[] = {
        cos(30.0 * M_PI / 180.0),
        cos(30.0 * M_PI / 180.0),
        cos(30.0 * M_PI / 180.0),
        2 * cos(30.0 * M_PI / 180.0),
        2 * cos(30.0 * M_PI / 180.0),
        2 * cos(30.0 * M_PI / 180.0)
    };

    int wx = window_ -> getSize().x;
    int wy = window_ -> getSize().y;
    int deltax = HEX_SIZE * cos(30.0 * M_PI / 180.0);
    int deltay = HEX_SIZE * (1 + sin(30.0 * M_PI / 180.0));

    std::vector<int> types;
    for (int i = TYPE_SEA; i <= TYPE_FOREST; ++i)
        for (int j = 0; j < TYPE_COUNT[i]; ++j)
            types.push_back(i);

    std::vector<int> nums(&NUMS_ARR[0], &NUMS_ARR[0]
            + (HEXES_NUM - 1));

    int m = HEXES_NUM - 1;
    int k = types.size();

    Hex* curr = root_;

    const int horizontal_start = (wx - HEX_SIZE * 2 *
            (2 + GRID_SIZE) * cos(30.0 * M_PI / 180.0)) / 2;
    int horizontal_offset = horizontal_start;

    for (int i = 0; i < GRID_SIZE + 2; ++i) {
        int vertical_offset = (wy - v_dims[i] * HEX_SIZE) / 2;
        horizontal_offset = horizontal_start;
        for (int j = 0; j < i; ++j)
            horizontal_offset += HEX_SIZE * h_dims[j];
        for (int j = 0; j < dims_[i]; ++j) {
            if (NULL == curr) {
                LOG(ERROR) << "NULL == curr";
                return;
            }

            curr -> set_pos(sf::Vector2f(horizontal_offset,
                        vertical_offset));
            if (i > 0 && j > 0) {
                Hex* left = curr -> up_left -> down_left;
                curr -> left = left;
                left -> right = curr;
                if (NULL != left -> down_right) {
                    curr -> down_left = left -> down_right;
                    left -> down_right -> up_right = curr;
                }
            }

            int type;
            if ((i > 0 && i < GRID_SIZE + 1) &&
                    (j > 0 && j < dims_[i] - 1)) {
                int type_ind = rand() % k;
                type = types[type_ind];
                types.erase(types.begin() + type_ind);
                --k;
            } else {
                type = TYPE_SEA;
            }

            curr -> set_type(type);
            if (TYPE_DESERT == type || TYPE_SEA == type) {
                curr -> set_num(0);
            } else {
                int neighbor_sum = 0;
                int neighbor_count = 0;
                if (NULL != curr -> up_left) {
                    neighbor_count++;
                    neighbor_sum += curr -> up_left -> get_num();
                }
                if (NULL != curr -> left) {
                    neighbor_count++;
                    neighbor_sum += curr -> left -> get_num();
                }
                if (NULL != curr -> down_left) {
                    neighbor_count++;
                    neighbor_sum += curr -> down_left -> get_num();
                }

                int num_ind;
                if (neighbor_count == 0) {
                    num_ind = rand() % m;
                } else {
                    num_ind = 0;
                    int desired = 6 + neighbor_count * 6 -
                        neighbor_sum;
                    int minabs = 6;
                    for (int ind = 0; ind < m; ++ind) {
                        int currabs = abs(desired - nums[ind]);
                        if (minabs > currabs) {
                            minabs = currabs;
                            num_ind = ind;
                        }
                    }
                }
                curr -> set_num(nums[num_ind]);
                nums.erase(nums.begin() + num_ind);
                --m;
            }
            hexes_.push_back(curr);
            hexes_by_num_[curr -> get_num()].push_back(curr);
            map_objects_.push_back(curr);
            if (j < dims_[i] - 1 && NULL == curr -> down_right) {
                curr -> down_right = new Hex;
                curr -> down_right -> up_left = curr;
                curr = curr -> down_right;
            }
            vertical_offset += deltay;
            horizontal_offset += deltax;
        }
        for (int j = 1; j < dims_[i]; ++j) {
            curr = curr -> up_left;
        }
        if (i < (GRID_SIZE + 2) / 2) {
            curr -> up_right = new Hex;
            curr -> up_right -> down_left = curr;

            curr = curr -> up_right;
        } else if (i < (GRID_SIZE + 2) - 1) {
            curr -> right = new Hex;
            curr -> right -> left = curr;

            curr = curr -> right;
            curr -> down_left = curr -> left -> down_right;
            curr -> down_left -> up_right = curr;
        }
    }

    GeneratePoints();
    GenerateLines();
    GenerateActionPanel();
    map_objects_.push_back(notifications_);
    map_objects_.push_back(dice_button_);
    map_objects_.push_back(action_panel_);
    LOG(INFO) << "Map generated successfully";
}

void Map::GeneratePoints() {
    int deltax = HEX_SIZE * cos(30.0 * M_PI / 180.0);
    int deltay = HEX_SIZE * (1 + sin(30.0 * M_PI / 180.0));
    double sqrt2 = sqrt(2.0);
    sf::Vector2f to_center = sf::Vector2f(HEX_SIZE * sqrt2 * .5,
                         HEX_SIZE * sqrt2 * .5);
    for (size_t i = 0; i < hexes_.size(); ++i) {
        Hex* curr = hexes_[i];
        Hex* ul = curr -> up_left;
        Hex* ur = curr -> up_right;
        Hex* r = curr -> right;

        sf::Vector2f curr_pos = curr -> get_pos();
        if (ul && ur) {
            Point* new_point = new Point(ul, ur, curr);
            new_point -> set_pos(
                    curr_pos + sf::Vector2f((float) deltax -
                        .5f * HEX_OUTLINE_SIZE,
                        -POINT_SIZE));
            AddPoint(new_point);
        }
        if (ur && r) {
            Point* new_point = new Point(ur, r, curr);
            new_point -> set_pos(
                    curr_pos + sf::Vector2f((float) 2 * deltax -
                        .5f * HEX_OUTLINE_SIZE,
                        cos(60.0 * M_PI / 180.0) * HEX_SIZE -
                        POINT_SIZE - HEX_OUTLINE_SIZE));
            AddPoint(new_point);
        }
    }
}

void Map::GenerateLines() {
    for (size_t i = 0; i < points_.size() - 1; ++i) {
        for (size_t j = i + 1; j < points_.size(); ++j) {
            Line* line = Line::FromPoints(points_[i], points_[j]);
            if (line) {
                line -> set_pos(points_[i] -> get_pos());

//                sf::Vector2f deltas = points_[i] -> get_pos() -
//                        points_[j] -> get_pos();
//                double dist = sqrt(deltas.x * deltas.x +
//                        deltas.y * deltas.y);
//                if (deltas.y < 0 || deltas.x < 0)
//                    dist *= -1;
//                double angle_rad = acos(deltas.x / dist);
//                double angle_deg = angle_rad * 180.f / M_PI;
//                line -> set_rotation(angle_deg);

                AddLine(line);
            }
        }
    }
}

void Map::GenerateActionPanel() {
    NewVillageButton* pnvb = new NewVillageButton(this);
    action_panel_ -> AddComponent(pnvb);
}

void Map::DrawMousePointer() const {
    sf::Vector2i point = sf::Mouse::getPosition(*window_);
    mouse_circle_.setPosition((float) point.x, (float) point.y);
    window_ -> draw(mouse_circle_);
}

void Map::DrawPoints() const {
//    TODO : remove stub
    for (size_t i = 0; i < points_.size(); ++i) {
        Point* curr = points_[i];
        point_circle_.setPosition(curr -> get_pos());
        sf::Color fill_color;
        switch (curr -> get_owner_id()) {
            case 0:
                fill_color = sf::Color::Red;
                break;
            case 1:
                fill_color = sf::Color::Cyan;
                break;
            case 2:
                fill_color = sf::Color::Magenta;
                break;
            case 3:
                fill_color = sf::Color::White;
                break;
            default:
                fill_color = sf::Color(255, 255, 255, 20);
                break;
        }
        point_circle_.setFillColor(fill_color);
        window_ -> draw(point_circle_);
    }
}

void Map::DrawLines() const {
    Point* points[2];
    sf::Vector2f positions[2];
    for (size_t i = 0; i < lines_.size(); ++i) {
        Line* curr = lines_[i];
//        line_rectangle_.setPosition(curr -> get_pos());
//        line_rectangle_.setRotation(curr -> get_rotation());
//        window_ -> draw(line_rectangle_);
        curr -> get_points(points);
        double sqrt2 = sqrt(2.0);
        sf::Vector2f to_center = sf::Vector2f(POINT_SIZE * sqrt2 * .5,
                         POINT_SIZE * sqrt2 * .5);
        sf::Color fill_color;
        switch (curr -> get_owner_id()) {
            case 0:
                fill_color = sf::Color::Red;
                break;
            case 1:
                fill_color = sf::Color::Cyan;
                break;
            case 2:
                fill_color = sf::Color::Magenta;
                break;
            case 3:
                fill_color = sf::Color::White;
                break;
            default:
                fill_color = sf::Color(255, 255, 255, 20);
                break;
        }
        line_array_[0].position = points[0] -> get_pos() + to_center;
        line_array_[0].color = fill_color;
        line_array_[1].position = points[1] -> get_pos() + to_center;
        line_array_[1].color = fill_color;
        window_ -> draw(line_array_);
    }
}

void Map::DrawMap() const {
    sf::Vector2i point = sf::Mouse::getPosition(*window_);

    int dims_[] = {GRID_SIZE - 1, GRID_SIZE, GRID_SIZE + 1,
        GRID_SIZE + 2, GRID_SIZE + 1, GRID_SIZE, GRID_SIZE - 1};
    int k = 0;
    Hex* curr = hexes_[k++];
//    sf::CircleShape pos_circle(10, 20);
//    pos_circle.setFillColor(sf::Color::Magenta);
//    sf::CircleShape center_circle(10, 20);
//    center_circle.setFillColor(sf::Color::Cyan);
    for (int i = 0; i < GRID_SIZE + 2; ++i) {
        for (int j = 0; j < dims_[i]; ++j) {
            if (0 == curr -> get_num())
                hexagon_.setFillColor(sf::Color::Yellow);
            else
                hexagon_.setFillColor(sf::Color::White);

            sf::Vector2f curr_pos = curr -> get_pos();
            hexagon_.setPosition(curr_pos);
            if (curr -> OnMouse(point)) {
                hexagon_.setFillColor(TYPE_COLOR[curr -> get_type()] +
                        sf::Color(0, 0, 0, 75));
            } else {
                hexagon_.setFillColor(TYPE_COLOR[curr -> get_type()]);
            }
            window_ -> draw(hexagon_);

//            pos_circle.setPosition(curr_pos);
//            window_ -> draw(pos_circle);
//            center_circle.setPosition(curr_pos + sf::Vector2f(HEX_SIZE * sqrt(2.0) * .5,
//                         HEX_SIZE * sqrt(2.0) * .5));
//            window_ -> draw(center_circle);

            hex_text_.setPosition(curr_pos +
                    sf::Vector2f(.7f * HEX_SIZE, .7f * HEX_SIZE));
            hex_text_.setString(std::to_string(curr -> get_num()));
            window_ -> draw(hex_text_);
            curr = hexes_[k++];
        }
    }
}

void Map::Draw() const {
    DrawMap();
    DrawPoints();
    DrawLines();
    notifications_ -> Draw(window_);
    notifications_ -> Update();

    dice_button_ -> Draw(window_);
    action_panel_ -> Draw(window_);
    DrawMousePointer();
}

void Map::Click(Player* requester) {
    last_requester_ = requester;
    sf::Vector2i point = sf::Mouse::getPosition(*window_);
    for (size_t i = 0; i < map_objects_.size(); ++i)
        if (map_objects_[i] -> OnMouse(point))
            map_objects_[i] -> Click();
}

bool Map::NextTurn() const {
   return NULL != dice_button_ &&
       dice_button_ -> OnMouse(sf::Mouse::getPosition(*window_));
}

void Map::ShowNotification(std::string text) {
    notifications_ -> MakeVisible();
    notifications_ -> SetContent(text);
}

void Map::ShowNotification(std::string text, int duration) {
    notifications_ -> MakeVisible(duration);
    notifications_ -> SetContent(text);
}

Point* Map::AddVillage(Player* player) {
    sf::Vector2i cursor = sf::Mouse::getPosition(*window_);
    int player_id = player -> get_id();
    for (size_t i = 0; i < points_.size(); ++i) {
        Point* point = points_[i];
        if (point -> OnMouse(cursor) &&
                -1 == point -> get_owner_id() &&
                TryAddPoint(point)) {
            point -> set_owner_id(player_id);
            return point;
        }
    }
    return NULL;
}

Point* Map::AddVillage() {
    return AddVillage(last_requester_);
}

Line* Map::AddRoad(Player* player) {
    sf::Vector2i cursor = sf::Mouse::getPosition(*window_);
    int player_id = player -> get_id();
    for (size_t i = 0; i < lines_.size(); ++i) {
        Line* line = lines_[i];
        if (line -> OnMouse(cursor) &&
                line -> CheckOwnership(player_id)) {
            line -> set_owner_id(player_id);
            return line;
        }
    }
    return NULL;
}

std::vector<Triple<int, int, int> >
        Map::GenerateResources(int num) {
    //DO STH
    LOG(INFO) << "GenerateResources of " + std::to_string(num) +
        " is called";
    std::vector<Triple<int, int, int> > result;

    std::vector<Hex*> matched_hexes = hexes_by_num_[num];
    for (size_t i = 0; i < matched_hexes.size(); ++i) {
        std::vector<int> players_ids =
                matched_hexes[i] -> GetBoundPlayersIds();
        for (size_t j = 0; j < players_ids.size(); ++j) {
            result.push_back(Triple<int, int, int>::make_triple(
                        players_ids[j],
                        matched_hexes[i] -> get_type(),
                        1)); //TODO: add support for cities
        }
    }
    return result;
}


Point* Map::AddPoint(Point* point) {
    points_.push_back(point);
    map_objects_.push_back(point);
    return point;
}

Line* Map::AddLine(Line* line) {
    lines_.push_back(line);
    map_objects_.push_back(line);
    return line;
}

bool Map::TryAddPoint(Point* point) {
    Hex* point_hexes[3];
    point -> get_hexes(point_hexes);
    bool res = true;
    for (int i = 0; i < 3 && res; ++i)
        res = point_hexes[i] -> CanBeAdded(point);
    if (res) {
        for (int i = 0; i < 3; ++i)
            point_hexes[i] -> AddPoint(point);
    }
    return res;
}
