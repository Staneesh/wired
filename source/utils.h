#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

typedef int8_t i8; 
typedef int16_t i16; 
typedef int32_t i32; 
typedef int64_t i64; 
typedef uint8_t u8; 
typedef uint16_t u16; 
typedef uint32_t u32; 
typedef uint64_t u64; 
typedef float real32;
typedef double real64;

#define LOG_INT(x) printf("Line %d in file %s says: %s = %i\n", \
		__LINE__, __FILE__, #x, x);
#define LOG_UINT(x) printf("Line %d in file %s says: %s = %u\n", \
		__LINE__, __FILE__, #x, x);

#define LOG_HEX(x) printf("Line %d in file %s says: %s = %X\n", \
		__LINE__, __FILE__, #x, x);

#define LOG_POINTER(x) printf("Line %d in file %s says: %s = %p\n", \
		__LINE__, __FILE__, #x, (void*)x);
 
#define LOG_STRING(x) printf("Line %d in file %s says: %s = %s\n", \
		__LINE__, __FILE__, #x, x);

#define LOG_FLOAT(x) printf("Line %d in file %s says: %s = %f\n", \
		__LINE__, __FILE__, #x, x);

#define LOG_HERE printf("Line %d in file %s says: HERE! \n", \
		__LINE__, __FILE__);

#define UNUSED(x) (void)(x)



#endif
