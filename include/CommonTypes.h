#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

// Timezone configuration
struct Timezone {
  const char* name;
  const char* description;
  int gmtOffset;  // GMT offset in seconds
  int dstOffset;  // DST offset in seconds
};

// DST (Daylight Saving Time) configuration
enum DstMode { DST_AUTO, DST_MANUAL };

#endif // COMMON_TYPES_H