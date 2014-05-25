CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
CFLAGS = -Wno-address -Wno-char-subscripts # -Wno-sign-compare

HTTPXX=$(wildcard httpxx/*.cpp)
TEST=SwiftSDK.cpp
CXXSOURCES=$(HTTPXX) $(TEST)
CSOURCES=httpxx/http_parser.c

CXXOBJS=$(CXXSOURCES:%.cpp=%.o)
COBJS=$(CSOURCES:%.c=%.o)

LIBS =

TARGET =	SwiftSDK


all:	$(TARGET)

$(TARGET):	$(CXXOBJS) $(COBJS)
	$(CXX) -o $(TARGET) $(CXXOBJS) $(LIBS) $(COBJS)
	
$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(COBJS) $(CXXOBJS) $(TARGET)
