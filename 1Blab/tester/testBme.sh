#!/bin/bash

# UCLA CS 111 Lab 1b testing script, written by Zhaoxing Bu (zbu@ucla.edu).
# This script should only be used when Zhaoxing is in the TA team for 111.

# DO NOT UPLOAD THIS SCRIPT TO WEB OR ANYWHERE ELSE. Any usage without permission
# is strictly forbidden.

# To reader: please read the entire script carefully.

# No partial credits.
# Do not run multiple testing scripts at the same time.
# Only run this on lnxsrv09.seas.ucla.edu please.
# REMEMBER to execute PATH=/usr/local/cs/bin:$PATH in shell to call the correct
# gcc for compiling students work.
# This PATH change is restored after logging out.
# This script automatically changes the PATH value for you.
# Any comments, suggestions, problem reports are greatly welcomed.

# How to run the script? Place it with students submissions in the same directory,
# run "./lab1b_sanity_script.sh UID", for example ./lab1b_sanity_script.sh 197912071.

if [ "${PATH:0:16}" == "/usr/local/cs/bin" ]
then
  true
else
  PATH=/usr/local/cs/bin:$PATH
fi


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
TEMPDIR="lab1breadergradingtempdir"
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

cat > b0.txt <<'EOF'
FEAR IS THE PATH TO THE DARK SIDE...FEAR LEADS TO ANGER...ANGER LEADS TO HATE...HATE LEADS TO SUFFERING.
DO. OR DO NOT. THERE IS NO TRY.
EOF
cp b0.txt b1.txt

echo "==="
echo "Please DO NOT run multiple testing scripts at the same time."
echo "Make sure there is no simpsh running by you."
echo "Infinite waiting of simpsh due to unclosed pipe is unacceptable."
echo "Starting grading:"
NUM_PASSED=0
NUM_FAILED=0



# Test case 21 --wait, part 6: multiple --wait and --verbose.
echo ""
echo "--->test case 21:"
./simpsh --creat --excl --rdwr test21io.txt --verbose --command 0 0 0 sleep 4 \
  --wait --creat --rdwr test21io2.txt --command 1 1 1 sleep 5 >c21out.txt \
  2>c21err.txt &
T21R=1
sleep 1
if ! ps --ppid $! -u | grep -q "sleep 4" || ps --ppid $! -u | grep -q "sleep 5" \
  || [ ! -e test21io.txt ] || [ -e test21io2.txt ] || [ ! -s c21out.txt ] \
  || ! grep -q -- "--command 0 0 0 sleep 4" c21out.txt \
  || grep -q -- "--rdwr test21io.txt" c21out.txt \
  || grep -q -- "--rdwr test21io2.txt" c21out.txt
then
echo "steppp 1"
  T21R=0
fi
sleep 5
if [ $T21R == 0 ] ||  ps -u | grep "sleep 4" | grep -q -v grep \
  || ! ps -u | grep "sleep 5" | grep -q -v grep || [ ! -e test21io2.txt ] \
  || ! grep -q -- "--command 1 1 1 sleep 5" c21out.txt \
  || ! grep -q -- "--rdwr test21io2.txt" c21out.txt \
  || ! grep -q "exit 0 sleep 4" c21out.txt
then
  echo "step 2"
  T21R=0
fi
sleep 6
if [ $T21R == 1 ] && ! ps -u | grep "sleep 5" | grep -q -v grep \
  && ! grep -q "exit 0 sleep 5" c21out.txt \
  && [ ! -s c21err.txt ] && [ ! -s test21io.txt ] && [ ! -s test21io2.txt ] \
  && ! ps -u | grep "test21io" | grep -q -v grep
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 21 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 21 failed"
fi

rm -f grading_helper.c
rm -f grading_helper
cat > grading_helper.c <<'EOF'
#include <signal.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  raise(atoi(argv[1]));
  return 0;
}
EOF
gcc -Wall grading_helper.c -o grading_helper

# Test case 22 --wait, part 6: wait collects exit status and signal.
# --wait only collects subprocesses executed before it. If no children terminated
# with signal, then exit with the highest return code it waited for.
echo ""
echo "--->test case 22:"
./simpsh --creat --excl --rdwr test22io.txt --command 0 0 0 bash -c "exit 10" \
  --command 0 0 0 bash -c "exit 20" --command 0 0 0 bash -c "exit 15" \
  --wait --command 0 0 0 ./grading_helper 30 >c22out.txt 2>c22err.txt
if [ $? == 20 ] && [ -e test22io.txt ] && [ ! -s test22io.txt ] && [ ! -s c22err.txt ] \
  && grep -q "exit 10 bash -c exit 10" c22out.txt \
  && grep -q "exit 20 bash -c exit 20" c22out.txt \
  && grep -q "exit 15 bash -c exit 15" c22out.txt \
  && ! grep -q "exit 30 bash -c exit 30" c22out.txt \
  && wc -l < c22out.txt | grep -q "3"
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 22 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 22 failed"
fi

# Test case 23 --wait, part 7: wait collects exit status and signal.
# simpsh exits with the highest signal number that terminated the children it
# waited for.
echo ""
echo "--->test case 23:"
./simpsh --creat --excl --rdwr test23io.txt --command 0 0 0 ./grading_helper 11 \
  --command 0 0 0 bash -c "exit 20" --wait --command 0 0 0 ./grading_helper 15 \
  --command 0 0 0 ./grading_helper 9 --wait --command 0 0 0 ./grading_helper 30 \
  >c23out.txt 2>c23err.txt
if [ $? == 143 ] && [ -e test23io.txt ] && [ ! -s test23io.txt ] && [ ! -s c23err.txt ] \
  && grep -q "exit 20 bash -c exit 20" c23out.txt \
  && grep -q "signal 15 ./grading_helper 15" c23out.txt \
  && grep -q "signal 11 ./grading_helper 11" c23out.txt \
  && grep -q "signal 9 ./grading_helper 9" c23out.txt \
  && ! grep -q "signal 30 ./grading_helper 30" c23out.txt \
  && wc -l < c23out.txt | grep -q "4"
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 23 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 23 failed"
fi

# Test case 24 --wait, part 8: wait and signal.
# Before simpsh exits with a signal, it should restore the default disposition
# for that signal, to make sure it could exits as we want.
# One thing keep in mind, in the child process created by fork() and execvp(),
# ignored signal remains being ignored.
echo ""
echo "--->test case 24:"
./simpsh --creat --excl --rdwr test24io.txt --wait --command 0 0 0 ./grading_helper 31 \
  --command 0 0 0 ./grading_helper 11 --ignore 31 --ignore 11 \
  --wait >c24out.txt 2>c24err.txt
if [ $? == 159 ] && [ -e test24io.txt ] && [ ! -s test24io.txt ] \
  && [ ! -s c24err.txt ] && grep -q "signal 31 ./grading_helper 31" c24out.txt \
  && grep -q "signal 11 ./grading_helper 11" c24out.txt \
  && wc -l < c24out.txt | grep -q "2"
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 24 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 24 failed"
fi

# Test case 25 --wait, part 9: wait and signal again.
# Since signal disposition for 31 is restored before exiting, "31 caught" should
# not be printed to stderr.
echo ""
echo "--->test case 25:"
./simpsh --creat --excl --rdwr test25io.txt --wait \
  --catch 31 --command 0 0 0 ./grading_helper 11 a b c d \
  --verbose --command 0 0 0 ./grading_helper 31 e f g h \
  --command 0 0 0 ./grading_helper 9 i j k l --wait \
  --command 0 0 0 bash -c "exit 50" --default >c25out.txt 2>c25err.txt
if [ $? == 159 ] && [ -e test25io.txt ] && [ ! -s test25io.txt ] && [ -s c25err.txt ] \
  && (wc -l < c25out.txt | grep -q "8" || wc -l < c25out.txt | grep -q "7") \
  && grep -q "signal 9 ./grading_helper 9 i j k l" c25out.txt \
  && grep -q "signal 11 ./grading_helper 11 a b c d" c25out.txt \
  && grep -q "signal 31 ./grading_helper 31 e f g h" c25out.txt \
  && ! grep -q -- "--command 0 0 0 ./grading_helper 11" c25out.txt \
  && grep -q -- "--command 0 0 0 ./grading_helper 31 e f g h" c25out.txt \
  && grep -q -- "--command 0 0 0 ./grading_helper 9 i j k l" c25out.txt \
  && grep -q -- "--wait" c25out.txt \
  && grep -q -- "--command 0 0 0 bash -c exit 50" c25out.txt \
  && ! grep -q -- "--catch" c25out.txt && ! grep -q -- "--default" c25out.txt \
  && ! grep -q -- "--creat" c25out.txt && ! grep -q -- "--rdwr" c25out.txt \
  && ! grep -q "31" c25err.txt && ! grep -q "caught" c25err.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 25 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 25 failed"
fi


# Finished testing.
echo ""
echo "Grading finished."
echo ""
echo "Among first 25 auto graded test cases:"
echo "$NUM_PASSED passed, $NUM_FAILED failed"
NUM_COLLECTED=`expr $NUM_PASSED + $NUM_FAILED`
if [ $NUM_COLLECTED != 25 ]
then
    echo "Sum is not 25, check what happend."
fi

if ps | grep "simpsh"
then
  echo "simpsh is running in background."
  echo "Check your code to make sure no simpsh is hanging there forever."
  echo "You will lose points for that."
fi

echo ""
echo "Student's README file:"
echo "==="
cat ../README
echo "==="
