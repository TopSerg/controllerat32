/* pm_flux_observer.c */
#include "pm_flux_observer.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float clampf(float x, float lo, float hi) {
	return (x < lo) ? lo : ((x > hi) ? hi : x);
}

static void wrap_theta(float* th) {
	while (*th >  2.0f * (float)M_PI) *th -= 2.0f * (float)M_PI;
	while (*th < -2.0f * (float)M_PI) *th += 2.0f * (float)M_PI;
}

static void dq2ab(float d, float q, float th, float* a, float* b) {
	float c = cosf(th), s = sinf(th);
	*a = c * d - s * q;
	*b = s * d + c * q;
}

static void ab2dq(float a, float b, float th, float* d, float* q) {
	float c = cosf(th), s = sinf(th);
	*d = c * a + s * b;
	*q = -s * a + c * b;
}

static void bilinear2nd(float b2,
	float b1,
	float b0,
	float a1,
	float a0,
	float T,
	float* B0,
	float* B1,
	float* B2,
	float* A1,
	float* A2) {
	float K = 2.0f / T;
	float A = K*K + K*a1 + a0;
	*B0 = (K*K*b2 + K*b1 + b0) / A;
	*B1 = (2.0f * (b0 - K*K*b2)) / A;
	*B2 = (K*K*b2 - K*b1 + b0) / A;
	*A1 = (2.0f * (a0 - K*K)) / A;
	*A2 = (K*K - K*a1 + a0) / A;
}

static void biquad_step(float x,
	float* z1,
	float* z2,
	float b0,
	float b1,
	float b2,
	float a1,
	float a2,
	float* y) {
	float v = b0 * x + *z1;
	float z1n = b1 * x + *z2 - a1 * v;
	float z2n = b2 * x - a2 * v;
	*z1 = z1n;
	*z2 = z2n;
	*y = v;
}

static void update_coeffs(const PMFluxObserverConfig* cfg, PMFluxObserverState* st, float w0) {
	float w = w0;
	if (cfg->prewarp) w = (2.0f / cfg->Ts) * tanf(0.5f * w0 * cfg->Ts);

	/* HP and LP pair */
	{
		float den_a1 = 2.0f * cfg->zeta_conv * w;
		float den_a0 = w * w;
		bilinear2nd(1.0f,
			0.0f,
			0.0f,
			den_a1,
			den_a0,
			cfg->Ts,
			&st->bhp0,
			&st->bhp1,
			&st->bhp2,
			&st->ahp1,
			&st->ahp2);
		bilinear2nd(0.0f,
			2.0f * cfg->zeta_conv * w,
			w*w,
			den_a1,
			den_a0,
			cfg->Ts,
			&st->blp0,
			&st->blp1,
			&st->blp2,
			&st->alp1,
			&st->alp2);
	}

	/* IM correction filter */
	{
		float w_im = cfg->eta_im * w0;
		if (cfg->prewarp) w_im = (2.0f / cfg->Ts) * tanf(0.5f * w_im * cfg->Ts);
		{
			float den_a1 = 2.0f * cfg->zeta_im * w_im;
			float den_a0 = w_im * w_im;
			bilinear2nd(0.0f,
				2.0f * cfg->zeta_im * w_im,
				0.0f,
				den_a1,
				den_a0,
				cfg->Ts,
				&st->bim0,
				&st->bim1,
				&st->bim2,
				&st->aim1,
				&st->aim2);
		}
	}

	st->w0_last = w0;
}

void PMFluxObserver_DefaultConfig(PMFluxObserverConfig* cfg) {
	memset(cfg, 0, sizeof(*cfg));
	cfg->Ts = 1e-4f;
	cfg->alpha_psi = 1.89e-3f;
	cfg->T0_C = 25.0f;
	cfg->pole_pairs = 4;
	cfg->w0_min_hz = 2.0f;
	cfg->w0_speed_gain = 0.04f;
	cfg->tau_leak = 5.0f;
	cfg->zeta_conv = 0.9f;
	cfg->prewarp = 1;
	cfg->use_im_correction = 0;
	cfg->k_im = 0.15f;
	cfg->eta_im = 0.7f;
	cfg->zeta_im = 0.95f;
	cfg->psi_f_lpf = 0.03f;
	cfg->im_flux_feedback = 0.0f;
	cfg->min_abs_id_for_ld = 5.0f;
	cfg->min_abs_iq_for_lq = 5.0f;
	cfg->Ld_min_H = 1e-6f; cfg->Ld_max_H = 5e-3f;
	cfg->Lq_min_H = 1e-6f; cfg->Lq_max_H = 5e-3f;
	cfg->T_min_C = 0.0f; cfg->T_max_C = 300.0f;
	cfg->psi0_ref_Wb = 0.06f;  /* replace by your calibrated psi0 */
	cfg->Rs0_Ohm = 0.01f;
}

void PMFluxObserver_Reset(const PMFluxObserverConfig* cfg, PMFluxObserverState* st) {
	memset(st, 0, sizeof(*st));
	st->psi_f = cfg->psi0_ref_Wb;
	st->psi_f_raw = cfg->psi0_ref_Wb;
	st->Ld_last = 0.15e-3f; /* initial guess */
	st->Lq_last = 0.35e-3f; /* initial guess */
	st->theta_e = 0.0f;
	st->w0_last = -1.0f;
	st->leak = expf(-cfg->Ts / ((cfg->tau_leak > cfg->Ts) ? cfg->tau_leak : cfg->Ts));
	update_coeffs(cfg, st, 2.0f * (float)M_PI * cfg->w0_min_hz);
}

void PMFluxObserver_Init(const PMFluxObserverConfig* cfg, PMFluxObserverState* st) {
	PMFluxObserver_Reset(cfg, st);
}


void PMFluxObserver_Step(const PMFluxObserverConfig* cfg,
                         PMFluxObserverState* st,
                         const PMFluxObserverInput* in,
                         PMFluxObserverOutput* out) {
    float omega_m = in->mech_rpm * (2.0f * (float)M_PI / 60.0f);
    float omega_e = omega_m * (float)cfg->pole_pairs;

    st->theta_e += omega_e * cfg->Ts;
    wrap_theta(&st->theta_e);

    {
        float w0 = fmaxf(2.0f * (float)M_PI * cfg->w0_min_hz, cfg->w0_speed_gain * fabsf(omega_e));
        if (fabsf(w0 - st->w0_last) > 1e-6f) update_coeffs(cfg, st, w0);
    }

    {
        float Rs = in->rs_valid ? in->rs_curr_ohm : cfg->Rs0_Ohm;

        /* online inductances */
        float Ld = st->Ld_last;
        float Lq = st->Lq_last;

        if (fabsf(omega_e) > 1e-9f) {
            if (fabsf(in->id) >= cfg->min_abs_id_for_ld) {
                float Ld_new = ((in->vq - Rs * in->iq) / omega_e - st->psi_f) / in->id;
                if (isfinite(Ld_new)) Ld = clampf(Ld_new, cfg->Ld_min_H, cfg->Ld_max_H);
            }
            if (fabsf(in->iq) >= cfg->min_abs_iq_for_lq) {
                float Lq_new = (Rs * in->id - in->vd) / (omega_e * in->iq);
                if (isfinite(Lq_new)) Lq = clampf(Lq_new, cfg->Lq_min_H, cfg->Lq_max_H);
            }
        }
        st->Ld_last = Ld;
        st->Lq_last = Lq;

        /* dq->ab */
        float i_a, i_b, v_a, v_b;
        dq2ab(in->id, in->iq, st->theta_e, &i_a, &i_b);
        dq2ab(in->vd, in->vq, st->theta_e, &v_a, &v_b);

        /* voltage model */
        {
            float x_a = v_a - Rs * i_a;
            float x_b = v_b - Rs * i_b;
            st->psi_v_a = st->psi_v_a * st->leak + 0.5f * cfg->Ts * (x_a + st->x_a_prev);
            st->psi_v_b = st->psi_v_b * st->leak + 0.5f * cfg->Ts * (x_b + st->x_b_prev);
            st->x_a_prev = x_a;
            st->x_b_prev = x_b;
        }

        /* current model */
        {
            float kfb = clampf(cfg->im_flux_feedback, 0.0f, 1.0f);
            float psi_pm_im = kfb * st->psi_f + (1.0f - kfb) * cfg->psi0_ref_Wb;
            float psi_i_d = Ld * in->id + psi_pm_im;
            float psi_i_q = Lq * in->iq;
            float psi_i_a, psi_i_b;
            float y_hp_a, y_lp_a, y_hp_b, y_lp_b;
            float psi_est_a, psi_est_b;
            float psi_est_d, psi_est_q;

            dq2ab(psi_i_d, psi_i_q, st->theta_e, &psi_i_a, &psi_i_b);

            biquad_step(st->psi_v_a, &st->zhp_a1, &st->zhp_a2, st->bhp0, st->bhp1, st->bhp2, st->ahp1, st->ahp2, &y_hp_a);
            biquad_step(psi_i_a,    &st->zlp_a1, &st->zlp_a2, st->blp0, st->blp1, st->blp2, st->alp1, st->alp2, &y_lp_a);
            biquad_step(st->psi_v_b, &st->zhp_b1, &st->zhp_b2, st->bhp0, st->bhp1, st->bhp2, st->ahp1, st->ahp2, &y_hp_b);
            biquad_step(psi_i_b,    &st->zlp_b1, &st->zlp_b2, st->blp0, st->blp1, st->blp2, st->alp1, st->alp2, &y_lp_b);

            psi_est_a = y_hp_a + y_lp_a;
            psi_est_b = y_hp_b + y_lp_b;

            if (cfg->use_im_correction) {
                float e_a = psi_i_a - psi_est_a;
                float e_b = psi_i_b - psi_est_b;
                float y_im_a, y_im_b;
                biquad_step(e_a, &st->zim_a1, &st->zim_a2, st->bim0, st->bim1, st->bim2, st->aim1, st->aim2, &y_im_a);
                biquad_step(e_b, &st->zim_b1, &st->zim_b2, st->bim0, st->bim1, st->bim2, st->aim1, st->aim2, &y_im_b);
                psi_est_a += cfg->k_im * y_im_a;
                psi_est_b += cfg->k_im * y_im_b;
            }

            ab2dq(psi_est_a, psi_est_b, st->theta_e, &psi_est_d, &psi_est_q);

            st->psi_f_raw = psi_est_d - Ld * in->id;
            st->psi_f = (1.0f - clampf(cfg->psi_f_lpf, 0.0f, 1.0f)) * st->psi_f
                      + clampf(cfg->psi_f_lpf, 0.0f, 1.0f) * st->psi_f_raw;
        }

        {
            float T_est = cfg->T0_C + (cfg->psi0_ref_Wb - st->psi_f) / (cfg->alpha_psi * cfg->psi0_ref_Wb);
            T_est = clampf(T_est, cfg->T_min_C, cfg->T_max_C);


			out->psi_f_Wb = st->psi_f;
			out->psi_f_raw_Wb = st->psi_f_raw;
			out->T_est_C = T_est;
			out->Ld_H = st->Ld_last;
			out->Lq_H = st->Lq_last;
			out->omega_e_rad_s = omega_e;
			out->theta_e_rad = st->theta_e;
		}
	}
}
