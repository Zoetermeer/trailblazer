#ifndef CHUNK_BUFFER_H
#define CHUNK_BUFFER_H

#include <glm/glm.hpp>
#include <vector>
#include "scene-object.hpp"
#include "events.hpp"
#include "process.hpp"
#include "chunk.hpp"
#include <GL/glfw.h>

template <class TChunk>
class ChunkBuffer : public IPlayerMoveListener, public IPlayerLookListener, public IKeyDownListener, public Process, public SceneObject {
private:
  glm::ivec3 m_curPlayerChunkCoords;
  std::vector<TChunk*> m_loadQueue;
  std::vector<TChunk*> m_visibleQueue;
  TChunk *m_curPlayerChunk;
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
  void removeChunksAtX(int xIndex, GLfloat deltaX)
  {
    typename std::vector<TChunk*>::iterator iter = m_visibleQueue.begin();
    bool under = deltaX > 0.f;
    bool remove;
    while (iter != m_visibleQueue.end()) {
      TChunk *ch = *iter;
      remove = under ? ch->getIndex().x <= xIndex : ch->getIndex().x >= xIndex;
      if (remove) {
        iter = m_visibleQueue.erase(iter);
        delete ch;
        continue;
      }
      
      ++iter;
    }
  }
  
  void removeChunksAtZ(int zIndex, GLfloat deltaZ)
  {
    typename std::vector<TChunk*>::iterator iter = m_visibleQueue.begin();
    bool under = deltaZ > 0.f;
    bool remove;
    while (iter != m_visibleQueue.end()) {
      TChunk *ch = *iter;
      remove = under ? ch->getIndex().y <= zIndex : ch->getIndex().y >= zIndex;
      if (remove) {
        iter = m_visibleQueue.erase(iter);
        delete ch;
        continue;
      }
      
      ++iter;
    }
  }
  
public:
  void init()
  {
    //Load a VISIBLE_CHUNKS * VISIBLE_CHUNKS grid, centered on the world-space origin
    int halfDown = floor((GLfloat)VISIBLE_CHUNKS * .5);
    for (int i = -halfDown; i <= halfDown; i++) {
      for (int j = -halfDown; j <= halfDown; j++) {
        TChunk *ch = newChunk(i, 0, j);
        m_loadQueue.push_back(ch);
      }
    }
  }
  
  virtual void onKeyDown(int key, bool special)
  {
    if (m_exploding)
      return;
    
    switch (key)
    {
      case GLFW_KEY_ENTER:
        m_exploding = true;
        m_explosionTime = 0.f;
        break;
    }
  }
  
  virtual void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
  {
    m_playerPos = new_pos;
    glm::ivec3 chunkCoords = Chunk::worldToChunkSpace(glm::vec3(new_pos));
    if (chunkCoords.x != m_curPlayerChunkCoords.x || chunkCoords.z != m_curPlayerChunkCoords.z) {
      //Set the new chunk as the player's 'current' one
      TChunk *newCh = this->chunkAt(chunkCoords.x, chunkCoords.z);
      if (m_curPlayerChunk)
        m_curPlayerChunk->setContainsPlayer(false);
      
      if (newCh) {
        m_curPlayerChunk = newCh;
        newCh->setContainsPlayer(true);
      }
      
      glm::ivec3 delta = chunkCoords - m_curPlayerChunkCoords;
      int halfUp = ceil((GLfloat)VISIBLE_CHUNKS * .5);
      int halfDown = floor((GLfloat)VISIBLE_CHUNKS * .5);
      if (delta.x != 0) {
        //Remove in x dimension
        int x = chunkCoords.x - (delta.x * halfUp);
        removeChunksAtX(x, delta.x);
        for (int i = chunkCoords.z - halfDown; i <= chunkCoords.z + halfDown; i++) {
          m_loadQueue.push_back(newChunk(chunkCoords.x + (delta.x * halfDown), 0, i));
        }
      }
      
      if (delta.z != 0) {
        //Remove in z dimension
        int z = chunkCoords.z - (delta.z * halfUp);
        removeChunksAtZ(z, delta.z);
        for (int i = chunkCoords.x - halfDown; i <= chunkCoords.x + halfDown; i++) {
          m_loadQueue.push_back(newChunk(i, 0, chunkCoords.z + (delta.z * halfDown)));
        }
      }
      
      m_curPlayerChunkCoords = chunkCoords;
    }
  }
  
  virtual void onPlayerLook(const Attitude &attitude, const glm::vec3 &lookVector, bool headlightOn)
  {
    m_playerLookVector = lookVector;
    m_isPlayerHeadlightOn = headlightOn;
  }
  
  virtual void draw(Env &env)
  {
    //Update stats
    debug_stats &stats = env.getStats();
    stats.load_queue_size = m_loadQueue.size();
    stats.render_queue_size = m_visibleQueue.size();
    
    for (TChunk *ch : m_visibleQueue) {
      ch->draw(env,
               m_playerPos,
               m_playerLookVector,
               m_isPlayerHeadlightOn,
               m_exploding,
               m_explosionTime);
      
      CHECK_OPENGL_ERROR;
    }
  }
  
  virtual void advance(int delta)
  {
    //If the load list is non-empty, load 1 chunk
    if (m_loadQueue.size() > 0) {
      TChunk *ch = m_loadQueue.back();
      if (ch->generateDataAsync()) {
        m_loadQueue.pop_back();
        m_visibleQueue.push_back(ch);
      }
    }
    
    //If animating a voxel explosion, advance the time counter
    if (m_exploding) {
      m_explosionTime += .005;
      if (m_explosionTime > 1.f) {
        m_exploding = false;
        m_explosionTime = 0.f;
      }
    }
  }
  
  virtual bool isDone()
  {
    return false;
  }
  
  TChunk *newChunk(int x, int y, int z)
  {
    return new TChunk(x, y, z);
  }
  
  TChunk *chunkAt(int x, int z)
  {
    for (TChunk *ch : m_visibleQueue) {
      glm::ivec3 ind = ch->getIndex();
      if (ind.x == x && ind.z == z)
        return ch;
    }
    
    //TODO: Look in the load queue -- if not there,
    //then we need to create it
    return NULL;
  }
};

#endif