#!/bin/sh

source test_base

echo "Task: a more thorough test of the root plugin."
echo "Prerequisites: root, python"

cd ../Plugins/Root/test && source release_test.sh
# source ../Plugins/Root/test/release_test.sh

