CXX = g++

DEPENDENCE = gtkmm-3.0
LDFLAGS = $(shell pkg-config --libs $(DEPENDENCE) | sed 's/ -I/ -isystem /g' )
CFLAGS = $(shell pkg-config --cflags $(DEPENDENCE) | sed 's/ -I/ -isystem /g') -std=c++17



PROGRAM = SpriteEd
SRC = main.cpp myApplication.cpp palette.cpp editArea.cpp editMode.cpp \
		editModeEllipse.cpp editModeFill.cpp editModePencil.cpp editModeRect.cpp \
		editModeSelect.cpp RPoint.cpp RRect.cpp spriteArea.cpp resources.cpp RHandle.cpp

		
OBJ = $(SRC:.cpp=.o)
OBJS_DEBUG = $(addprefix debug/,$(OBJ))
OBJS_RELEASE = $(addprefix release/,$(OBJ))

DEPS = $(SRC:.cpp=.d)
DEPS_DEBUG = $(addprefix debug/,$(DEPS))
DEPS_RELEASE = $(addprefix release/,$(DEPS))

.PHONY : debug release all clean clean_all clean_debug clean_release gen_resources

all: debug release

debug: CFLAGS += -g
debug: debug/$(PROGRAM)
release: release/$(PROGRAM)

clean : clean_all

clean_all: clean_debug clean_release 

clean_debug:
	rm debug/$(PROGRAM) debug/*.o debug/*.d

clean_release:
	rm release/$(PROGRAM) release/*.o release/*.d

gen_resources:
	glib-compile-resources --sourcedir res --generate-source myapp.gresource.xml --target=resources.cpp

debug/%.o: %.cpp
	$(CXX) -std=c++11 $(CFLAGS) -MMD -o $@ -c $<

release/%.o: %.cpp
	$(CXX) -std=c++11 $(CFLAGS) -MMD -o $@ -c $<

debug/$(PROGRAM) : $(OBJS_DEBUG)
	$(CXX) $(OBJS_DEBUG) $(LDFLAGS) -o debug/$(PROGRAM)

release/$(PROGRAM) : $(OBJS_RELEASE)
	$(CXX) $(OBJS_RELEASE) $(LDFLAGS) -o release/$(PROGRAM)

-include $(DEPS_DEBUG)
-include $(DEPS_RELEASE)

