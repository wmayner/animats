// constants.h

#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#define VIRTUALENV_PATH "/Users/will/.virtualenvs/matching/lib/python3.4/site-packages"

// Analysis parameters
#define LOD_RECORD_INTERVAL 100

// Agent parameters
// TODO(wmayner) make sure this is used everywhere
#define NUM_NODES 8
#define NUM_STATES (1 << NUM_NODES)
#define DETERMINISTIC true

// World parameters
#define WORLD_HEIGHT 36
#define WORLD_WIDTH 16
#define SCRAMBLE_WORLD false

// Evolution parameters
#define FITNESS_MULTIPLIER 1.01

#define randDouble ((double)rand() / (double)RAND_MAX)

#endif  // SRC_CONSTANTS_H_
