/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#pragma once

#include <ocs2_core/constraint/ConstraintBase.h>
#include <ocs2_core/cost/CostFunctionBase.h>
#include <ocs2_core/dynamics/SystemDynamicsBase.h>

#include "ocs2_qp_solver/QpSolverTypes.h"
#include "ocs2_qp_solver/QpTrajectories.h"

namespace ocs2 {
namespace qp_solver {

/**
 * Solves an unconstrained discrete-time linear quadratic control problem around a provided linearization trajectory.
 * The time horizon and discretization steps are defined by the time trajectory of the provided linearization.
 *
 * @param cost : continuous cost function
 * @param system : continuous system dynamics
 * @param nominalTrajectory : time, state and input trajectory to make the linear quadratic approximation around
 * @param initialState : state at the start of the horizon.
 * @return time, state, and input solution.
 */
ContinuousTrajectory solveLinearQuadraticOptimalControlProblem(CostFunctionBase& costFunction, SystemDynamicsBase& systemDynamics,
                                                               const ContinuousTrajectory& nominalTrajectory, const vector_t& initialState);

/**
 * Solves a constrained discrete-time linear quadratic control problem around a provided linearization trajectory.
 * The time horizon and discretization steps are defined by the time trajectory of the provided linearization.
 *
 * @param cost : continuous cost function
 * @param system : continuous system dynamics
 * @param constraints : state-input constraints.
 * @param nominalTrajectory : time, state and input trajectory to make the linear quadratic approximation around
 * @param initialState : state at the start of the horizon.
 * @return time, state, and input solution.
 */
ContinuousTrajectory solveLinearQuadraticOptimalControlProblem(CostFunctionBase& costFunction, SystemDynamicsBase& systemDynamics,
                                                               ConstraintBase& constraints, const ContinuousTrajectory& nominalTrajectory,
                                                               const vector_t& initialState);

}  // namespace qp_solver
}  // namespace ocs2
