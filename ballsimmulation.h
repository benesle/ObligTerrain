#ifndef BALLSIMMULATION_H
#define BALLSIMMULATION_H

#include "innpch.h"
#include <iostream>
#include <conio.h>

//Stille på et flattunderlag
//Frittfall -Krefter som virker inn G^(->) = mg^(->) og luftmotstand. Uten kontakt med andre OBJ.
//Kan være i kontakt med andre OBJ
//Når ballen ligger stille på et underlag -
//Normalkraften N^-> fra flaten  oppveier tyngdekraften G^(->) .
//Kreftene er like store og motsatt rettet. Σ(i)F(i)^(->) = G^(->)+ N^(->) = 0
//Ballen beveger seg på underlaget. Ruller, med en konstant fart eller akselerasjon.
//Ballen kan kollidere ned en vegg eller et annet objekt - altså kan den være i kontakt med flere objekter samtidig.
//Ser først på en ball som faller fritt, deretter en ball som ruller.
//Underlaget er en overfate som består av triangler. Ballen er til enhver tid på et triangel. Vi kan finne ut hvilket, ved hjelp av Barysentriske koordinater.


//v = v0 + at
//s = v0 * t + 1/2a * t^2
//v0 er initiell hastighet, t er tiden og a er akselerasjonen.

//Hastighet [v] = m/s
//Akselerasjon [a] = m/s^2
//Kraft(Newton) N = (kg * m)/s^2

//hastighet er endring av posisjon s'(t) = v(t)
//Akselerasjon er endring av hastighet v'(t) = a(t)
//Da blir a(t) = s''(t)
//Akselerasjonen er vanligvis konstant eller påvirket av andre variabler enn tiden


//v = v0 + at
//p = v0t + 1/2a * t^2

class BallSimmulation
{
public:
    BallSimmulation();
    float getData();
    void formula(float f, float a, float m);
    void showData();

private:
    float F;
    float A;
    float M;

};

#endif // BALLSIMMULATION_H
