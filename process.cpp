#include "process.hpp"
#include "events.hpp"
#include "animation.hpp"

Process *ProcessList::s_processes = NULL;

void ProcessList::add(Process *p)
{
  p->m_attached = true;
  Process *pcur = s_processes;
  if (!pcur) {
    s_processes = p;
    return;
  }

  while (pcur->getNext()) {
    pcur = pcur->getNext();
  }

  pcur->setNext(p);
  p->setPrev(pcur);
}

void ProcessList::remove(Process *p)
{
  if (p->getNext())
    p->getNext()->setPrev(p->getPrev());
  
  if (p->getPrev())
    p->getPrev()->setNext(p->getNext());
  else
    s_processes = p->getNext();
  
  p->m_attached = false;
}

void ProcessList::advanceAll(int delta)
{
  Process *proc = s_processes, *tmp;
  while (proc) {
    if (proc->isDone()) {
      ProcessList::remove(proc);
      tmp = proc;
      proc = proc->getNext();
      
      //The process list violates the 'creation-is-ownership'
      //principle, but the creating code will probably never
      //need to hang onto a process reference after it completes
      delete tmp;
      continue;
    }

    proc->advance(delta);
    proc = proc->getNext();
  }
}

void MaybeSpawnShipProcess::advance(int delta)
{
  Process::advance(delta);
  
  if (percentChance(.2f)) {
    //Pick two random points and send
    //the ship flying from one to the other
    PtoPShipFlyProcess *proc;
    glm::vec3 startPos = randVec3(-100.f, 100.f);
    glm::vec3 endPos = randVec3(-100.f, 100.f);
    
    proc = new PtoPShipFlyProcess(startPos, endPos);
    ProcessList::add(proc);
  }
}

bool MaybeSpawnShipProcess::isDone()
{
  return false;
}

PtoPShipFlyProcess::PtoPShipFlyProcess(glm::vec3 &startPos, glm::vec3 &endPos)
  : Process(68000)
{
  m_startPos = startPos;
  m_endPos = endPos;
  m_ship = new Ship(randBetween(3.f, 10.f), m_startPos.x, m_startPos.y, m_startPos.z);
}

#define SHIP_SPEED 1.0
#define WARP_OFFSET 500.f

void PtoPShipFlyProcess::advance(int delta)
{
  Process::advance(delta);
  
  if (delta == getElapsedTime()) {
    Events::shipLaunchEvent(m_ship);
    
    //First, ease in from a "warp point" far out to
    //the start point
    glm::vec3 diff = m_startPos - m_endPos;
    glm::vec3 warpPoint = randVec3(-200.f, 200.f);
    m_ship->position.x = warpPoint.x;
    m_ship->position.y = warpPoint.y;
    m_ship->position.z = warpPoint.z;
    
    //Rotate the ship to point toward its destination
    GLfloat ang = glm::angle(glm::normalize(m_startPos), glm::normalize(m_endPos));
    m_ship->attitude.pitch = m_ship->attitude.roll = m_ship->attitude.yaw = ang;
    glm::vec3 direcVec = m_endPos - m_startPos;
    m_ship->setRotAxis(direcVec.x, direcVec.y, direcVec.z);
    
    glm::vec3 wdiff = warpPoint - m_startPos;
    ProcessList::add(new Animation(Interpolation::QuadraticEaseInOut, m_ship->position.x, wdiff.x, 4000));
    ProcessList::add(new Animation(Interpolation::QuadraticEaseInOut, m_ship->position.y, wdiff.y, 4000));
    ProcessList::add(new Animation(Interpolation::QuadraticEaseInOut, m_ship->position.z, wdiff.z, 4000));
    
    //Once at the start, move (interpolating linearly) to the end point
    ProcessList::add(new Animation(Interpolation::Linear, m_ship->position.x, direcVec.x, 60000, 4000));
    ProcessList::add(new Animation(Interpolation::Linear, m_ship->position.y, direcVec.y, 60000, 4000));
    ProcessList::add(new Animation(Interpolation::Linear, m_ship->position.z, direcVec.z, 60000, 4000));
    
    //Once at the endpoint, "warp" out
    glm::vec3 woffset(direcVec.x < 0 ? -WARP_OFFSET : WARP_OFFSET,
                      direcVec.y < 0 ? -WARP_OFFSET : WARP_OFFSET,
                      direcVec.z < 0 ? -WARP_OFFSET : WARP_OFFSET);
    ProcessList::add(new Animation(Interpolation::QuadraticEaseIn, m_ship->position.x, woffset.x, 4000, 64000));
    ProcessList::add(new Animation(Interpolation::QuadraticEaseIn, m_ship->position.y, woffset.y, 4000, 64000));
    ProcessList::add(new Animation(Interpolation::QuadraticEaseIn, m_ship->position.z, woffset.z, 4000, 64000));
  }
}

bool PtoPShipFlyProcess::isDone()
{
  bool done = getElapsedTime() >= 68000;
  if (done)
    Events::shipLandEvent(m_ship);
  
  return done;
}













