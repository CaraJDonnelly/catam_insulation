#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <vector>

#include "insulated_box.h"

namespace catam {

InsulatedBox::InsulatedBox(double convergence_tolerance,
  double relaxation_constant, bool verbose_log,
  std::unique_ptr<std::vector<std::vector<char>>> walls)
    : resolution_(walls->size()), convergence_tolerance_(convergence_tolerance),
      relaxation_constant_(relaxation_constant), verbose_log_(verbose_log),
      walls_(std::move(walls)) {
  grid_width_ = 1.0/resolution_;
  // Initialize position_array to be pairs of (x, y) co-ordinates.
  position_.resize(resolution_);
  for (int i = 0; i < resolution_; ++i) {
    position_[i].resize(resolution_);
    for (int j = 0; j < resolution_; ++j) {
      position_[i][j] = std::make_pair(i*grid_width_, j*grid_width_);
    }
  }
  // Initialize temperature_ with random noise everywhere.  DoTimestep takes
  // care of boundary conditions.
  temperature_.resize(resolution_);
  std::srand(std::time(0));
  for (int i = 0; i < resolution_; ++i) {
    temperature_[i].resize(resolution_);
    for (int j = 0; j < resolution_; ++j) {
      temperature_[i][j] = 1.0*(std::rand()%100)/100;
    }
  }
}

InsulatedBox::BoundaryType InsulatedBox::GetBoundaryType(int i, int j) {
  char wall_code = (*walls_)[i][j];
  switch (wall_code) {
    case 'U':
      return InsulatedBox::UPPER_BOUNDARY;
      break;
    case 'L':
      return InsulatedBox::LOWER_BOUNDARY;
      break;
    case 'H':
      return InsulatedBox::HEATED_HOT_BOUNDARY;
      break;
    case 'C':
      return InsulatedBox::HEATED_COLD_BOUNDARY;
      break;
    case 'I':
      return InsulatedBox::INSIDE_INSULATION;
      break;
    case '"':
      return InsulatedBox::RIGHT_BOUNDARY;
      break;
    case 'v':
      return InsulatedBox::LEFT_BOUNDARY;
      break;
    case ' ':
      return InsulatedBox::FREE_AIR;
      break;
    case '/':
      return InsulatedBox::UPPER_RIGHT_CORNER;
      break;
    case '\\':
      return InsulatedBox::LOWER_RIGHT_CORNER;
      break;
    case '7':
      return InsulatedBox::LOWER_LEFT_CORNER;
      break;
    case '6':
      return InsulatedBox::UPPER_LEFT_CORNER;
      break;
    default:
      std::cerr << "Unknown wall type: " << wall_code;
      exit(0);
  };
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
  double max_temperature_delta = DBL_MIN;
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
        // dT/dx = 0.
        case LEFT_BOUNDARY:
          temperature_[i][j] = temperature_[i+1][j];
          break;
        // dT/dx = 0.
        case RIGHT_BOUNDARY:
          temperature_[i][j] = temperature_[i-1][j];
          break;
        // Convex corners.  These have del T . n = 0,
        // where n is (1/sqrt(2)) (\pm 1, \pm 1).
        case UPPER_RIGHT_CORNER:
          temperature_[i][j] = 0.5*(
            temperature_[i-1][j] + temperature_[i][j-1]);
          break;
        case UPPER_LEFT_CORNER:
          temperature_[i][j] = 0.5*(
            temperature_[i-1][j] + temperature_[i][j+1]);
          break;
        case LOWER_RIGHT_CORNER:
          temperature_[i][j] = 0.5*(
            temperature_[i+1][j] + temperature_[i][j-1]);
          break;
        case LOWER_LEFT_CORNER:
          temperature_[i][j] = 0.5*(
            temperature_[i+1][j] + temperature_[i][j+1]);
          break;
        // This should never be processed, so we set it to a large
        // negative value that will disrupt our calculation visibly if
        // it does.
        case INSIDE_INSULATION:
          temperature_[i][j] = -DBL_MAX;
          break;
        // The diffusion equation!
        case FREE_AIR:
          delsq_T = ( 
              (temperature_[i - 1][j] - 2*temperature_[i][j] + temperature_[i + 1][j])
              + (temperature_[i][j-1] - 2*temperature_[i][j] + temperature_[i][j+1])
              )/(grid_width_*grid_width_);
          if (std::abs(delsq_T) > max_abs_delsq_T) max_abs_delsq_T = std::abs(delsq_T);
          temperature_[i][j] = (temperature_[i][j]
              + relaxation_constant_*delsq_T);
      };
    }
  }
  if (verbose_log_ && (iteration_ % 1000 == 0)) {
    std::cerr << "#" << iteration_
              << " convergence percentage: "
              << std::setprecision(9)
              << max_abs_delsq_T/convergence_tolerance_ << std::endl;
  }
  if (max_abs_delsq_T < convergence_tolerance_) {
    is_converged_ = true;
  }
  return;
}

bool InsulatedBox::IsConverged() {
  return is_converged_;
}

double InsulatedBox::GetHeatFluxPerUnitLength() {
  double integral_dy_dT_by_dx = 0.0;
  int i = resolution_ - 2;
  for (int j = 0; j < resolution_; ++j) {
    integral_dy_dT_by_dx += (temperature_[i+1][j] - temperature_[i-1][j])/(2*grid_width_);
  }
  return integral_dy_dT_by_dx/resolution_;
}

void InsulatedBox::Log() {
  std::cout << "# Iteration " << iteration_
            << ", is_converged_ " << is_converged_ << std::endl;
  std::cout << "# Heat flux: " << GetHeatFluxPerUnitLength() << std::endl;
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
