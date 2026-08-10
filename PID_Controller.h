#include <Arduino.h>


class PIDController {
  public:

    //Parametric Constructor
    PIDController(double dt, double Kp, double Ki, double Kd, double min, double max)
      : dt_(dt), Kp_(Kp), Ki_(Ki), Kd_(Kd), min_(min), max_(max), integral_(0), last_error_(0){}

    double calculate(double setpoint, double pv) {

      //Step 1: Compute true systemic tracking deviation
      double error = setpoint - pv;

      //Step 2: Calculate Proportional Term
      double Pout = Kp_ * error;
     
      //Step 3: Calculate Integral Term
      integral_ += error * dt_;

      //limit to how much the Integral can help/hurt
      if (integral_ > 10) integral_ = 10;
      else if (integral_ < -10) integral_ = -10;
      double Iout = Ki_ * integral_;


      //Step 4: Calculate Derivative Term
      double derivative = (error - last_error_) / dt_;
      double Dout = Kd_ * derivative;

      //Step 5: Combine components into single output
      double output = Pout + Iout + Dout;


      //Step 6: Output Boundary Clamping Protection
      if (output > max_) output = max_;
      else if (output <min_) output = min_;

      //Cache current error parameter 
      last_error_ = error;
      return output;
    }


    void reset() {
      integral_ = 0;
      last_error_ = 0;
    }
  private:
    double dt_, Kp_, Ki_, Kd_, min_, max_, integral_, last_error_;
};




