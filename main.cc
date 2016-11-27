#include "insulated_box.h"

int main(int argc, char** argv) {
  catam::InsulatedBox box(/*resolution=*/100, /*convergence_tolerance=*/0.0001,
      /*relaxation_constant=*/0.000001, /*verbose_logging=*/true);
  while(!box.IsConverged()) {
    box.DoTimestep();
  }
  box.Log();
  return 0;
}
