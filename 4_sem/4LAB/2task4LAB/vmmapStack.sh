while true; do
    echo
    echo "new"
    echo
    vmmap 28762 | grep -E "Stack|STACK GUARD"
    sleep 1
done
