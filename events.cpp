#include "events.hpp"

std::vector<IPlayerMoveListener*> Events::m_pmListeners;
std::vector<IPlayerLookListener*> Events::m_plListeners;
std::vector<IKeyDownListener*> Events::m_keyDownListeners;
std::vector<IKeyUpListener*> Events::m_keyUpListeners;

void Events::addListener(IPlayerMoveListener *listener)
{
  Events::m_pmListeners.push_back(listener);
}

void Events::addListener(IPlayerLookListener* listener)
{
  Events::m_plListeners.push_back(listener);
}

void Events::addListener(IKeyDownListener *listener)
{
  Events::m_keyDownListeners.push_back(listener);
}

void Events::addListener(IKeyUpListener *listener)
{
  Events::m_keyUpListeners.push_back(listener);
}


typedef std::vector<IPlayerMoveListener*>::iterator pml_iterator;
void Events::removeListener(EventType type, void *listener)
{
  //TODO: Fix this!!!!
  //Currently no way to remove a listener!
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
  for (IPlayerMoveListener *listener : Events::m_pmListeners) {
    listener->onPlayerMove(old_pos, new_pos);
  }
}

void Events::playerLookEvent(const Attitude &att)
{
  for (IPlayerLookListener *listener : m_plListeners) {
    listener->onPlayerLook(att);
  }
}

void Events::keyDownEvent(int key, bool special)
{
  for (IKeyDownListener *listener : Events::m_keyDownListeners) {
    listener->onKeyDown(key, special);
  }
}

void Events::keyUpEvent(int key, bool special)
{
  for (IKeyUpListener *listener : Events::m_keyUpListeners) {
    listener->onKeyUp(key, special);
  }
}
