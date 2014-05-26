CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
CFLAGS = -Wno-address -Wno-char-subscripts # -Wno-sign-compare

#HTTPXX=$(wildcard httpxx/*.cpp)
TEST=test.cpp
CXXSOURCES=$(HTTPXX) $(TEST)
#CSOURCES=httpxx/http_parser.c

CXXOBJS=$(CXXSOURCES:%.cpp=%.o)
#COBJS=$(CSOURCES:%.c=%.o)

#Includes
LFLAGS=-IUtils/poco/include
CXXFLAGS+= $(LFLAGS)

#Libraries
#LD = -static -LUtils/poco/lib
LD = -LUtils/poco/lib
CXXFLAGS+=$(LD)

LIBS =-lPocoUtild -lPocoUtil -lPocoXML -lPocoNet -lPocoNetd -lPocoFoundation -lPocoXMLd -lPocoFoundationd -lpthread


TARGET =	SwiftSDK


all:	$(TARGET)

$(TARGET):	$(CXXOBJS) $(COBJS)
#	$(CXX) -o $(TARGET) $(CXXOBJS) $(LIBS) $(COBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CXXOBJS) $(LIBS)
	
#$(COBJS): %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@


clean:
#	rm -f $(COBJS) $(CXXOBJS) $(TARGET)
	rm -f $(CXXOBJS) $(TARGET)