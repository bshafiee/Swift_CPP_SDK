#CXXFLAGS =	-O3 -g -Wall -fmessage-length=0 -std=c++11
CXXFLAGS = -g -Wall -fmessage-length=0 -std=c++11
CFLAGS = -Wno-address -Wno-char-subscripts # -Wno-sign-compare

SWIFT=$(wildcard interface/*.cpp io/*.cpp utils/jsoncpp/*.cpp model/*.cpp header/*.cpp)
LIBSWIFTHEADERS=$(wildcard interface/*.h io/*.h model/*.h header/*.h)
TEST=test.cpp
CXXSOURCES=$(SWIFT)
TESTSOURCES=$(TEST)
#CSOURCES=httpxx/http_parser.c

CXXOBJS=$(CXXSOURCES:%.cpp=%.o)
TESTOBJS=$(TESTSOURCES:%.cpp=%.o)
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
LIBSWIFT = libSwift.a

CXX=clang++
all: $(LIBSWIFT) $(TARGET)

$(LIBSWIFT): $(CXXOBJS)
	ar rcs $@ $^
	mkdir build/libSwift
	mkdir build/libSwift/include
	mkdir build/libSwift/lib
	mv -f $(LIBSWIFT) build/libSwift/lib
	cp -rf $(LIBSWIFTHEADERS) build/libSwift/include
	cp -rf utils/jsoncpp/json utils/poco build

$(TARGET):	$(CXXOBJS) $(COBJS) $(TESTOBJS)
#	$(CXX) -o $(TARGET) $(CXXOBJS) $(LIBS) $(COBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CXXOBJS) $(TESTOBJS) $(LIBS)

#$(COBJS): %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(CXXOBJS) $(TARGET) $(TESTOBJS) $(LIBSWIFT) $(wildcard build/*)
