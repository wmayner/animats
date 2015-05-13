// constants.h

#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#define VIRTUALENV_PATH "/Users/will/.virtualenvs/matching/lib/python3.4/site-packages"

#define randDouble ((double)rand() / (double)RAND_MAX)

// Simulation parameters
// ---------------------
#define NUM_GENERATIONS 60000
#define NUM_AGENTS 100

// Evolution parameters
// --------------------
// Probability of each indiviudal nucleotide changed to random one
#define MUTATION_RATE 0.005
#define FITNESS_BASE 1.02

// World parameters
// ----------------
#define WORLD_HEIGHT 36
#define WORLD_WIDTH 16
#define SCRAMBLE_WORLD false

// Agent parameters
// ----------------
// TODO(wmayner) make sure this is used everywhere
#define NUM_NODES 8
#define NUM_STATES (1 << NUM_NODES)
#define DETERMINISTIC true

// Analysis parameters
// -------------------
#define LOD_RECORD_INTERVAL 512

#endif  // SRC_CONSTANTS_H_
