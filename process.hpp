#ifndef PROCESS_H
#define PROCESS_H

#include "common.hpp"
#include "ship.hpp"

class Process;

class ProcessList {
private:
  static Process *s_processes;
  
private:
  ProcessList()
  {
    s_processes = NULL;
  }
  
public:
  static void add(Process *p);
  static void remove(Process *p);
  static void advanceAll(int delta);
};

class Process {
private:
  unsigned int m_absTimeElapsed;
  unsigned int m_timeElapsed;
  unsigned int m_duration;
  unsigned int m_delay;
  Process *m_next;
  Process *m_prev;
  bool m_attached;
  
protected:
  Process(unsigned int duration = 0, unsigned int delay = 0)
  : m_absTimeElapsed(0), m_timeElapsed(0), m_duration(duration), m_delay(delay), m_attached(false)
  {
    m_next = NULL;
    m_prev = NULL;
  }
  
public:
  virtual ~Process()
  {
    if (m_attached)
      ProcessList::remove(this);
  }
  
public:
  Process *getNext() const { return m_next; }
  Process *getPrev() const { return m_prev; }
  void setNext(Process *p) { m_next = p; }
  void setPrev(Process *p) { m_prev = p; }
  unsigned int getDuration() const { return m_duration; }
  unsigned int getDelay() const { return m_delay; }
  unsigned int getElapsedTime() const { return m_timeElapsed; }
  GLclampf getNormalizedTimeElapsed() const
  {
    return (GLfloat)m_timeElapsed / (GLfloat)m_duration;
  }
  
  friend class ProcessList;
  
public:
  virtual void advance(int delta)
  {
    m_absTimeElapsed += delta;
    if (m_absTimeElapsed <= m_delay)
      return;
    
    m_timeElapsed += delta;
    //If we exceed duration, reset the elapsed counter.
    //If we don't do this, the process may mutate some value
    //beyond the bounds expected
    if (m_timeElapsed > m_duration)
      m_timeElapsed = m_duration;
  }
  
  virtual bool isDone()
  {
    return getNormalizedTimeElapsed() == 1.f;
  }
};

class MaybeSpawnShipProcess : public Process {
private:
  
public:
  MaybeSpawnShipProcess()
  : Process(0)
  {
    
  }
  
public:
  void advance(int delta);
  bool isDone();
};

class PtoPShipFlyProcess : public Process {
private:
  Ship *m_ship;
  glm::vec3 m_startPos;
  glm::vec3 m_endPos;
  
public:
  PtoPShipFlyProcess(glm::vec3 &startPos, glm::vec3 &endPos);
  
  ~PtoPShipFlyProcess()
  {
    delete m_ship;
  }
  
  void advance(int delta);
  bool isDone();
};

#endif










