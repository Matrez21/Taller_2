# Taller_2

Implementacion: Aunque haya problemas con el cargado de partida, las partidas se van guardando en el archivo partidas.csv de forma vertical, en la cual el numero 0 es el tablero, el 1 es el jugador y el 2 la IA. Ademas teniendo en cuenta que el tablero es de 6x7.

Decision de Diseño:
Por una parte deje el main solo, y decidi hacer todo en una clase aparte llamada Game.h para evitar cualquier tipo de problemas.

Resultado de pruebas:
La mayor diferencia al comparar el minimax con poda y sin poda alfa-beta en este codigo fue el tiempo que se demora la IA en tomar una decision, cuando se escogia una profundidad max de 10, y se utilizaba la poda alfa-beta la IA en tomar la decision se demoraba maximo 7 seg, al no utilizar la poda alfa-beta se demoraba al menos 1 minuto y medio.