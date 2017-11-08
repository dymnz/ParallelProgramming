#!/bin/sh

./p2_pthread test/test$1_$2k.in test/test$1_$2k.out

diff test/test$1_$2k.out test/gtest$1_$2k.out