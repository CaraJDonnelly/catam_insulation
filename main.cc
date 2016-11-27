#include "insulated_box.h"

int main(int argc, char** argv) {
  catam::InsulatedBox box(100, 0.0001, true);
  while(!box.IsConverged()) {
    box.DoTimestep();
  }
  box.Log();
  return 0;
}
