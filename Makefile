CXX=g++
LDFLAGS=-lGL -lGLU -lglfw -lGLEW

main: main.cpp
	mkdir -p dist
	$(CXX) main.cpp -o dist/main $(LDFLAGS)

clean:
	rm -rf dist