# File: LDR-Widerstand.plt, Version 2022-05-22
# Skript für gnuplot
set font "Arial, bold, 16"
set title "LDR Widerstand vs. Beleuchtungsstaerke" font "Arial,16"
set grid dt 1
set logscale xy
rb(x, r10, gamma) = r10 * exp(- gamma * (log(x/10)))
adc(x, rpd,  r10, gamma) = 10 * exp( - log(rpd/gamma * (1023/x - 1)) / gamma)
set xrange [1:10000]
set xlabel "B / lx" font "Arial,16"
set ylabel "R_B / kOhm" font "Arial,16"
set terminal png
set output "LDR.png"
plot rb(x,  8, 0.5) lw 3   title "R_{10} = 8 kOhm, gamma = 0.5", \
     rb(x, 15, 0.6) lw 3   title "R_{10} = 15 kOhm, gamma = 0.6", \
     rb(x, 25, 0.7) lw 3   title "R_{10} = 25 kOhm, gamma = 0.6", \
     rb(x, 40, 0.7) lw 3   title "R_{10} = 40 kOhm, gamma = 0.7", \
     rb(x, 75, 0.8) lw 3   title "R_{10} = 75 kOhm, gamma = 0.8", \
     rb(x, 150, 0.9) lw 3  title "R_{10} = 150 kOhm, gamma = 0.9"
set terminal x11
replot
pause 5
b(x, rpd,  r10, gamma) = 10 * exp( - log(rpd/gamma * (1023/x - 1)) / gamma)
set title "Beleuchtungsstaerke vs. ADC-Wert" font "Arial,16"
set xrange [1:1022]
set yrange [0.1:10000]
unset logscale
set logscale y
set xlabel "ADC - Wert" font "Arial,16"
set ylabel "B / lx" font "Arial,16"
set terminal png
set output "ADC.png"
plot b(x, 1.0, 8, 0.5) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 8 kOhm, gamma = 0.5" ,\
     b(x, 1.0, 15, 0.6) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 15 kOhm, gamma = 0.6" ,\
     b(x, 1.0, 25, 0.6) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 25 kOhm, gamma = 0.6" ,\
     b(x, 1.0, 40, 0.7) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 40 kOhm, gamma = 0.7" ,\
     b(x, 1.0, 75, 0.8) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 75 kOhm, gamma = 0.8" ,\
     b(x, 1.0, 150, 0.9) lw 3   title "R_{pd} = 1 kOhm, R_{10} = 150 kOhm, gamma = 0.9"

set terminal x11
replot
pause 10

