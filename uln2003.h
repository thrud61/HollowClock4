// cut down class to use uln2003 single steps

class Motor {
public:
    int position = 0;

private:
    int motor_pins[4] = {6,7,8,9};

    int sequence[8][4] =
                {   // run forwards
                    {0,0,0,1},
                    {0,0,1,1},
                    {0,0,1,0},
                    {0,1,1,0},
                    {0,1,0,0},
                    {1,1,0,0},
                    {1,0,0,0},
                    {1,0,0,1}
                };

public:
    Motor ();
    void step(bool dir);
    void halt();
};
