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

namespace ocs2{

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::CostFunctionBaseAD(
		const bool& dynamicLibraryIsCompiled /*= false*/)
	: BASE()
	, dynamicLibraryIsCompiled_(dynamicLibraryIsCompiled)
	, modelName_("")
	, libraryFolder_("")
{
	if (dynamicLibraryIsCompiled==true)
		setADInterfaces();
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::CostFunctionBaseAD(
		const CostFunctionBaseAD& rhs)

	: BASE(rhs)
	, dynamicLibraryIsCompiled_(rhs.dynamicLibraryIsCompiled_)
	, modelName_(rhs.modelName_)
	, libraryFolder_(rhs.libraryFolder_)
	, intermediateADInterfacePtr_(rhs.intermediateADInterfacePtr_->clone())
	, terminalADInterfacePtr_(rhs.terminalADInterfacePtr_->clone())

{
	if (rhs.dynamicLibraryIsCompiled_==true)
		loadModels(false);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
typename CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::BASE*
	CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::clone() const {

		return new Derived(static_cast<Derived const&>(*this));
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
template <typename SCALAR_T>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::intermediateCostFunction(
		const SCALAR_T& time,
		const Eigen::Matrix<SCALAR_T, STATE_DIM, 1>& state,
		const Eigen::Matrix<SCALAR_T, INPUT_DIM, 1>& input,
		SCALAR_T& costValue) {

	throw std::runtime_error("intermediateCostFunction() method should be implemented by the derived class.");
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
template <typename SCALAR_T>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::terminalCostFunction(
		const SCALAR_T& time,
		const Eigen::Matrix<SCALAR_T, STATE_DIM, 1>& state,
		SCALAR_T& costValue) {

	throw std::runtime_error("terminalCostFunction() method should be implemented by the derived class.");
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::createModels(
		const std::string& modelName,
		const std::string& libraryFolder) {

	modelName_ = modelName;
	libraryFolder_ = libraryFolder;

	createModels(true);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::loadModels(
		const std::string& modelName,
		const std::string& libraryFolder) {

	modelName_ = modelName;
	libraryFolder_ = libraryFolder;

	if (dynamicLibraryIsCompiled_==true) {
		bool libraryLoaded = loadModels(false);
		if (libraryLoaded==false)
			throw std::runtime_error("CostFunction library is not found!");

	} else {
		throw std::runtime_error("CostFunction library has not been compiled!");
	}
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
const bool& CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::isDynamicLibraryCompiled() const {

	return dynamicLibraryIsCompiled_;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
std::string CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getModelName() const {

	return modelName_;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::setCurrentStateAndControl(
		const scalar_t& t,
		const state_vector_t& x,
		const input_vector_t& u) {

	BASE::setCurrentStateAndControl(t, x, u);

	tapedInput_ << t, x, u;

	intermediateADInterfacePtr_->getJacobian(tapedInput_, intermediateJacobian_);
	intermediateADInterfacePtr_->getHessian(tapedInput_, intermediateHessian_, 0);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCost(
		scalar_t& L) {

	Eigen::Matrix<scalar_t, 1, 1> costValue;
	intermediateADInterfacePtr_->getFunctionValue(tapedInput_, costValue);
	L = costValue(0);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCostDerivativeState(
		state_vector_t& dLdx) {

	dLdx = intermediateJacobian_.template segment<state_dim_>(1);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCostSecondDerivativeState(
		state_matrix_t& dLdxx) {

	dLdxx = intermediateHessian_.template block<state_dim_, state_dim_>(1, 1);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCostDerivativeInput(
		input_vector_t& dLdu) {

	dLdu = intermediateJacobian_.template segment<input_dim_>(1+state_dim_);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCostSecondDerivativeInput(
		input_matrix_t& dLduu) {

	dLduu = intermediateHessian_.template block<input_dim_, input_dim_>(1+state_dim_, 1+state_dim_);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getIntermediateCostDerivativeInputState(
		input_state_matrix_t& dLdux) {

	dLdux = intermediateHessian_.template block<input_dim_, state_dim_>(1+state_dim_, 1);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getTerminalCost(
		scalar_t& Phi) {

	Eigen::Matrix<scalar_t, 1, 1> costValue;
	terminalADInterfacePtr_->getFunctionValue(tapedInput_, costValue);
	Phi = costValue(0);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getTerminalCostDerivativeState(
		state_vector_t& dPhidx) {

	terminalADInterfacePtr_->getJacobian(tapedInput_, terminalJacobian_);
	dPhidx = terminalJacobian_.template segment<state_dim_>(1);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::getTerminalCostSecondDerivativeState(
		state_matrix_t& dPhidxx) {

	terminalADInterfacePtr_->getHessian(tapedInput_, terminalHessian_, 0);
	dPhidxx = terminalHessian_.template block<state_dim_, state_dim_>(1, 1);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::intermediateCostFunctionAD(
		const ad_dynamic_vector_t& tapedInput,
		ad_dynamic_vector_t& costValue) {

	ad_scalar_t& t = const_cast<ad_scalar_t&>(tapedInput(0));
	Eigen::Matrix<ad_scalar_t, STATE_DIM, 1> x = tapedInput.segment(1, STATE_DIM);
	Eigen::Matrix<ad_scalar_t, INPUT_DIM, 1> u = tapedInput.segment(1+STATE_DIM, INPUT_DIM);

	costValue.resize(1);
	ad_scalar_t& costValueScalar = costValue(0);
	static_cast<Derived *>(this)->template intermediateCostFunction<ad_scalar_t>(t, x, u, costValueScalar);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::terminalCostFunctionAD(
		const ad_dynamic_vector_t& tapedInput,
		ad_dynamic_vector_t& costValue) {

	ad_scalar_t& t = const_cast<ad_scalar_t&>(tapedInput(0));
	Eigen::Matrix<ad_scalar_t, STATE_DIM, 1> x = tapedInput.segment(1, STATE_DIM);

	costValue.resize(1);
	ad_scalar_t& costValueScalar = costValue(0);
	static_cast<Derived *>(this)->template terminalCostFunction<ad_scalar_t>(t, x, costValueScalar);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::setADInterfaces() {

	intermediateCostAD_ = [this](
			const ad_dynamic_vector_t& x,
			ad_dynamic_vector_t& y) {
		this->intermediateCostFunctionAD(x, y);
	};

	terminalCostAD_ = [this](
			const ad_dynamic_vector_t& x,
			ad_dynamic_vector_t& y) {
		this->terminalCostFunctionAD(x, y);
	};

	intermediateSparsityPattern_.setOnes();
	intermediateSparsityPattern_.col(0).setZero();

	terminalSparsityPattern_.setOnes();
	terminalSparsityPattern_.col(0).setZero();
	terminalSparsityPattern_.template rightCols<input_dim_>().setZero();

	intermediateADInterfacePtr_.reset( new ad_interface_t(
			intermediateCostAD_, intermediateSparsityPattern_) );

	terminalADInterfacePtr_.reset( new ad_interface_t(
			terminalCostAD_, terminalSparsityPattern_) );

	intermediateADInterfacePtr_->computeForwardModel(true);
	intermediateADInterfacePtr_->computeJacobianModel(true);
	intermediateADInterfacePtr_->computeHessianModel(true);

	terminalADInterfacePtr_->computeForwardModel(true);
	terminalADInterfacePtr_->computeJacobianModel(true);
	terminalADInterfacePtr_->computeHessianModel(true);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
void CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::createModels(
		bool verbose) {

	// sets all the required CppAdCodeGenInterfaces
	setADInterfaces();

	intermediateADInterfacePtr_->createModels(modelName_+"_intermediate", libraryFolder_, verbose);
	terminalADInterfacePtr_->createModels(modelName_+"_terminal", libraryFolder_, verbose);

	dynamicLibraryIsCompiled_ = true;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <class Derived, size_t STATE_DIM, size_t INPUT_DIM, class logic_rules_template_t>
bool CostFunctionBaseAD<Derived, STATE_DIM, INPUT_DIM, logic_rules_template_t>::loadModels(
		bool verbose) {

	bool intermediateLoaded = intermediateADInterfacePtr_->loadModels(modelName_+"_intermediate", libraryFolder_, verbose);

	bool terminalLoaded = terminalADInterfacePtr_->loadModels(modelName_+"_terminal", libraryFolder_, verbose);

	return (intermediateLoaded && terminalLoaded);
}

} // namespace ocs2
