#!/bin/bash

# UCLA CS 111 Lab 1c testing script, written by Zhaoxing Bu (zbu@ucla.edu).
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

# How to run the script? Place it with students' submissions in the same directory.
# Also, make sure you have downloaded pg98.txt and put it in the same directory.
# Run "./lab1c_sanity_script.sh UID", for example ./lab1c_sanity_script.sh 197912071.

if [ "${PATH:0:16}" == "/usr/local/cs/bin" ]
then
  true
else
  PATH=/usr/local/cs/bin:$PATH
fi

if ps | grep "simpsh"
then
  echo "simpsh is running in background."
  echo "Testing cannot continue."
  echo "Kill it and then run the script."
  exit 1
fi

if ! ls -lh pg98.txt
then
  echo "Please download pg98.txt!"
  exit 2
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
TEMPDIR="lab1creadergradingtempdir"
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

# in Lab 1b, --rdwr, --pipe, --wait, --close, --abort, --catch, --ignore,
# --default, --pause, and various file flags

# Test case 1 makes sure simpsh works for options in 1A.
echo ""
echo "--->test case 1:"
touch test1out.txt
touch test1err.txt
./simpsh --wronly test1out.txt --wronly test1err.txt --rdonly test1_none_exist.txt \
  --rdonly a1.txt --command 3 0 1 cat >c1out.txt 2>c1err.txt
T1RC=$?
sleep 1
if [ $T1RC == 1 ] && cmp -s a0.txt test1out.txt && [ ! -s test1err.txt ] \
  && [ ! -s c1out.txt ] && [ -s c1err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 1 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 1 failed"
fi

# Test case 2 makes sure simpsh works for options in 1A.
echo ""
echo "--->test case 2:"
touch test2in.txt
touch test2out.txt
touch test2err.txt
./simpsh --wronly test2err.txt --wronly test2out.txt --rdonly test2in.txt \
  --command 2 1 0 ls -la .. >c2out.txt 2>&1
T2RC=$?
ls -la .. > test2outstd.txt
sleep 1
if [ $T2RC == 0 ] && cmp -s test2outstd.txt test2out.txt \
  && [ ! -s test2err.txt ] && [ ! -s c2out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 2 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 2 failed"
fi

# Test case 3 --command should report error message if missing command.
echo ""
echo "--->test case 3:"
touch test3out.txt
touch test3err.txt
./simpsh --wronly test3out.txt --wronly test3err.txt --rdonly a2.txt \
  --command 2 0 1 --verbose --rdonly a2.txt >c3out.txt 2>c3err.txt
T3RC=$?
sleep 1
if [ $T3RC == 1 ] && [ ! -s test3out.txt ] && [ ! -s test3err.txt ] \
  && cmp -s a0.txt a2.txt && grep -q -- "--rdonly a2.txt" c3out.txt \
  && ! grep -q -- "--command" c3out.txt && ! grep -q -- "--wronly" c3out.txt \
  && [ -s c3err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 3 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 3 failed"
fi

# Test case 4 --rdwr and file flags, part 1: --trunc.
echo ""
echo "--->test case 4:"
cp b0.txt test4out.txt
cp b0.txt test4err.txt
./simpsh --rdonly a3.txt --trunc --rdwr test4out.txt --wronly test4err.txt \
  --command 0 1 2 cat >c4out.txt 2>&1
T4RC=$?
sleep 1
if [ $T4RC == 0 ] && cmp -s a0.txt test4out.txt \
  && cmp -s b0.txt test4err.txt && [ ! -s c4out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 4 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 4 failed"
fi

# Test case 5 file flags, part 2: --directory, --creat, --append.
echo ""
echo "--->test case 5:"
cp a0.txt test5out.txt
./simpsh --directory --rdonly a4.txt --rdonly a4.txt --creat --wronly test5err.txt \
  --creat --append --wronly test5out.txt --command 1 3 2 cat >c5out.txt 2>c5err.txt
T5RC=$?
cat a0.txt a0.txt > test5outstd.txt
sleep 1
if [ $T5RC == 1 ] && cmp -s a0.txt a4.txt && cmp -s test5outstd.txt test5out.txt \
  && [ -e test5err.txt ] && [ ! -s test5err.txt ] && [ ! -s c5out.txt ] \
  && [ -s c5err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 5 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 5 failed"
fi

# Test case 6 file flags, part 3: --excl.
echo ""
echo "--->test case 6:"
cp b0.txt test6out.txt
./simpsh --creat --excl --wronly test6out.txt >c6out.txt 2>c6err.txt
if [ $? == 1 ] && cmp -s b0.txt test6out.txt && [ ! -s c6out.txt ] && [ -s c6err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 6 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 6 failed"
fi

# Test case 7 --rdwr and file flags, part 4: --nofollow.
echo ""
echo "--->test case 7:"
ln -s b1.txt b1s.txt
./simpsh --nofollow --rdonly b1s.txt --rdwr b1s.txt --creat --rdwr test7out.txt \
  --command 1 2 2 cat >c7out.txt 2>c7err.txt
T7RC=$?
sleep 1
if [ $T7RC == 1 ] && cmp -s b0.txt b1s.txt && cmp -s b0.txt test7out.txt \
  && [ ! -s c7out.txt ] && [ -s c7err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 7 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 7 failed"
fi

# Test case 8 --pipe, part 1.
echo ""
echo "--->test case 8:"
touch test8out.txt
touch test8err.txt
./simpsh --rdonly a5.txt --wronly test8err.txt --pipe --wronly test8out.txt \
  --command 0 3 1 cat --command 2 4 1 cat >c8out.txt 2>&1
T8RC=$?
sleep 1
if [ $T8RC == 0 ] && cmp -s a0.txt test8out.txt && [ ! -s test8err.txt ] \
  && [ ! -s c8out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 8 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 8 failed"
fi

# Test case 9 --pipe, part 2.
echo ""
echo "--->test case 9:"
cp b0.txt b2.txt
./simpsh --pipe --rdwr b2.txt --creat --rdwr test9err.txt --creat \
  --rdwr test9out.txt --pipe --command 2 1 3 cat --command 0 6 3 cat b2.txt - \
  --command 5 4 3 wc >c9out.txt 2>&1
T9RC=$?
cat b0.txt b0.txt | wc > test9outstd.txt
sleep 1
if [ $T9RC == 0 ] && [ -e test9err.txt ]&& [ -e test9out.txt ] \
  && [ ! -s test9err.txt ] && cmp -s test9outstd.txt test9out.txt \
  && [ ! -s c9out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 9 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 9 failed"
fi

# Test case 10 --abort and --verbose.
# simpsh should immediately generate the SEGV signal when seeing --abort.
# Therefore, simpsh should not report error for --catch. But, we should still be
# able to see "--abort" from stdout.
echo ""
echo "--->test case 10:"
./simpsh --verbose --abort --catch >c10out.txt 2>c10err.txt
if [ $? == 139 ] && grep -q -- "--abort" c10out.txt && [ ! -s c10err.txt ] \
  && ! grep -q "catch" c10out.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 10 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 10 failed"
fi

# Test case 11 --catch and --abort.
echo ""
echo "--->test case 11:"
./simpsh --catch 11 --abort >c11out.txt 2>c11err.txt
if [ $? == 11 ] && grep -q "11 caught" c11err.txt && [ ! -s c11out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 11 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 11 failed"
fi

# Test case 12 --default, --catch, and --abort.
echo ""
echo "--->test case 12:"
./simpsh --catch 11 --default 11 --abort >c12out.txt 2>&1
if [ $? == 139 ] && [ ! -s c12out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 12 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 12 failed"
fi

# Test case 13 --pause.
echo ""
echo "--->test case 13:"
./simpsh --pause >c13out.txt 2>&1 &
T13R=1
sleep 2
if ! ps --pid $! > /dev/null
then
  T13R=0
fi
kill -10 $!
sleep 1
if ps --pid $! > /dev/null
then
  T13R=0
fi
if [ $T13R == 1 ] && [ ! -s c13out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 13 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 13 failed"
fi

# Test case 14 --ignore, --catch, and --pause.
echo ""
echo "--->test case 14:"
./simpsh --ignore 10 --catch 30 --pause >c14out.txt 2>c14err.txt &
T14R=1
sleep 2
if ! ps --pid $! > /dev/null
then
  T14R=0
fi
kill -10 $!
sleep 1
if ! ps --pid $! > /dev/null
then
  T14R=0
fi
kill -30 $!
sleep 1
if ps --pid $! > /dev/null
then
  T14R=0
fi
if [ $T14R == 1 ] && grep -q "30 caught" c14err.txt && [ ! -s c14out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 14 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 14 failed"
fi

# Test case 15 --close.
echo ""
echo "--->test case 15:"
./simpsh --creat --rdwr test15io.txt --close 0 --command 0 0 0 sleep 1 \
  >c15out.txt 2>c15err.txt
if [ $? == 1 ] && [ -e test15io.txt ] && [ ! -s test15io.txt ] \
  && [ ! -s c15out.txt ] && [ -s c15err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 15 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 15 failed"
fi

# Test case 16 --wait, part 1.
echo ""
echo "--->test case 16:"
./simpsh --wait >c16out.txt 2>&1
if [ $? == 0 ] && [ ! -s c16out.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 16 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 16 failed"
fi

# Test case 17 --wait, part 2: works well with pipe and no infinite wait.
# I have explained why there could be an infinite wait and how to avoid it in my
# discussion. This issue has always been a classic problem in Lab 1, no matter
# which version.
echo ""
echo "--->test case 17:"
cat a0.txt | wc -c > test17outstd.txt
./simpsh --rdonly a6.txt --creat --wronly test17out.txt --creat --rdwr test17err.txt \
  --pipe --command 0 4 2 cat --command 3 1 2 wc -c --close 3 --close 4 --wait \
  >c17out.txt 2>c17err.txt
if [ $? == 0 ] && [ -e test17out.txt ] && [ -e test17err.txt ] \
  && cmp -s test17outstd.txt test17out.txt && [ ! -s test17err.txt ] \
  && [ ! -s c17err.txt ] && grep -q "exit 0 cat" c17out.txt \
  && grep -q "exit 0 wc -c" c17out.txt && wc -l < c17out.txt | grep -q "2"
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 17 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 17 failed"
fi

# Test case 18 --wait, part 3: if we don't close 4 in parent process, wc will
# hang there forever.
echo ""
echo "--->test case 18:"
./simpsh --rdonly a7.txt --creat --wronly test18out.txt --creat --rdwr test18err.txt \
  --pipe --command 0 4 2 cat --command 3 1 2 wc -c --close 3 --wait >c18out.txt 2>&1 &
T18R=1
sleep 5
if ! ps --pid $! > /dev/null
then
  T18R=0
fi
kill -10 $!
sleep 1
if ps --pid $! > /dev/null
then
  T18R=0
fi
if [ $T18R == 1 ] && [ -e test18out.txt ] && [ -e test18err.txt ] \
  && cmp -s test17outstd.txt test18out.txt && [ ! -s test18err.txt ] \
  && ! grep -q "wc" c18out.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 18 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 18 failed"
fi

# Test case 19 --wait, part 4: timing.
echo ""
echo "--->test case 19:"
(time -p ./simpsh --creat --excl --rdwr test19io.txt --command 0 0 0 time -p sleep 1 \
  --wait) >c19out.txt 2>c19err.txt
if [ $? == 0 ] && [ -e test19io.txt ] && grep -q "real 1." test19io.txt \
  && grep -q "real 1." c19err.txt && grep -q "exit 0 time -p sleep 1" c19out.txt
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 19 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 19 failed"
fi

# Test case 20 --wait, part 5: --wait should not close any fd.
echo ""
echo "--->test case 20:"
./simpsh --creat --excl --rdwr test20io.txt --command 0 0 0 time -p sleep 1 \
  --pipe --wait --rdonly a8.txt --command 3 2 0 cat --command 1 0 0 cat \
  >c20out.txt 2>c20err.txt
T20RC=$?
sleep 1
if [ $T20RC == 0 ] && [ -e test20io.txt ] && grep -q "real 1." test20io.txt \
  && grep -q "Knowledge crowns those who seek her" test20io.txt \
  && grep -q "Hello world! CS 111! " test20io.txt \
  && grep -q "exit 0 time -p sleep 1" c20out.txt && ! grep -q "cat" c20out.txt \
  && [ ! -s c20err.txt ]
then
  NUM_PASSED=`expr $NUM_PASSED + 1`
  echo "===>test case 20 passed"
else
  NUM_FAILED=`expr $NUM_FAILED + 1`
  echo "===>test case 20 failed"
fi

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
  T21R=0
fi
sleep 5
if [ $T21R == 0 ] ||  ps -u | grep "sleep 4" | grep -q -v grep \
  || ! ps -u | grep "sleep 5" | grep -q -v grep || [ ! -e test21io2.txt ] \
  || ! grep -q -- "--command 1 1 1 sleep 5" c21out.txt \
  || ! grep -q -- "--rdwr test21io2.txt" c21out.txt \
  || ! grep -q "exit 0 sleep 4" c21out.txt
then
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


echo -e "\n\n======ALERT! MANUAL CHECKING BEGINS======"

# Manual testing for 1c.
LOOP_INDEX_I=0
while [ $LOOP_INDEX_I -lt 100 ]
do
    cat ../../pg98.txt >> pg98_100.txt
    LOOP_INDEX_I=`expr $LOOP_INDEX_I + 1`
done
if ! ls -lh pg98_100.txt
then
  echo "Fatal error!"
  exit 3
fi


# Test case 26 --profile almost no time.
echo ""
echo "--->test case 26:"
echo "Check if there is time info for --rdwr option."
echo "Time should be (almost) 0"
./simpsh --creat --profile --rdwr test26io.txt >c26out.txt 2>c26err.txt
if [ $? == 0 ] && [ ! -s c26err.txt ] && [ -s c26out.txt ] && [ -e test26io.txt ] \
  && [ ! -s test26io.txt ] && wc -l < c26out.txt | grep -q "1"
then
  echo "----------c26out.txt----------"
  cat c26out.txt
  echo "----------c26out.txt----------"
else
  echo "===>test case 26 failed"
fi

# Test case 27 --profile scope.
echo ""
echo "--->test case 27:"
echo "Should only see time info for --rdwr, but not for --pipe or --command."
echo "And, we don't have wait here."
./simpsh --pipe --command 0 1 1 sleep 1 --creat --profile --rdwr test27io.txt \
  >c27out.txt 2>c27err.txt
if [ $? == 0 ] && [ ! -s c27err.txt ] && [ -s c27out.txt ] && [ -e test27io.txt ] \
  && [ ! -s test27io.txt ] && wc -l < c27out.txt | grep -q "1"
then
  echo "----------c27out.txt----------"
  cat c27out.txt
  echo "----------c27out.txt----------"
else
  echo "===>test case 27 failed"
fi

# Test case 28 --profile sort a large file.
echo ""
echo "--->test case 28:"
echo "Here you would see three time info, one for --command, which should be (almost) 0."
echo "Another time info, which is for --wait, should also be (almost) 0."
echo "The last time info is for child process, should not be 0."
echo "You can output time info for each child process, or just output a sum for"
echo "all children you waited for."
./simpsh --rdonly pg98_100.txt --creat --wronly test28out.txt --creat \
  --wronly test28err.txt --profile --command 0 1 2 sort --wait >c28out.txt \
  2>c28err.txt
if [ $? == 0 ] && [ ! -s c28err.txt ] && [ -s c28out.txt ] && [ -e test28out.txt ] \
  && [ -s test28out.txt ] && [ -e test28err.txt ] && [ ! -s test28err.txt ] \
  && (wc -l < c28out.txt | grep -q "4" || wc -l < c28out.txt | grep -q "5") 
then
  echo "----------c28out.txt----------"
  cat c28out.txt
  echo "----------c28out.txt----------"
else
  echo "===>test case 28 failed"
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

echo -e "\nOutput from times:"
times

echo ""
echo "Student's README file:"
echo "==="
cat ../README
echo "==="

echo "**********Report check**********"
if ls -lh ../*.pdf
then
  echo "Report submitted."
else
  echo "ALERT!!! No report was submitted!"
fi
