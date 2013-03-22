#ifndef EVENTS_H
#define EVENTS_H

#include <vector>
#include "common.hpp"

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
  virtual void onPlayerLook(const Attitude &att, const glm::vec3 &lookVEctor, bool headlightOn) = 0;
};

class IKeyDownListener {
public:
  virtual void onKeyDown(int key, bool special) = 0;
};

class IKeyUpListener {
public:
  virtual void onKeyUp(int key, bool special) = 0;
};

class Events {
private:
  Events() { }
  Events(const Events &e) { }

private:
  static std::vector<IPlayerMoveListener*> m_pmListeners;
  static std::vector<IPlayerLookListener*> m_plListeners;
  static std::vector<IKeyDownListener*> m_keyDownListeners;
  static std::vector<IKeyUpListener*> m_keyUpListeners;

public:
  static void addListener(IPlayerMoveListener *listener);
  static void addListener(IPlayerLookListener *listener);
  static void addListener(IKeyDownListener *listener);
  static void addListener(IKeyUpListener *listener);
  static void removeListener(EventType type, void *listener);
  
  static void removeListener(IPlayerMoveListener *listener);
  static void removeListener(IPlayerLookListener *listener);
  static void removeListener(IKeyDownListener *listener);
  
  template <class TListener>
  static void removeListenerFrom(typename std::vector<TListener*> &vec, TListener *listener)
  {
    typename std::vector<TListener*>::iterator it;
    typename std::vector<TListener*>::iterator remove_it = vec.end();
    for (it = vec.begin(); it != vec.end(); ++it) {
      if (listener == *it) {
        remove_it = it;
        break;
      }
    }
    
    if (remove_it != vec.end())
      vec.erase(remove_it);
  }

  static void playerMoveEvent(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  static void playerLookEvent(const Attitude &att, const glm::vec3 &lookVector, bool headlightOn);
  static void keyDownEvent(int key, bool special);
  static void keyUpEvent(int key, bool special);
};

#endif
