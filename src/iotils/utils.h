#ifndef __IOTILS_UTILS_H__
#define __IOTILS_UTILS_H__

#include <Arduino.h> // handling arduino strings

String escape_json(const String unescaped) {
  String copy = unescaped;
  copy.replace("\\","\\\\");
  copy.replace("\"","\\\"");
  copy.replace("\r","\\r");
  copy.replace("\n","\\n");
  return copy;
}

#endif // __IOTILS_UTILS_H__