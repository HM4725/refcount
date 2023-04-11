#ifndef __UTILS_H__
#define __UTILS_H__

#define min(a, b) (((a) < (b)) ? (a) : (b))
inline int roundup(int x, int unit) { return (x + unit - 1) / unit * unit; }

#endif /* __UTILS_H__ */