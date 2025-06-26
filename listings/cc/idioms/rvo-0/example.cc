struct Widget {
    signed char x;
    double y;
    long z;
};

Widget make(signed char x, double y, long z) {
    return Widget{x, y, z};
}
