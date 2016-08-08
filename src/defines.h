#ifdef _WIN32
#define __alloca(type, count) alloca(sizeof(type) * count)
#elif __linux__
#define __alloca(type, count) type[count]
#endif
