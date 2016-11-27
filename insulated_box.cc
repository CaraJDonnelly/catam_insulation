#include <cfloat>
#include <iostream>
#include <vector>

#include "insulated_box.h"

namespace catam {

InsulatedBox::InsulatedBox(int resolution, double convergence_tolerance,
  double relaxation_constant, bool verbose_log)
    : resolution_(resolution), convergence_tolerance_(convergence_tolerance),
      relaxation_constant_(relaxation_constant), verbose_log_(verbose_log) {
  grid_width_ = 1.0/resolution_;
  // Initialize position_array to be pairs of (x, y) co-ordinates.
  position_.resize(resolution_);
  for (int i = 0; i < resolution_; ++i) {
    position_[i].resize(resolution_);
    for (int j = 0; j < resolution_; ++j) {
      position_[i][j] = std::make_pair(i*grid_width_, j*grid_width_);
    }
  }
  // Initialize temperature_ with zeroes everywhere.  DoTimestep takes care
  // of boundary conditions.
  temperature_.resize(resolution_);
  for (int i = 0; i < resolution_; ++i) {
    temperature_[i].resize(resolution_);
  }
}

InsulatedBox::BoundaryType InsulatedBox::GetBoundaryType(int i, int j) {
  if (i == 0) return InsulatedBox::HEATED_HOT_BOUNDARY;
  if (i == resolution_ - 1) return InsulatedBox::HEATED_COLD_BOUNDARY;
  if (j == 0) return InsulatedBox::UPPER_BOUNDARY;
  if (j == resolution_ - 1) return InsulatedBox::LOWER_BOUNDARY;
  return InsulatedBox::FREE_AIR;
}

void InsulatedBox::DoTimestep() {
  if (is_converged_) {
    std::cerr << "Called DoTimestep() after convergence, doing nothing."
              << std::endl;
    return;
  }
  ++iteration_;
  double delsq_T;
  double max_abs_delsq_T = DBL_MIN;
  for (int i = 0; i < resolution_; ++i) {
    for (int j = 0; j < resolution_; ++j) {
      switch (GetBoundaryType(i, j)) {
        case HEATED_HOT_BOUNDARY:
          temperature_[i][j] = 1.0;
          break;
        case HEATED_COLD_BOUNDARY:
          temperature_[i][j] = 0.0;
          break;
        // dT/dy = 0.
        case UPPER_BOUNDARY:
          temperature_[i][j] = temperature_[i][j-1];
          break;
        // dT/dy = 0.
        case LOWER_BOUNDARY:
          temperature_[i][j] = temperature_[i][j+1];
          break;
        case FREE_AIR:
          delsq_T = ( 
              (temperature_[i - 1][j] - 2*temperature_[i][j] + temperature_[i + 1][j])
              + (temperature_[i][j-1] - 2*temperature_[i][j] + temperature_[i][j+1])
              )/(grid_width_*grid_width_);
          if (abs(delsq_T) > max_abs_delsq_T) max_abs_delsq_T = abs(delsq_T);
          temperature_[i][j] = (temperature_[i][j]
              + relaxation_constant_*delsq_T);
      };
    }
  }
  if (verbose_log_) {
    std::cerr << "#" << iteration_
              << " Max abs del^2 T: " << max_abs_delsq_T << std::endl;
  }
  if (max_abs_delsq_T < convergence_tolerance_) {
    is_converged_ = true;
  }
  return;
}

bool InsulatedBox::IsConverged() {
  return is_converged_;
}

void InsulatedBox::Log() {
  std::cout << "# Iteration " << iteration_
            << ", is_converged_ " << is_converged_ << std::endl;
  for (int i = 0; i < resolution_; ++i) {
    for (int j = 0; j < resolution_; ++j) {
      std::cout << position_[i][j].first << " "
                << position_[i][j].second << " "
                << temperature_[i][j] << std::endl;
    }
    std::cout << std::endl;
  }
  return;
}

}  // namespace catam
