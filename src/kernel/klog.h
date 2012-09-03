void kprintf(int level,char *fmt,...);

#define kpanic(fmt,...) kprintf(0,fmt, ##__VA_ARGS__)
#define kerror(fmt,...) kprintf(1,fmt, ##__VA_ARGS__)
#define kwarn(fmt,...)  kprintf(2,fmt, ##__VA_ARGS__)
#define kinfo(fmt,...)  kprintf(3,fmt, ##__VA_ARGS__)
#define kdebug(fmt,...) kprintf(4,fmt, ##__VA_ARGS__)

