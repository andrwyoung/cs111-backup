#!/bin/bash

# UCLA CS 111 Lab 1a testing script, written by Zhaoxing Bu (zbu@ucla.edu).
# This script should only be used when Zhaoxing is in the TA team for 111.

# DO NOT UPLOAD THIS SCRIPT TO WEB OR ANYWHERE ELSE. Any usage without permission
# is strictly forbidden.

# To reader: please read the entire script carefully.

# No partial credits.
# Do not run multiple testing scripts at the same time.
# Only run this on lnxsrv09.seas.ucla.edu please.
# REMEMBER to execute PATH=/usr/local/cs/bin:$PATH in shell to call the correct
# gcc for compiling students' work.
# This PATH change is restored after logging out.
# This script automatically changes the PATH value for you.
# Any comments, suggestions, problem reports are greatly welcomed.

# How to run the script? Place it with students' submissions in the same directory,
# run "./lab1a_sanity_script.sh UID", for example ./lab1a_sanity_script.sh 197705251.

if [ "${PATH:0:16}" == "/usr/local/cs/bin" ]
then
  true
else
  PATH=/usr/local/cs/bin:$PATH
fi

echo "DO NOT run multiple testing scripts at the same time."
echo "Please check if there is any error message below."
echo "==="

# Check tarball.
STUDENT_UID="$1"
SUBMISSION="lab1-$STUDENT_UID.tar.gz"
if [ -e "$SUBMISSION" ]
then
  true
else
  echo "No submission: lab1-$STUDENT_UID.tar.gz"
	exit 1
fi

# Untar into student's directory.
rm -rf $STUDENT_UID
mkdir $STUDENT_UID
tar -C $STUDENT_UID -zxvf $SUBMISSION
cd $STUDENT_UID

# Make.
if [ -e "simpsh" ]
then
  rm -rf simpsh
fi
make || exit

make check
if [ $? == 0 ]
then
  echo "===>make check passed"
else
  echo "===>make check failed"
fi

rm -rf $SUBMISSION
make dist
if [ -e "$SUBMISSION" ]
then
  echo "===>make dist passed"
else
  echo "===>make dist failed"
fi

# Create testing directory.
TEMPDIR="lab1areadergradingtempdir"
rm -rf $TEMPDIR
mkdir $TEMPDIR
if [ "$(ls -A $TEMPDIR 2> /dev/null)" == "" ]
then
  true
else
  echo "Fatal error! The testing directory is not empty."
  exit 1
fi
mv simpsh ./$TEMPDIR/
cd $TEMPDIR

# Create testing files.
cat > a0.txt <<'EOF'
Hello world! CS 111! Knowledge crowns those who seek her.
EOF
cat a0.txt > a1.txt
cat a0.txt > a2.txt
cat a0.txt > a3.txt
cat a0.txt > a4.txt
cat a0.txt > a5.txt
cat a0.txt > a6.txt
cat a0.txt > a7.txt
cat a0.txt > a8.txt
cat a0.txt > a9.txt
cat a0.txt > a10.txt

echo "==="

# Test cases.
echo "Starting grading:"
NUM_PASSED=0
NUM_FAILED=0
# In Lab 1a, --rdonly, --wronly, --command, and --verbose.

# Test case 1 no option.
echo ""
echo "--->test case 1:"
./simpsh >c1out.txt 2>&1
if [ $? == 0 ] && [ ! -s c1out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 1 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 1 failed"
fi

# Test case 2 bogus option.
echo ""
echo "--->test case 2:"
./simpsh --bogus >c2out.txt 2>c2err.txt
if [ $? == 1 ] && [ ! -s c2out.txt ] && [ -s c2err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 2 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 2 failed"
fi

# Test case 3 --rdonly can be called with no error.
echo ""
echo "--->test case 3:"
./simpsh --rdonly a1.txt >c3out.txt 2>&1
if [ $? == 0 ] && [ ! -s c3out.txt ] && cmp -s a0.txt a1.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 3 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 3 failed"
fi

# Test case 4 --rdonly does not have any default flag, for example --create.
echo ""
echo "--->test case 4:"
./simpsh --rdonly test4_none_exist.txt >c4out.txt 2>c4err.txt
if [ $? == 1 ] && [ ! -s c4out.txt ] && [ -s c4err.txt ] && [ ! -e "test4_none_exist.txt" ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 4 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 4 failed"
fi

# Test case 5 --wronly does not have any default flag, for example --create.
echo ""
echo "--->test case 5:"
./simpsh --wronly test5_none_exist.txt >c5out.txt 2>c5err.txt
if [ $? == 1 ] && [ ! -s c5out.txt ] && [ -s c5err.txt ] && [ ! -e "test5_none_exist.txt" ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 5 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 5 failed"
fi

# Test case 6 --wronly does not have any default flag, for example --trunc.
echo ""
echo "--->test case 6:"
./simpsh --wronly a2.txt >c6out.txt 2>&1
if [ $? == 0 ] && [ ! -s c6out.txt ] && cmp -s a0.txt a2.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 6 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 6 failed"
fi

# Test case 7 --command cat with rdonly and wronly, correct input.
echo ""
echo "--->test case 7:"
touch test7out.txt
touch test7err.txt
./simpsh --rdonly a3.txt --wronly test7out.txt --wronly test7err.txt \
  --command 0 1 2 cat >c7out.txt 2>&1
T7RC=$?
sleep 1
if [ $T7RC == 0 ] && cmp -s a0.txt test7out.txt && [ ! -s test7err.txt ] \
  && [ ! -s c7out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 7 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 7 failed"
fi

# Test case 8 --command wc with rdonly and wronly, correct input.
echo ""
echo "--->test case 8:"
touch test8out.txt
touch test8err.txt
./simpsh --wronly test8out.txt --wronly test8err.txt --rdonly a4.txt   \
  --command 2 0 1 wc >c8out.txt 2>&1
T8RC=$?
wc < a0.txt > test8outstd.txt
sleep 1
if [ $T8RC == 0 ] && cmp -s test8outstd.txt test8out.txt \
  && [ ! -s test8err.txt ] && [ ! -s c8out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 8 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 8 failed"
fi

# Test case 9 --command cat with rdonly and wronly, wrong fd index, simpsh should
# report error.
echo ""
echo "--->test case 9:"
touch test9out.txt
touch test9err.txt
./simpsh --rdonly a5.txt --wronly test9out.txt --wronly test9err.txt \
  --command 0 1 3 cat >c9out.txt 2>c9err.txt
T9RC=$?
sleep 1
if [ $T9RC == 1 ] && [ ! -s test9out.txt ] && [ ! -s test9err.txt ] \
  && [ ! -s c9out.txt ] && [ -s c9err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 9 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 9 failed"
fi

# Test case 10 --command cat with rdonly and wronly, one option wrong, simpsh
# should report error but continues to next options.
echo ""
echo "--->test case 10:"
touch test10out.txt
touch test10err.txt
./simpsh --wronly test10out.txt --wronly test10err.txt --rdonly test10_none_exist.txt \
  --rdonly a6.txt --command 3 0 1 cat >c10out.txt 2>c10err.txt
T10RC=$?
sleep 1
if [ $T10RC == 1 ] && cmp -s a0.txt test10out.txt && [ ! -s test10err.txt ] \
  && [ ! -s c10out.txt ] && [ -s c10err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 10 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 10 failed"
fi

# Test case 11 --command cat with rdonly and wronly, cannot access future fds.
echo ""
echo "--->test case 11:"
touch test11out.txt
touch test11err.txt
./simpsh --rdonly a7.txt --wronly test11out.txt --command 0 1 2 cat \
  --wronly test11err.txt >c11out.txt 2>c11err.txt
T11RC=$?
sleep 1
if [ $T11RC == 1 ] && [ ! -s test11out.txt ] && [ ! -s test11err.txt ] \
  && [ ! -s c11out.txt ] && [ -s c11err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 11 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 11 failed"
fi

# Test case 12 --verbose.
echo ""
echo "--->test case 12:"
#echo "must show verbose info for --rdonly a8.txt  --wronly test12err.txt and --command 1 0 2 cat"
#echo "must NOT show verbose info for --wronly test12out.txt"
#echo "showing --verbose itself or not is both OK"
touch test12out.txt
touch test12err.txt
./simpsh --wronly test12out.txt --verbose --rdonly a8.txt  --wronly test12err.txt \
  --command 1 0 2 cat >c12out.txt 2>c12err.txt
T12RC=$?
sleep 1
if [ $T12RC == 0 ] && cmp -s a0.txt test12out.txt && [ ! -s test12err.txt ] \
  && [ ! -s c12err.txt ] && ! grep -q -- "--wronly test12out.txt" c12out.txt \
  && grep -q -- "--rdonly a8.txt" c12out.txt \
  && grep -q -- "--wronly test12err.txt" c12out.txt \
  && grep -q -- "--command 1 0 2 cat" c12out.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 12 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 12 failed"
fi

# Test case 13 there is no wait.
echo ""
echo "--->test case 13:"
echo "simpsh should immediately exits without waiting for 61 seconds."
echo "If you find simpsh waiting for many seconds, then definitely failed."
#echo "please check the real time listed below"
#echo "if no time reported, manually run the test case without time command"
touch test13in.txt
touch test13out.txt
touch test13err.txt
touch test13time.txt
(time ./simpsh --rdonly test13in.txt --wronly test13out.txt --wronly test13err.txt \
  --command 0 1 2 sleep 61) 2>test13time.txt >/dev/null
T13RC=$?
sleep 1
if [ $T13RC == 0 ] && [ ! -s test13out.txt ] && [ ! -s test13err.txt ] \
  && grep -q "real	0m" test13time.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 13 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 13 failed"
fi

# Test case 14 --command with multiple arguments, correc input.
echo ""
echo "--->test case 14:"
touch test14in.txt
touch test14out.txt
touch test14err.txt
./simpsh --rdonly test14in.txt --wronly test14out.txt --wronly test14err.txt \
  --command 0 1 2 time -p sleep 1 >c14out.txt 2>&1
T14RC=$?
sleep 2
if [ $T14RC == 0 ] && grep -q "real 1." test14err.txt && [ ! -s test14out.txt ] \
  && [ ! -s c14out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 14 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 14 failed"
fi

# Test case 15 multiple --command, correct input.
echo ""
echo "--->test case 15:"
touch test15out1.txt
touch test15out2.txt
touch test15out3.txt
touch test15err1.txt
touch test15err2.txt
touch test15err3.txt
./simpsh --rdonly a9.txt --rdonly a9.txt --wronly test15out1.txt --wronly test15err1.txt \
  --command 0 2 3 time -p sleep 2 --wronly test15out2.txt --wronly test15err2.txt \
  --wronly test15out3.txt --wronly test15err3.txt --command 0 4 5 cat \
  --command 1 6 7 cat a0.txt - >c15out.txt 2>&1
T15RC=$?
cat a0.txt a0.txt > test15out3std.txt
sleep 3
if [ $T15RC == 0 ] && grep -q "real 2." test15err1.txt && [ ! -s test15out1.txt ] \
  && cmp -s a0.txt test15out2.txt && [ ! -s test15err2.txt ] \
  && cmp -s test15out3std.txt test15out3.txt && [ ! -s test15err3.txt ] \
  && [ ! -s c15out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 15 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 15 failed"
fi

# Test case 16 only in grading script.

# Test case 17 only in grading script.

# Test case 18 only in grading script.

# finished testing
echo ""
echo "Grading finished."
echo ""
echo "Among first 15 auto graded test cases:"
echo "$NUM_PASSED passed, $NUM_FAILED failed"
NUM_COLLECTED=`expr $NUM_PASSED + $NUM_FAILED`
if [ $NUM_COLLECTED != 15 ]
then
  echo "Sum is not 15, check what happend."
fi

echo ""
echo "Student's README file:"
echo "==="
cat ../README
echo "==="
