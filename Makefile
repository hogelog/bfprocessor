TARGETS = bf bf-vm bf-jit

CXXFLAGS = -Wall -W -O2 -fno-operator-names

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS)
