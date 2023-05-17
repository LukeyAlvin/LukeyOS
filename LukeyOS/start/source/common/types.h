/*
 * @Description: 基本整数类型
 * @Author: Alvin
 * @Date: 2023-04-13 17:14:02
 * @LastEditTime: 2023-05-17 15:48:28
 */
#ifndef TYPES_H_
#define TYPES_H_

// 因为将要使用newlib库，newlib有同样使用typedef定义uint8_t类型
// 为了避免冲突，加上_UINT8_T_DECLARED的配置
#ifndef _UINT8_T_DECLARED
#define _UINT8_T_DECLARED
typedef unsigned char uint8_t;
#endif

#ifndef _UINT16_T_DECLARED
#define _UINT16_T_DECLARED
typedef unsigned short uint16_t;
#endif

#ifndef _UINT32_T_DECLARED
#define _UINT32_T_DECLARED
typedef unsigned long uint32_t;
#endif

#endif
