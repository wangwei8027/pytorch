#include <gtest/gtest.h>

#include <torch/torch.h>

#include <test/cpp/api/support.h>

namespace F = torch::nn::functional;

using namespace torch::nn;

struct FunctionalTest : torch::test::SeedingFixture {};

TEST_F(FunctionalTest, MaxPool1d) {
  auto x = torch::ones({1, 1, 5});
  auto y = F::max_pool1d(x, MaxPool1dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({1, 1, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 2}));
}

TEST_F(FunctionalTest, MaxPool2d) {
  auto x = torch::ones({2, 5, 5});
  auto y = F::max_pool2d(x, MaxPool2dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 2, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 2, 2}));
}

TEST_F(FunctionalTest, MaxPool3d) {
  auto x = torch::ones({2, 5, 5, 5});
  auto y = F::max_pool3d(x, MaxPool3dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 4);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 2, 2, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 2, 2, 2}));
}

TEST_F(FunctionalTest, AvgPool1d) {
  auto x = torch::ones({1, 1, 5});
  auto y = F::avg_pool1d(x, AvgPool1dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({1, 1, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 2}));
}

TEST_F(FunctionalTest, AvgPool2d) {
  auto x = torch::ones({2, 5, 5});
  auto y = F::avg_pool2d(x, AvgPool2dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 2, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 2, 2}));
}

TEST_F(FunctionalTest, AvgPool3d) {
  auto x = torch::ones({2, 5, 5, 5});
  auto y = F::avg_pool3d(x, AvgPool3dOptions(3).stride(2));

  ASSERT_EQ(y.ndimension(), 4);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 2, 2, 2})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 2, 2, 2}));
}

TEST_F(FunctionalTest, LPPool1d) {
  int norm_type = 2;
  int stride = 2;
  int kernel_size = 3;

  auto x = torch::ones({1, 1, 5});
  auto y = F::lp_pool1d(x, LPPool1dOptions(norm_type, kernel_size).stride(stride));
  auto expected = (torch::pow(torch::tensor({{{1, 1}}}, torch::kFloat), norm_type) * kernel_size).pow(1. / norm_type);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, expected));
  ASSERT_EQ(y.sizes(), torch::IntArrayRef({1, 1, 2}));
}

TEST_F(FunctionalTest, LPPool2d) {
  int norm_type = 2;
  int stride = 2;
  std::vector<int64_t> kernel_size({2, 3});

  auto x = torch::ones({1, 2, 5});
  auto y = F::lp_pool2d(x, LPPool2dOptions(norm_type, kernel_size).stride(stride));
  auto expected = (torch::pow(torch::tensor({{{1, 1}}}, torch::kFloat), norm_type) * (kernel_size[0] * kernel_size[1])).pow(1. / norm_type);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, expected));
  ASSERT_EQ(y.sizes(), torch::IntArrayRef({1, 1, 2}));
}

TEST_F(FunctionalTest, CosineSimilarity) {
  auto input1 = torch::tensor({{1, 2, 3}, {4, 5, 6}}, torch::kFloat);
  auto input2 = torch::tensor({{1, 8, 3}, {2, 1, 6}}, torch::kFloat);
  auto output =
      F::cosine_similarity(input1, input2, CosineSimilarityOptions().dim(1));
  auto expected = torch::tensor({0.8078, 0.8721}, torch::kFloat);
  ASSERT_TRUE(output.allclose(expected, 1e-04));
}

TEST_F(FunctionalTest, SoftMarginLossDefaultOptions) {
  auto input = torch::tensor({2., 4., 1., 3.}, torch::requires_grad());
  auto target = torch::tensor({-1., 1., 1., -1.}, torch::kFloat);
  auto output =
      F::soft_margin_loss(input, target);
  auto expected = torch::tensor({1.3767317}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, MultiLabelSoftMarginLossDefaultOptions) {
  auto input = torch::tensor({{0., 2., 2., 0.}, {2., 1., 0., 1.}}, torch::requires_grad());
  auto target = torch::tensor({{0., 0., 1., 0.}, {1., 0., 1., 1.}}, torch::kFloat);
  auto output =
      F::multilabel_soft_margin_loss(input, target);
  auto expected = torch::tensor({0.7608436}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, SoftMarginLossNoReduction) {
  auto input = torch::tensor({2., 4., 1., 3.}, torch::requires_grad());
  auto target = torch::tensor({-1., 1., 1., -1.}, torch::kFloat);
  auto output =
      F::soft_margin_loss(input, target, torch::Reduction::None);
  auto expected = torch::tensor({2.1269281, 0.01814993, 0.3132617, 3.0485873}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, MultiLabelSoftMarginLossWeightedNoReduction) {
  auto input = torch::tensor({{0., 2., 2., 0.}, {2., 1., 0., 1.}}, torch::requires_grad());
  auto target = torch::tensor({{0., 0., 1., 0.}, {1., 0., 1., 1.}}, torch::kFloat);
  auto weight = torch::tensor({0.1, 0.6, 0.4, 0.8}, torch::kFloat);
  auto options = MultiLabelSoftMarginLossOptions().reduction(torch::Reduction::None).weight(weight);
  auto output =
      F::multilabel_soft_margin_loss(input, target, options);
  auto expected = torch::tensor({0.4876902, 0.3321295}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, PairwiseDistance) {
  auto input1 = torch::tensor({{1, 2, 3}, {4, 5, 6}}, torch::kFloat);
  auto input2 = torch::tensor({{1, 8, 3}, {2, 1, 6}}, torch::kFloat);
  auto output =
      F::pairwise_distance(input1, input2, PairwiseDistanceOptions(1));
  auto expected = torch::tensor({6, 6}, torch::kFloat);
  ASSERT_TRUE(output.allclose(expected));
}

TEST_F(FunctionalTest, PDist) {
  {
    auto input = torch::tensor({{-1.0, -5.0, -1.0}, {2.0, 4.0, 6.0}});
    auto output = F::pdist(input);
    auto expected = torch::tensor({11.7898});
    ASSERT_TRUE(output.allclose(expected));
  }
  {
    auto input = torch::tensor({{1.0, -1.0}, {1.0, 3.0}, {3.0, 3.0}});
    auto output = F::pdist(input, 1.5);
    auto expected = torch::tensor({4.0, 4.8945, 2.0});
    ASSERT_TRUE(output.allclose(expected));
  }
}

TEST_F(FunctionalTest, AdaptiveMaxPool1d) {
  auto x = torch::ones({1, 1, 5});
  auto y = F::adaptive_max_pool1d(x, AdaptiveMaxPool1dOptions(3));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({1, 1, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 3}));
}

TEST_F(FunctionalTest, AdaptiveMaxPool2d) {
  auto x = torch::ones({2, 5, 5});
  auto y = F::adaptive_max_pool2d(x, AdaptiveMaxPool2dOptions(3));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 3, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 3, 3}));
}

TEST_F(FunctionalTest, AdaptiveMaxPool3d) {
  auto x = torch::ones({2, 5, 5, 5});
  auto y = F::adaptive_max_pool3d(x, AdaptiveMaxPool3dOptions(3));

  ASSERT_EQ(y.ndimension(), 4);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 3, 3, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 3, 3, 3}));
}

TEST_F(FunctionalTest, AdaptiveAvgPool1d) {
  auto x = torch::ones({1, 1, 5});
  auto y = F::adaptive_avg_pool1d(x, AdaptiveAvgPool1dOptions(3));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({1, 1, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 3}));
}

TEST_F(FunctionalTest, AdaptiveAvgPool2d) {
  auto x = torch::ones({2, 5, 5});
  auto y = F::adaptive_avg_pool2d(x, AdaptiveAvgPool2dOptions(3));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 3, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 3, 3}));
}

TEST_F(FunctionalTest, AdaptiveAvgPool3d) {
  auto x = torch::ones({2, 5, 5, 5});
  auto y = F::adaptive_avg_pool3d(x, AdaptiveAvgPool3dOptions(3));

  ASSERT_EQ(y.ndimension(), 4);
  ASSERT_TRUE(torch::allclose(y, torch::ones({2, 3, 3, 3})));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 3, 3, 3}));
}

TEST_F(FunctionalTest, L1Loss) {
  auto input = torch::randn({5,6}, torch::requires_grad());
  auto target = torch::empty({5,6}).random_(2);
  auto output = F::l1_loss(torch::sigmoid(input), target);
  auto s = output.sum();
  s.backward();

  ASSERT_EQ(output.sizes(), torch::IntArrayRef());
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, MSELoss) {
  auto input = torch::randn({5,6}, torch::requires_grad());
  auto target = torch::empty({5,6}).random_(2);
  auto output = F::mse_loss(torch::sigmoid(input), target);
  auto s = output.sum();
  s.backward();

  ASSERT_EQ(output.sizes(), torch::IntArrayRef());
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, BCELoss) {
  auto input = torch::randn({5,6}, torch::requires_grad());
  auto target = torch::empty({5,6}).random_(2);
  auto output = F::binary_cross_entropy(torch::sigmoid(input), target);
  auto s = output.sum();
  s.backward();

  ASSERT_EQ(output.sizes(), torch::IntArrayRef());
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, KLDivLoss) {
  KLDivLoss loss;
  auto input = torch::randn({5,6}, torch::requires_grad());
  auto target = torch::empty({5,6}).random_(2);
  auto output = F::kl_div(torch::sigmoid(input), target);
  auto s = output.sum();
  s.backward();

  ASSERT_EQ(output.sizes(), torch::IntArrayRef());
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, HingeEmbeddingLoss) {
  auto input = torch::tensor({{2, 22, 4}, {20, 10, 0}}, torch::kFloat);
  auto target = torch::tensor({{2, 6, 4}, {1, 10, 0}}, torch::kFloat);
  auto output = F::hinge_embedding_loss(
      input, target, HingeEmbeddingLossOptions().margin(2));
  auto expected = torch::tensor({10}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));
}

TEST_F(FunctionalTest, GridSample) {
  auto input = torch::arange(9, torch::kFloat).view(std::vector<int64_t>({1, 1, 3, 3}));
  auto grid = torch::tensor({{
      {{-2., -1.}, {-1., -1.}, {0., -1.}},
      {{-1., 0.}, {0., 0.}, {1., 0.}},
      {{0., 1.}, {1., 1.}, {2., 1.}}
  }}, torch::kFloat);

  // bilinear, zeros, true
  auto options = GridSampleOptions()
                    .mode("bilinear")
                    .padding_mode("zeros")
                    .align_corners(true);
  auto output = F::grid_sample(input, grid, options);
  auto expected = torch::tensor({{{{0., 0., 1.}, {3., 4., 5.}, {7., 8., 0.}}}}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));

  // bilinear, zeros, false
  options = GridSampleOptions()
                .mode("bilinear")
                .padding_mode("zeros")
                .align_corners(false);
  output = F::grid_sample(input, grid, options);
  expected = torch::tensor({{{{0., 0., 0.5}, {1.5, 4., 2.5}, {3.5, 2., 0.}}}}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));

  // default options (bilinear, zeros, false) same result as above
  output = F::grid_sample(input, grid);

  ASSERT_TRUE(output.allclose(expected));

  // nearest, zeros, true
  options = GridSampleOptions()
                .mode("nearest")
                .padding_mode("zeros")
                .align_corners(true);
  output = F::grid_sample(input, grid, options);
  expected = torch::tensor({{{{0., 0., 1.}, {3., 4., 5.}, {7., 8., 0.}}}}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));

  // bilinear, border, true
  options = GridSampleOptions()
                .mode("bilinear")
                .padding_mode("border")
                .align_corners(true);
  output = F::grid_sample(input, grid, options);
  expected = torch::tensor({{{{0., 0., 1.}, {3., 4., 5.}, {7., 8., 8.}}}}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));

  // bilinear, reflection, true
  options = GridSampleOptions()
                .mode("bilinear")
                .padding_mode("reflection")
                .align_corners(true);
  output = F::grid_sample(input, grid, options);
  expected = torch::tensor({{{{1., 0., 1.}, {3., 4., 5.}, {7., 8., 7.}}}}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected));
}

TEST_F(FunctionalTest, AffineGrid) {
  {
    // 2D affine.
    auto theta = torch::arange(1, 13, torch::kDouble)
                     .view(std::vector<int64_t>({2, 2, 3}));
    auto size = std::vector<int64_t>({2, 3, 2, 2});
    auto align_corners = true;
    auto output = F::affine_grid(theta, size, !align_corners);
    auto expected = torch::tensor(
        {{{{1.50, 1.50}, {2.50, 5.50}}, {{3.50, 6.50}, {4.50, 10.50}}},
         {{{1.50, 1.50}, {8.50, 11.50}}, {{9.50, 12.50}, {16.50, 22.50}}}});
    auto output_aligned = F::affine_grid(theta, size, align_corners);
    auto expected_aligned = torch::tensor(
        {{{{0.0, -3.0}, {2.0, 5.0}}, {{4.0, 7.0}, {6.0, 15.0}}},
         {{{-6.0, -9.0}, {8.0, 11.0}}, {{10.0, 13.0}, {24.0, 33.0}}}});

    ASSERT_TRUE(output.allclose(expected));
    ASSERT_TRUE(output_aligned.allclose(expected_aligned));
  }
  {
    // 3D affine.
    auto theta = torch::arange(1, 13, torch::kDouble)
                     .view(std::vector<int64_t>({1, 3, 4}));
    auto size = std::vector<int64_t>({1, 1, 3, 2, 2});
    auto align_corners = true;
    auto output = F::affine_grid(theta, size, !align_corners);
    auto expected = torch::tensor(
        {{{{{0.5000, -2.1667, -4.8333}, {1.5000, 2.8333, 4.1667}},
           {{2.5000, 3.8333, 5.1667}, {3.5000, 8.8333, 14.1667}}},
          {{{2.5000, 2.5000, 2.5000}, {3.5000, 7.5000, 11.5000}},
           {{4.5000, 8.5000, 12.5000}, {5.5000, 13.5000, 21.5000}}},
          {{{4.5000, 7.1667, 9.8333}, {5.5000, 12.1667, 18.8333}},
           {{6.5000, 13.1667, 19.8333}, {7.5000, 18.1667, 28.8333}}}}});
    auto output_aligned = F::affine_grid(theta, size, align_corners);
    auto expected_aligned =
        torch::tensor({{{{{-2.0, -10.0, -18.0}, {0.0, 0.0, 0.0}},
                         {{2.0, 2.0, 2.0}, {4.0, 12.0, 20.0}}},
                        {{{1.0, -3.0, -7.0}, {3.0, 7.0, 11.0}},
                         {{5.0, 9.0, 13.0}, {7.0, 19.0, 31.0}}},
                        {{{4.0, 4.0, 4.0}, {6.0, 14.0, 22.0}},
                         {{8.0, 16.0, 24.0}, {10.0, 26.0, 42.0}}}}});

    ASSERT_TRUE(output.allclose(expected, 1e-2));
    ASSERT_TRUE(output_aligned.allclose(expected_aligned));
  }
  {
    auto theta = torch::empty({1, 2, 3}, torch::kDouble);
    auto size = std::vector<int64_t>({1, 1, 2, 2});
    ASSERT_THROWS_WITH(
        F::affine_grid(torch::empty({2, 2, 3}), {-1, 1, 2, 2}),
        "Expected non-zero, positive output size. Got [-1, 1, 2, 2]");
    ASSERT_THROWS_WITH(
        F::affine_grid(torch::empty({2, 2, 3}, torch::kInt), size),
        "Expected theta to have floating point type, but got int");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta[0], size),
        "Expected a batch of 2D affine matrices of shape Nx2x3 for size "
        "[1, 1, 2, 2]. Got [2, 3].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.unsqueeze(0), size),
        "Expected a batch of 2D affine matrices of shape Nx2x3 for size "
        "[1, 1, 2, 2]. Got [1, 1, 2, 3].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.repeat({1, 2, 1}), size),
        "Expected a batch of 2D affine matrices of shape Nx2x3 for size "
        "[1, 1, 2, 2]. Got [1, 4, 3].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.repeat({1, 1, 2}), size),
        "Expected a batch of 2D affine matrices of shape Nx2x3 for size "
        "[1, 1, 2, 2]. Got [1, 2, 6].");
  }
  {
    auto theta = torch::empty({1, 3, 4}, torch::kDouble);
    auto size = std::vector<int64_t>({1, 1, 2, 2, 3});
    ASSERT_THROWS_WITH(
        F::affine_grid(theta[0], size),
        "Expected a batch of 3D affine matrices of shape Nx3x4 for size "
        "[1, 1, 2, 2, 3]. Got [3, 4].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.unsqueeze(0), size),
        "Expected a batch of 3D affine matrices of shape Nx3x4 for size "
        "[1, 1, 2, 2, 3]. Got [1, 1, 3, 4].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.repeat({1, 2, 1}), size),
        "Expected a batch of 3D affine matrices of shape Nx3x4 for size "
        "[1, 1, 2, 2, 3]. Got [1, 6, 4].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta.repeat({1, 1, 2}), size),
        "Expected a batch of 3D affine matrices of shape Nx3x4 for size "
        "[1, 1, 2, 2, 3]. Got [1, 3, 8].");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta, {1, 1, 1, 2, 2, 3}),
        "affine_grid only supports 4D and 5D sizes, for 2D and 3D affine "
        "transforms, respectively. Got size [1, 1, 1, 2, 2, 3]");
    ASSERT_THROWS_WITH(
        F::affine_grid(theta, {1, 1}),
        "affine_grid only supports 4D and 5D sizes, for 2D and 3D affine "
        "transforms, respectively. Got size [1, 1]");
  }
}

TEST_F(FunctionalTest, MultiMarginLoss) {
  auto weight = torch::tensor({0.3, 0.3, 0.4}, torch::kFloat);
  auto input = torch::tensor({{0.2, 0.2, 0.6}, {0.1, 0.8, 0.1}, {0.9, 0.09, 0.01}}, torch::requires_grad());
  auto target = torch::tensor({2, 1, 0}, torch::kLong);
  auto output = F::multi_margin_loss(
    input, target, MultiMarginLossOptions().margin(2).weight(weight));
  auto expected = torch::tensor({0.305556}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected, 1e-04));
}

TEST_F(FunctionalTest, CosineEmbeddingLoss) {
  auto input1 = torch::tensor({{2, 3, 4}, {6, 2, 4}});
  auto input2 = torch::tensor({{2, 3, 5}, {9, 12, 0}});
  auto target = torch::tensor({1, -1});
  auto output = F::cosine_embedding_loss(
      input1, input2, target, CosineEmbeddingLossOptions().margin(0.5));
  auto expected = torch::tensor({0.1004}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected, 1e-4));
}

TEST_F(FunctionalTest, MultiLabelMarginLossDefaultOptions) {
  auto input = torch::tensor({{0.1, 0.2, 0.4, 0.8}}, torch::requires_grad());
  auto target = torch::tensor({{3, 0, -1, 1}}, torch::kLong);
  auto output = F::multilabel_margin_loss(input, target);
  auto expected = torch::tensor({0.8500}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, MultiLabelMarginLossNoReduction) {
  auto input = torch::tensor({{0.1, 0.2, 0.4, 0.8}}, torch::requires_grad());
  auto target = torch::tensor({{3, 0, -1, 1}}, torch::kLong);
  auto output = F::multilabel_margin_loss(
    input, target, torch::Reduction::None);
  auto expected = torch::tensor({0.8500}, torch::kFloat);
  auto s = output.sum();
  s.backward();

  ASSERT_TRUE(output.allclose(expected));
  ASSERT_EQ(input.sizes(), input.grad().sizes());
}

TEST_F(FunctionalTest, TripletMarginLoss) {
  auto anchor = torch::tensor({{3., 3.}}, torch::kFloat);
  auto positive = torch::tensor({{2., 2.}}, torch::kFloat);
  auto negative = torch::tensor({{0., 0.}}, torch::kFloat);
  auto output = F::triplet_margin_loss(
      anchor, positive, negative, TripletMarginLossOptions().margin(1.0));
  auto expected = torch::tensor({0.}, torch::kFloat);

  ASSERT_TRUE(output.allclose(expected, 1e-04));
}

TEST_F(FunctionalTest, MaxUnpool1d) {
  auto x = torch::tensor({{{2, 4, 5}}}, torch::requires_grad());
  auto indices = torch::tensor({{{1, 3, 4}}}, torch::kLong);
  auto y = F::max_unpool1d(x, indices, MaxUnpool1dOptions(3));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(
      y, torch::tensor({{{0, 2, 0, 4, 5, 0, 0, 0, 0}}}, torch::kFloat)));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 9}));

  x = torch::tensor({{{2, 4, 5}}}, torch::requires_grad());
  indices = torch::tensor({{{1, 3, 4}}}, torch::kLong);
  y = F::max_unpool1d(
      x, indices, MaxUnpool1dOptions(3), std::vector<int64_t>({1, 1, 9}));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(torch::allclose(
      y, torch::tensor({{{0, 2, 0, 4, 5, 0, 0, 0, 0}}}, torch::kFloat)));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 9}));

  x = torch::tensor({{{2, 4, 5}}}, torch::requires_grad());
  indices = torch::tensor({{{1, 3, 4}}}, torch::kLong);
  y = F::max_unpool1d(x, indices, MaxUnpool1dOptions(3).stride(2).padding(1));

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_TRUE(
      torch::allclose(y, torch::tensor({{{0, 2, 0, 4, 5}}}, torch::kFloat)));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({1, 1, 5}));
}

TEST_F(FunctionalTest, MaxUnpool2d) {
  auto indices = torch::tensor({
  {{{ 6,  8,  9},
    {16, 18, 19},
    {21, 23, 24}}},
  {{{ 6,  8,  9},
    {16, 18, 19},
    {21, 23, 24}}}}, torch::kLong);
  auto x = torch::tensor({
  {{{ 6,  8,  9},
    {16, 18, 19},
    {21, 23, 24}}},
  {{{31, 33, 34},
    {41, 43, 44},
    {46, 48, 49}}}}, torch::requires_grad());
  auto y = F::max_unpool2d(x, indices, MaxUnpool2dOptions(3).stride(2).padding(1));

  ASSERT_EQ(y.dim(), 4);
  ASSERT_TRUE(torch::allclose(y, torch::tensor(
   {{{{ 0,  0,  0,  0,  0},
      { 0,  6,  0,  8,  9},
      { 0,  0,  0,  0,  0},
      { 0, 16,  0, 18, 19},
      { 0, 21,  0, 23, 24}}},
    {{{ 0,  0,  0,  0,  0},
      { 0, 31,  0, 33, 34},
      { 0,  0,  0,  0,  0},
      { 0, 41,  0, 43, 44},
      { 0, 46,  0, 48, 49}}}} , torch::kFloat)));
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 1, 5, 5}));
}

TEST_F(FunctionalTest, ELU) {
  const auto size = 3;
  for (const auto inplace : {false, true}) {
    for (const auto alpha : {0.0, 0.42, 1.0, 4.2, 42.42}) {
      auto x = torch::linspace(-10.0, 10.0, size * size * size);
      x.resize_({size, size, size});
      auto y_exp = torch::max(torch::zeros_like(x), x) +
                torch::min(torch::zeros_like(x), alpha * (torch::exp(x) - 1.0));
      auto y = F::elu(x, ELUOptions().alpha(alpha).inplace(inplace));

      ASSERT_EQ(y.ndimension(), 3);
      ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
      ASSERT_TRUE(torch::allclose(y, y_exp));
      if (inplace) {
        ASSERT_TRUE(torch::allclose(x, y_exp));
      }
    }
  }
}

TEST_F(FunctionalTest, SELU) {
  {
    const double scale = 1.0507009873554804934193349852946;
    const double alpha = 1.6732632423543772848170429916717;
    for (const auto inplace : {false, true}) {
      auto input = torch::randn({5, 5});
      auto expected = scale *
          (torch::max(torch::zeros_like(input), input) +
           torch::min(
               torch::zeros_like(input), alpha * (torch::exp(input) - 1)));
      auto output = F::selu(input, inplace);

      ASSERT_TRUE(output.allclose(expected));
      if (inplace) {
        ASSERT_TRUE(input.allclose(expected));
      }
    }
  }
  {
    auto input = torch::arange(0, 9, torch::kDouble).view({3, 3});
    auto output = F::selu(input);
    auto expected = F::selu(input, false);
    ASSERT_TRUE(output.allclose(expected));
  }
}

TEST_F(FunctionalTest, Hardshrink) {
  const auto size = 3;
  for (const auto lambda : {-4.2, -1.0, -0.42, 0.0, 0.42, 1.0, 4.2, 42.42}) {
    auto x = torch::linspace(-10.0, 10.0, size * size * size);
    x.resize_({size, size, size}).set_requires_grad(true);
    auto y = F::hardshrink(x, HardshrinkOptions().lambda(lambda));
    torch::Tensor s = y.sum();

    s.backward();
    ASSERT_EQ(s.ndimension(), 0);

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    auto y_exp = (x.abs() > lambda) * x;
    ASSERT_TRUE(torch::allclose(y, y_exp));
  }
}

TEST_F(FunctionalTest, OneHot) {
  { // Test #1
    auto x = torch::arange(0, 5, torch::kLong);
    auto y = F::one_hot(x % 3);
    auto expected = torch::tensor(
        {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}}, torch::kLong);

    ASSERT_EQ(y.ndimension(), 2);
    ASSERT_TRUE(torch::allclose(y, expected));
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({5, 3}));
  }

  { // Test #2
    auto x = torch::arange(0, 5, torch::kLong);
    auto y = F::one_hot(x % 3, 5);
    auto expected = torch::tensor(
        {{1, 0, 0, 0, 0},
         {0, 1, 0, 0, 0},
         {0, 0, 1, 0, 0},
         {1, 0, 0, 0, 0},
         {0, 1, 0, 0, 0}},
        torch::kLong);

    ASSERT_EQ(y.ndimension(), 2);
    ASSERT_TRUE(torch::allclose(y, expected));
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({5, 5}));
  }

  { // Test #3
    auto x = torch::arange(0, 6, torch::kLong);
    auto y = F::one_hot(x.view(std::vector<int64_t>({3, 2})) % 3);
    auto expected = torch::tensor(
        {{{1, 0, 0}, {0, 1, 0}},
         {{0, 0, 1}, {1, 0, 0}},
         {{0, 1, 0}, {0, 0, 1}}},
        torch::kLong);

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_TRUE(torch::allclose(y, expected));
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({3, 2, 3}));
  }
}

TEST_F(FunctionalTest, Hardtanh) {
  const auto size = 3;
  for (const auto min_val : {-4.2, -1.0, -0.42, 0.0}) {
    for (const auto max_val : {0.0, 0.42, 1.0, 4.2}) {
      for (const auto inplace : {false, true}) {
        auto x = torch::linspace(-10.0, 10.0, size * size * size);
        x.resize_({size, size, size});
        auto y_exp = (x < min_val) * min_val +
                     ((x >= min_val) * (x <= max_val)) * x +
                     (x > max_val) * max_val;
        auto y = F::hardtanh(x,HardtanhOptions().min_val(min_val)
          .max_val(max_val).inplace(inplace));

        ASSERT_EQ(y.ndimension(), 3);
        ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
        ASSERT_TRUE(torch::allclose(y, y_exp));
        if (inplace) {
          ASSERT_TRUE(torch::allclose(x, y_exp));
        }
      }
    }
  }
}

TEST_F(FunctionalTest, LeakyReLU) {
  const auto size = 3;
  for (const auto negative_slope : {0.0, 0.42, 1.0}) {
    for (const auto inplace : {false, true}) {
      auto x = torch::linspace(-10.0, 10.0, size * size * size);
      x.resize_({size, size, size});
      auto y_exp = (x < 0) * x * negative_slope + (x >= 0) * x;
      auto y = F::leaky_relu(x, LeakyReLUOptions()
        .negative_slope(negative_slope).inplace(inplace));

      ASSERT_EQ(y.ndimension(), 3);
      ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
      ASSERT_TRUE(torch::allclose(y, y_exp));
      if (inplace) {
        ASSERT_TRUE(torch::allclose(x, y_exp));
      }
    }
  }
}

TEST_F(FunctionalTest, LogSigmoid) {
  const auto size = 3;
  LogSigmoid model;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size});
  auto y = F::logsigmoid(x);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  auto y_exp = torch::log(torch::ones_like(x)/(torch::ones_like(x) + torch::exp(torch::neg(x))));
  ASSERT_TRUE(torch::allclose(y, y_exp, 1e-4, 1e-7));
}

TEST_F(FunctionalTest, GumbelSoftmax) {
  // Test 1: No-options
  {
    auto logits = torch::randn({5});
    int expected_count = 1;
    auto y_draw = F::gumbel_softmax(logits);

    // All values positive
    ASSERT_GE(y_draw.min().item<int>(), 0);
    // Shape unchanged
    ASSERT_TRUE(y_draw.sizes() == logits.sizes());
    // One choice per draw
    ASSERT_TRUE(torch::allclose(y_draw.sum(), torch::tensor(expected_count, torch::kFloat)));
  }

  // Test 2: 1D shape, 0 and -1 dim
  for(const auto dim: {0, -1}) {
    auto logits = torch::randn({5});
    int expected_count = 1;
    auto y_draw = F::gumbel_softmax(logits, GumbelSoftmaxOptions().hard(true).dim(dim));

    // All values positive
    ASSERT_GE(y_draw.min().item<int>(), 0);
    // Shape unchanged
    ASSERT_TRUE(y_draw.sizes() == logits.sizes());
    // One choice per draw
    ASSERT_TRUE(torch::allclose(y_draw.sum(), torch::tensor(expected_count, torch::kFloat)));
  }

  { // Test 3: 2D shape, 1 dim
    auto logits = torch::randn({5, 4});
    int expected_count = 5;
    auto y_draw = F::gumbel_softmax(logits, GumbelSoftmaxOptions().hard(true).dim(1));

    // All values positive
    ASSERT_GE(y_draw.min().item<int>(), 0);
    // Shape unchanged
    ASSERT_TRUE(y_draw.sizes() == logits.sizes());
    // One choice per draw
    ASSERT_TRUE(torch::allclose(y_draw.sum(), torch::tensor(expected_count, torch::kFloat)));
  }

  // Test 4: 3D shape, 1 and -1 dim
  int dims[] = {1, -1};
  int expected[] = {5*3, 5*4};
  for(auto i=0; i<2; i++) {
    auto logits = torch::randn({5, 4, 3});
    int expected_count = expected[i];
    auto y_draw = F::gumbel_softmax(logits, GumbelSoftmaxOptions().hard(true).dim(dims[i]));

    // All values positive
    ASSERT_GE(y_draw.min().item<int>(), 0);
    // Shape unchanged
    ASSERT_TRUE(y_draw.sizes() == logits.sizes());
    // One choice per draw
    ASSERT_TRUE(torch::allclose(y_draw.sum(), torch::tensor(expected_count, torch::kFloat)));
  }

  { // Test 5: Straight through
    int num_draws = 100;
    auto logits = torch::tensor({{0.2, 0.8, 0.1}});
    logits = logits.reshape({1, 3});
    logits.requires_grad();
    auto probs = logits.softmax(-1);

    auto counts = torch::zeros_like(logits);
    torch::Tensor y_draw;
    for (auto i=0; i<num_draws; i++) {
        y_draw = F::gumbel_softmax(logits, GumbelSoftmaxOptions().hard(true));
        counts += y_draw;
    }

    // All values positive
    ASSERT_GE(y_draw.min().item<int>(), 0);
    // Each experiment should result in 1 draw
    ASSERT_EQ(counts.sum().item<int>(), num_draws);

    // Check results are asymptotically as expected
    auto expected = probs * num_draws;
    // ~z is approximately N(0,1) for unbiased count
    auto z = (counts - expected) / (expected * (1 - probs)).sqrt();
    // A (lazy) approximate 99% two-sided test:
    // occurs with prob alpha~>=0.01 if unbiased
    ASSERT_LT(z.abs().max().item<float>(), 2.58);
  }
}

TEST_F(FunctionalTest, Softmax) {
  auto input = torch::arange(10, torch::kFloat).reshape({2, 5});
  auto output = F::softmax(input, /*dim=*/1);
  auto sum = torch::sum(torch::exp(input), 1);

  for (int i = 0; i < 2; i++) {
    auto expected = torch::exp(input[i]) / sum[i];
    ASSERT_TRUE(torch::allclose(output[i], expected));
  }
}

TEST_F(FunctionalTest, Softmin) {
  auto input = torch::arange(10, torch::kFloat).reshape({2, 5});
  auto output = F::softmin(input, /*dim=*/1);
  auto sum = torch::sum(torch::exp(-input), 1);

  for (int i = 0; i < 2; i++) {
    auto expected = torch::exp(-input[i]) / sum[i];
    ASSERT_TRUE(torch::allclose(output[i], expected));
  }
}

TEST_F(FunctionalTest, LogSoftmax) {
  auto input = torch::arange(10, torch::kFloat).reshape({2, 5});
  auto output = F::log_softmax(input, /*dim=*/1);
  auto sum = torch::sum(torch::exp(input), 1);

  for (int i = 0; i < 2; i++) {
    auto expected = torch::log(torch::exp(input[i]) / sum[i]);
    ASSERT_TRUE(torch::allclose(output[i], expected));
  }
}

TEST_F(FunctionalTest, PReLU) {
  const auto x = torch::rand({42, 24}) * 200 - 100;
  const auto w = torch::rand(24) * 200 - 100;
  const auto y = F::prelu(x, w);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({42, 24}));
  const auto y_exp = (x < 0) * w * x  + (x >= 0) * x;
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, LayerNorm) {
  const auto input = torch::randn({2, 2});
  auto y = F::layer_norm(input, LayerNormOptions({2, 2}).eps(2e-5));
  auto y_exp = torch::layer_norm(input, {2, 2}, torch::Tensor(), torch::Tensor(), 2e-5);
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, Linear) {
  {
    const auto x = torch::arange(100, 118).resize_({3, 3, 2});
    const auto w = torch::arange(200, 206).resize_({3, 2});
    const auto b = torch::arange(300, 303);
    const auto y = F::linear(x, w, b);
    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), torch::IntArrayRef({3, 3, 3}));
    const auto y_exp = torch::tensor(
      {{{40601, 41004, 41407},
        {41403, 41814, 42225},
        {42205, 42624, 43043}},
      {{43007, 43434, 43861},
        {43809, 44244, 44679},
        {44611, 45054, 45497}},
      {{45413, 45864, 46315},
        {46215, 46674, 47133},
        {47017, 47484, 47951}}},
      torch::kFloat
    );
    ASSERT_TRUE(torch::allclose(y, y_exp));
  }
  {
    const auto x = torch::arange(100, 118).resize_({3, 3, 2});
    const auto w = torch::arange(200, 206).resize_({3, 2});
    const auto y = F::linear(x, w);
    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), torch::IntArrayRef({3, 3, 3}));
    const auto y_exp = torch::tensor(
      {{{40301, 40703, 41105},
        {41103, 41513, 41923},
        {41905, 42323, 42741}},
       {{42707, 43133, 43559},
        {43509, 43943, 44377},
        {44311, 44753, 45195}},
       {{45113, 45563, 46013},
        {45915, 46373, 46831},
        {46717, 47183, 47649}}},
      torch::kFloat
    );
    ASSERT_TRUE(torch::allclose(y, y_exp));
  }
}

TEST_F(FunctionalTest, Bilinear) {
  auto input1 = torch::tensor({{1, 2, 3}, {7, 6, 5}});
  auto input2 = torch::tensor({{7, 4}, {8 ,9}});
  auto weight = torch::tensor({{{2, 3}, {9, 7}, {8, 6}}});
  auto bias = torch::tensor({1});

  auto y_with_bias = F::bilinear(input1, input2, weight, bias);
  ASSERT_EQ(y_with_bias.ndimension(), 2);
  ASSERT_EQ(y_with_bias.sizes(), torch::IntArrayRef({2, 1}));
  auto y_with_bias_exp = torch::tensor({{449}, {1702}}).reshape({2, 1});
  ASSERT_TRUE(torch::allclose(y_with_bias, y_with_bias_exp, 1e-4, 1e-7));

  auto y_no_bias = F::bilinear(input1, input2, weight);
  ASSERT_EQ(y_no_bias.ndimension(), 2);
  ASSERT_EQ(y_no_bias.sizes(), torch::IntArrayRef({2, 1}));
  auto y_no_bias_exp = torch::tensor({{448, 1701}}).reshape({2, 1});
  ASSERT_TRUE(torch::allclose(y_no_bias, y_no_bias_exp, 1e-4, 1e-7));
}

TEST_F(FunctionalTest, Normalize) {
  const auto expected = torch::tensor(
    {{{0.00000000, 0.10000000, 0.2000, 0.30000000, 0.40000000},
      {0.14285715, 0.17142858, 0.2000, 0.22857143, 0.25714287}}}, torch::requires_grad().dtype(torch::kFloat));
  { // Test #1
    auto input = torch::tensor({{{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}}}, torch::dtype(torch::kFloat).requires_grad(true));
    auto norm = F::normalize(input, NormalizeOptions().p(1).dim(-1));

    // reduce to scalar to call .backward()
    torch::Tensor s = norm.sum();
    s.backward();

    ASSERT_EQ(s.ndimension(), 0);
    ASSERT_EQ(input.grad().numel(), 10);
    ASSERT_TRUE(torch::allclose(norm, expected));
  }

  { // Test #2 Check variations of optional arguments
    auto input = torch::tensor({{{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}}}, torch::dtype(torch::kFloat));
    auto output = torch::randn({1,2,5}, torch::dtype(torch::kFloat));
    // non-null output argument
    F::normalize(input, NormalizeOptions().p(1).dim(-1), output);
    // default options
    F::normalize(input);

    ASSERT_TRUE(torch::allclose(output, expected));
  }

  { // Test #3 Base case of scalar tensor
    auto input = torch::randn({}, torch::requires_grad());
    torch::Tensor norm = F::normalize(input, NormalizeOptions().p(1).dim(-1));
    norm.backward();

    ASSERT_EQ(input.grad().numel(), 1);
  }
}

TEST_F(FunctionalTest, GeLU) {
  auto x = torch::tensor({{2., 3.}, {4., 5.}});
  auto y_exp = torch::tensor({{1.9545, 2.9960}, {3.9999, 5.0000}}) +
        torch::tensor({{-2.3842e-07, -4.9829e-05}, {-2.6703e-05, -1.4305e-06}});
  auto y = F::gelu(x);
  ASSERT_EQ(y.ndimension(), 2);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({2, 2}));
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, ReLU) {
  const auto size = 3;
  for (const auto inplace : {false, true}) {
    auto x = torch::linspace(-10.0, 10.0, size * size * size);
    x.resize_({size, size, size});
    auto y_exp = (x < 0) * 0 + (x >= 0) * x;
    auto y = F::relu(x, ReLUOptions().inplace(inplace));

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    ASSERT_TRUE(torch::allclose(y, y_exp));
    if (inplace) {
      ASSERT_TRUE(torch::allclose(x, y_exp));
    }

    y = F::relu(x, /*inplace=*/inplace);

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    ASSERT_TRUE(torch::allclose(y, y_exp));
    if (inplace) {
      ASSERT_TRUE(torch::allclose(x, y_exp));
    }
  }
}

TEST_F(FunctionalTest, ReLUDefaultOptions) {
  const auto size = 3;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size});
  auto y_exp = (x < 0) * 0 + (x >= 0) * x;
  auto y = F::relu(x);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, ReLU6) {
  const auto size = 3;
  for (const auto inplace : {false, true}) {
    auto x = torch::linspace(-10.0, 10.0, size * size * size);
    x.resize_({size, size, size});
    auto y_exp = (x < 0) * 0 + ((x >= 0) * (x <= 6)) * x + (x > 6) * 6;
    auto y = F::relu6(x, ReLU6Options().inplace(inplace));

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    ASSERT_TRUE(torch::allclose(y, y_exp));
    if (inplace) {
      ASSERT_TRUE(torch::allclose(x, y_exp));
    }

    y = F::relu6(x, /*inplace=*/inplace);

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    ASSERT_TRUE(torch::allclose(y, y_exp));
    if (inplace) {
      ASSERT_TRUE(torch::allclose(x, y_exp));
    }
  }
}

TEST_F(FunctionalTest, ReLU6DefaultOptions) {
  const auto size = 3;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size});
  auto y_exp = (x < 0) * 0 + ((x >= 0) * (x <= 6)) * x + (x > 6) * 6;
  auto y = F::relu6(x);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, RReLU) {
  const auto size = 3;
  for (const auto lower : {0.01, 0.1, 0.2}) {
    for (const auto upper : {0.3, 0.4, 0.5}) {
      for (const auto inplace : {false, true}) {
        auto x = torch::linspace(-10.0, 10.0, size * size * size);
        x.resize_({size, size, size});
        auto x_copy = x.clone();
        auto y = F::rrelu(x, RReLUOptions().lower(lower)
          .upper(upper).inplace(inplace));
        auto z = ((x_copy >= 0) * (x_copy == y) +
          (x_copy < 0) * (y >= x_copy * upper) * (y <= lower * x_copy)) * 1.0;

        ASSERT_EQ(y.ndimension(), 3);
        ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
        ASSERT_TRUE(torch::allclose(z, torch::ones_like(z)));
        if (inplace) {
          ASSERT_TRUE(torch::allclose(x, y));
        }
      }
    }
  }
}

TEST_F(FunctionalTest, RReLUDefaultOptions) {
  const auto size = 3;
  const auto lower = 1.0 / 8.0;
  const auto upper = 1.0 / 3.0;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size});
  auto x_copy = x.clone();
  auto y = F::rrelu(x);
  auto z = ((x_copy >= 0) * (x_copy == y) +
    (x_copy < 0) * (y >= x_copy * upper) * (y <= lower * x_copy)) * 1.0;

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  ASSERT_TRUE(torch::allclose(z, torch::ones_like(z)));
}

TEST_F(FunctionalTest, CELU) {
  const auto size = 3;
  for (const auto inplace : {false, true}) {
    for (const auto alpha : {0.42, 1.0, 4.2, 42.42}) {
      auto x = torch::linspace(-10.0, 10.0, size * size * size);
      x.resize_({size, size, size});
      auto y_exp = torch::max(torch::zeros_like(x), x) +
        torch::min(torch::zeros_like(x), alpha * (torch::exp(x / alpha) - 1.0));
      auto y = F::celu(x, CELUOptions().alpha(alpha).inplace(inplace));

      ASSERT_EQ(y.ndimension(), 3);
      ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
      ASSERT_TRUE(torch::allclose(y, y_exp));
      if (inplace) {
        ASSERT_TRUE(torch::allclose(x, y_exp));
      }
    }
  }
}

TEST_F(FunctionalTest, CELUDefaultOptions) {
  const auto size = 3;
  const auto alpha = 1.0;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size});
  auto y_exp = torch::max(torch::zeros_like(x), x) +
    torch::min(torch::zeros_like(x), alpha * (torch::exp(x / alpha) - 1.0));
  auto y = F::celu(x);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, PixelShuffle) {
  auto x = torch::tensor(
    {{{{-17, 19}, {-1, 2}},
      {{7, 14}, {-3, 1}},
      {{0, -2}, {-12, 14}},
      {{-15, 0}, {-3, 9}}}}, torch::kFloat);
  auto y_exp = torch::tensor(
    {{{{-17, 7, 19, 14},
       {0, -15, -2, 0},
       {-1, -3, 2, 1},
       {-12, -3, 14, 9}}}}, torch::kFloat);
  auto y = F::pixel_shuffle(x, 2);

  ASSERT_EQ(y.ndimension(), 4);
  ASSERT_EQ(y.sizes(), torch::IntArrayRef({1, 1, 4, 4}));
  ASSERT_TRUE(y.allclose(y_exp));
}

TEST_F(FunctionalTest, Softplus) {
  const auto size = 3;
  for (const auto beta : {0.5, 1.0, 2.0}) {
    for (const auto threshold : {1.0, 3.0, 5.0}) {
      auto x = torch::linspace(-3.0, 3.0, 61);
      x.resize_({size, size, size});
      auto y_exp =
        (x <= threshold) * torch::log(1 + torch::exp(x * beta)) / beta +
        (x > threshold) * x;
      auto y = F::softplus(x,
        SoftplusOptions().beta(beta).threshold(threshold));

      ASSERT_EQ(y.ndimension(), 3);
      ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
      ASSERT_TRUE(torch::allclose(y, y_exp));
    }
  }
}

TEST_F(FunctionalTest, SoftplusDefaultOptions) {
  const auto size = 3;
  const auto beta = 1.0;
  const auto threshold = 20.0;
  auto x = torch::linspace(-3.0, 3.0, 61);
  x.resize_({size, size, size});
  auto y_exp =
    (x <= threshold) * torch::log(1 + torch::exp(x * beta)) / beta +
    (x > threshold) * x;
  auto y = F::softplus(x);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, Unfold) {
  auto input = torch::randn({2, 2, 4, 4}, torch::requires_grad());
  auto output = F::unfold(input, UnfoldOptions({2, 4}).padding(1).stride(2));
  auto expected_sizes = std::vector<int64_t>({2, 16, 6});

  ASSERT_EQ(output.sizes(), expected_sizes);
}

TEST_F(FunctionalTest, Softshrink) {
  const auto size = 3;
  for (const auto lambda : {0.0, 0.42, 1.0, 4.2, 42.42}) {
    auto x = torch::linspace(-10.0, 10.0, size * size * size);
    x.resize_({size, size, size}).set_requires_grad(true);
    auto y = F::softshrink(x, /*lambda=*/lambda);
    torch::Tensor s = y.sum();

    s.backward();
    ASSERT_EQ(s.ndimension(), 0);

    ASSERT_EQ(y.ndimension(), 3);
    ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
    auto y_exp = (x < -lambda) * (x + lambda) + (x > lambda) * (x - lambda);
    ASSERT_TRUE(torch::allclose(y, y_exp));
  }
}

TEST_F(FunctionalTest, SoftshrinkDefaultOptions) {
  const auto size = 3;
  const auto lambda = 0.5;
  auto x = torch::linspace(-10.0, 10.0, size * size * size);
  x.resize_({size, size, size}).set_requires_grad(true);
  auto y = F::softshrink(x);
  torch::Tensor s = y.sum();

  s.backward();
  ASSERT_EQ(s.ndimension(), 0);

  ASSERT_EQ(y.ndimension(), 3);
  ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
  auto y_exp = (x < -lambda) * (x + lambda) + (x > lambda) * (x - lambda);
}

TEST_F(FunctionalTest, Softsign) {
  auto x = torch::randn(100) * 10;
  auto y_exp = x / (1 + x.abs());
  auto y = F::softsign(x);

  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, Tanhshrink) {
  auto x = torch::randn(100) * 10;
  auto y_exp = x - x.tanh();
  auto y = F::tanhshrink(x);

  ASSERT_TRUE(torch::allclose(y, y_exp));
}

TEST_F(FunctionalTest, Threshold) {
  const auto size = 3;
  for (const auto threshold : {0.5, 1.0, 2.0}) {
    for (const auto value : {0.5, 1.0, 2.0}) {
      for (const auto inplace : {false, true}) {
        auto x = torch::linspace(-3.0, 3.0, 61);
        x.resize_({size, size, size});
        auto y_exp = (x <= threshold) * value + (x > threshold) * x;
        auto y = F::threshold(x,
          ThresholdOptions(threshold, value).inplace(inplace));

        ASSERT_EQ(y.ndimension(), 3);
        ASSERT_EQ(y.sizes(), std::vector<int64_t>({size, size, size}));
        ASSERT_TRUE(torch::allclose(y, y_exp));
        if (inplace) {
          ASSERT_TRUE(torch::allclose(x, y_exp));
        }
      }
    }
  }
}

TEST_F(FunctionalTest, Pad) {
  {
    auto input = torch::arange(6, torch::kDouble).reshape({1, 2, 3});
    auto output = F::pad(input, PadOptions({1, 2}).mode(torch::kCircular));
    auto expected = torch::tensor({{{2., 0., 1., 2., 0., 1.},
                                    {5., 3., 4., 5., 3., 4.}}}, torch::kDouble);
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 2, 6}));
    ASSERT_TRUE(output.allclose(expected, 1e-04));
  }
  {
    auto input = torch::arange(9, torch::kDouble).reshape({1, 1, 3, 3});
    auto output = F::pad(input, PadOptions({3, 3, 3, 1}).mode(torch::kCircular));
    auto expected = torch::tensor(
       {{{{0., 1., 2., 0., 1., 2., 0., 1., 2.},
          {3., 4., 5., 3., 4., 5., 3., 4., 5.},
          {6., 7., 8., 6., 7., 8., 6., 7., 8.},
          {0., 1., 2., 0., 1., 2., 0., 1., 2.},
          {3., 4., 5., 3., 4., 5., 3., 4., 5.},
          {6., 7., 8., 6., 7., 8., 6., 7., 8.},
          {0., 1., 2., 0., 1., 2., 0., 1., 2.}}}}, torch::kDouble);
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 1, 7, 9}));
    ASSERT_TRUE(output.allclose(expected, 1e-04));
  }
  {
    auto input = torch::arange(12, torch::kDouble).reshape({1, 1, 2, 2, 3});
    auto output = F::pad(input, PadOptions({3, 3, 2, 1, 2, 2}).mode(torch::kCircular));
    auto expected = torch::tensor(
       {{{{{ 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.}},

          {{ 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.}},

          {{ 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.}},

          {{ 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.}},

          {{ 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.},
           { 3.,  4.,  5.,  3.,  4.,  5.,  3.,  4.,  5.},
           { 0.,  1.,  2.,  0.,  1.,  2.,  0.,  1.,  2.}},

          {{ 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.},
           { 9., 10., 11.,  9., 10., 11.,  9., 10., 11.},
           { 6.,  7.,  8.,  6.,  7.,  8.,  6.,  7.,  8.}}}}}, torch::kDouble);
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 1, 6, 5, 9}));
    ASSERT_TRUE(output.allclose(expected, 1e-04));
  }
  {
    auto input = torch::arange(16, torch::kDouble).reshape({2, 2, 2, 2});
    auto output = F::pad(input, PadOptions({1, 1, 1, 1}).mode(torch::kReflect));
    auto expected = torch::tensor(
       {{{{ 3.,  2.,  3.,  2.},
          { 1.,  0.,  1.,  0.},
          { 3.,  2.,  3.,  2.},
          { 1.,  0.,  1.,  0.}},

         {{ 7.,  6.,  7.,  6.},
          { 5.,  4.,  5.,  4.},
          { 7.,  6.,  7.,  6.},
          { 5.,  4.,  5.,  4.}}},

        {{{11., 10., 11., 10.},
          { 9.,  8.,  9.,  8.},
          {11., 10., 11., 10.},
          { 9.,  8.,  9.,  8.}},

         {{15., 14., 15., 14.},
          {13., 12., 13., 12.},
          {15., 14., 15., 14.},
          {13., 12., 13., 12.}}}}, torch::kDouble);
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({2, 2, 4, 4}));
    ASSERT_TRUE(output.allclose(expected, 1e-04));
  }
  {
    auto input = torch::arange(12, torch::kDouble).reshape({1, 1, 2, 2, 3});
    auto output = F::pad(input, PadOptions({1, 2, 2, 1, 1, 2}).mode(torch::kReplicate));
    auto expected = torch::tensor(
       {{{{{ 0.,  0.,  1.,  2.,  2.,  2.},
           { 0.,  0.,  1.,  2.,  2.,  2.},
           { 0.,  0.,  1.,  2.,  2.,  2.},
           { 3.,  3.,  4.,  5.,  5.,  5.},
           { 3.,  3.,  4.,  5.,  5.,  5.}},

          {{ 0.,  0.,  1.,  2.,  2.,  2.},
           { 0.,  0.,  1.,  2.,  2.,  2.},
           { 0.,  0.,  1.,  2.,  2.,  2.},
           { 3.,  3.,  4.,  5.,  5.,  5.},
           { 3.,  3.,  4.,  5.,  5.,  5.}},

          {{ 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 9.,  9., 10., 11., 11., 11.},
           { 9.,  9., 10., 11., 11., 11.}},

          {{ 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 9.,  9., 10., 11., 11., 11.},
           { 9.,  9., 10., 11., 11., 11.}},

          {{ 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 6.,  6.,  7.,  8.,  8.,  8.},
           { 9.,  9., 10., 11., 11., 11.},
           { 9.,  9., 10., 11., 11., 11.}}}}}, torch::kDouble);
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 1, 5, 5, 6}));
    ASSERT_TRUE(output.allclose(expected, 1e-04));
  }
  {
    auto input = torch::ones({1, 1, 1, 1}, torch::kDouble);
    auto output = F::pad(input, PadOptions({1, 1}).mode(torch::kConstant).value(0));
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 1, 1, 3}));
    auto expected = torch::tensor({{{{0., 1., 0.}}}}, torch::kDouble);
  }
  {
    auto input = torch::ones({1, 1, 1, 1}, torch::kDouble);
    auto output = F::pad(input, PadOptions({1, 1}));
    ASSERT_EQ(output.sizes(), std::vector<int64_t>({1, 1, 1, 3}));
    auto expected = torch::tensor({{{{0., 1., 0.}}}}, torch::kDouble);
  }
}
