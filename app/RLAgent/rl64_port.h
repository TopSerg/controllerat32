#ifndef RL64_PORT_H
#define RL64_PORT_H
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define RL64_STATE_DIM 9U
#define RL64_HIDDEN_DIM 64U
#define RL64_ACTION_DIM 2U
typedef struct { float h0[RL64_HIDDEN_DIM]; float h1[RL64_HIDDEN_DIM]; } Rl64Workspace;
typedef struct { float speed_norm_rpm, torque_norm_nm, current_norm_a, du_step_v, dt_base_s, u_max_dc_ratio, efficiency_default_percent; } Rl64AgentConfig;
typedef struct { uint32_t sequence; bool enabled; float speed_rpm, torque_nm, torque_ref_nm, id_a, iq_a, ud_cmd_v, uq_cmd_v, u_dc_v, efficiency_percent, dt_s; } Rl64AgentInput;
typedef struct { uint32_t source_sequence; bool valid; float state[RL64_STATE_DIM], action[RL64_ACTION_DIM], d_ud_v, d_uq_v, ud_new_v, uq_new_v, u_max_v; } Rl64AgentOutput;
typedef struct { Rl64AgentConfig cfg; Rl64Workspace workspace; } Rl64Agent;
void rl64_agent_default_config(Rl64AgentConfig *cfg);
bool rl64_agent_init(Rl64Agent *agent,const Rl64AgentConfig *cfg);
bool rl64_agent_build_state(const Rl64AgentConfig *cfg,const Rl64AgentInput *input,float state[RL64_STATE_DIM],float *u_max_v);
bool rl64_agent_step(Rl64Agent *agent,const Rl64AgentInput *input,Rl64AgentOutput *output);
void rl64_at32_init(void);
bool rl64_at32_shadow_step(float torque_ref_nm,float efficiency_percent);
extern volatile float rl64_step_time_us;
extern volatile uint32_t rl64_step_count,rl64_valid_count;
extern Rl64AgentOutput rl64_last_output;
#ifdef __cplusplus
}
#endif
#endif
