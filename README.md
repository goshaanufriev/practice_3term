# Ознакомительная практика
## 3 семестр, ФН2
Данный репозиторий создан для выполнения задания по "Ознакомительной практике" 3 семестра ФН2.

### Требования к практике

Требуется реализовать заданные алгоритмы решения задачи комбинаторной оптимизации.
Алгоритмы должны быть реализованы в двух вариантах:
- в виде программы на языке C++;
- в системе компьютерной алгебры Wolfram Mathematica.

Отчет оформляется в системе MS Word и должен содержать условие и математическую
постановку задачи, описание алгоритмов (возможно, с изображением в виде блок-схемы),
деталей их реализации, а также анализ результатов, полученных в ходе решения тестовых
задач. Отчет должен содержать ссылки на использованную литературу, в том числе
содержащую описание реализованных алгоритмов. Исходные коды программ включать в
отчет не требуется.

### Постановка задачи
**Идентификация положения точек в системе ячеек**

Плоская многоугольная сетка задана следующим образом: дан список точек, каждая из
которых задана двумя своими координатами, и список ячеек, для каждой из которых записаны
последовательно против часовой стрелки номера вершин из первого списка точек. При этом
гарантируется, что любые две ячейки либо не имеют общих точек, либо имеют одну общую
вершину, либо примыкают к одному общему отрезку (т.е. имеют общую сторону и две общих
вершины).
Требуется идентифицировать положение, т.е. установить принадлежность конкретной
ячейке, системы точек, про которые известно лишь то, что они лежат строго внутри ячеек.

а) решить задачу «перебором»;

б) решить задачу эффективно, используя «метод полос», называемый также «Slab
decomposition method»
