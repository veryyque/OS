#!/bin/bash
echo "-----Запуск программы-----"
./first &
PID=$!

echo 

sleep 3

echo "------1. Анализ с vmmap------"
vmmap $PID | grep -E "(__TEXT|__DATA|Stack)"
vmmap $PID | grep -E "Stack|STACK GUARD"
echo 
echo "------2.Анализ с nm-----"
nm ./first | grep -E "(global|static|local)"

echo 
echo "Программа все еще работает (PID: $PID)"
wait $PID 2>/dev/null

