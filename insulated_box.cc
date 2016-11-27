#include <iostream>
#include <cfloat>

#include "insulated_box.h"

namespace catam {

InsulatedBox::InsulatedBox(int resolution, double convergence_tolerance, bool verbose_log)
    : resolution_(resolution), convergence_tolerance_(convergence_tolerance), verbose_log_(verbose_log) {}

void InsulatedBox::DoTimestep() {
  if (is_converged_) {
    std::cerr << "Called DoTimestep() after convergence, doing nothing.";
    return;
  }
  ++iteration_;
  return;
}

bool InsulatedBox::IsConverged() {
  return is_converged_;
}

void InsulatedBox::Log() {
  std::cout << "# Iteration " << iteration_
            << ", is_converged_ " << is_converged_;
  return;
}

}  // namespace catam
