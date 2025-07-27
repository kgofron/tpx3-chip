CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = $(shell pkg-config --libs nlohmann_json)
CXXFLAGS += $(shell pkg-config --cflags nlohmann_json)

TARGET = pixel_config_compare
SOURCE = pixel_config_compare.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean 