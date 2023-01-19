#include "model.h"

void PrintMap()
{
    printf("Printing map [%d, %d]\n", map.height, map.width);
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
            printf("%d ", map.data[y][x].type);
        printf("\n");
    }
}

const char *getfiekd(char *line, int num)
{
    const char *tok;
    for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n"))
    {
        if (!--num)
        {
            return tok;
        }
    }
    return NULL;
}

// Converts a CSV file to a 2D int array

/*
void parse(char* path)
{
    struct stat stats;
    stat(path, &stats);
    printf("  path %s\n",path);
    FILE* f = fopen(path, "r");
    assert(f); //to check if the path is correct
    char buf[MAX_ROOM_HEIGHT * MAX_ROOM_LENGTH * 2 + MAX_COMMENT_LENGTH]; // * 2 because of commas
    fread(buf, sizeof(buf[0]), stats.st_size, f);
    fclose(f);

    int start_index = 0;
    if (buf[0] == '#') {
        for (start_index = 0; buf[start_index] != '\n' ; ++start_index)
            ;
        ++start_index; //next symbol after #bla-bla-bla\n
    }

    map.height = 0;
    int x = 0;
    for (int i = start_index; i < stats.st_size; ++i) {
        if (buf[i] == '\n') {
            ++map.height;
            map.width = x;
            x = 0;
            continue;
        }
        if (isdigit(buf[i])) {
            map.data[map.height][x] = buf[i] - '0';
            ++x;
        }
    }
}*/

void RoomAddRobot(struct Room *this, struct Robot *r)
{
    this->data[r->y][r->x].robot = r;
}

void RoomInit(struct Room *this)
{
    this->height = 0;
    this->width = 0;
    this->box_number = 0;
    this->receiver_number = 0;
}

void RoomInitCell(struct Room *this, int i, int j, int tile_type)
{
    if (tile_type > 4)
    {
        // that's a drop box
        this->data[i][j].type = 5;
        this->drop_tiles[tile_type - 5] = &(this->data[i][j]);
        ++(this->box_number);
    }
    else if (tile_type == 4)
    {
        this->data[i][j].type = tile_type;
        this->receiver_tiles[this->receiver_number] = &(this->data[i][j]);
        ++(this->receiver_number);
    }
    else
    {
        this->data[i][j].type = tile_type;
    }
}

char CONFIG_SIM_NAME[MAX_STRING_LEN] = "";
char CONFIG_MAP_PATH[MAX_STRING_LEN] = "";
char CONFIG_DROP_BOX_PATH[MAX_STRING_LEN] = "";
char CONFIG_ROBOT_PATH[MAX_STRING_LEN] = "";
char CONFIG_PACKAGE_PATH[MAX_STRING_LEN] = "";

int8_t CONFIG_MODEL_MOD = 0;
char CONFIG_LOGGING_PATH [MAX_STRING_LEN] = "";
int8_t CONFIG_LOGGING_MOD = 0;

void read_config(char *path)
{
    printf("Loading config file\n");
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        printf("Can't read configuration file %s", path);
        fclose(f);
        return 1;
    }
    char buf[MAX_STRING_LEN];
    fscanf(f, "CONFIG_SIM_NAME: %s\n", CONFIG_SIM_NAME);           // string name to use in logs
    fscanf(f, "CONFIG_MAP_PATH: %s\n", CONFIG_MAP_PATH);           // absolute path
    fscanf(f, "CONFIG_DROP_BOX_PATH: %s\n", CONFIG_DROP_BOX_PATH); // absolute path
    fscanf(f, "CONFIG_ROBOT_PATH: %s\n", CONFIG_ROBOT_PATH);       // absolute path
    fscanf(f, "CONFIG_PACKAGE_PATH: %s\n", CONFIG_PACKAGE_PATH);   // absolute path
    fscanf(f, "CONFIG_MODEL_MOD: %d\n", &CONFIG_MODEL_MOD);         // uselell right now
    fscanf(f, "CONFIG_LOGGING_PATH: %s\n", CONFIG_LOGGING_PATH);   // absolute path
    fscanf(f, "CONFIG_LOGGING_MOD: %s\n", buf);                    // t for true, f for false
    if (strcmp(buf, "f") == 0 && strcmp(buf, "t") == 0)
    {
        printf("Can't read configuration file: incorrect field\nCONFIG_LOGGING_MOD must be t or f, while your's is %s\n", buf);
        fclose(f);
        return 1;
    }
    CONFIG_LOGGING_MOD = (strcmp(buf, "t"));
    fclose(f);

    printf("Loaded config with:\n\tmap: %s\n\trobots: %s\n", CONFIG_MAP_PATH, CONFIG_ROBOT_PATH);
}

void read_robots(char *path)
{
    /*
    input: path to robots configuration file
        csv format, much alike with map configuration - a table of numbers of size MAP_WIDTH x MAP_HEIGHT
        every number is from 0 to 3 if theres's a robot, 0 - north, 1 - east, 2 - south, 3 - west, any other number stands for nothing
    Be aware that this function considers that all the main global fields of simulation have been filled
    So the config file and map config file must be read before that (read_config and read_map functions)
    */
    printf("Loading robots...\n");
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        printf("Can't read robot config file %s", path);
        fclose(f);
        exit(1);
    }
    int direction;
    int robot_id = 0;
    for (int i = 0; i < map.width; ++i)
    {
        for (int j = 0; j < map.height; ++j)
        {
            if (fscanf(f, "%d", &direction) == 0)
            {
                printf("Can't load robots, configuration file doesn't fit the map");
                fclose(f);
                exit(1);
            }
            if (direction < 4)
            {
                robots.data[robot_id].direction = direction;
                robots.data[robot_id].x = i;
                robots.data[robot_id].y = j;
                RoomAddRobot(&map, &(robots.data[robot_id]));
                ++robot_id;
            }
        }
    }
    robots.N = robot_id;
    printf("Loaded robots, total: %d\n", robots.N);
    fclose(f);
}

void read_map(char *path)
{
    /*
    input: path to robots configuration file
        first line is like:  MAP_WIDTH : ...
        second line is like: MAP_HEIGHT : ...
        then there're MAP_HEIGHT lines, each line has MAP_WIDTH numbers each of them are TILE_NUMBERS
        TILE_NUMBER is one of
                0-empty place,
                1-wall,
                2-sending tile, (tile from which the package is sent)
                3-charger tile,
                4-receiver tile (tile from which the package is received)
        >=5 - a drop point itself, stores number = (direction id + 5)
    */
    printf("Loading map...\n");
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        printf("Can't read map config file %s", path);
        fclose(f);
        exit(1);
    }
    RoomInit(&map);

    if (!fscanf(f, "MAP_WIDTH: %d\n", &map.width) || !fscanf(f, "MAP_HEIGHT: %d\n", &map.height))
    {
        printf("Can't read map config file, incorrect header");
        fclose(f);
        exit(1);
    }
    printf("map width: %d, map height: %d\n", map.width, map.height);
    int type_id;
    for (int i = 0; i < map.width; ++i)
    {
        for (int j = 0; j < map.height; ++j)
        {
            if (fscanf(f, "%d", &type_id) == 0)
            {
                printf("Can't init map, wrong data");
                fclose(f);
                exit(1);
            }
            RoomInitCell(&map, i, j, type_id);
        }
    }
    printf("Map Loaded\n");
}

void read_cargo(char *path)
{
    printf("Loading cargo config...\n");

    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Can not find distr.txt\n");
        fclose(fp);
    }

    int cargo = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int id = 0;
    cargo_gen.cargo_total = 0;
    cargo_gen.receivers_total = map.receiver_number;
    while (fscanf(fp, "%02d:%02d:%02d %d", &hour, &minute, &second, &cargo) == 4)
    {
        if (id >= MAX_CARGO_NUMBER)
        {
            printf("Warning: cargo config has too many writings or receivers, reading stopped\n");
            break;
        }

        cargo_gen.cargo_timetable[id][0] = second + minute * 60 + hour * 60 * 60;
        cargo_gen.cargo_timetable[id][1] = cargo;
        ++id; // THE RULE OF EVERY C PROGRAMMIST, do NOT write var++
    }
    cargo_gen.cargo_total = id;

    fclose(fp);

    printf("Cargo init (randomly deviding between receivers)\n");
    srand(20);

    for (int i = 0; i < cargo_gen.receivers_total; ++i)
    {
        cargo_gen.cargos_current[i] = -1;
    }

    
    for (int i = 0; i < cargo_gen.cargo_total; ++i)
    {
        id = rand() % cargo_gen.receivers_total;
        cargo_gen.receiver_timetable[id][++cargo_gen.cargos_current[id]] = i;
    }

    for (int i = 0; i < 5; ++i)
    {
        printf("\treceiver 0; time: %4d; num: %2d\n",
               cargo_gen.cargo_timetable[i][0], cargo_gen.cargo_timetable[i][1]);
    }
    
}

void RobotsPrint()
{
    for (int i = 0; i < robots.N; ++i)
    {
        printf("Robot #%d is located at (%d, %d) and is facing ",
               i + 1, robots.data[i].x, robots.data[i].y);

        switch (robots.data[i].direction)
        {
        case UP:
            printf("UP\n");
            break;
        case DOWN:
            printf("DOWN\n");
            break;
        case LEFT:
            printf("LEFT\n");
            break;
        case RIGHT:
            printf("RIGHT\n");
            break;
        }
    }
}
