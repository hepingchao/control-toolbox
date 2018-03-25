/**********************************************************************************************************************
This file is part of the Control Toolbox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Licensed under Apache2 license (see LICENSE file in main directory)
**********************************************************************************************************************/

#pragma once

#include "SystemModelBase.h"

namespace ct {
namespace optcon {

/*!
 * \ingroup Filter
 *
 * \brief System model adapted to CT. System model encapsulates the integrator, so it is able to propagate the system,
 *        but also computes derivatives w.r.t. both state and noise. When propagating the system, CTSystemModel does not
 *        use the specified control input, but uses the assigned system controller instead.
 *
 * @tparam STATE_DIM
 * @tparam CONTROL_DIM
 */
template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR = double>
class CTSystemModel : public SystemModelBase<STATE_DIM, CONTROL_DIM, SCALAR>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using Base = SystemModelBase<STATE_DIM, CONTROL_DIM, SCALAR>;
    using typename Base::state_vector_t;
    using typename Base::state_matrix_t;
    using typename Base::control_vector_t;
    using typename Base::Time_t;

    //! Constructor. Takes in the system with defined controller, and sens approximator for computing the derivatives
    CTSystemModel(std::shared_ptr<ct::core::ControlledSystem<STATE_DIM, CONTROL_DIM, SCALAR>> system,
        const ct::core::SensitivityApproximation<STATE_DIM, CONTROL_DIM, STATE_DIM / 2, STATE_DIM / 2, SCALAR>&
            sensApprox,
        double dt,
        unsigned numSubsteps                     = 0u,
        const state_matrix_t& dFdv               = state_matrix_t::Identity(),
        const ct::core::IntegrationType& intType = ct::core::IntegrationType::EULERCT);

    //! Propagates the system giving the next state as output. Control input is generated by the system controller.
    state_vector_t computeDynamics(const state_vector_t& state,
        const control_vector_t& controlPlaceholder,
        Time_t t) override;

    //! Computes the derivative w.r.t state. Control input is generated by the system controller.
    state_matrix_t computeDerivativeState(const state_vector_t& state,
        const control_vector_t& controlPlaceholder,
        Time_t t) override;
    //! Computes the derivative w.r.t noise. Control input is generated by the system controller.
    state_matrix_t computeDerivativeNoise(const state_vector_t& state,
        const control_vector_t& control,
        Time_t t) override;

protected:
    std::shared_ptr<ct::core::ControlledSystem<STATE_DIM, CONTROL_DIM, SCALAR>> system_;  //! The underlying CT system.
    ct::core::SensitivityApproximation<STATE_DIM, CONTROL_DIM, STATE_DIM / 2, STATE_DIM / 2, SCALAR>
        sensApprox_;                                                  //! CT sens approximator.
    double dt_;                                                       //! Time step for the approximator.
    state_matrix_t dFdv_;                                             // Derivative w.r.t. noise.
    ct::core::Integrator<STATE_DIM, SCALAR> integrator_;              //! Integrator.
    unsigned numSubsteps_;                                            // Integration substeps.
    state_matrix_t A_;                                                // dFdx or the derivative w.r.t. state.
    ct::core::StateControlMatrix<STATE_DIM, CONTROL_DIM, SCALAR> B_;  //! Matrix for storing control linearization.
};

}  // optcon
}  // ct
