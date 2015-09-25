#!/bin/sh

echo "MultiTypeTest currently not tested"

exit 0

source test_base

echo "Task: Make sure that the MultiType handles all conversion the right way."
echo "Prerequisites: None."
echo ""
MultiTypeTest && 
{
  echo "${fg_green}Test OK${fg_reset}";
  exit 0;
} ||
{
  echo "${fg_red}Test NOT OK${fg_reset}";
  exit -1;
};
