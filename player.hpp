#ifndef PLAYER_H
#define PLAYER_H

#include "common.hpp"

typedef enum Direction {
  DirectionIdle = 0, 
  DirectionForward = 1, 
  DirectionBackward = 2, 
  DirectionLeft = 4, 
  DirectionRight = 8
} Direction;

/* The player's position can be 
   obtained by rotating world space 
   by 'heading' degrees, then 
   translating by 'offset'.
   'pitch' is the rotation angle about a Y-axis centered 
   on the player. 
   'yaw' is the rotation angle about an X-axis centered 
   on the player (assuming the player is looking down the 
   negative Z-axis). */
typedef struct player {
  glm::vec4 offset;
  GLfloat heading;
  GLfloat pitch;
  GLfloat roll;
  GLfloat yaw;
} player_t;

void move(player_t *player, GLfloat distance, Direction dir);

#endif
