CPP=c++
# Use `python3.4-config --ldflags` and `python3.4-config --cflags` to get the
# flags needed to use Python from within C++.
CPPFLAGS=$(shell python3.4-config --cflags)
LDFLAGS=$(shell python3.4-config --ldflags)

SRC=src/*.cpp
BUILD=build/main.o

# Compile everything.
all:
	$(CPP) $(SRC) -o $(BUILD) $(CPPFLAGS) $(LDFLAGS)

clean:
	$(RM) $(BUILD)


SEED=0
PARAM_FILE=parameters/basic0.txt
RESULT_DIR=results/current/seed-$(SEED)

run: all
	# Run the program with 5 arguments:
	# 1. File describing the task for the animats to do
	# 2. Output file for the line of descent
	# 3. Output file for the animat genome
	# 4. The name of the trial
	# 5. The random seed (positive integer of any size)
	# 6. Noise level (float between 0 and 1)
	./build/main.o $(PARAM_FILE) $(RESULT_DIR)/seed-$(SEED)_LOD.csv $(RESULT_DIR)/seed-$(SEED)_genomes.csv $(RESULT_DIR)/seed-$(SEED) $(SEED) 0

.PHONY: clean all
