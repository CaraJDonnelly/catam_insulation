namespace catam {

class InsulatedBox {
  public:
    // Initializes grid.
    // Arguments:
    //   resolution: the number of points to use in each direction.
    //   convergence_tolerance: the max tolerable value of |del^2 T| in the
    //     domain.
    //   verbose_log: whether or not to log the grid after each
    //     timestep.
    InsulatedBox(int resolution, double convergence_tolerance, bool verbose_log);

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
  private:
    // The resolution of the box.  Given the question will shortly involve
    // insulated inner sections, must be at least high enough to resolve
    // the width of an inner section.
    int resolution_;
    // Controls whether we log after every timestep.
    bool verbose_log_;
    // How many times DoTimestep has been called.  Convenience for logging.
    int iteration_ = 0;
    // Whether or not our the maximum |del^2 T| in our domain dropped below
    // convergence_tolerance_ with the last timestep.
    bool is_converged_ = false;
    // The maximum desired value of |del^2 T| in our domain for this
    // calculation to be considered finished.
    double convergence_tolerance_;
};

}  // namespace catam
