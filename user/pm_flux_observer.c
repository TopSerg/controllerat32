// PM Flux Observer for inverter firmware (C++17)
// Inputs per step: vd, vq, id, iq, mech speed rpm (motor shaft), Rs_curr (optional measured stator resistance).
// Inductances Ld/Lq are estimated online from voltage equations.

#include <cmath>
#include <algorithm>
#include <cfloat>

struct PMFluxObserverCfg {
    // ---- from pm_flux_observer_map.ipynb ----
    double Ts            = 1e-4;
    double alpha_psi     = 1.89e-3;
    double T0_C          = 25.0;
    int    pole_pairs    = 4;

    double w0_min_hz     = 2.0;    // W0_MIN_HZ
    double w0_speed_gain = 0.04;   // W0_SPEED_GAIN (rad/s scale from |omega_e|)

    double tau_leak      = 5.0;
    double zeta_conv     = 0.9;
    bool   prewarp       = true;

    bool   use_im_correction = false;
    double k_im          = 0.15;
    double eta_im        = 0.7;
    double zeta_im       = 0.95;

    double psi_f_lpf     = 0.03;
    double im_flux_feedback = 0.0; // in notebook = 0.0

    // inductance estimation thresholds (from notebook logic)
    double min_abs_id_for_ld = 5.0;
    double min_abs_iq_for_lq = 5.0;

    // safety bounds (set for your machine)
    double Ld_min_H = 1e-6, Ld_max_H = 5e-3;
    double Lq_min_H = 1e-6, Lq_max_H = 5e-3;

    // temperature clamp
    double T_min_C = 0.0, T_max_C = 300.0;

    // Reference PM flux (same role as psi0_peak in observer)
    double psi0_ref_Wb = 0.06;
    double Rs0_Ohm     = 0.01;
    double alpha_Cu    = 0.00398;
};

struct PMFluxObserverOut {
    double psi_f_Wb = NAN;
    double psi_f_raw_Wb = NAN;
    double T_est_C = NAN;
    double Ld_H = NAN;
    double Lq_H = NAN;
    double omega_e_rad_s = NAN;
    double theta_e_rad = NAN;
};

class PMFluxObserver {
public:
    explicit PMFluxObserver(const PMFluxObserverCfg& cfg) : c(cfg) {
        reset();
        updateW0Coeffs(2.0 * M_PI * c.w0_min_hz);
    }

    void reset() {
        psi_v_a = psi_v_b = 0.0;
        x_a_prev = x_b_prev = 0.0;

        zhp_a1 = zhp_a2 = zhp_b1 = zhp_b2 = 0.0;
        zlp_a1 = zlp_a2 = zlp_b1 = zlp_b2 = 0.0;
        zim_a1 = zim_a2 = zim_b1 = zim_b2 = 0.0;

        psi_f = c.psi0_ref_Wb;
        psi_f_raw = c.psi0_ref_Wb;

        // initial guesses (replace with your nominal values if known)
        Ld_last = 0.15e-3;
        Lq_last = 0.35e-3;

        theta_e = 0.0;
        w0_last = -1.0;
    }

    PMFluxObserverOut step(double vd, double vq,
                           double id, double iq,
                           double mech_rpm,
                           double Rs_curr = NAN)
    {
        // 1) electrical speed + theta
        const double omega_m = mech_rpm * (2.0 * M_PI / 60.0);
        const double omega_e = omega_m * c.pole_pairs;
        theta_e += omega_e * c.Ts;
        wrapAngle(theta_e);

        // 2) dynamic crossover frequency (same idea as notebook)
        const double w0 = std::max(2.0 * M_PI * c.w0_min_hz, c.w0_speed_gain * std::fabs(omega_e));
        if (std::fabs(w0 - w0_last) > 1e-6) updateW0Coeffs(w0);

        // 3) resistance
        double Rs = Rs_curr;
        if (!std::isfinite(Rs)) Rs = c.Rs0_Ohm;

        // 4) online Ld/Lq from voltage equations
        // Ld = ((vq - Rs*iq)/we - psi_pm_ref) / id
        // Lq = (Rs*id - vd) / (we*iq)
        // psi_pm_ref here = current psi_f estimate.
        double Ld = Ld_last;
        double Lq = Lq_last;

        if (std::fabs(omega_e) > 1e-9) {
            if (std::fabs(id) >= c.min_abs_id_for_ld) {
                const double Ld_new = ((vq - Rs * iq) / omega_e - psi_f) / id;
                if (std::isfinite(Ld_new)) Ld = clamp(Ld_new, c.Ld_min_H, c.Ld_max_H);
            }
            if (std::fabs(iq) >= c.min_abs_iq_for_lq) {
                const double Lq_new = (Rs * id - vd) / (omega_e * iq);
                if (std::isfinite(Lq_new)) Lq = clamp(Lq_new, c.Lq_min_H, c.Lq_max_H);
            }
        }
        Ld_last = Ld;
        Lq_last = Lq;

        // 5) dq -> alpha-beta
        double i_a, i_b, v_a, v_b;
        dq2ab(id, iq, theta_e, i_a, i_b);
        dq2ab(vd, vq, theta_e, v_a, v_b);


        // 6) voltage model integrator with leakage
        const double leak = std::exp(-c.Ts / std::max(c.tau_leak, c.Ts));
        const double x_a = v_a - Rs * i_a;
        const double x_b = v_b - Rs * i_b;
        psi_v_a = psi_v_a * leak + 0.5 * c.Ts * (x_a + x_a_prev);
        psi_v_b = psi_v_b * leak + 0.5 * c.Ts * (x_b + x_b_prev);
        x_a_prev = x_a; x_b_prev = x_b;

        // 7) current model
        const double k_fb = clamp(c.im_flux_feedback, 0.0, 1.0);
        const double psi_pm_im = k_fb * psi_f + (1.0 - k_fb) * c.psi0_ref_Wb;
        const double psi_i_d = Ld * id + psi_pm_im;
        const double psi_i_q = Lq * iq;
        double psi_i_a, psi_i_b;
        dq2ab(psi_i_d, psi_i_q, theta_e, psi_i_a, psi_i_b);

        // 8) complementary fusion in alpha-beta
        double y_hp_a, y_lp_a, y_hp_b, y_lp_b;
        stepBiquad(psi_v_a, zhp_a1, zhp_a2, bhp0, bhp1, bhp2, ahp1, ahp2, y_hp_a);
        stepBiquad(psi_i_a, zlp_a1, zlp_a2, blp0, blp1, blp2, alp1, alp2, y_lp_a);
        stepBiquad(psi_v_b, zhp_b1, zhp_b2, bhp0, bhp1, bhp2, ahp1, ahp2, y_hp_b);
        stepBiquad(psi_i_b, zlp_b1, zlp_b2, blp0, blp1, blp2, alp1, alp2, y_lp_b);

        double psi_est_a = y_hp_a + y_lp_a;
        double psi_est_b = y_hp_b + y_lp_b;

        // optional IM correction
        if (c.use_im_correction) {
            const double e_a = psi_i_a - psi_est_a;
            const double e_b = psi_i_b - psi_est_b;
            double y_im_a, y_im_b;
            stepBiquad(e_a, zim_a1, zim_a2, bim0, bim1, bim2, aim1, aim2, y_im_a);
            stepBiquad(e_b, zim_b1, zim_b2, bim0, bim1, bim2, aim1, aim2, y_im_b);
            psi_est_a += c.k_im * y_im_a;
            psi_est_b += c.k_im * y_im_b;
        }

        // 9) back to dq and PM flux extraction
        double psi_est_d, psi_est_q;
        ab2dq(psi_est_a, psi_est_b, theta_e, psi_est_d, psi_est_q);

        psi_f_raw = psi_est_d - Ld * id;
        const double a = clamp(c.psi_f_lpf, 0.0, 1.0);
        psi_f = (1.0 - a) * psi_f + a * psi_f_raw;

        // 10) temperature from linear PM-flux law
        double T_est = c.T0_C + (c.psi0_ref_Wb - psi_f) / (c.alpha_psi * c.psi0_ref_Wb);
        T_est = clamp(T_est, c.T_min_C, c.T_max_C);

        PMFluxObserverOut out;
        out.psi_f_Wb = psi_f;
        out.psi_f_raw_Wb = psi_f_raw;
        out.T_est_C = T_est;
        out.Ld_H = Ld;
        out.Lq_H = Lq;
        out.omega_e_rad_s = omega_e;
        out.theta_e_rad = theta_e;
        return out;
    }

private:
    PMFluxObserverCfg c;

    // state
    double psi_v_a=0, psi_v_b=0, x_a_prev=0, x_b_prev=0;
    double zhp_a1=0, zhp_a2=0, zhp_b1=0, zhp_b2=0;
    double zlp_a1=0, zlp_a2=0, zlp_b1=0, zlp_b2=0;
    double zim_a1=0, zim_a2=0, zim_b1=0, zim_b2=0;
    double psi_f=0, psi_f_raw=0;
    double Ld_last=0, Lq_last=0;
    double theta_e=0;
    double w0_last=-1;

    // coeffs
    double bhp0=0,bhp1=0,bhp2=0,ahp1=0,ahp2=0;
    double blp0=0,blp1=0,blp2=0,alp1=0,alp2=0;
    double bim0=0,bim1=0,bim2=0,aim1=0,aim2=0;

    static double clamp(double x, double lo, double hi) {
        return std::min(std::max(x, lo), hi);
    }

    static void wrapAngle(double& th) {
        while (th >  2.0 * M_PI) th -= 2.0 * M_PI;
        while (th < -2.0 * M_PI) th += 2.0 * M_PI;
    }

    static void dq2ab(double d, double q, double th, double& a, double& b) {
        const double c = std::cos(th), s = std::sin(th);
        a = c * d - s * q;
        b = s * d + c * q;
    }

    static void ab2dq(double a, double b, double th, double& d, double& q) {
        const double c = std::cos(th), s = std::sin(th);
        d = c * a + s * b;
        q = -s * a + c * b;
    }


    static void bilinear2ndOrder(double b2,double b1,double b0,
                                 double a1,double a0,double T,
                                 double& B0,double& B1,double& B2,double& A1,double& A2)
    {
        const double K = 2.0 / T;
        const double A = K*K + K*a1 + a0;
        B0 = (K*K*b2 + K*b1 + b0) / A;
        B1 = (2.0*(b0 - K*K*b2)) / A;
        B2 = (K*K*b2 - K*b1 + b0) / A;
        A1 = (2.0*(a0 - K*K)) / A;
        A2 = (K*K - K*a1 + a0) / A;
    }

    static void stepBiquad(double x, double& z1, double& z2,
                           double b0,double b1,double b2,double a1,double a2,
                           double& y)
    {
        y = b0 * x + z1;
        const double z1n = b1 * x + z2 - a1 * y;
        const double z2n = b2 * x - a2 * y;
        z1 = z1n; z2 = z2n;
    }

    void updateW0Coeffs(double w0) {
        w0_last = w0;
        double w = w0;
        if (c.prewarp) w = (2.0 / c.Ts) * std::tan(0.5 * w0 * c.Ts);

        // HP: s^2 / (s^2 + 2*zeta*w*s + w^2)
        // LP: (2*zeta*w*s + w^2) / (same den)
        const double den_a1 = 2.0 * c.zeta_conv * w;
        const double den_a0 = w * w;
        bilinear2ndOrder(1.0, 0.0, 0.0, den_a1, den_a0, c.Ts, bhp0,bhp1,bhp2, ahp1,ahp2);
        bilinear2ndOrder(0.0, 2.0*c.zeta_conv*w, w*w, den_a1, den_a0, c.Ts, blp0,blp1,blp2, alp1,alp2);

        // IM correction LP-like section from notebook (if enabled)
        double w_im = c.eta_im * w0;
        if (c.prewarp) w_im = (2.0 / c.Ts) * std::tan(0.5 * w_im * c.Ts);
        const double den_im_a1 = 2.0 * c.zeta_im * w_im;
        const double den_im_a0 = w_im * w_im;
        bilinear2ndOrder(0.0, 2.0*c.zeta_im*w_im, 0.0, den_im_a1, den_im_a0, c.Ts, bim0,bim1,bim2, aim1,aim2);
    }
};
