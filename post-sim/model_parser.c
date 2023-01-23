#include "model.h"

#include <libxml/parser.h>

int zoneSize = 0;
int unitSize = 0;
int safeUnitSize = 0;
float unitSpeed = 0;
float unitAccelerationTime = 0;
float unitStopTime = 0;
float unitRotateTime = 0;
float unitAccelerationEnergy = 0;
float unitMoveEnergy = 0;
float unitRotateEnergy = 0;
float loadTime = 0;
float unloadTime = 0;
float unitLoadEnergy = 0;
float unitUnloadEnergy = 0;
float unitWaitEnergy = 0;
float unitChargeTime = 0;
double unitChargeValue = 0;
float unitCount = 0;

char CONFIG_SIM_NAME[MAX_STRING_LEN] = "";
char CONFIG_MAP_PATH[MAX_STRING_LEN] = "";
char CONFIG_DROP_BOX_PATH[MAX_STRING_LEN] = "";
char CONFIG_ROBOT_PATH[MAX_STRING_LEN] = "";
char CONFIG_PACKAGE_PATH[MAX_STRING_LEN] = "";
char CONFIG_LOGGING_PATH[MAX_STRING_LEN] = "";

// not implemented at the moment
int8_t CONFIG_MODEL_MOD = 0;
int8_t CONFIG_LOGGING_MOD = 0;

void read_config(char *path)
{
    /*
    Loads xml file path to which is given in the argument. It is preferred to ALWAYS
    give absolute pathes, but if you are sure that the configurations will be loaded correctly the risk is all yours.
    So make sure you replace all the parameters with path in the xml. Also while loading only the types of
    arguments will be checked, so if there are some arguments missing they are gonna be filled
    with defalt values which may cause unexpected issues, so be carefull. I also renamed some global variables
    to keep the names which resemble the class names.
    */

    printf("Loading configuration file\n");

    xmlDoc *document;
    xmlNode *root, *first_child, *node;
    xmlChar *key;

    document = xmlReadFile(path, NULL, 0);

    if (document == NULL)
    {
        printf("Can't read configuration file: %s\n", path);
        assert(0); // instead of throw to force quit
    }

    root = xmlDocGetRootElement(document);
    assert(strcmp(root->name, "StorageConfig") == 0);
    first_child = root->children;
    for (node = first_child; node; node = node->next)
    {
        if (node->type == 1)
        {
            key = xmlNodeGetContent(node);
            if (strcmp(node->name, "zoneSize") == 0)
            {
                assert(sscanf((char *)key, "%d", &zoneSize));
            }
            else if (strcmp(node->name, "unitSize") == 0)
            {
                assert(sscanf((char *)key, "%d", &unitSize));
            }
            else if (strcmp(node->name, "safeUnitSize") == 0)
            {
                assert(sscanf((char *)key, "%d", &safeUnitSize));
            }
            else if (strcmp(node->name, "unitSpeed") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitSpeed));
            }
            else if (strcmp(node->name, "unitAccelerationTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitAccelerationTime));
            }
            else if (strcmp(node->name, "unitStopTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitStopTime));
            }
            else if (strcmp(node->name, "unitRotateTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitRotateTime));
            }
            else if (strcmp(node->name, "unitAccelerationEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitAccelerationEnergy));
            }
            else if (strcmp(node->name, "unitMoveEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitMoveEnergy));
            }
            else if (strcmp(node->name, "unitRotateEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitRotateEnergy));
            }
            else if (strcmp(node->name, "loadTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &loadTime));
            }
            else if (strcmp(node->name, "unloadTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &unloadTime));
            }
            else if (strcmp(node->name, "unitLoadEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitLoadEnergy));
            }
            else if (strcmp(node->name, "unitUnloadEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitUnloadEnergy));
            }
            else if (strcmp(node->name, "unitWaitEnergy") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitWaitEnergy));
            }
            else if (strcmp(node->name, "unitChargeTime") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitChargeTime));
            }
            else if (strcmp(node->name, "unitChargeValue") == 0)
            {
                assert(sscanf((char *)key, "%lf", &unitChargeValue));
            }
            else if (strcmp(node->name, "unitCount") == 0)
            {
                assert(sscanf((char *)key, "%f", &unitCount));
            }
            else if (strcmp(node->name, "storageLayout") == 0)
            {
                assert(sscanf((char *)key, "%s", CONFIG_MAP_PATH));
            }
            else if (strcmp(node->name, "targetLayout") == 0)
            {
                assert(sscanf((char *)key, "%s", CONFIG_DROP_BOX_PATH));
            }
            else if (strcmp(node->name, "botLayout") == 0)
            {
                assert(sscanf((char *)key, "%s", CONFIG_ROBOT_PATH));
            }
            else if (strcmp(node->name, "logPath") == 0)
            {
                assert(sscanf((char *)key, "%s", CONFIG_LOGGING_PATH));
            }
            else if (strcmp(node->name, "simName") == 0)
            {
                assert(sscanf((char *)key, "%s", CONFIG_SIM_NAME));
            }
        }
    }
    printf("Configuration loaded\n");
}

void RoomAddRobot(struct Room *this, struct Robot *r)
{
    assert(r->charge >= 0 && r->charge <= unitChargeValue);
    assert(this->data[r->y][r->x].type != WALL);
    assert(!this->data[r->y][r->x].robot);
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

void read_robots(char *path)
{
    /*
    input: path to robots configuration file
        xml format, every robot has two coordinates, charge level and direction
        direction: every number is from 0 to 3 if theres's a robot, 0 - north, 1 - east, 2 - south, 3 - west
    Be aware that this function considers that all the main global fields of simulation have been filled
    So the config file and map config file must be read before that (read_config and read_map functions)
    The function WILL check that the charge level is correct and that robot is not standing on a non-empty tile
    */

    printf("Loading robots\n");
    xmlDoc *document;
    xmlNode *root, *first_child, *node;
    xmlChar *key;

    document = xmlReadFile(path, NULL, 0);

    if (document == NULL)
    {
        printf("Can't read configuration file: %s\n", path);
        assert(0); // instead of throw to force quit
    }

    int robot_number = 0;
    int robot_id = 0;
    int direction = 0;
    int x = 0, y = 0;
    float charge = 0;

    root = xmlDocGetRootElement(document);
    assert(strcmp(root->name, "RobotConfig") == 0);
    first_child = root->children;
    for (node = first_child; node; node = node->next)
    {
        if (node->type == 1 && strcmp(node->name, "robot") == 0)
        {
            x = -1;
            y = -1;
            direction = -1;
            charge = -1;
            
            key = xmlNodeGetContent(node);
            assert(sscanf((char *)key, "%d", &robot_id));
            assert(robot_id < MAX_ROBOTS);

            for (xmlAttr *attribute = node->properties; attribute; attribute = attribute->next)
            {
                xmlChar *value = xmlNodeListGetString(node->doc, attribute->children, 1);
                if (strcmp(attribute->name, "x") == 0) {
                    assert(sscanf(value, "%d", &x));
                } else if (strcmp(attribute->name, "y") == 0) {
                    assert(sscanf(value, "%d", &y));
                } else if (strcmp(attribute->name, "direction") == 0) {
                    assert(sscanf(value, "%d", &direction));
                } else if (strcmp(attribute->name, "charge") == 0) {
                    assert(sscanf(value, "%f", &charge));
                }
            }

            robots.data[robot_id].x = x;
            robots.data[robot_id].y = y;
            robots.data[robot_id].direction = direction;
            robots.data[robot_id].charge = charge;
            RoomAddRobot(&map, &(robots.data[robot_id]));
            ++robot_number;
        }
    }
    robots.N = robot_number;
    printf("Loaded robots, total: %d\n", robots.N);
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
    printf("Map width: %d, map height: %d\n", map.width, map.height);
    int type_id;
    for (int i = 0; i < map.height; ++i)
    {
        for (int j = 0; j < map.width; ++j)
        {
            if (fscanf(f, "%d", &type_id) == 0)
            {
                printf("Can't init map, wrong data\n");
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