#include "ballsimmulation.h"
#include <iostream>

BallSimmulation::BallSimmulation()
{
    //Newtons andre lov
    //(Dersom massen m til et legme er konstant)
    //F er resultatkraft og akselerasjonen "a =dv/dt" -Utrykker forandringer av hastigheten v

    //F = ma
    //Resultatkraften er lik endringen i legmes impuls
    //F = dp/dt
    //p = mv er impulsen
}

float BallSimmulation::getData()
{
//    formula(1,0,0);
    std::cout << "Newton Second Law " << F << std::endl;
    return F;
}

void BallSimmulation::formula(float f, float a, float m)
{

    float force = f;
    float mass = m;
    float acceleration = a;

    force = mass * acceleration;
}

void BallSimmulation::showData()
{
std::cout << "Nei";

}
