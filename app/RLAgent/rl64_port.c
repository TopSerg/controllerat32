#include "rl64_port.h"
#include "rl64_actor_weights.h"

#include <stddef.h>

#if defined(RL64_USE_LIBM_TANHF)
#include <math.h>
#endif

#if defined(__GNUC__)
#define RL64_RESTRICT __restrict
#define RL64_ALWAYS_INLINE static inline __attribute__((always_inline))
#if defined(RL64_CODE_IN_FLASH)
#define RL64_FAST_CODE
#else
/* The target linker already maps .code_in_ram into SRAM. */
#define RL64_FAST_CODE __attribute__((long_call, section(".code_in_ram")))
#endif
#else
#define RL64_RESTRICT
#define RL64_ALWAYS_INLINE static inline
#define RL64_FAST_CODE
#endif

_Static_assert(RL64_STATE_DIM == 9U, "H64 state dimension mismatch");
_Static_assert(RL64_HIDDEN_DIM == 64U, "H64 hidden dimension mismatch");
_Static_assert(RL64_ACTION_DIM == 2U, "H64 action dimension mismatch");

RL64_ALWAYS_INLINE float rl64_tanh_fast(float x)
{
#if defined(RL64_USE_LIBM_TANHF)
    return tanhf(x);
#else
    /*
     * 7/6 Pade approximation. On [-5, 5] max error vs tanh is ~1.01e-4;
     * outside this range the actor is already effectively saturated.
     * This avoids pulling libm/tanhf into the AT32 firmware.
     */
    if (x >= 5.0f) return 1.0f;
    if (x <= -5.0f) return -1.0f;

    const float x2 = x * x;
    const float num = x * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
    const float den = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + 28.0f * x2));
    float y = num / den;
    if (y > 1.0f) y = 1.0f;
    if (y < -1.0f) y = -1.0f;
    return y;
#endif
}

RL64_ALWAYS_INLINE void dense_relu_9x64_unrolled4(
    const float *RL64_RESTRICT weights,
    const float *RL64_RESTRICT bias,
    const float *RL64_RESTRICT input,
    float *RL64_RESTRICT output)
{
    for (size_t i = 0; i < RL64_HIDDEN_DIM; ++i) {
        const float *RL64_RESTRICT row = weights + i * RL64_STATE_DIM;
        float sum = bias[i];

        sum += row[0] * input[0];
        sum += row[1] * input[1];
        sum += row[2] * input[2];
        sum += row[3] * input[3];
        sum += row[4] * input[4];
        sum += row[5] * input[5];
        sum += row[6] * input[6];
        sum += row[7] * input[7];
        sum += row[8] * input[8];

        output[i] = (sum > 0.0f) ? sum : 0.0f;
    }
}

RL64_ALWAYS_INLINE const float *rl64_w1_row(size_t i)
{
    const size_t block = i >> 3U;
    const size_t offset = (i & 7U) * RL64_HIDDEN_DIM;
    switch (block) {
        case 0: return rl64_w1_0 + offset; case 1: return rl64_w1_1 + offset;
        case 2: return rl64_w1_2 + offset; case 3: return rl64_w1_3 + offset;
        case 4: return rl64_w1_4 + offset; case 5: return rl64_w1_5 + offset;
        case 6: return rl64_w1_6 + offset; default: return rl64_w1_7 + offset;
    }
}

RL64_ALWAYS_INLINE void dense_relu_64x64_unrolled4(
    const float *RL64_RESTRICT bias,
    const float *RL64_RESTRICT input,
    float *RL64_RESTRICT output)
{
    for (size_t i = 0; i < RL64_HIDDEN_DIM; ++i) {
        const float *RL64_RESTRICT row = rl64_w1_row(i);
        float sum = bias[i];

        for (size_t j = 0; j < RL64_HIDDEN_DIM; j += 4U) {
            sum += row[j]      * input[j];
            sum += row[j + 1U] * input[j + 1U];
            sum += row[j + 2U] * input[j + 2U];
            sum += row[j + 3U] * input[j + 3U];
        }
        output[i] = (sum > 0.0f) ? sum : 0.0f;
    }
}

RL64_ALWAYS_INLINE void dense_tanh_64x2_unrolled4(
    const float *RL64_RESTRICT weights,
    const float *RL64_RESTRICT bias,
    const float *RL64_RESTRICT input,
    float output[RL64_ACTION_DIM])
{
    for (size_t i = 0; i < RL64_ACTION_DIM; ++i) {
        const float *RL64_RESTRICT row = weights + i * RL64_HIDDEN_DIM;
        float sum = bias[i];

        for (size_t j = 0; j < RL64_HIDDEN_DIM; j += 4U) {
            sum += row[j]      * input[j];
            sum += row[j + 1U] * input[j + 1U];
            sum += row[j + 2U] * input[j + 2U];
            sum += row[j + 3U] * input[j + 3U];
        }
        output[i] = rl64_tanh_fast(sum);
    }
}

RL64_FAST_CODE void rl64_actor_infer_unrolled4(
    Rl64Workspace *workspace,
    const float state[RL64_STATE_DIM],
    float action[RL64_ACTION_DIM])
{
    dense_relu_9x64_unrolled4(rl64_w0, rl64_b0, state, workspace->h0);
    dense_relu_64x64_unrolled4(rl64_b1, workspace->h0, workspace->h1);
    dense_tanh_64x2_unrolled4(rl64_w2, rl64_b2, workspace->h1, action);
}


/* ---- full agent step ---- */
#include "rl64_port.h"
#include "platform_math.h"

#include <stddef.h>
#include <string.h>

static inline float clampf_local(float x, float lo, float hi)
{
    return (x < lo) ? lo : ((x > hi) ? hi : x);
}

static inline float maxf_local(float a, float b)
{
    return (a > b) ? a : b;
}

static inline float absf_local(float x)
{
    return (x >= 0.0f) ? x : -x;
}

static inline int finitef_fast(float x)
{
    return (x > -1.0e30f) && (x < 1.0e30f);
}

static int finite_input(const Rl64AgentInput *in)
{
    return finitef_fast(in->speed_rpm) &&
           finitef_fast(in->torque_nm) &&
           finitef_fast(in->torque_ref_nm) &&
           finitef_fast(in->id_a) &&
           finitef_fast(in->iq_a) &&
           finitef_fast(in->ud_cmd_v) &&
           finitef_fast(in->uq_cmd_v) &&
           finitef_fast(in->u_dc_v) &&
           finitef_fast(in->dt_s);
}

void rl64_agent_default_config(Rl64AgentConfig *cfg)
{
    if (cfg == NULL) return;

    cfg->speed_norm_rpm = 5000.0f;
    cfg->torque_norm_nm = 5.0f;
    cfg->current_norm_a = 100.0f;
    cfg->du_step_v = 1.0f;
    cfg->dt_base_s = 1.0e-4f;
    cfg->u_max_dc_ratio = 0.5f;
    cfg->efficiency_default_percent = 95.0f;
}

bool rl64_agent_init(Rl64Agent *agent, const Rl64AgentConfig *cfg)
{
    if (agent == NULL || cfg == NULL) return false;

    if (!finitef_fast(cfg->speed_norm_rpm) || cfg->speed_norm_rpm <= 0.0f ||
        !finitef_fast(cfg->torque_norm_nm) || cfg->torque_norm_nm <= 0.0f ||
        !finitef_fast(cfg->current_norm_a) || cfg->current_norm_a <= 0.0f ||
        !finitef_fast(cfg->du_step_v) || cfg->du_step_v <= 0.0f ||
        !finitef_fast(cfg->dt_base_s) || cfg->dt_base_s <= 0.0f ||
        !finitef_fast(cfg->u_max_dc_ratio) || cfg->u_max_dc_ratio <= 0.0f) {
        return false;
    }

    memset(agent, 0, sizeof(*agent));
    agent->cfg = *cfg;
    return true;
}

bool rl64_agent_build_state(const Rl64AgentConfig *cfg,
                            const Rl64AgentInput *input,
                            float state[RL64_STATE_DIM],
                            float *u_max_v)
{
    if (cfg == NULL || input == NULL || state == NULL ||
        !input->enabled || !finite_input(input) || input->u_dc_v <= 0.0f) {
        return false;
    }

    const float torque_den = maxf_local(cfg->torque_norm_nm,
                                        absf_local(input->torque_ref_nm));
    const float i_sq = input->id_a * input->id_a + input->iq_a * input->iq_a;
    const float i_mag = platform_sqrt(i_sq);
    const float u_max = input->u_dc_v * cfg->u_max_dc_ratio;
    const float efficiency = finitef_fast(input->efficiency_percent)
        ? input->efficiency_percent
        : cfg->efficiency_default_percent;

    if (!finitef_fast(i_mag) || !finitef_fast(u_max) || u_max <= 0.0f) {
        return false;
    }

    state[0] = input->speed_rpm / cfg->speed_norm_rpm;
    state[1] = input->torque_ref_nm / torque_den;
    state[2] = (input->torque_nm - input->torque_ref_nm) / torque_den;
    state[3] = input->id_a / cfg->current_norm_a;
    state[4] = input->iq_a / cfg->current_norm_a;
    state[5] = input->ud_cmd_v / u_max;
    state[6] = input->uq_cmd_v / u_max;
    state[7] = i_mag / cfg->current_norm_a;
    state[8] = clampf_local(efficiency / 100.0f, 0.0f, 1.0f);

    if (u_max_v != NULL) *u_max_v = u_max;
    return true;
}

bool rl64_agent_step(Rl64Agent *agent,
                     const Rl64AgentInput *input,
                     Rl64AgentOutput *output)
{
    if (agent == NULL || input == NULL || output == NULL) return false;

    memset(output, 0, sizeof(*output));
    output->source_sequence = input->sequence;
    output->ud_new_v = input->ud_cmd_v;
    output->uq_new_v = input->uq_cmd_v;

    if (!rl64_agent_build_state(&agent->cfg, input, output->state, &output->u_max_v)) {
        return false;
    }

    rl64_actor_infer_unrolled4(&agent->workspace, output->state, output->action);

    const float dt = (input->dt_s > 0.0f) ? input->dt_s : agent->cfg.dt_base_s;
    const float dt_scale = dt / agent->cfg.dt_base_s;

    output->d_ud_v = output->action[0] * agent->cfg.du_step_v * dt_scale;
    output->d_uq_v = output->action[1] * agent->cfg.du_step_v * dt_scale;
    output->ud_new_v = input->ud_cmd_v + output->d_ud_v;
    output->uq_new_v = input->uq_cmd_v + output->d_uq_v;

    const float u_sq = output->ud_new_v * output->ud_new_v +
                       output->uq_new_v * output->uq_new_v;
    const float u_mag = platform_sqrt(u_sq);
    if (u_mag > output->u_max_v && u_mag > 0.0f) {
        const float scale = output->u_max_v / u_mag;
        output->ud_new_v *= scale;
        output->uq_new_v *= scale;
    }

    output->valid = finitef_fast(output->action[0]) &&
                    finitef_fast(output->action[1]) &&
                    finitef_fast(output->ud_new_v) &&
                    finitef_fast(output->uq_new_v);
    return output->valid;
}


/* ---- AT32 shadow bridge ---- */
#include "rl64_port.h"

#include "ControlSystem_v2.h"
#include "timerInit.h"

#define RAD_S_TO_RPM  (9.54929658551372f)
#define RL64_DT_S     (1.0e-4f)

static Rl64Agent g_rl64_agent;
static uint32_t g_rl64_sequence = 0U;

volatile float rl64_step_time_us = 0.0f;
volatile uint32_t rl64_step_count = 0U;
volatile uint32_t rl64_valid_count = 0U;
Rl64AgentOutput rl64_last_output;

void rl64_at32_init(void)
{
    Rl64AgentConfig cfg;
    rl64_agent_default_config(&cfg);

    /* Keep these identical to the training/ESP32 full_agent_step configuration. */
    cfg.speed_norm_rpm = 5000.0f;
    cfg.torque_norm_nm = 5.0f;
    cfg.current_norm_a = 100.0f;
    cfg.du_step_v = 1.0f;
    cfg.dt_base_s = RL64_DT_S;
    cfg.u_max_dc_ratio = 0.5f;
    cfg.efficiency_default_percent = 95.0f;

    (void)rl64_agent_init(&g_rl64_agent, &cfg);
}

bool rl64_at32_shadow_step(float torque_ref_nm, float efficiency_percent)
{
    Rl64AgentInput in;
    const uint32_t start_tick = GET_ACTUAL_TIMECNT();

    in.sequence = ++g_rl64_sequence;
    in.enabled = (outSignals.enable != 0);

    /* Control.Wmechanical is the mechanical angular speed from the observer. */
    in.speed_rpm = Control.Wmechanical * RAD_S_TO_RPM;
    in.torque_nm = Control.motorTorque;
    in.torque_ref_nm = torque_ref_nm;

    in.id_a = Control.Id;
    in.iq_a = Control.Iq;
    in.ud_cmd_v = Control.Ud;
    in.uq_cmd_v = Control.Uq;
    in.u_dc_v = Control.UdcFiltered;
    in.efficiency_percent = efficiency_percent;
    in.dt_s = RL64_DT_S;

    const bool valid = rl64_agent_step(&g_rl64_agent, &in, &rl64_last_output);

    rl64_step_time_us = CONVERT_toUs(start_tick - GET_ACTUAL_TIMECNT()) * 1.0e6f;
    ++rl64_step_count;
    if (valid) ++rl64_valid_count;

    /* Shadow mode: no assignment to Control.Ud/Uq or PWM outputs here. */
    return valid;
}
