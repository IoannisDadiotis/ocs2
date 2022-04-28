#include <gtest/gtest.h>

#include <ocs2_core/thread_support/ThreadPool.h>

#include "ocs2_oc/oc_problem/OcpMatrixConstruction.h"
#include "ocs2_oc/pre_condition/Scaling.h"
#include "ocs2_oc/test/testProblemsGeneration.h"

class ScalingTest : public testing::Test {
 protected:
  // x_0, x_1, ... x_{N - 1}, X_{N}
  static constexpr size_t N_ = 10;  // numStages
  static constexpr size_t nx_ = 4;
  static constexpr size_t nu_ = 3;
  static constexpr size_t numDecisionVariables = N_ * (nx_ + nu_);
  static constexpr size_t numConstraints = N_ * nx_;

  ScalingTest() {
    srand(0);

    x0 = ocs2::vector_t::Random(nx_);
    for (int i = 0; i < N_; i++) {
      dynamicsArray.push_back(ocs2::getRandomDynamics(nx_, nu_));
      costArray.push_back(ocs2::getRandomCost(nx_, nu_));
    }
    costArray.push_back(ocs2::getRandomCost(nx_, nu_));

    ocpSize_ = ocs2::extractSizesFromProblem(dynamicsArray, costArray, nullptr);
  }

  ocs2::OcpSize ocpSize_;
  ocs2::vector_t x0;
  std::vector<ocs2::VectorFunctionLinearApproximation> dynamicsArray;
  std::vector<ocs2::ScalarFunctionQuadraticApproximation> costArray;
};

TEST_F(ScalingTest, preConditioningSparseMatrix) {
  ocs2::vector_t D, E;
  ocs2::scalar_t c;

  Eigen::SparseMatrix<ocs2::scalar_t> H;
  ocs2::vector_t h;
  ocs2::getCostMatrixSparse(ocpSize_, x0, costArray, H, h);
  // Copy of the original matrix/vector
  const Eigen::SparseMatrix<ocs2::scalar_t> H_src = H;
  const ocs2::vector_t h_src = h;

  Eigen::SparseMatrix<ocs2::scalar_t> G;
  ocs2::vector_t g;
  ocs2::getConstraintMatrixSparse(ocpSize_, x0, dynamicsArray, nullptr, nullptr, G, g);
  // Copy of the original matrix/vector
  const Eigen::SparseMatrix<ocs2::scalar_t> G_src = G;
  const ocs2::vector_t g_src = g;

  // Test 1: Construct the stacked cost and constraints matrices first and scale next.
  ocs2::preConditioningSparseMatrixInPlace(H, h, G, 5, D, E, c);

  // After pre-conditioning, H, h, G will be scaled in place. g remains the same.
  const Eigen::SparseMatrix<ocs2::scalar_t> H_ref = c * D.asDiagonal() * H_src * D.asDiagonal();
  const ocs2::vector_t h_ref = c * D.asDiagonal() * h_src;
  const Eigen::SparseMatrix<ocs2::scalar_t> G_ref = E.asDiagonal() * G_src * D.asDiagonal();
  const ocs2::vector_t g_ref = E.asDiagonal() * g_src;

  // The scaled matrix given by calculatePreConditioningFactors function and the one calculated with D, E and c factors should be the same.
  ASSERT_TRUE(H_ref.isApprox(H));                                                    // H
  ASSERT_TRUE(h_ref.isApprox(h));                                                    // h
  ASSERT_TRUE(G_ref.isApprox(G)) << "G_ref:\n" << G_ref << "\nG:\n" << G.toDense();  // G

  // Normalized the inf-Norm of both rows and cols of KKT matrix should be closer to 1
  const ocs2::matrix_t KKT = (ocs2::matrix_t(H_src.rows() + G_src.rows(), H_src.rows() + G_src.rows()) << H_src.toDense(),
                              G_src.transpose().toDense(), G_src.toDense(), ocs2::matrix_t::Zero(G_src.rows(), G_src.rows()))
                                 .finished();

  const ocs2::matrix_t KKTScaled = (ocs2::matrix_t(H.rows() + G.rows(), H.rows() + G.rows()) << H.toDense(), G.transpose().toDense(),
                                    G.toDense(), ocs2::matrix_t::Zero(G.rows(), G.rows()))
                                       .finished();

  // Inf-norm of row vectors
  ocs2::vector_t infNormRows = KKT.cwiseAbs().rowwise().maxCoeff();
  ocs2::vector_t infNormRowsScaled = KKTScaled.cwiseAbs().rowwise().maxCoeff();
  EXPECT_LT((infNormRowsScaled.array() - 1).abs().maxCoeff(), (infNormRows.array() - 1).abs().maxCoeff());
  // Inf-norm of column vectors
  ocs2::vector_t infNormCols = KKT.cwiseAbs().colwise().maxCoeff();
  ocs2::vector_t infNormColsScaled = KKTScaled.cwiseAbs().colwise().maxCoeff();
  EXPECT_LT((infNormColsScaled.array() - 1).abs().maxCoeff(), (infNormCols.array() - 1).abs().maxCoeff());

  // Test 2: Scale const and dynamics data of each time step first and then construct the stacked matrices from the scaled data.
  std::vector<ocs2::vector_t> scalingVectors;
  ocs2::scaleDataInPlace(ocpSize_, D, E, c, dynamicsArray, costArray, scalingVectors);

  Eigen::SparseMatrix<ocs2::scalar_t> H_scaledData;
  ocs2::vector_t h_scaledData;
  ocs2::getCostMatrixSparse(ocpSize_, x0, costArray, H_scaledData, h_scaledData);
  Eigen::SparseMatrix<ocs2::scalar_t> G_scaledData;
  ocs2::vector_t g_scaledData;
  ocs2::getConstraintMatrixSparse(ocpSize_, x0, dynamicsArray, nullptr, &scalingVectors, G_scaledData, g_scaledData);
  EXPECT_TRUE(H_ref.isApprox(H_scaledData));  // H
  EXPECT_TRUE(h_ref.isApprox(h_scaledData));  // h
  EXPECT_TRUE(G_ref.isApprox(G_scaledData));  // G
  EXPECT_TRUE(g_ref.isApprox(g_scaledData));  // g
}

TEST_F(ScalingTest, preConditioningInPlaceInParallel) {
  ocs2::ThreadPool threadPool(5, 99);

  ocs2::vector_t D_ref, E_ref;
  ocs2::scalar_t c_ref;

  Eigen::SparseMatrix<ocs2::scalar_t> H_ref;
  ocs2::vector_t h_ref;
  ocs2::getCostMatrixSparse(ocpSize_, x0, costArray, H_ref, h_ref);
  // Copy of the original matrix/vector
  const Eigen::SparseMatrix<ocs2::scalar_t> H_src = H_ref;
  const ocs2::vector_t h_src = h_ref;

  Eigen::SparseMatrix<ocs2::scalar_t> G_ref;
  ocs2::vector_t g_ref;
  ocs2::getConstraintMatrixSparse(ocpSize_, x0, dynamicsArray, nullptr, nullptr, G_ref, g_ref);
  // Copy of the original matrix/vector
  const Eigen::SparseMatrix<ocs2::scalar_t> G_src = G_ref;
  const ocs2::vector_t g_src = g_ref;
  // Generate reference
  ocs2::preConditioningSparseMatrixInPlace(H_ref, h_ref, G_ref, 5, D_ref, E_ref, c_ref);
  g_ref = E_ref.asDiagonal() * g_src;

  // Test start
  ocs2::vector_array_t D_array, E_array;
  ocs2::scalar_t c;
  ocs2::vector_array_t scalingVectors(N_);
  ocs2::preConditioningInPlaceInParallel(x0, ocpSize_, 5, dynamicsArray, costArray, D_array, E_array, scalingVectors, c, threadPool, H_src,
                                         h_src, G_src);

  ocs2::vector_t D_stacked(D_ref.rows()), E_stacked(E_ref.rows());
  int curRow = 0;
  for (auto& v : D_array) {
    D_stacked.segment(curRow, v.size()) = v;
    curRow += v.size();
  }
  curRow = 0;
  for (auto& v : E_array) {
    E_stacked.segment(curRow, v.size()) = v;
    curRow += v.size();
  }

  EXPECT_TRUE(D_stacked.isApprox(D_ref));
  EXPECT_TRUE(E_stacked.isApprox(E_ref));
  EXPECT_DOUBLE_EQ(c, c_ref);

  Eigen::SparseMatrix<ocs2::scalar_t> H_scaledData;
  ocs2::vector_t h_scaledData;
  ocs2::getCostMatrixSparse(ocpSize_, x0, costArray, H_scaledData, h_scaledData);
  Eigen::SparseMatrix<ocs2::scalar_t> G_scaledData;
  ocs2::vector_t g_scaledData;
  ocs2::getConstraintMatrixSparse(ocpSize_, x0, dynamicsArray, nullptr, &scalingVectors, G_scaledData, g_scaledData);

  EXPECT_TRUE(H_ref.isApprox(H_scaledData));  // H
  EXPECT_TRUE(h_ref.isApprox(h_scaledData));  // h
  EXPECT_TRUE(G_ref.isApprox(G_scaledData));  // G
  EXPECT_TRUE(g_ref.isApprox(g_scaledData));  // g
}
