#include "events.hpp"

typedef std::vector<IPlayerMoveListener*>::iterator pml_iterator;
typedef std::vector<IPlayerLookListener*>::iterator pll_iterator;
typedef std::vector<IShipLaunchListener*>::iterator launch_iterator;
typedef std::vector<IShipLandListener*>::iterator land_iterator;

std::vector<IPlayerMoveListener*> Events::m_pmListeners;
std::vector<IPlayerLookListener*> Events::m_plListeners;
std::vector<IShipLaunchListener*> Events::m_launchListeners;
std::vector<IShipLandListener*> Events::m_landListeners;

void Events::addListener(IPlayerMoveListener *listener)
{
  Events::m_pmListeners.push_back(listener);
}

void Events::addListener(IPlayerLookListener* listener)
{
  Events::m_plListeners.push_back(listener);
}

void Events::addListener(IShipLaunchListener *listener)
{
  Events::m_launchListeners.push_back(listener);
}

void Events::addListener(IShipLandListener *listener)
{
  Events::m_landListeners.push_back(listener);
}

void Events::removeListener(EventType type, void *listener)
{
  //TODO: Fix this!!!!
  pml_iterator it;
  pml_iterator remove_it = Events::m_pmListeners.end();
  IPlayerMoveListener *l;
  for (it = Events::m_pmListeners.begin(); it != Events::m_pmListeners.end(); ++it) {
    if (listener == *it) {
      remove_it = it;
      break;
    }
  }

  if (remove_it != Events::m_pmListeners.end())
    Events::m_pmListeners.erase(remove_it);
}

void Events::playerMoveEvent(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
{
  pml_iterator it;
  IPlayerMoveListener *listener;
  for (it = Events::m_pmListeners.begin(); it != Events::m_pmListeners.end(); ++it) {
    listener = *it;
    listener->onPlayerMove(old_pos, new_pos);
  }
}

void Events::playerLookEvent(const Attitude &att)
{
  for (IPlayerLookListener *listener : m_plListeners) {
    listener->onPlayerLook(att);
  }
}

void Events::shipLaunchEvent(Ship *ship)
{
  launch_iterator it;
  IShipLaunchListener *listener;
  for (it = Events::m_launchListeners.begin(); it != Events::m_launchListeners.end(); ++it) {
    listener = *it;
    listener->onShipLaunch(ship);
  }
}

void Events::shipLandEvent(Ship *ship)
{
  land_iterator it;
  IShipLandListener *listener;
  for (it = Events::m_landListeners.begin(); it != Events::m_landListeners.end(); ++it) {
    listener = *it;
    listener->onShipLand(ship);
  }
}
