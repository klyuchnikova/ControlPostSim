// The C main file for a ROSS model
// This file includes:
//  - definition of the LP types
//  - command line argument setup
//  - a main function

// includes
#include "model.h"

// Define LP types
//   these are the functions called by ROSS for each LP
//   multiple sets can be defined (for multiple LP types)
tw_lptype model_lps[] = {
    {(init_f)model_init,
     (pre_run_f)NULL,
     (event_f)model_event,
     (revent_f)model_event_reverse,
     (commit_f)NULL,
     (final_f)model_final,
     (map_f)model_map,
     sizeof(state)},
    {0},
};

// Define command line arguments default values
unsigned int mod = 0;
char configuration_path[MAX_STRING_LEN] = "/home/ulyana/Desktop/Experiments/RossPostSim/model_input/WMS-config.xml";

// add your command line opts
const tw_optdef model_opts[] = {
    TWOPT_GROUP("ROSS Model"),
    TWOPT_CHAR("configuration_path", *(configuration_path), "[string] [nessesary] absolute path to configuration"),
    TWOPT_UINT("mod", mod, "[int] [[optional] mod of simulation run"),
    TWOPT_END(),
};

// for doxygen
#define model_main main

int model_main(int argc, char *argv[])
{
  tw_opt_add(model_opts);
  tw_init(&argc, &argv);

  read_config(configuration_path);
  read_map(CONFIG_MAP_PATH);
  //read_cargo(CONFIG_PACKAGE_PATH);
  read_robots(CONFIG_ROBOT_PATH);

  assert(robots.N);

  PrintMap();
  RobotsPrint();

  int i;
  int num_lps_per_pe;

  // Do some error checking?
  // Print out some settings?

  // Custom Mapping
  /*
  g_tw_mapping = CUSTOM;
  g_tw_custom_initial_mapping = &model_custom_mapping;
  g_tw_custom_lp_global_to_local_map = &model_mapping_to_lp;
  */

  // Useful ROSS variables and functions
  //  tw_nnodes() : number of nodes/processors defined
  //  g_tw_mynode : my node/processor id (mpi rank)

  // Useful ROSS variables (set from command line)
  //  g_tw_events_per_pe
  //  g_tw_lookahead
  //  g_tw_nlp
  //  g_tw_nkp
  //  g_tw_synchronization_protocol

  // assume 1 lp per node
  num_lps_per_pe = robots.N + 1; // n robots + command center

  // set up LPs within ROSS
  tw_define_lps(num_lps_per_pe, sizeof(message));
  // note that g_tw_nlp gets set here by tw_define_lps

  // IF there are multiple LP types
  //    you should define the mapping of GID -> lptype index
  g_tw_lp_typemap = &model_typemap;

  // set the global variable and initialize each LP's type
  //  g_tw_lp_types = model_lps;
  //  tw_lp_setup_types();

  // printf("g_tw_nlp == %ld\n", g_tw_nlp);
  for (int i = 0; i < g_tw_nlp; ++i)
    tw_lp_settype(i, &model_lps[0]);

  // Do some file I/O here? on a per-node (not per-LP) basis

  tw_run();

  tw_end();

  return 0;
}
