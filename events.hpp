#ifndef EVENTS_H
#define EVENTS_H

#include <vector>
#include "common.hpp"
#include "ship.hpp"

typedef enum EventType {
  EventPlayerMove, 
  EventPlayerJump,
  EventPlayerLook
} EventType;

class IPlayerMoveListener {
public:
    virtual void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos) = 0;
};

class IPlayerLookListener {
public:
  virtual void onPlayerLook(const Attitude &att) = 0;
};

class IShipLaunchListener {
public:
  virtual void onShipLaunch(Ship *ship) = 0;
};

class IShipLandListener {
public:
  virtual void onShipLand(Ship *ship) = 0;
};

class Events {
private:
  Events() { }
  Events(const Events &e) { }

private:
  static std::vector<IPlayerMoveListener*> m_pmListeners;
  static std::vector<IPlayerLookListener*> m_plListeners;
  static std::vector<IShipLaunchListener*> m_launchListeners;
  static std::vector<IShipLandListener*> m_landListeners;

public:
  static void addListener(IPlayerMoveListener *listener);
  static void addListener(IPlayerLookListener *listener);
  static void addListener(IShipLaunchListener *listener);
  static void addListener(IShipLandListener *listener);
  static void removeListener(EventType type, void *listener);

  static void playerMoveEvent(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  static void playerLookEvent(const Attitude &att);
  static void shipLaunchEvent(Ship *ship);
  static void shipLandEvent(Ship *ship);
};

#endif
