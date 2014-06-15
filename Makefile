#CXXFLAGS =	-O3 -g -Wall -fmessage-length=0 -std=c++11
CXXFLAGS = -g -Wall -fmessage-length=0 -std=c++11
CFLAGS = -Wno-address -Wno-char-subscripts # -Wno-sign-compare

SWIFT=$(wildcard interface/*.cpp io/*.cpp utils/jsoncpp/*.cpp model/*.cpp header/*.cpp)
TEST=test.cpp
CXXSOURCES=$(SWIFT) $(TEST)
#CSOURCES=httpxx/http_parser.c

CXXOBJS=$(CXXSOURCES:%.cpp=%.o)
#COBJS=$(CSOURCES:%.c=%.o)

#Includes
LFLAGS=-Iutils/poco/include -Iutils/jsoncpp
CXXFLAGS+= $(LFLAGS)

#Libraries
#LD = -static -LUtils/poco/lib
LD = -Lutils/poco/lib
CXXFLAGS+=$(LD)

LIBS =-lPocoUtild -lPocoUtil -lPocoXML -lPocoNet -lPocoNetd -lPocoFoundation -lPocoXMLd -lPocoFoundationd -lpthread


TARGET =	SwiftSDK

#CXX=clang++
all:	$(TARGET)

$(TARGET):	$(CXXOBJS) $(COBJS)
#	$(CXX) -o $(TARGET) $(CXXOBJS) $(LIBS) $(COBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CXXOBJS) $(LIBS)

#$(COBJS): %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@


clean:
#	rm -f $(COBJS) $(CXXOBJS) $(TARGET)
	rm -f $(CXXOBJS) $(TARGET)