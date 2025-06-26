// counter.h
#ifndef COUNTER_H
#define COUNTER_H

class Counter {
  unsigned int count;

public:
  Counter();
  unsigned int get();
  void increment();
};

#endif

// counter.cc
#include "counter.h"
Counter::Counter() : count(0) {}

unsigned int Counter::get() {
  return count;
}

void Counter::increment() {
  ++count;
}

// test_main.cc
#define BOOST_TEST_MODULE my_tests
#include <boost/test/included/unit_test.hpp>
#include "counter.h"

BOOST_AUTO_TEST_CASE(test_counter_initialize) {
  Counter c;
  BOOST_TEST(c.get() == 0);
}

BOOST_AUTO_TEST_CASE(test_counter_increment) {
  Counter c;
  c.increment();
  BOOST_TEST(c.get() == 1);
}
