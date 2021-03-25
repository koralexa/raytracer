#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <iostream>
#include <string>
#include <vector>

constexpr int playerWidth = 64;
constexpr int playerHeight = 98;

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class PlayerAction
{
    STAY,
    MOVE,
    DIE,
    FALL,
    CARROT1,
    CARROT2,
    CARROT3,
    PORTAL_UP,
    PORTAL_DOWN,
    PORTAL_RIGHT,
    PORTAL_LEFT,
    WIN,
    HEART,
    WOLF
};

struct Wolf;

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                  coords(pos), old_coords(coords) {};


  bool Moved() const;
  void ProcessInput(MovementDir dir, std::string &room, Image &currentBackground, Image &screen, std::vector<Wolf> &wolves);
  void Draw(Image &screen, Image &currentBackground, std::vector<Wolf> &wolves);
  
  bool GetActive() { return active; }
  void SetActive(bool a) { active = a; }
  void SetWaiting(bool w) { waiting = w; }
  Point GetCoords() { return coords; }
  void SetCoords(int x, int y) { coords.x = x; coords.y = y; }
  void DecreaseLives() { lives -= 1; }
  int GetLives() { return lives; }
  void SetFallingCount(int f) { falling_count = f; }
  void SetGettingOutCount(int g) { getting_out_count = g; }
  void SetDyingCount(int d) { dying_count = d; }
  void SetCurrentImage(int i) { current_image = i; }
  enum PlayerAction GetPlayerAction() { return player_action; }
  void SetPlayerAction(enum PlayerAction a) { player_action = a; }
  enum PlayerAction GetLastPortal() { return last_portal; }
  void DrawCarrots(Image &screen);
  void DrawLives(Image &screen);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;
  int carrots = 0;
  int lives = 5;
  bool active = true;
  bool waiting = false;
  int current_image = 8;
  int switch_image = 3;
  enum PlayerAction player_action = PlayerAction::MOVE;
  int falling_count = 0;
  int getting_out_count = 0;
  int dying_count = 0;
  enum PlayerAction last_portal = PlayerAction::PORTAL_LEFT;
};



struct Wolf
{
  explicit Wolf(int a, char c, Point pos = {.x = 10, .y = 10}) :
                  coords(pos), old_coords(pos), home(coords), area(a), home_char(c) {};

  bool Move(Player &player, std::string &room, Image &currentBackground, Image &screen);
  void Draw(Image &screen, Image &currentBackground);
  
  void SetCoords(int x, int y) { coords.x = x; coords.y = y; }
  void SetCurrentImage(int i) { current_image = i; }
  void SetActive(bool a) { active = a; }
  void SetFiringCount(int f) { firing_count = f; }
  int GetFiringCount() { return firing_count; }
  void EraseWolfAndPlayer(Player &player, Image &screen, Image &currentBackground);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  int width = 128;
  int height = 64;
  int move_speed = 1;
  int area = 5;
  char home_char = '0';
  int switch_image = 3;
  Point home {.x = 10, .y = 10};
  MovementDir last_dir = MovementDir::LEFT;
  int current_image = 0;
  bool active = true;
  int firing_count = 0;
};

#endif //MAIN_PLAYER_H
