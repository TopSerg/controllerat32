#ifndef RL64_ACTOR_WEIGHTS_H
#define RL64_ACTOR_WEIGHTS_H
#include "rl64_port.h"
#ifdef __cplusplus
extern "C" {
#endif
extern float rl64_w0[RL64_HIDDEN_DIM*RL64_STATE_DIM];
extern float rl64_b0[RL64_HIDDEN_DIM];
extern float rl64_b1[RL64_HIDDEN_DIM];
extern float rl64_w2[RL64_ACTION_DIM*RL64_HIDDEN_DIM];
extern float rl64_b2[RL64_ACTION_DIM];
extern float rl64_w1_0[512],rl64_w1_1[512],rl64_w1_2[512],rl64_w1_3[512];
extern float rl64_w1_4[512],rl64_w1_5[512],rl64_w1_6[512],rl64_w1_7[512];
#ifdef __cplusplus
}
#endif
#endif
