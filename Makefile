TINYOBJ_PATH=/home/bardi/imma-coder/cpp/libs/tinyobjectloader

CFLAGS = -std=c++17 -O2 -I. -I${VULKAN_SDK_PATH}/include -I${TINYOBJ_PATH}
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vertSources = $(shell find shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

Target = a.out
$(Target): *.cpp *.hpp 
$(Target): $(vertObjFiles) $(fragObjFiles)
	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS)

%.vert.spv: %.vert
	glslc $< -o $@

%.frag.spv: %.frag
	glslc $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out

shaders: $(vertObjFiles) $(fragObjFiles)