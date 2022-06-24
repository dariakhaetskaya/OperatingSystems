Модифицируйте программу упражнения 8 так, чтобы сама по себе она не завершалась. 
Вместо этого, после нажания ^-C (после получения сигнала SIGINT) программа должна как можно скорее завершаться,
собирать частичные суммы ряда и выводить полученное приближение числа.

Рекомендации: ожидаемое решение состоит в том, что вы установите обработчик SIGINT. 
Этот обработчик должен устанавливать глобальную флаговую переменную. 
Вычислительные нити должны просматривать значение флага через некоторое количество итераций, 
например через 1000000, и выходить при помощи pthread_exit, если флаг установлен. 
Подумайте, как минимизировать ошибку, обусловленную тем, что разные потоки к моменту завершения успели пройти разное количество итераций. 
Скорее всего, такая минимизация должна обеспечиваться за счет увеличения времени между получением сигнала и выходом.