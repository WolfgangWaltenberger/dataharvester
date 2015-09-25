#!/bin/sh

# get current directory name
cwd=`dirname $0`
#`echo $0 | sed -e 's/\/.*//'`;


# wherever we start, the scripts
# expect "test/" to be the working directory
cd $cwd

source test_base

tests=`ls release_test*`;
errs=0;    # number of "not ok's"
n_tests=`echo ${tests} | wc -w`; # number of tests

for i in ${tests}; do 
  echo;
  i="$cwd/$i"
  echo "Now executing ${i}";
  echo "================================";
  ${i} || errs=`echo $errs+1 | bc`;
  echo "";
done

echo "$errs / $n_tests tests failed."
