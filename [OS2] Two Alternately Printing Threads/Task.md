Модифицируйте программу упр. 1 так, чтобы вывод родительской и дочерней нитей был синхронизован: 
сначала родительская нить выводила первую строку, затем дочерняя, затем родительская вторую строку и т.д. 
Используйте мутексы. Рекомендуется использовать мутексы типа PTHREAD_MUTEX_ERRORCHECK.

Явные и неявные передачи управления между нитями (sleep(3C)/usleep(3C), sched_yield(3RT)) и 
холостые циклы разрешается использовать только на этапе инициализации.