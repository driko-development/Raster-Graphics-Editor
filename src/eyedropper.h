#ifndef __EYEDROPPER_H
#define __EYEDROPPER_H

class Eyedropper
{
    public:
        Eyedropper();
        void Update(int b, int g, int r);
        int blue;
        int green;
        int red;
};
#endif