

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__)
#define Plateform_Linux
#elif defined(_WIN32) || defined(_WIN64)
#define Plateform_Windows
#endif
