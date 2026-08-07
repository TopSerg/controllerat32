/* pm_flux_observer.h */
#ifndef PM_FLUX_OBSERVER_H
#define PM_FLUX_OBSERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float Ts;                  /* 1e-4 */
    float alpha_psi;           /* 1.89e-3 */
    float T0_C;                /* 25 */
    int   pole_pairs;          /* 4 */

    float w0_min_hz;           /* 2.0 */
    float w0_speed_gain;       /* 0.04 */

    float tau_leak;            /* 5.0 */
    float zeta_conv;           /* 0.9 */
    uint8_t prewarp;           /* 1 */

    uint8_t use_im_correction; /* 0 */
    float k_im;                /* 0.15 */
    float eta_im;              /* 0.7 */
    float zeta_im;             /* 0.95 */

    float psi_f_lpf;           /* 0.03 */
    float im_flux_feedback;    /* 0.0 */

    float min_abs_id_for_ld;   /* 5.0 */
    float min_abs_iq_for_lq;   /* 5.0 */

    float Ld_min_H;            /* 1e-6 */
    float Ld_max_H;            /* 5e-3 */
    float Lq_min_H;            /* 1e-6 */
    float Lq_max_H;            /* 5e-3 */

    float T_min_C;             /* 0 */
    float T_max_C;             /* 300 */

    float psi0_ref_Wb;         /* cold reference psi0 */
    float Rs0_Ohm;             /* nominal Rs */
} PMFluxObserverConfig;

typedef struct {
    float vd, vq;              /* V */
    float id, iq;              /* A */
    float mech_rpm;            /* rpm (motor shaft) */
    float rs_curr_ohm;         /* optional measured Rs */
    uint8_t rs_valid;          /* 1 if rs_curr_ohm valid */
} PMFluxObserverInput;

typedef struct {
    float psi_f_Wb;
    float psi_f_raw_Wb;
    float T_est_C;
    float Ld_H;
    float Lq_H;
    float omega_e_rad_s;
    float theta_e_rad;
} PMFluxObserverOutput;

typedef struct {
    /* runtime state */
    float psi_v_a, psi_v_b, x_a_prev, x_b_prev;
    float zhp_a1, zhp_a2, zhp_b1, zhp_b2;
    float zlp_a1, zlp_a2, zlp_b1, zlp_b2;
    float zim_a1, zim_a2, zim_b1, zim_b2;
    float psi_f, psi_f_raw;
    float Ld_last, Lq_last;
    float theta_e, w0_last;

    /* cached coeffs */
    float leak;
    float bhp0, bhp1, bhp2, ahp1, ahp2;
    float blp0, blp1, blp2, alp1, alp2;
    float bim0, bim1, bim2, aim1, aim2;
} PMFluxObserverState;

/* API */
void PMFluxObserver_DefaultConfig(PMFluxObserverConfig* cfg);
void PMFluxObserver_Init(const PMFluxObserverConfig* cfg, PMFluxObserverState* st);
void PMFluxObserver_Reset(const PMFluxObserverConfig* cfg, PMFluxObserverState* st);
void PMFluxObserver_Step(const PMFluxObserverConfig* cfg,
                         PMFluxObserverState* st,
                         const PMFluxObserverInput* in,
                         PMFluxObserverOutput* out);

#ifdef __cplusplus
}
#endif
#endif
