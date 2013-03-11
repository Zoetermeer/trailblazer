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













