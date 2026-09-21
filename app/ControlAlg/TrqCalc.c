/*
 * File: TrqCalc.c
 *
 * Code generated for Simulink model 'ControlSystem_v2'.
 *
 * Model version                  : 5.217
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Mon Apr 21 10:17:07 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. MISRA C:2012 guidelines
 *    2. Execution efficiency
 * Validation result: Not run
 */

#include "ControlSystem_v2.h"
#include "rtwtypes.h"
#include "TrqCalc.h"
#include "indTable.h"
#include "systemDefinations.h"
#include "ControlSystem_v2_private.h"

/*
 * Compile-time torque-model selector.
 *
 *   0 - Proposed COTM (current five-coefficient model)
 *   1 - Original dq model with Ld/Lq lookup tables
 *   2 - Constant-parameter dq model
 *   3 - Full polynomial surface, degree 2
 *   4 - Full polynomial surface, degree 3
 */
#define TRQ_MODEL_COTM             0
#define TRQ_MODEL_LUT_DQ           1
#define TRQ_MODEL_CONSTANT_DQ      2
#define TRQ_MODEL_POLYNOMIAL_2     3
#define TRQ_MODEL_POLYNOMIAL_3     4

#ifndef isFunk
#define isFunk TRQ_MODEL_LUT_DQ
#endif

/*
 * The notebook models predict motor-shaft torque.
 *
 * The current COTM implementation used 58.59 = 1.5 * 4 * 9.765,
 * therefore this file keeps the gearbox/output-shaft scaling for every
 * selectable model. Set TRQ_MODEL_OUTPUT_SCALE to 1.0F if TrqCalc must
 * return motor-shaft torque instead.
 */
#define TRQ_MODEL_OUTPUT_SCALE     9.765F

/*
 * Polynomial preprocessing from total.ipynb:
 *
 *   Id_n = (Id - mean_Id) / scale_Id
 *   Iq_n = (Iq - mean_Iq) / scale_Iq
 *
 * Reciprocal scales are stored to replace division with multiplication.
 */
#define TRQ_POLY_ID_MEAN           (-141.9758064516129F)
#define TRQ_POLY_IQ_MEAN           (269.1720430107527F)
#define TRQ_POLY_ID_INV_SCALE      (0.01142910558732231F)
#define TRQ_POLY_IQ_INV_SCALE      (0.00692074044951777F)

static real32_T trq_model_cotm(
  real32_T Id,
	real32_T Iq,
	uint16_T motorPoles)
{
	const real32_T Iq2 = Iq * Iq;
	const real32_T Iq4 = Iq2 * Iq2;

	const real32_T torque_pm =
	  (-3.085404536910588E-5F * Iq2) +
	  (5.724472636066932E-2F * Iq);

	const real32_T torque_rel =
	  ((-2.3702624307921933E-13F * Iq4) +
	   (3.6328865609212724E-7F  * Iq2) +
	   (-2.4256518605057296E-4F  * Iq)) * Id;

	return TRQ_MODEL_OUTPUT_SCALE *
	  (1.5F * (real32_T)motorPoles) *
	  (torque_pm + torque_rel);
}

static real32_T trq_model_constant_dq(
  real32_T Id,
	real32_T Iq,
	uint16_T motorPoles)
{
	const real32_T psi_f_eff =
	  0.04555926446300784F;

	const real32_T ld_minus_lq_eff =
	  -0.00011628260419340186F;

	return TRQ_MODEL_OUTPUT_SCALE *
	  (1.5F * (real32_T)motorPoles) *
	  Iq *
	  (psi_f_eff + (ld_minus_lq_eff * Id));
}

static real32_T trq_model_polynomial_2(
  real32_T Id,
	real32_T Iq)
{
	const real32_T x =
	  (Id - TRQ_POLY_ID_MEAN) * TRQ_POLY_ID_INV_SCALE;

	const real32_T y =
	  (Iq - TRQ_POLY_IQ_MEAN) * TRQ_POLY_IQ_INV_SCALE;

	/*
	 * Feature order from sklearn PolynomialFeatures:
	 * 1, x, y, x^2, x*y, y^2
	 *
	 * Horner-like form reduces temporary values and multiplications.
	 */
	const real32_T torque_motor =
	  112.84743939343328F +
	  x * (
	    -18.636856017893233F +
	    x * 0.4006895946430921F +
	    y * -5.36766969777831F
	) +
	y * (
	  43.96062464213155F +
	  y * -8.014755473638074F
	);

	return TRQ_MODEL_OUTPUT_SCALE * torque_motor;
}

static real32_T trq_model_polynomial_3(
  real32_T Id,
	real32_T Iq)
{
	const real32_T x =
	  (Id - TRQ_POLY_ID_MEAN) * TRQ_POLY_ID_INV_SCALE;

	const real32_T y =
	  (Iq - TRQ_POLY_IQ_MEAN) * TRQ_POLY_IQ_INV_SCALE;

	/*
	 * Feature order from sklearn PolynomialFeatures:
	 * 1, x, y, x^2, x*y, y^2, x^3, x^2*y, x*y^2, y^3
	 */
	const real32_T torque_motor =
	  113.24561888008624F +
	  x * (
	    -21.570082419815794F +
	    x * (
	      -0.09883991010963569F +
	      0.20363994336909896F * x
	) +
	y * (
	  -5.768956575036627F +
	  0.23268889423516756F * x +
	  2.908564975731124F * y
	)
  ) +
  y * (
    42.31645987054697F +
    y * (
      -8.131577432656636F +
      1.0425220855903423F * y
)
);

	return TRQ_MODEL_OUTPUT_SCALE * torque_motor;
}

/* Output and update for atomic system: '<S376>/LdLq_Tables' */
void LdLq_Tables(real32_T Id,
	real32_T iq,
	real32_T *Lq_table,
	real32_T
                 *Ld_table_c,
	DW_LdLq_Tables *localDW)
{
	int16_T Abs_di;
	uint16_T Ld_lookUp;

	/* Abs: '<S381>/Abs' incorporates:
	 *  DataTypeConversion: '<S381>/Data Type Conversion1'
	 */
	if ((int16_T)iq < 0) {
		Abs_di = (int16_T)(-((int16_T)iq));
	}
	else {
		Abs_di = (int16_T)iq;
	}

	/* End of Abs: '<S381>/Abs' */

	/* Lookup_n-D: '<S381>/Lq_lookUp' incorporates:
	 *  Abs: '<S381>/Abs'
	 *  DataTypeConversion: '<S381>/Data Type Conversion'
	 */
	Ld_lookUp = look2_is16lu32n16tu16_pbinlcse((int16_T)Id,
		Abs_di,
		&LookUpData.IdVect[0],
		&LookUpData.IqVect[0],
		&LookUpData.Lqmatr[0],
		localDW->m_bpIndex,
		ConstP_d.pooled31,
		50UL);

	/* Gain: '<S381>/Gain1' */
	*Lq_table = 9.99989425E-9F * (real32_T)Ld_lookUp;

	/* Lookup_n-D: '<S381>/Ld_lookUp' incorporates:
	 *  Abs: '<S381>/Abs'
	 *  DataTypeConversion: '<S381>/Data Type Conversion'
	 */
	Ld_lookUp = look2_is16lu32n16tu16_pbinlcse((int16_T)Id,
		Abs_di,
		&LookUpData.IdVect[0],
		&LookUpData.IqVect[0],
		&LookUpData.Ldmatr[0],
		localDW->m_bpIndex_i,
		ConstP_d.pooled31,
		50UL);

	/* Gain: '<S381>/Gain2' */
	*Ld_table_c = 9.99989425E-9F * (real32_T)Ld_lookUp;
}

/* Output and update for atomic system: '<S320>/TrqCalc' */
void TrqCalc(real32_T Id,
	real32_T Iq,
	real32_T ImaxRef,
	real32_T Emf,
	uint16_T
             motorPoles,
	real32_T motorLq_k,
	real32_T motorLd_g,
	real32_T tMotor,
	DW_TrqCalc *localDW)
{
	real32_T Id_f;
	real32_T Iq_f;

	/* Sum: '<S386>/Add1' incorporates:
	 *  Constant: '<S386>/Filter_Constant'
	 *  Constant: '<S386>/One'
	 *  Product: '<S386>/Product'
	 *  Product: '<S386>/Product1'
	 *  UnitDelay: '<S386>/Unit Delay'
	 */
	localDW->UnitDelay_DSTATE =
	  (Id * 0.2F) +
	  (0.8F * localDW->UnitDelay_DSTATE);

	/* Sum: '<S380>/Add' incorporates:
	 *  Constant: '<S389>/Filter_Constant'
	 *  Product: '<S389>/Product'
	 */
	localDW->Merge_e = Iq * 0.2F;

	/* Gain: '<S381>/Gain2' incorporates:
	 *  UnitDelay: '<S389>/Unit Delay'
	 */
	localDW->Merge1_i = localDW->UnitDelay_DSTATE_n;

	/* Sum: '<S389>/Add1' incorporates:
	 *  Constant: '<S389>/One'
	 *  Product: '<S389>/Product1'
	 *  UnitDelay: '<S389>/Unit Delay'
	 */
	localDW->UnitDelay_DSTATE_n =
	  localDW->Merge_e +
	  (0.8F * localDW->UnitDelay_DSTATE_n);

	Id_f = localDW->UnitDelay_DSTATE;
	Iq_f = localDW->UnitDelay_DSTATE_n;

#if (isFunk == TRQ_MODEL_LUT_DQ)

	{
		real32_T torque_factor;
		real32_T torque_reluctance;

		/* Outputs for Atomic SubSystem: '<S376>/LdLq_Tables' */
		LdLq_Tables(
		  Id_f,
			Iq_f,
			&localDW->Merge_e,
			&localDW->Merge1_i,
			&localDW->LdLq_Tables_h);

		if (SystemParameters.StaticInductionFlg > 0U) {
			localDW->Merge_e = motorLq_k;
			localDW->Merge1_i = motorLd_g;
		}

		localDW->Add_ct =
		  localDW->Merge1_i -
		  localDW->Merge_e;

		torque_reluctance =
		  localDW->Add_ct * Id_f;

		torque_factor =
		  TRQ_MODEL_OUTPUT_SCALE *
		  1.5F *
		  (real32_T)motorPoles;

		localDW->Torque_calc =
		  torque_factor *
		  ((Emf * Iq_f) +
		   (torque_reluctance * Iq_f));

		localDW->TeMAxCalcc =
		  torque_factor *
		  ((Emf * ImaxRef) +
		   (torque_reluctance * ImaxRef));
	}

#elif (isFunk == TRQ_MODEL_COTM)

	(void)Emf;
	(void)motorLq_k;
	(void)motorLd_g;

	localDW->Torque_calc =
	  trq_model_cotm(
	    Id_f,
		Iq_f,
		motorPoles);

	localDW->TeMAxCalcc =
	  trq_model_cotm(
	    Id_f,
		ImaxRef,
		motorPoles);

#elif (isFunk == TRQ_MODEL_CONSTANT_DQ)

	(void)Emf;
	(void)motorLq_k;
	(void)motorLd_g;

	localDW->Torque_calc =
	  trq_model_constant_dq(
	    Id_f,
		Iq_f,
		motorPoles);

	localDW->TeMAxCalcc =
	  trq_model_constant_dq(
	    Id_f,
		ImaxRef,
		motorPoles);

#elif (isFunk == TRQ_MODEL_POLYNOMIAL_2)

	(void)Emf;
	(void)motorLq_k;
	(void)motorLd_g;
	(void)motorPoles;

	localDW->Torque_calc =
	  trq_model_polynomial_2(
	    Id_f,
		Iq_f);

	localDW->TeMAxCalcc =
	  trq_model_polynomial_2(
	    Id_f,
		ImaxRef);

#elif (isFunk == TRQ_MODEL_POLYNOMIAL_3)

	(void)Emf;
	(void)motorLq_k;
	(void)motorLd_g;
	(void)motorPoles;

	localDW->Torque_calc =
	  trq_model_polynomial_3(
	    Id_f,
		Iq_f);

	localDW->TeMAxCalcc =
	  trq_model_polynomial_3(
	    Id_f,
		ImaxRef);

#else
#error "Unsupported isFunk torque-model selector"
#endif

	/*
	 * Keep the resistance-temperature update active for every torque model.
	 * In the previous #if/#else version it was skipped when isFunc == 0.
	 */
	localDW->Product_ob =
	  SystemParameters.MotorParams.motorRs *
	  ((0.004F * (tMotor - 25.0F)) + 1.0F);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */