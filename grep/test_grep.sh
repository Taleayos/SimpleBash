#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
TEST_FILE="test_grep.txt"

for var in -i -v -c -l -n -h -s
do
          TEST1="r $var $TEST_FILE"
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
            echo "$TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt
done

for var in -i -v -c -n -h -s
do
  for var2 in -i -v -c -n -h -s
  do
        if [ $var != $var2 ]
        then
          TEST1="r $var $var2 $TEST_FILE"
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt
        fi
  done
done

for var in -i -v -c -n -h -s
do
  for var2 in -i -v -c -n -h -s
  do
      for var3 in -i -v -c -n -h -s
      do
          for var4 in -i -v -c -n -h -s
          do
            if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var2 != $var4 ] && [ $var3 != $var4 ]
            then
              TEST1="-e r $var $var2 $var3 $var4 $TEST_FILE"
              echo "$TEST1"
              ./s21_grep $TEST1 > s21_grep.txt
              grep $TEST1 > grep.txt
              DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
              if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                then
                  (( COUNTER_SUCCESS++ ))
                else
                  echo "$TEST1" >> log.txt
                  (( COUNTER_FAIL++ ))
              fi
              rm s21_grep.txt grep.txt

            fi
          done
      done
  done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
