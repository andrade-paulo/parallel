# Execute each program with 1, 2, and 4 threads and store the results in a markdown table
echo "| Número de Threads | Tempo (ms) - Critical | Tempo (ms) - False Sharing |" > relatorio.md
echo "|-------------------|-----------------------|---------------------------|" >> relatorio.md
for threads in 1 2 4 8; do
    time_critical=$(./out_integral_critical $((100000000)) $threads | grep "Tempo" | awk '{print $3}')
    time_false_sharing=$(./out_integral_false_sharing $((100000000)) $threads | grep "Tempo" | awk '{print $3}')
    echo "| $threads | $time_critical | $time_false_sharing |" >> relatorio.md
done