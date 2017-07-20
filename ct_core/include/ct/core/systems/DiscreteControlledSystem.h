/*
 * DiscreteControlledSystem.h
 *
 *  Created on: Jul 13, 2017
 *      Author: neunertm
 */

#ifndef INCLUDE_CT_CORE_SYSTEMS_DISCRETECONTROLLEDSYSTEM_H_
#define INCLUDE_CT_CORE_SYSTEMS_DISCRETECONTROLLEDSYSTEM_H_

#include "DiscreteSystem.h"
#include <ct/core/control/DiscreteController.h>

namespace ct {
namespace core {

//! A general, non-linear discrete dynamic system with a control input
/*!
 * This describes a general, non-linear discrete dynamic system of the following form
 *
 * \f[
 *  x_{n+1} = f(x_n,u_n,n)
 * \f]
 *
 * where \f$ x_{n} \f$ is the state, \f$ u_{n} \f$ the control input and \f$ n \f$ the time index.
 *
 * For implementing your own ControlledSystem, derive from this class.
 *
 * We generally assume that the Controller is a state and time index dependent function \f$ u_n = g(x_n,n) \f$
 * which allows any ControlledSystem to be re-written as a System of the form
 *
 * \f[
 *  x_{n+1} = f(x_n,u_n(x_n,n),n) = g(x_n,n)
 * \f]
 *
 * which can be forward propagated directly.
 *
 * @tparam STATE_DIM dimension of state vector
 * @tparam CONTROL_DIM dimension of input vector
 * @tparam SCALAR scalar type
 */
template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR = double>
class DiscreteControlledSystem : public DiscreteSystem<STATE_DIM, CONTROL_DIM, SCALAR>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef typename std::shared_ptr<DiscreteControlledSystem<STATE_DIM, CONTROL_DIM, SCALAR>> Ptr;

	//! default constructor
	/*!
	 * @param type system type
	 */
	DiscreteControlledSystem(const SYSTEM_TYPE& type = SYSTEM_TYPE::GENERAL)
	: 	DiscreteSystem<STATE_DIM, CONTROL_DIM, SCALAR>(type),
	  	controller_(nullptr)
	{};

	//! constructor
	/*!
	 *
	 * @param controller controller
	 * @param type system type
	 */
	DiscreteControlledSystem(
			std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> > controller,
			const SYSTEM_TYPE& type = SYSTEM_TYPE::GENERAL)
	: 	DiscreteSystem<STATE_DIM, CONTROL_DIM, SCALAR>(type),
	  	controller_(controller)
	{};

	//! copy constructor
	DiscreteControlledSystem(const ControlledSystem<STATE_DIM, CONTROL_DIM, SCALAR>& arg):
		DiscreteSystem<STATE_DIM, CONTROL_DIM, SCALAR>(arg)
	{
		if(arg.controller_)
			controller_ = std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> > (arg.controller_->clone());
	}

	//! destructor
	virtual ~DiscreteControlledSystem() {};

	//! deep copy
	virtual DiscreteControlledSystem<STATE_DIM, CONTROL_DIM, SCALAR>* clone() const override = 0;

	//! set a new controller
	/*!
	 * @param controller new controller
	 */
	void setController(const std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> >& controller)
	{
		controller_ = controller;
	}

	//! get the controller instance
	/*!
	 * \todo remove this function (duplicate of getController() below)
	 * @param controller controller instance
	 */
	void getController(std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> >& controller) const
	{
		controller = controller_;
	}

	//! get the controller instace
	/*!
	 * @return controller instance
	 */
	std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> > getController()
	{
		return controller_;
	}


	//! propagates the system dynamics forward by one step
	/*!
	 * evaluates \f$ x_{n+1} = f(x_n, n) \f$ at a given state and index
	 * @param state start state to propagate from
	 * @param n time index to propagate the dynamics at
	 * @param stateNext propagated state
	 */
	virtual void propagateDynamics(
		const StateVector<STATE_DIM, SCALAR>& state,
		const int& n,
		StateVector<STATE_DIM, SCALAR>& stateNext) override
	{
		ControlVector<CONTROL_DIM, SCALAR> controlAction;
		if(controller_)
			controller_->computeControl(state, n, controlAction);
		else
			controlAction.setZero();

		propagateControlledDynamics(state, n, controlAction, stateNext);
	}


	virtual void propagateControlledDynamics(
			const StateVector<STATE_DIM, SCALAR>& state,
			const int& n,
			const ControlVector<CONTROL_DIM, SCALAR>& control,
			StateVector<STATE_DIM, SCALAR>& stateNext
	) = 0;


protected:
	std::shared_ptr<DiscreteController<STATE_DIM, CONTROL_DIM, SCALAR> > controller_; //!< the controller instance

};

}
}



#endif /* INCLUDE_CT_CORE_SYSTEMS_DISCRETECONTROLLEDSYSTEM_H_ */