#include <iostream>
#include <memory>
#include <fstream>
#include <string>

#include "insulated_box.h"

void ReadWalls(std::vector<std::vector<char>>* walls) {
  std::ifstream walls_file;
  walls_file.open("walls.txt");
  if (!walls_file.is_open()) {
    std::cerr << "Couldn't open walls file." << std::endl;
    exit(1);
  }
  std::string line;
  int line_length = -1;
  int i = 0;
  int num_lines = 0;
  while(std::getline(walls_file, line)) {
    ++num_lines;
    if (line_length < 0) {
      line_length = line.size();
    }
    if (line.size() != line_length) {
      std::cerr << "Square domains only." << std::endl;
      exit(1);
    }
    std::vector<char> new_line;
    for (int j=0; j < line.size(); ++j) {
      new_line.push_back(line[j]);
    }
    walls->push_back(new_line);
  }
  std::cerr << "Read domain: " << std::endl;
  for (int i = 0; i < num_lines; ++i) {
    for (int j = 0; j < line_length; ++j) {
      std::cerr << (*walls)[i][j];
    }
    std::cerr << std::endl;
  }
}

int main(int argc, char** argv) {
  std::unique_ptr<std::vector<std::vector<char>>> walls;
  walls.reset(new std::vector<std::vector<char>>());
  ReadWalls(walls.get());
  double resolution = walls->size();
  double safe_relaxation_constant = 0.25/resolution/resolution;
  catam::InsulatedBox box(/*convergence_tolerance=*/0.000001,
      /*relaxation_constant=*/safe_relaxation_constant, /*verbose_logging=*/true,
      std::move(walls));
  while(!box.IsConverged()) {
    box.DoTimestep();
  }
  box.Log();
  return 0;
}
