#ifndef CHUNK_BUFFER_H
#define CHUNK_BUFFER_H

#include <glm/glm.hpp>
#include <vector>
#include "scene-object.hpp"
#include "events.hpp"
#include "process.hpp"

class Chunk;

class ChunkBuffer : public IPlayerMoveListener, public IPlayerLookListener, public IKeyDownListener, public Process, public SceneObject {
private:
  glm::ivec3 m_curPlayerChunkCoords;
  std::vector<Chunk*> m_loadQueue;
  std::vector<Chunk*> m_visibleQueue;
  Chunk *m_curPlayerChunk;
  glm::vec4 m_playerPos;
  glm::vec3 m_playerLookVector;
  bool m_isPlayerHeadlightOn;
  bool m_exploding;
  GLclampf m_explosionTime;
  
public:
  ChunkBuffer()
  : m_curPlayerChunk(NULL), m_exploding(false), m_explosionTime(0.f)
  {
    Events::addListener((IPlayerMoveListener*)this);
    Events::addListener((IPlayerLookListener*)this);
    Events::addListener((IKeyDownListener*)this);
    ProcessList::add(this);
  }
  
  ~ChunkBuffer()
  {
    ProcessList::remove(this);
    Events::removeListener(EventPlayerMove, this);
  }
  
private:
  void removeChunksAtX(int xIndex, GLfloat deltaX);
  void removeChunksAtZ(int zIndex, GLfloat deltaZ);
  
public:
  void init();
  virtual void onKeyDown(int key, bool special);
  virtual void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  virtual void onPlayerLook(const Attitude &attitude, const glm::vec3 &lookVector, bool headlightOn);
  virtual void draw(Env &env);
  virtual void advance(int delta);
  virtual bool isDone();
  
  Chunk *chunkAt(int x, int z);
};

#endif