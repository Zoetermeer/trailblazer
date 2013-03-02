#include "player.hpp"
#include <stdio.h>
#include <math.h>
#include "events.hpp" 

void move(player_t *player, GLfloat distance, Direction dir)
{
  GLfloat ang;
  GLfloat dist;
  bool northSouth = false;
  glm::vec4 old = player->offset;
  switch (dir) 
  {
    case DirectionForward:
      northSouth = true;
      break;
    case DirectionBackward:
      northSouth = true;
      distance = -distance;
      break;
    case DirectionRight:
      ang = nextAngle(player->yaw, 90.0);
      break;
    case DirectionLeft:
      ang = nextAngle(player->yaw, -90.0);
      break;
    case DirectionIdle:
      break;
  }

  if (northSouth) {
    player->heading = player->yaw;
    ang = player->heading;
  }

  player->offset.x += distance * -sin(DEG_TO_RAD(ang));
  player->offset.z += distance * cos(DEG_TO_RAD(ang));

#ifdef PRINT_PLAYER_LOCATION
  printf("pos: (%f, %f, %f), heading=%f, rotation=%f\n",
          player->offset.x, 
          player->offset.y, 
          player->offset.z, 
          player->heading, 
          player->yaw);
#endif
}
