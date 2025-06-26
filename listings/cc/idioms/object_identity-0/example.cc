struct Person
{
    std::string name;
    // many other expensive-to-copy fields

    Person& operator=(const Person& other) {
        // compare object identity first
        if (this != &other) {
            this.name = other.name;
            // copy the other expensive-to-copy fields
        }

        return *this;
    }
};
