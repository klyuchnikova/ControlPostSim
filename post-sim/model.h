//The header file template for a ROSS model
//This file includes:
// - the state and message structs
// - extern'ed command line arguments
// - custom mapping function prototypes (if needed)
// - any other needed structs, enums, unions, or #defines

#ifndef _model_h
#define _model_h

#include "ross.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <assert.h>


#define CARGO_PLACE_MAX_NUM 200
#define CARGO_PLACE_MIN_NUM 1
#define MAX_ROOM_HEIGHT 40  // actually 35
#define MAX_ROOM_LENGTH 50  // actually 50

#define MAX_STRING_LEN 80
#define MAX_COMMAND_LENGTH 100  // commands for one robot
#define MAX_PATH_STRING_LENGTH 100
#define MAX_DROP_BOXES 150
#define MAX_QUEUE_LENGTH 50
#define MAX_RECEIVERS 10


extern char CONFIG_SIM_NAME [MAX_STRING_LEN];
extern char CONFIG_MAP_PATH [MAX_STRING_LEN];
extern char CONFIG_DROP_BOX_PATH[MAX_STRING_LEN];
extern char CONFIG_ROBOT_PATH [MAX_STRING_LEN];
extern char CONFIG_PACKAGE_PATH [MAX_STRING_LEN];
extern int8_t CONFIG_MODEL_MOD;
extern char CONFIG_LOGGING_PATH [MAX_STRING_LEN];
extern int8_t CONFIG_LOGGING_MOD;


// All times in second
extern double g_robot_calc_time; //= 0.001;
extern double g_robot_wait_time; //= 0.5;
extern double g_robot_move_time; //= 0.5;
extern double g_robot_round_time; // = 3;
extern double g_robot_load_time;// = 5;
extern double g_robot_unload_time;// = 5;
extern double g_robot_charge_time;// = 12 * 60;
extern double g_tick_duration;// = 1;

// All distances in meters
const static double g_robot_max_mileage;// = 12 * 60 * 10;
const static double g_robot_min_mileage;// = 12 * 60 * 5;

#define MAX_CARGO_NUMBER 10000 // maybe not that much
#define NO_CARGO 6666 // ???


#define CELL_ROBOT_UP    8
#define CELL_ROBOT_RIGHT 6
#define CELL_ROBOT_LEFT  4
#define CELL_ROBOT_DOWN  2

#define CELL_WALL  1
#define CELL_EMPTY 0

#define MAX_COMMENT_LENGTH 1000
#define MAX_ROBOTS 100

typedef enum {
  UP    = 0,
  DOWN  = 1,
  LEFT  = 2,
  RIGHT = 3,
} Direction;

struct Robot {
    int x;
    int y;
    Direction direction;  
    int package_id; // id_in_line
};

typedef enum {
  EMPTY,
  WALL,
  SENDER,
  CHARGER, 
  RECEIVER,
  DROP
} TileType; 

struct Cell {
    int x;
    int y;
    TileType type;
    struct Robot* robot;
};

struct Robots {
    struct Robot data[MAX_ROBOTS];
    int N;
};

struct Room {
    
    int height;
    int width;

    int box_number;
    int receiver_number;

    struct Cell data[MAX_ROOM_HEIGHT][MAX_ROOM_LENGTH];
    struct Cell * drop_tiles [MAX_DROP_BOXES]; // drop id -> tile
    struct Cell * receiver_tiles [MAX_DROP_BOXES]; // receiver_id -> tile
};

typedef struct CargoGenerator
{
  // cargo_timetable[i][0] - arrival of cargo in offset from simulation start
  // cargo_timetable[i][1] - id of direction to send cargo
  int cargo_timetable [MAX_CARGO_NUMBER][2]; // cargo_id -> [time, direction_id]
  int receiver_timetable [MAX_RECEIVERS][MAX_CARGO_NUMBER]; // [receiver_id][order_id] -> cargo_id 
  int cargos_current [MAX_RECEIVERS]; // [receiver_id] -> current order_id
  int receivers_total;
  int cargo_total;
};

struct Room map;
struct Robots robots;
struct CargoGenerator cargo_gen;

typedef enum {
    FALSE = 0,
    TRUE  = 1,
} boolean;


//Example enumeration of message type... could also use #defines
typedef enum {
  ROTATE_LEFT,
  ROTATE_RIGHT,
  MOVE,
  BOX_GRAB,
  BOX_DROP,
  RECEIVED,
  EXECUTED,
  INIT,
} message_type;

typedef enum {
  COMMAND_CENTER,
  ROBOT,
} lp_type;

//Message struct
//   this contains all data sent in an event
typedef struct {
  message_type type;
  double contents;
  tw_lpid sender;
} message;

//State struct
//   this defines the state of each LP
typedef struct {
  int got_msgs_ROTATE_LEFT;
  int got_msgs_ROTATE_RIGHT;
  int got_msgs_MOVE;
  int got_msgs_BOX_GRAB;
  int got_msgs_BOX_DROP;
  int got_msgs_RECEIVED;
  int got_msgs_EXECUTED;
  int got_msgs_INIT;

  int sent_msgs_ROTATE_LEFT;
  int sent_msgs_ROTATE_RIGHT;
  int sent_msgs_MOVE;
  int sent_msgs_BOX_GRAB;
  int sent_msgs_BOX_DROP;
  int sent_msgs_RECEIVED;
  int sent_msgs_EXECUTED;
  int sent_msgs_INIT;

  lp_type type; 
  double value;
  Direction direction;  

  int x;
  int y;

} state;


//Command Line Argument declarations
extern unsigned int setting_1;

//Global variables used by both main and driver
// - this defines the LP types
extern tw_lptype model_lps[];

//Function Declarations
// defined in model_driver.c:
extern void model_init(state *s, tw_lp *lp);
extern void model_event(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_event_reverse(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_final(state *s, tw_lp *lp);
// defined in model_map.c:
extern tw_peid model_map(tw_lpid gid);
extern tw_lpid model_typemap (tw_lpid gid);

/*
//Custom mapping prototypes
void model_cutom_mapping(void);
tw_lp * model_mapping_to_lp(tw_lpid lpid);
tw_peid model_map(tw_lpid gid);
*/

// extern void parse(char* path);
extern void read_config(char* path);
extern void read_robots(char* path);
extern void read_map(char* path);
extern void read_cargo(char* path);

extern void PrintMap();
extern void RobotsPrint();

#endif
