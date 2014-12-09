#CXXFLAGS =	-O3 -Wall -fmessage-length=0 -std=c++11
CXXFLAGS = -fPIC -g -O3 -Wall -fmessage-length=0 -std=c++11
CFLAGS = -Wno-address -Wno-char-subscripts # -Wno-sign-compare

SWIFT=$(wildcard src/*.cpp)
LIBSWIFTHEADERS=$(wildcard src/*.h)
TEST=test.cpp
CXXSOURCES=$(SWIFT)
TESTSOURCES=$(TEST)
#CSOURCES=httpxx/http_parser.c

CXXOBJS=$(CXXSOURCES:%.cpp=%.o)
TESTOBJS=$(TESTSOURCES:%.cpp=%.o)
#COBJS=$(CSOURCES:%.c=%.o)

#build dir
BUILDDIR=build

#Libraries
LDFLAGS = -shared

LIBS =-lPocoUtild -lPocoUtil -lPocoXML -lPocoNet -lPocoNetd -lPocoFoundation -lPocoXMLd -lPocoFoundationd -lpthread


TARGET =	SwiftSDK
LIBSWIFT = $(BUILDDIR)/libSwift.so

#CXX=clang++
all: $(LIBSWIFT) $(TARGET)

$(LIBSWIFT) : $(CXXOBJS)
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/include/Swift
	$(CXX) $(CXXFLAGS) $(CXXOBJS) -o $@ $(LDFLAGS) $(LIBS)
	cp -rf $(LIBSWIFTHEADERS) $(BUILDDIR)/include/Swift

$(TARGET):	$(CXXOBJS) $(COBJS) $(TESTOBJS)
#	$(CXX) -o $(TARGET) $(CXXOBJS) $(LIBS) $(COBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CXXOBJS) $(TESTOBJS) $(LIBS)

install:
	cp -r $(BUILDDIR)/include/Swift /usr/local/include
	cp $(LIBSWIFT) /usr/local/lib
	
uninstall:
	rm -rf /usr/local/include/Swift
	rm -f /usr/local/lib/libSwift.so
	ldconfig

#$(COBJS): %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(CXXOBJS) $(TARGET) $(TESTOBJS) $(LIBSWIFT) $(wildcard build/*) $(BUILDDIR)
