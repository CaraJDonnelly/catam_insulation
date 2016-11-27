#include "insulated_box.h"

int main(int argc, char** argv) {
  double resolution = 100;
  double safe_relaxation_constant = 0.25/resolution/resolution;
  catam::InsulatedBox box(/*resolution=*/resolution, /*convergence_tolerance=*/0.00000001,
      /*relaxation_constant=*/safe_relaxation_constant, /*verbose_logging=*/true);
  while(!box.IsConverged()) {
    box.DoTimestep();
  }
  box.Log();
  return 0;
}
