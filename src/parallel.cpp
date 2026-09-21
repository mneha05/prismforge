#include "prismforge/parallel.hpp"

#include <stdexcept>
#include <vector>

#ifdef PRISMFORGE_USE_MPI
#include <mpi.h>
#endif

#ifdef PRISMFORGE_USE_OPENMP
#include <omp.h>
#endif

namespace prismforge {

ParallelRuntime::ParallelRuntime(int &argc, char **&argv) {
#ifdef PRISMFORGE_USE_MPI
  int initialized = 0;
  MPI_Initialized(&initialized);
  if (!initialized) {
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) throw std::runtime_error("MPI_Init failed");
    owns_mpi_ = true;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &size_);
#else
  (void)argc;
  (void)argv;
#endif
}

ParallelRuntime::~ParallelRuntime() {
#ifdef PRISMFORGE_USE_MPI
  int finalized = 0;
  MPI_Finalized(&finalized);
  if (owns_mpi_ && !finalized) MPI_Finalize();
#endif
}

void ParallelRuntime::setThreadCount(int threads) const {
  if (threads <= 0) throw std::invalid_argument("thread count must be positive");
#ifdef PRISMFORGE_USE_OPENMP
  omp_set_dynamic(0);
  omp_set_num_threads(threads);
#else
  (void)threads;
#endif
}

void ParallelRuntime::reduceImage(Image &image) const {
#ifdef PRISMFORGE_USE_MPI
  std::vector<double> local(image.pixels().size() * 3);
  std::vector<double> reduced(isRoot() ? local.size() : 0);
  for (std::size_t i = 0; i < image.pixels().size(); ++i) {
    local[3*i] = image.pixels()[i].x;
    local[3*i + 1] = image.pixels()[i].y;
    local[3*i + 2] = image.pixels()[i].z;
  }
  MPI_Reduce(local.data(), isRoot() ? reduced.data() : nullptr,
             static_cast<int>(local.size()), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (isRoot()) {
    for (std::size_t i = 0; i < image.pixels().size(); ++i) {
      image.pixels()[i] = {reduced[3*i], reduced[3*i + 1], reduced[3*i + 2]};
    }
  }
#else
  (void)image;
#endif
}

}  // namespace prismforge
