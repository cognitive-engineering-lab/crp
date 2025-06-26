struct Person
{
    std::string name;
    // many other expensive-to-compare fields
};


bool operator==(const Person& lhs, const Person& rhs) {
    // compare object identity first
    if (&lhs == &rhs) {
        return true;
    }

    // compare the other expensive-to-compare fields

    return true;
}
