4.7 (3 балла). Родительский процесс генерирует наборы из случайного количества случайных чисел и помещает в разделяемую память. Дочерний процесс находит максимальное и минимальное число и также помещает их в разделяемую память, после чего родительский процесс выводит найденные значения на экран. Процесс повторяется до получения сигнала SIGINT, после чего выводится количество обработанных наборов данных.
4.8 (3 балла). Скорректировать решение задачи 4.7 так, чтобы порождались дополнительные дочерние процессы, находящие минимум, сумму и среднее значение элементов.

Дочерние процессы по нахождению максимума и минимума объединены в один, как и процессы по нахождению суммы и среднего арифметического.

Разделяемая память - System V.

Семафоры - POSIX (для удобства).