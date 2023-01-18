# My code
Firstly,
this code reads a table of numbers from a .csv file and fills a 2D array with these numbers. `void parse(char* path);` function from `model_parser.c` does this (`path` is the path to the .csv file). Global `struct Room map` is represented below.
```
struct Room {
	int data[MAX_ROOM_HEIGHT][MAX_ROOM_LENGTH];
	int height;
	int width;
};

struct Room map;
```
`parse()` is called from `main()` in `model_main.c` before the simulation starts. If the first line of the .csv file start with `#`, it is treated as a commentary and ignored. Macros `MAX_ROOM_HEIGHT`, `MAX_ROOM_LENGTH` and `MAX_COMMENT_LENGTH` are specified in  `model.h` .

Secondly, 
it reads info about, where the robots are and which direction they are facing initially. The following macros set the correspondence between the digits in the .csv file and their meaning.
```
#define CELL_ROBOT_UP    8
#define CELL_ROBOT_RIGHT 6
#define CELL_ROBOT_LEFT  4
#define CELL_ROBOT_DOWN  2

#define CELL_WALL  1
#define CELL_EMPTY 0
```
In the current simulation, there are two robots that move one after another. It creates a collision between the two. To avoid this, the robot that cannot move forward delays it's movement until the way clears out.

# Usage

If you are creating your own model feel free to fork this repository.
As you develop, please replace "model" with appropriately descriptive names for your variables, files, and functions.

# Installation

This model can be built by ROSS by sym-linking it into the ROSS/models directory and building with `-DROSS_BUILD_MODELS=ON`

``` shell
git clone https://github.com/ROSS-org/ROSS
git clone https://github.com/ROSS-org/template-model
cd ROSS/models
ln -s ../../template-model ./
cd ../
mkdir build
cd build 
cmake ../ -DROSS_BUILD_MODELS=ON
make
```
