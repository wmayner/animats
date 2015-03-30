// Game.cpp

#include "Game.h"

// TODO(wmayner) figure out and document cruel black magic voodoo sorcery
#define KISSRND (                                                        \
    ((((rndZ = 36969 * (rndZ & 65535) + (rndZ >> 16)) << 16) +           \
      (rndW = 18000 * (rndW & 65535) + (rndW >> 16)) )                   \
     ^(rndY = 69069 * rndY + 1234567)) +                                 \
    (rndX ^= (rndX << 17), rndX ^= (rndX >> 13), rndX ^= (rndX << 5))    \
)
#define INTABS(number) (((((0x80) << ((sizeof(int) - 1) << 3)) & number) \
            ? (~number) + 1 : number))

#define randDouble ((double)rand() / (double)RAND_MAX)

int rndX, rndY, rndZ, rndW;


Game::Game(char* filename) {
    FILE *f = fopen(filename, "r+w");
    int i;
    patterns.clear();
    while (!feof(f)) {
        fscanf(f, "%i  ", &i);
        patterns.push_back(i & 65535);
    }
    fclose(f);
}

Game::~Game() {}

double Game::agentDependentRandDouble(void) {
    int A = KISSRND;
    return (double)((INTABS(A)) & 65535) / (double)65535;
}

int Game::agentDependentRandInt(void) {
    int A = KISSRND;
    return (INTABS(A));
}

void Game::applyNoise(Agent *agent, double sensorNoise) {
    // Larissa: If I don't have noise in evaluation, then I can just use random
    // numbers always.
    // if (agentDependentRandDouble() < sensorNoise) {
    if (randDouble < sensorNoise) {
        agent->states[0] = !agent->states[0];
    }
    // if (agentDependentRandDouble() < sensorNoise)
    if (randDouble < sensorNoise) {
        agent->states[1] = !agent->states[1];
    }
}

vector< vector<int> > Game::executeGame(Agent* agent, double sensorNoise, int repeat) {
    int world, agentPosition, blockPos, past_state, current_state;
    int patternIndex, direction, timestep, agentCell;
    int action;
    // This holds the state transitions over the agent's lifetime.
    vector< vector<int> > stateTransitions;
    stateTransitions.clear();
    stateTransitions.resize(2);
    // Make random seeds unique from one another by including index.
    rndW = agent->ID + repeat;
    rndX = ~(agent->ID + repeat);
    rndY = (agent->ID + repeat)^0b01010101010101010101010101010101;
    rndZ = (agent->ID + repeat)^0b10101010101010101010101010101010;

    agent->fitness = 1.0;
    agent->correct = agent->incorrect = 0;

    bool hit;

    agent->numCorrectByPattern.resize(patterns.size());

    // Record the number of correct outcomes for each different type of block
    for (int i = 0; i < agent->numCorrectByPattern.size(); i++) {
        agent->numCorrectByPattern[i] = 0;
    }

    // Block patterns
    for (patternIndex = 0; patternIndex < patterns.size(); patternIndex++) {
        // Directions (left/right)
        for (direction = -1; direction < 2; direction += 2) {
            // Agent starting position
            for (agentPosition = 0; agentPosition < 16; agentPosition++) {
                // Larissa: Change environment after 30,000 Gen, if patterns is
                // 1 7 15 3 it changes from 2 blocks with 1 7 to 4 blocks with
                // 1 7 15 3

                // TODO(wmayner) add logic outside of Game to change the
                // patterns mid-evolution

                world = patterns[patternIndex];

                agent->resetBrain();

                blockPos = 0;

                // World loop
                for (timestep = 0; timestep < numWorldTimesteps; timestep++) {
                    // AH: Sensors have no noise in them now.
                    agent->states[0] = (world >> agentPosition) & 1;
                    agent->states[1] = (world >> ((agentPosition + 2) & 15)) & 1;

                    // TODO(wmayner) parameterize changing sensors mid-evolution
                    // Larissa: Set to 0 to evolve agents with just one sensor.

                    // AH: Apply noise does apply noise to them now.
                    applyNoise(agent, sensorNoise);
                    // Set motors to 0 to prevent reading from them.
                    agent->states[6] = 0; agent->states[7] = 0;

                    past_state = 0;
                    for (int n = 0; n < 8; n++) {
                        // Set the nth bit to the nth node's state
                        past_state |= (agent->states[n] & 1) << n;
                    }
                    stateTransitions[0].push_back(past_state);

                    agent->updateStates();

                    current_state = 0;
                    for(int n = 0; n < 8; n++) {
                        // Set the nth bit to the nth node's state
                        current_state |= (agent->states[n] & 1) << n;
                    }
                    stateTransitions[1].push_back(current_state);

                    // TODO(wmayner) parameterize this
                    // Larissa: limit to one motor.
                    // agent->states[7]=0;
                    // if (agent->born < nowUpdate) {
                    //     agent->states[7] = 0;
                    // }
                    action = agent->states[6] + (agent->states[7] << 1);

                    // Move agent
                    // Larissa: this makes the agent stop moving:
                    // action = 0;
                    switch (action) {
                        // No motors on.
                        case 0:
                            // Don't move.
                            break;
                        // Both motors on.
                        case 3:
                            // Don't move.
                            break;
                        // Left motor on.
                        case 1:
                            // Move right.
                            // TODO(wmayner) replace with constant
                            agentPosition = (agentPosition + 1) & 15;
                            break;
                        // Right motor on.
                        case 2:
                            // Move left.
                            // TODO(wmayner) replace with constant
                            agentPosition = (agentPosition - 1) & 15;
                            break;
                    }

                    // Move the block.
                    if (direction == -1) {
                        // Left.
                        world = ((world >> 1) & 65535) + ((world & 1) << 15);
                    } else {
                        // Right.
                        world = ((world << 1) & 65535) + ((world >> 15) & 1);
                    }
                }

                // Check for hit.
                hit = false;
                for (agentCell = 0; agentCell < 3; agentCell++) {
                    if (((world >> ((agentPosition + agentCell) & 15)) & 1) == 1) {
                        hit = true;
                    }
                }

                // Update fitness.
                if ((patternIndex & 1) == 0) {
                    if (hit) {
                        agent->correct++;
                        agent->fitness *= fitnessMultiplier;
                        agent->numCorrectByPattern[patternIndex]++;
                    } else {
                        agent->fitness /= fitnessMultiplier;
                        agent->incorrect++;
                    }
                } else {
                    if (hit) {
                        agent->incorrect++;
                        agent->fitness /= fitnessMultiplier;
                    } else {
                        agent->correct++;
                        agent->fitness *= fitnessMultiplier;
                        agent->numCorrectByPattern[patternIndex]++;
                    }
                }
            }  // Agent starting position
        }  // Directions
    }  // Block patterns
    return stateTransitions;
}  // executeGame
