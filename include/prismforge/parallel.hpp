#pragma once

#include "prismforge/tracer.hpp"

namespace prismforge {

class ParallelRuntime {
 public:
  ParallelRuntime(int &argc, char **&argv);
  ~ParallelRuntime();

  ParallelRuntime(const ParallelRuntime &) = delete;
  ParallelRuntime &operator=(const ParallelRuntime &) = delete;

  int rank() const { return rank_; }
  int size() const { return size_; }
  bool isRoot() const { return rank_ == 0; }
  void setThreadCount(int threads) const;
  void reduceImage(Image &image) const;

 private:
  int rank_{0};
  int size_{1};
  bool owns_mpi_{false};
};

}  // namespace prismforge
