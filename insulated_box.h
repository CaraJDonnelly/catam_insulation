#include <memory>
#include <vector>

namespace catam {

class InsulatedBox {
  public:
    // The return type of GetBoundaryType(), which affects which
    // boundary condition we will apply to the temperature grid at the
    // provided point.
    enum BoundaryType {
      // Will be kept at T = 1.0.
      HEATED_HOT_BOUNDARY,
      // Will be kept at T = 0.0.
      HEATED_COLD_BOUNDARY,
      // Will be kept with dT/dy = 0.0.
      UPPER_BOUNDARY,
      LOWER_BOUNDARY,
      // Will be kept with dT/dx = 0.0
      LEFT_BOUNDARY,
      RIGHT_BOUNDARY,
      // Will be kept with del T . n = 0.0
      UPPER_RIGHT_CORNER,
      UPPER_LEFT_CORNER,
      LOWER_RIGHT_CORNER,
      LOWER_LEFT_CORNER,
      // Wraps around the j-walls.
      PERIODIC_UPPER,
      PERIODIC_LOWER,
      // Will be set to -DBL_MAX for ease of plotting.
      INSIDE_INSULATION,
      // Will relax under the diffusion equation.
      FREE_AIR,
    };

    // Initializes grid.
    // Arguments:
    //   resolution: the number of points to use in each direction.
    //   convergence_tolerance: the max tolerable value of |del^2 T| in the
    //     domain.
    //   verbose_log: whether or not to log the grid after each
    //     timestep.
    // Sets up a valid initial condition for the temperature.
    InsulatedBox(double convergence_tolerance,
        double relaxation_constant, bool verbose_log,
        std::unique_ptr<std::vector<std::vector<char>>> walls);

    // If is_converged_ is true, this function does nothing.
    //
    // If is_converged_ is false, perform a relaxation step. Keeps track
    // during each step of the maximum value of
    //   |del^2 T|
    // and if this is less than convergence_tolerance_ sets
    // is_converged_ to true.
    void DoTimestep();

    // Print the grid to stdout in a format readable by GNUplot's
    // heatmaps.
    void Log();

    // Getter for is_converged_.
    bool IsConverged();

    // Does the integral over y of dT/dx to get the heat flux across the box per unit length.
    double GetHeatFluxPerUnitLength();

    // The function which defines the shape of the box and where its
    // inner walls are.
    BoundaryType GetBoundaryType(int i, int j);
  private:
    // The resolution of the box.  Given the question will shortly involve
    // insulated inner sections, must be at least high enough to resolve
    // the width of an inner section.
    int resolution_;
    // Synactic sugar.  The distance between two grid point.  1.0/resolution_;
    double grid_width_;
    // How many times DoTimestep has been called.  Convenience for logging.
    int iteration_ = 0;
    // Whether or not our the maximum |del^2 T| in our domain dropped below
    // convergence_tolerance_ with the last timestep.
    bool is_converged_ = false;
    // The maximum desired value of |del^2 T| in our domain for this
    // calculation to be considered finished.
    double convergence_tolerance_;
    // Effectively the size of the forward timestep if we had a thermal
    // diffusivity of one.  Larger values will cause faster convergence
    // at the risk of losing numerical stability.
    double relaxation_constant_;
    // Controls whether we log after every timestep.
    bool verbose_log_;
    // The array that holds the (x, y) co-ordinates of where we are in the
    // box.  This could be calculated on-the-fly from resolution and some
    // iterators, but it's nicer to store it once.
    std::vector<std::vector<std::pair<double, double>>> position_;
    // The array that holds the temperature.
    std::vector<std::vector<double>> temperature_;
    // The array holding the position of the walls.  User supplied.
    std::unique_ptr<std::vector<std::vector<char>>> walls_;
};

}  // namespace catam
