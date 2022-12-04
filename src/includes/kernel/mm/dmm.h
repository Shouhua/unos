#ifndef __DMM_H__
#define __DMM_H__

// 为什么需要heap，动态分配内存(why dynamic memory allocation)
/**
 * CSAPP(Computer Science A Programmer's Perspective)深入计算机原理
 * 有段描述，大意是，如果想定义一个数组去存储用户想要输入，但是提前是没法知道用户先要输入多少个，
 * 即数组的size无法确定，那只能使用最大可能值定义，有风险超过预先设定的值
*/

/**
 * A=0空闲block才有footer，需要做合并使用
 * PF=1前面block空闲
 * header
 * 31     2 1   0
 * *      * PF  A
*/

void dmm_init();
void* dmm_sbrk(int incr);

#endif