#include "eyedropper.h"

Eyedropper::Eyedropper() : 
    blue{255},
    green{255},
    red{255}
{
}

void Eyedropper::Update(int b, int g, int r)
{
    blue = b;
    green = g;
    red = r;
}