#include "Calc.h"

#include <math.h>

namespace medicyc::cyclotroncontrolsystem::global {

bool AlmostEqual(double a, double b, double maxRelDiff) {
    double diff = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    double largest = (b > a) ? b : a;
    return (diff <= largest * maxRelDiff);
}

}
