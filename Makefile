# Compile everything into an executable called main.o.
# Use `python3.4-config --ldflags` and `python3.4-config --cflags` to get the
# flags needed to use Python from within C++.
default:
	c++ src/*.cpp -o build/main.o -I/usr/local/Cellar/python3/3.4.3/Frameworks/Python.framework/Versions/3.4/include/python3.4m -I/usr/local/Cellar/python3/3.4.3/Frameworks/Python.framework/Versions/3.4/include/python3.4m -Wno-unused-result -fno-common -dynamic -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I/usr/local/include -I/usr/local/opt/sqlite/include -L/usr/local/Cellar/python3/3.4.3/Frameworks/Python.framework/Versions/3.4/lib/python3.4/config-3.4m -lpython3.4m -ldl -framework CoreFoundation

clean:
	$(RM) build/main.o

run: build/main.o
	# Run the program with 5 arguments:
	# 1. File describing the task for the animats to do
	# 2. Output file for the line of descent
	# 3. Output file for the animat genome
	# 4. The name of the trial
	# 5. The random seed (positive integer of any size)
	# 6. Noise level (float between 0 and 1)
	./build/main.o parameters/basic0.txt results/trial0_LOD.txt results/trial0_GEN.txt trial0 0 0 
