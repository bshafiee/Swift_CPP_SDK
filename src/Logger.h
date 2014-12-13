/*
 * Logger.h
 *
 *  Created on: Dec 13, 2014
 *      Author: behrooz
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <iostream>
#include <streambuf>

namespace Swift {

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) { return c; }
};

class Logger {
private:
  static NullBuffer null_buffer;
  Logger() {}
  ~Logger() {}
  static std::ostream* errStream;
  static std::ostream* infoStream;
  static std::ostream* debugStream;
  static std::ostream* fatalStream;
public:
  static std::ostream null_stream;

  static void setInfoStream(std::ostream& _infoStream) {
    infoStream = &_infoStream;
  }
  static void setDebugStream(std::ostream& _debugStream) {
    debugStream = &_debugStream;
  }
  static void setFatalStream(std::ostream& _fatalStream) {
    fatalStream = &_fatalStream;
  }
  static void setErrorStream(std::ostream& _errStream) {
    errStream = &_errStream;
  }
  static void setLogStreams(std::ostream& _infoStream,std::ostream& _debugStream,
      std::ostream& _errorStream,std::ostream& _fatalStream) {
    infoStream = &_infoStream;
    debugStream = &_debugStream;
    fatalStream = &_fatalStream;
    errStream = &_errorStream;
  }

  static std::ostream& INFO() {return *infoStream;}
  static std::ostream& FATAL() {return *fatalStream;}
  static std::ostream& ERROR() {return *errStream;}
  static std::ostream& DEBUG() {return *debugStream;}
};
NullBuffer Logger::null_buffer;
std::ostream Logger::null_stream(&Logger::null_buffer);
std::ostream* Logger::errStream  = &std::cerr;
std::ostream* Logger::infoStream  = &std::cerr;
std::ostream* Logger::debugStream  = &std::cerr;
std::ostream* Logger::fatalStream  = &std::cerr;

}//End of namespace
#endif /* SRC_LOGGER_H_ */
