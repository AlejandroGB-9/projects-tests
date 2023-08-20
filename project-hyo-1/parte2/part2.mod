#Sets
set RUTAS;
set PARKING;
set ESTACION;
set COLE;
set ALUMNOS;

#Parameters
param M_KM{i in RUTAS, j in RUTAS};
param M_ST{i in RUTAS, j in RUTAS};
param A_ST{a in ALUMNOS, e in ESTACION};
param A_HERMNS{a in ALUMNOS, t in ALUMNOS};
param B_CAP;
param COST_BUS;
param N_ST;
param COST_KM;
param N_BUSES;

#Decision Variables
/*Variable para el número de autobuses necesarios*/
var x >= 0, integer;

/*Matriz de coeficientes de valores binarios*/
var y{v in RUTAS, m in RUTAS} >= 0, binary;

/*Matriz de coeficientes para las asignaciones*/
var distribucion_alumnos_en_paradas{k in ALUMNOS, l in ESTACION} >= 0, binary;

#Objective function
/* Si multiplicamos nuestra variable de decision y que es una matriz de coeficientes por la matriz de kilómetros en los parametros obtenemos el mínimo de kilómetros*/
/*Con el mínimo de kilómetros ya podemos obtener la solución óptima, mínimo de kilómetros*coste de un kilómetro + autobuses necesarios * coste de un autobús*/
minimize q: sum{v in RUTAS, m in RUTAS}((y[v,m]*M_KM[v,m])*COST_KM) + x*COST_BUS;

/*Restricciones que afectan a las paradas, es decir, que se sale y se llegan a ellas y que no se generen bucles o repeticiones*/
/* Salidas una vez a las paradas S1, S2, S3 */
s.t. una_ocurrencia_llegada {c in ESTACION}: sum{z in RUTAS} y[c,z] = 1;

/* Llegadas una vez de las paradas S1, S2, S3 */
s.t. una_ocurrencia_salida {z in ESTACION}: sum{c in RUTAS} y[c,z] = 1;

/* No se repiten paradas entre rutas*/
s.t. paradas_no_se_repiten {i in ESTACION, j in ESTACION: i<>j}: y[i,j]+y[j,i] <= 1;

/* Restricciones relacioandas a los autobuses*/
/* Rutas <= NºBuses */
s.t. rutas_menor_igual_buses{p in PARKING}: sum{j in RUTAS} y[p,j] <= N_BUSES;

/* Las buses que salen del parking = a los que llegan al colegio */
s.t. buses_parking_igual_colegio {z in PARKING, i in COLE}: sum{c in RUTAS} y[z,c] = sum{j in RUTAS} y[j,i];

/*Minimas salidas del parking | mínimo de autobuses*/
s.t. minimo_autobuses{i in PARKING}: sum{j in RUTAS} y[i,j] >= 1;

/* Buses usados = buses que salen del parking */
s.t. nbuses_igual_salida_parking{p in PARKING}: sum{j in RUTAS}y[p,j] = x;

/* Buses en relación a la capacidad */
s.t. num_buses_capacidad_flujo: B_CAP*x >= N_ST;

/*Restricciones relacionadas al flujo de alumnos*/
/*Flujo Alumnos Capacidad Autobuses*/
s.t. capacida_bus_flujo_alumnos{i in ESTACION, j in ESTACION: i<>j}: (y[i,j]*(M_ST[i,j]+M_ST[j,i])) <= B_CAP;

/*Flujo Alumnos a llegar al colegio */
s.t. llegada_todos_alumnos_colegio: sum{i in RUTAS, j in RUTAS}(y[i,j]*M_ST[i,j]) = N_ST;

/*Restricciones para las asignaciones*/
/* Las paradas con estudiantes ha de ser <= a la capacidad del bus*/
s.t. estudiantes_parada_capacidad_bus{p in ESTACION}:sum{j in ALUMNOS}distribucion_alumnos_en_paradas[j,p] <= B_CAP;

/* La suma de estudiantes en las paradas no puede ser mayor a la cantidad que hay*/
s.t. total_alumnos_paradas: sum{p in ESTACION, j in ALUMNOS}distribucion_alumnos_en_paradas[j,p] = N_ST;

/* La multiplicación entre las asignaciones de alumnos a paradas y como se asignan en la varible de decisión han de ser <= 1 */
s.t. almunos_se_distribuyen_en_sus_asignaciones_posibles{a in ALUMNOS, e in ESTACION}: distribucion_alumnos_en_paradas[a,e]*A_ST[a,e] <= 1;

/* Cada alumnno se debe asignar a su parada*/
s.t. alumno_parada{a in ALUMNOS}: sum{e in ESTACION} distribucion_alumnos_en_paradas[a,e]*A_ST[a,e] = 1;

solve;

printf "Coste óptimo %g./n", q;

end;