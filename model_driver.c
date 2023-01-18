//The C driver file for a ROSS model
//This file includes:
// - an initialization function for each LP type
// - a forward event function for each LP type
// - a reverse event function for each LP type
// - a finalization function for each LP type

//Includes
#include <stdio.h>

#include "ross.h"
#include "model.h"

//Helper Functions
void SWAP (double *a, double *b) {
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

int dest = 1;

//Init function
// - called once for each LP
// ! LP can only send messages to itself during init !
void model_init (state *s, tw_lp *lp) {
  if (lp->gid == 0) { //same as mapping (check)
    s->type = COMMAND_CENTER;
    printf("COMMAND_CENTER is initialized\n");
  } else {
    s->type = ROBOT;
    assert(lp->gid <= robots.N);
    
    s->x         = robots.data[lp->gid - 1].x;
    s->y         = robots.data[lp->gid - 1].y;
    s->direction = robots.data[lp->gid - 1].direction;
    
    printf("ROBOT #%ld is initialized\n", lp->gid);
  }

  int self = lp->gid;

  // init state data
  s->value = -1;

  s->got_msgs_ROTATE_LEFT   = 0;
  s->got_msgs_ROTATE_RIGHT  = 0;
  s->got_msgs_MOVE          = 0;
  s->got_msgs_BOX_GRAB      = 0;
  s->got_msgs_BOX_DROP      = 0;
  s->got_msgs_RECEIVED      = 0;
  s->got_msgs_EXECUTED      = 0;
  s->got_msgs_INIT          = 0;

  s->sent_msgs_ROTATE_LEFT  = 0;
  s->sent_msgs_ROTATE_RIGHT = 0;
  s->sent_msgs_MOVE         = 0;
  s->sent_msgs_BOX_GRAB     = 0;
  s->sent_msgs_BOX_DROP     = 0;
  s->sent_msgs_RECEIVED     = 0;
  s->sent_msgs_EXECUTED     = 0;
  s->sent_msgs_INIT         = 0;

  // Init message to myself
  tw_event* e = tw_event_new(self, 1, lp);
  message* msg = tw_event_data(e);
  switch(s->type)
  {
    case COMMAND_CENTER:
        msg->type = INIT;
        break;
    case ROBOT:
        msg->type = INIT;
        break;
    default:
        printf("Invalid s->type = %d\n", s->type);
  }
  msg->contents = tw_rand_unif(lp->rng);
  msg->sender = self;
  tw_event_send(e);
  ++s->sent_msgs_INIT; 
}

//Forward event handler
void model_event (state* s, tw_bf* bf, message* in_msg, tw_lp* lp) {
  int self = lp->gid;
  boolean is_executed = FALSE;
  // initialize the bit field
  *(int*)bf = (int)0;

  // update the current state
  // however, save the old value in the 'reverse' message
  SWAP(&(s->value), &(in_msg->contents));

  // handle the message
  switch(s->type)
  {
    case COMMAND_CENTER:
      switch (in_msg->type)
      {
        case RECEIVED:
          ++s->got_msgs_RECEIVED; 
          break;
        case EXECUTED:
          ++s->got_msgs_EXECUTED; 
          break;
        case INIT:
          ++s->got_msgs_INIT; 
          break;
        default:
          printf("Unhandeled forward message type %d\n", in_msg->type);
      }

      if (in_msg->type != EXECUTED) {
          if      (dest == 1) dest = 2;
          else if (dest == 2) dest = 1;
          else abort();
          
          tw_event* cc_e = tw_event_new(dest, 1, lp);
          message* cc_msg = tw_event_data(cc_e);
         
          cc_msg->type = MOVE;
          ++s->sent_msgs_MOVE;
          
          cc_msg->contents = tw_rand_unif(lp->rng);
          cc_msg->sender = self;
          tw_event_send(cc_e);
      }

      break;

    case ROBOT:
      switch (in_msg->type)
      {
        case ROTATE_LEFT:
          ++s->got_msgs_ROTATE_LEFT; 
          break;
        case ROTATE_RIGHT:
          ++s->got_msgs_ROTATE_RIGHT; 
          break;
        case MOVE: 
          ++s->got_msgs_MOVE; 
          struct Cell dest_cell;
          struct Robot* This = &robots.data[self-1];
          switch(This->direction)         
          {
              case UP:
                  assert(This->y - 1 >= 0);
                  dest_cell.x     = This->x    ;
                  dest_cell.y     = This->y - 1;
                  break;
              case DOWN:
                  assert(This->y + 1 <= map.height - 1);
                  dest_cell.x     = This->x    ;
                  dest_cell.y     = This->y + 1;
                  break;
              case LEFT:
                  assert(This->x - 1 >= 0);
                  dest_cell.x     = This->x - 1;
                  dest_cell.y     = This->y    ;
                  break;    
              case RIGHT:
                  assert(This->x + 1 <= map.width - 1);
                  dest_cell.x     = This->x + 1;
                  dest_cell.y     = This->y    ;
                  break;
          }

          dest_cell = map.data[dest_cell.y][dest_cell.x];

          
          if (dest_cell.robot == NULL) {
            map.data[dest_cell.y][dest_cell.x].robot = This;
            map.data[This->y][This->x].robot = NULL;
            This->x = dest_cell.x;
            This->y = dest_cell.y;
            is_executed = TRUE;
            PrintMap();
            RobotsPrint();
          }

          break;
        case BOX_GRAB:
          ++s->got_msgs_BOX_GRAB; 
          break;
        case BOX_DROP:
          ++s->got_msgs_BOX_DROP; 
          break;
        case INIT:
          ++s->got_msgs_INIT; 
          break;    
        default:
          printf("Unhandeled forward message type %d\n", in_msg->type);
      }

      if (in_msg->sender == 0) //the message came from the command center
                               //so it's not INIT
      {
        tw_event* r_e1 = tw_event_new(0, 1, lp); // COMMAND_CENTER's gid is 0
        message* r_msg1 = tw_event_data(r_e1);
        r_msg1->type = RECEIVED;
        r_msg1->contents = tw_rand_unif(lp->rng);
        r_msg1->sender = self;
        tw_event_send(r_e1);
        ++s->sent_msgs_RECEIVED;

        if (is_executed == TRUE) {
            tw_event* r_e2 = tw_event_new(0, 2, lp); // COMMAND_CENTER's gid is 0
            message* r_msg2 = tw_event_data(r_e2);
            r_msg2->type = EXECUTED;
            r_msg2->contents = tw_rand_unif(lp->rng);
            r_msg2->sender = self;
            tw_event_send(r_e2);
            ++s->sent_msgs_EXECUTED;
        }

      }

      break;
   } 
}

//Reverse Event Handler
void model_event_reverse (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
/*  int self = lp->gid;

  // undo the state update using the value stored in the 'reverse' message
  SWAP(&(s->value), &(in_msg->contents));

  // handle the message
  switch (in_msg->type) {
    case HELLO :
    {
      s->rcvd_count_H--;
      break;
    }
    case GOODBYE :
    {
      s->rcvd_count_G--;
      break;
    }
    default :
      printf("Unhandeled reverse message type %d\n", in_msg->type);
  }

  // don't forget to undo all rng calls
  tw_rand_reverse_unif(lp->rng);  */
}

//report any final statistics for this LP
void model_final (state* s, tw_lp* lp)
{
  int self = lp->gid;
  if      (s->type == COMMAND_CENTER) {
    printf("COMMAND_CENTER: sent %d and got %d messages of type ROTATE_LEFT\n",\
            s->sent_msgs_ROTATE_LEFT, s->got_msgs_ROTATE_LEFT); 
    printf("                sent %d and got %d messages of type ROTATE_RIGHT\n",\
            s->sent_msgs_ROTATE_RIGHT, s->got_msgs_ROTATE_RIGHT);
    printf("                sent %d and got %d messages of type MOVE\n",\
            s->sent_msgs_MOVE, s->got_msgs_MOVE);
    printf("                sent %d and got %d messages of type BOX_GRAB\n",\
            s->sent_msgs_BOX_GRAB, s->got_msgs_BOX_GRAB);
    printf("                sent %d and got %d messages of type BOX_DROP\n",\
            s->sent_msgs_BOX_DROP, s->got_msgs_BOX_DROP);
    printf("                sent %d and got %d messages of type RECEIVED\n",\
            s->sent_msgs_RECEIVED, s->got_msgs_RECEIVED);
    printf("                sent %d and got %d messages of type EXECUTED\n",\
            s->sent_msgs_EXECUTED, s->got_msgs_EXECUTED);
    printf("                sent %d and got %d messages of type INIT\n",\
            s->sent_msgs_INIT, s->got_msgs_INIT);

  }
  else if (s->type == ROBOT) {  
    printf("ROBOT #%d:       sent %d and got %d messages of type ROTATE_LEFT\n",\
            self, s->sent_msgs_ROTATE_LEFT, s->got_msgs_ROTATE_LEFT);   
    printf("                sent %d and got %d messages of type ROTATE_RIGHT\n",\
            s->sent_msgs_ROTATE_RIGHT, s->got_msgs_ROTATE_RIGHT);
    printf("                sent %d and got %d messages of type MOVE\n",\
            s->sent_msgs_MOVE, s->got_msgs_MOVE);
    printf("                sent %d and got %d messages of type BOX_GRAB\n",\
            s->sent_msgs_BOX_GRAB, s->got_msgs_BOX_GRAB);
    printf("                sent %d and got %d messages of type BOX_DROP\n",\
            s->sent_msgs_BOX_DROP, s->got_msgs_BOX_DROP);
    printf("                sent %d and got %d messages of type RECEIVED\n",\
            s->sent_msgs_RECEIVED, s->got_msgs_RECEIVED);
    printf("                sent %d and got %d messages of type EXECUTED\n",\
            s->sent_msgs_EXECUTED, s->got_msgs_EXECUTED);
    printf("                sent %d and got %d messages of type INIT\n",\
            s->sent_msgs_INIT, s->got_msgs_INIT);

  }
}
