#!/usr/bin/env python
# -*- coding: utf-8 -*-

#Visualización del problema, se invierte el autobús para que se desarrolle mejor el problema

#    *----*----*----*----*----*----*----*----*                     *----*----*----*----*
#    |    |    |    |    |    |    |    |    |                     |    |    |    |    |
#    *----*----*----*----*----*----*----*----*                     *----*----*----*----*
#    |    |    |    |    |    |    |    |    |                     |    |    |    |    |
#    *----*----*----*----*----*----*----*----*       --->          *----*----*----*----*
#    |    |    |    |    |    |    |    |    |                     |    |    |    |    |
#    *----*----*----*----*----*----*----*----*                     *----*----*----*----* 
#    |    |    |    |    |    |    |    |    |                     |    |    |    |    |
#    *----*----*----*----*----*----*----*----*                     *----*----*----*----* 
#                                                                  |    |    |    |    |
#                                                                  *----*----*----*----*
#                                                                  |    |    |    |    |
#                                                                  *----*----*----*----*
#                                                                  |    |    |    |    |
#                                                                  *----*----*----*----*
#                                                                  |    |    |    |    |
#                                                                  *----*----*----*----*  

#------------------------------------------------------------
#Importamos las librerías necesarias para el funcionamiento del programa.

import constraint
from constraint import *
import csv
import random
import sys
import os.path
import pathlib

#Creación del problema y lectura del fichero de entrada.

problem = constraint.Problem()

#Se guarda el archivo de entrada introducido por terminal en una variable.

data = sys.argv[1]  
data.replace(".txt", ".csv")

#Leemos el archivo de entrada y se guardan los datos leidos en una lista de tuplas.

file = open(data, "r")
alumnos_bus = [tuple(line) for line in csv.reader(file, delimiter=",")]
file.close()

#------------------------------------------------------------

#Los distintos tipos de alumnos separados en listas.
alumnos_comunes = []
alumnos_conflictivos = []
alumnos_mov_reducida = []
alumnos_mov_reducida_conflictivos = []
alumnos_hermanos = []

#------------------------------------------------------------

#Dominios para las categorias de alumnos según los atributos recibidos.

dominio_ciclo1 = [(0,0),(0,1),(0,2),(0,3),(1,0),(1,1),(1,2),(1,3),(2,0),(2,1),(2,2),(2,3),(3,0),(3,1),(3,2),(3,3)]
dominio_ciclo2 = [(4,0),(4,1),(4,2),(4,3),(5,0),(5,1),(5,2),(5,3),(6,0),(6,1),(6,2),(6,3),(7,0),(7,1),(7,2),(7,3)]
dominio_mov_red_ciclo1 = [(0,0),(0,1),(0,2),(0,3),(3,0),(3,1),(3,2),(3,3)]
dominio_mov_red_ciclo2 = [(4,0),(4,1),(4,2),(4,3)] 
        
#------------------------------------------------------------

#Se registran los alumnos en sus listas correspondientes.
for i in alumnos_bus:
    
    #En el caso de que el alumno tenga un hermano. Se busca el hermanos y se registran.
    #Siempre se comprueba que los hermanos no estén ya registrados. Así evitamos que se dupliquen.
    
    if i[4] != "0":
        index_hermano = int(i[4]) - 1
        
        hermano = alumnos_bus[index_hermano]
        
        if i not in alumnos_hermanos and hermano not in alumnos_hermanos:
            
            alumnos_hermanos.append(i)
            alumnos_hermanos.append(hermano)
    
    #En el caso de que el alumno no tenga hermanos.
    
    elif i[4] == "0":
        
        #Se comprueban los ciclos y si son de movilidad reducida, conflictivos o ambos.
        #Según el caso, se añaden a las listas correspondientes.
        
        if i[2] == "C":
            
            if i[3] == "R":
                
                alumnos_mov_reducida_conflictivos.append(i)
                
            else:
                
                alumnos_conflictivos.append(i)
                
        elif i[3] == "R":
            
            alumnos_mov_reducida.append(i)
                
        else:
            
            alumnos_comunes.append(i)

#------------------------------------------------------------

#Se añaden las variables al problema.
    
for i in alumnos_bus:
        
        if i in alumnos_hermanos:
            
            pass
        
        else:
            
            #Se comprube a que ciclo pertenece el alumno y si es de movilidad reducida.
            
            if i[1] == "1":
                    
                if i[3] == "R":
                        
                    problem.addVariable(i, dominio_mov_red_ciclo1) 
                    
                else:
                    
                    problem.addVariable(i, dominio_ciclo1)
                    
            elif i[1] == "2":
                    
                if i[3] == "R":
                        
                    problem.addVariable(i, dominio_mov_red_ciclo2)
                    
                else:
                    
                    problem.addVariable(i, dominio_ciclo2)
     
     #Para los hermanos, nos aseguramos que la i sea impar para coger al hermano y que luego no se duplique y de problemas con el siguiente alumno en la lista.
j = 1
hermano = 0
                 
for i in alumnos_hermanos:
    
    if j % 2 != 0:
        
        hermano = i
        pass
    
    else:
        
        #Se comprueba las caracteristicas de los hermanos para añadirlas como variables al problema según sean.
        #En otras palabras, se comprueba si son de movilidad reducida y si pertenecen al mismo ciclo o no, ambos casos resultan en distintos dominios.
        #Se debe entender que i corresponde al alumno actual y i+1 al hermano.
        
        #Mismo ciclo 1 o distinto ciclo.
        
        if (hermano[1] == "1" and i[1] == "1") or (hermano[1] != i[1]):
            
            if hermano[3] == "R" and i[3] == "R":
                
                problem.addVariable(hermano, dominio_mov_red_ciclo1)
                problem.addVariable(i, dominio_mov_red_ciclo1)
                
            elif hermano[3] == "R" and i[3] != "R":
                
                problem.addVariable(hermano, dominio_mov_red_ciclo1)
                problem.addVariable(i, dominio_ciclo1)
                
            elif hermano[3] != "R" and i[3] == "R":
                
                problem.addVariable(hermano, dominio_ciclo1)
                problem.addVariable(i, dominio_mov_red_ciclo1)
                
            else:
                
                problem.addVariable(hermano, dominio_ciclo1) 
                problem.addVariable(i, dominio_ciclo1)   
        
        #Mismo ciclo 2.
                
        elif hermano[1] == "2" and i[1] == "2":
            
            if hermano[3] == "R" and i[3] == "R":
                
                problem.addVariable(hermano, dominio_mov_red_ciclo2)
                problem.addVariable(i, dominio_mov_red_ciclo2)
                
            elif hermano[3] == "R" and i[3] != "R":
                
                problem.addVariable(hermano, dominio_mov_red_ciclo2)
                problem.addVariable(i, dominio_ciclo2)
                
            elif hermano[3] != "R" and i[3] == "R":
                
                problem.addVariable(hermano, dominio_ciclo2)
                problem.addVariable(i, dominio_mov_red_ciclo2)
                
            else:
                
                problem.addVariable(hermano, dominio_ciclo2) 
                problem.addVariable(i, dominio_ciclo2)     

    j += 1

#------------------------------------------------------------

#Funciones para restricciones.

def conflictivos(a,b):
    
    #Un alumno conflictivo no puede tener otros alumnos alrededor.
    
    #Se comprueba si los alumnos están en la misma fila, esto es la horizontal de la matriz.
    #Si la diferencia entre ambos alumnos en mayor a 1 entre los asientos de al lado, se devuelve True.
    
    if a[0] == b[0] and abs(a[1] - b[1]) > 1:
        
        return True
    
    #Se comprueba el mismo caso de la horizontal para la horizontal de arriba y de abajo. Es decir, la fila de de arriba y la de abajo al alumno conflictivo.
    
    elif abs(a[0] - b[0]) == 1 and abs(a[1] - b[1]) > 1:
        
        return True
    
    elif abs(a[0] - b[0]) > 1:
        
        return True

def mov_reducida(a,b):
    
    #Un alumno con movilidad reducida no puede tener otros alumnos en el asiento de al lado.
    #Se comprueba que en las secciones de movilidad reducida si un asiento de una fila especifica está ocupado, el asiento de al lado no puede estar ocupado.
    
    if a[0] == b[0] and abs(a[1] - b[1]) == 1 and ((a[1] == 1 and b[1] == 2) or (a[1] == 2 and b[1] == 1)):
        
        return True
    
    elif a[0] == b[0] and abs(a[1] - b[1]) > 1:
        
        return True
        
    elif abs(a[0] - b[0]) > 0:
        
        return True

def hermanos_ciclos(a,b):  
    
    #Dos alumnos que sean hermanos deben estar sentados uno al lado del otro.
    #Esto es, tengan mismo ciclo o no según, el dominio de los hermanos será el mismo y han de estar al lado.
    #La diferencia entre uno y otro en asientos de la horizontal o fila debe ser 1.
    
    if a[0] == b[0] and abs(a[1] - b[1]) == 1:
        
        if (a[1] == 1 and b[1] != 2) or (a[1] == 2 and b[1] != 1):
            
            return True
        
        elif (b[1] == 1 and a[1] != 2) or (b[1] == 2 and a[1] != 1):
        
            return True

def hermanos_distinto_ciclo(a,b):
    
    #Dos alumnos que sean hermanos deben estar sentados uno al lado del otro. En este caso, 'a' es el hermano menor y 'b' el mayor.
    #Al ser de distintos ciclos, el hermano mayor debe tener el dominio de los pasillos del ciclo 1.
    
    if a[0] == b[0] and abs(a[1] - b[1]) == 1 and ((a[1] != 1 and a[1] != 2) and (b[1] == 1 or b[1] == 2)):
        
        return True
#------------------------------------------------------------

#Se añaden las restricciones al problema.

for i in alumnos_bus:
    
    for j in alumnos_bus:
        
        #Se comprueba todos los alumnos del autobus con todos. Se comprueba siempre que los alumnos seleccionados no sean el mismo.
        #Se comprueba que el alumno sea conflictivo o conflictivo y de movilidad reducida, si el alumno a comparar es conflictivo o de movilidad reducida o ambas se aplica la restrición de conflictivos, de movilidad reducida o ambas.
        
        if i != j and (j in alumnos_conflictivos or j in alumnos_mov_reducida or j in alumnos_mov_reducida_conflictivos) and (i in alumnos_conflictivos or i in alumnos_mov_reducida_conflictivos):
               
            problem.addConstraint(conflictivos, (i,j))
                
        
        #Si sólo es de movilidad reducida el alumno.
                     
        elif i != j and (j in alumnos_mov_reducida or alumnos_comunes) and i in alumnos_mov_reducida:
            
            problem.addConstraint(mov_reducida, (i,j))
        
        #Si el alumno esta en la lista de hermanos.
            
        elif i != j and i in alumnos_hermanos:
            
            #Se comprueba si el alumno a comparar es hermano del alumno seleccionado. Se aplican las restricciones según el caso.
            
            if i[4] == j[0]:
                
                #Ambos conflictivos.
                
                if i[2] == j[2]:
                    
                    #No son de movilida reducida ninguno.
                    
                    if i[3] != "R" and j[3] != "R":
                        
                        #Mismo ciclo.
                        
                        if i[1] == j[1]:
                        
                            problem.addConstraint(hermanos_ciclos, (i,j))
                        
                        #Distinto ciclo.
                            
                        else:
                            
                            if i[1] > j[1]:
                                
                                problem.addConstraint(hermanos_distinto_ciclo, (j,i))
                                
                            else:
                                
                                problem.addConstraint(hermanos_distinto_ciclo, (i,j))   
                    
                    #Ambos de movilidad reducida.
                        
                    else:
                        
                        problem.addConstraint(conflictivos, (i,j))
                
                #Si uno es conflictivo y el otro no o ninguno es conflictivo.
                            
                else:
                    
                    if i[3] != "R" and j[3] != "R":
                        
                        if i[1] == j[1]:
                                
                            problem.addConstraint(hermanos_ciclos, (i,j))
                                
                        else:
                            
                            if i[1] > j[1]:
                                
                                problem.addConstraint(hermanos_distinto_ciclo, (j,i))
                                
                            else:
                                
                                problem.addConstraint(hermanos_distinto_ciclo, (i,j))   
                                
                    elif i[3] == "R" and j[3] == "R":
                        
                        if i[2] != j[2]:
                            
                            problem.addConstraint(conflictivos, (i,j))
                            
                        else:
                            
                            problem.addConstraint(mov_reducida, (i,j))
                        
                    elif i[3] == "R" or j[3] == "R":
                        
                        if i[2] == "C" and j[3] == "R":
                            
                            problem.addConstraint(conflictivos, (i,j))
                            
                        elif i[3] == "R" and j[2] == "C":
                            
                            problem.addConstraint(conflictivos, (j,i))
                        
                        else:
                        
                            problem.addConstraint(mov_reducida, (i,j))
            
            #Si los alumnos no son hermanos. Se comprueban los casos posibles.
                        
            else:
                
                #Alumno seleccionado conflictivo.
                
                if (i[2] == "C" and i[3] != "R") or (i[2] == "C" and i[3] == "R"):
                    
                    if j[2] == "C" or j[3] == "R" or (j[2] == "C" and j[3] == "R"):
                        
                        problem.addConstraint(conflictivos, (i,j))
                
                #Alumno seleccionado de movilidad reducida.
                        
                elif i[2] != "C" and i[3] == "R":
                    
                    if (j[2] == "C" and j[3] == "R") or (j[2] == "C" and j[3] != "R"):
                        
                        problem.addConstraint(conflictivos, (j,i))
                        
                    elif (j[2] != "C" and j[3] == "R") or (j[2] != "C" and j[3] != "R"):
                        
                        problem.addConstraint(mov_reducida, (i,j))
                        
                elif i!= "C" and i[3] != "R":
                    
                    if j[3] == "R":
                        
                        problem.addConstraint(mov_reducida, (i,j))     

#Se comprueba que los alumnos no tengan un mismo asiento asignado.
                                
problem.addConstraint(AllDifferentConstraint())

#------------------------------------------------------------

def transfromToSolution(alumnos, aSolution):
    
    for i in alumnos:
        
        alumno = str(i[0] + i[3] + i[2])
        for key, value in aSolution.items(): 
            
            alumnos_sol = str(key[0] + key[3] + key[2])
            if alumno == alumnos_sol:
                
                position = value[0]*4 + value[1] + 1
                text_solution[alumno] = position
                
    return text_solution

#------------------------------------------------------------
#Conseguir las soluciones.

soluciones = problem.getSolutions()
n_soluciones = len(soluciones)
text_solution = {}

#print(soluciones)
print(n_soluciones)

if n_soluciones == 0:
    
    text_solution = "No hay soluciones posibles."
    
else:
#Solución aleatoria.
        
    solucion = random.choice(soluciones)

#Imprimir las soluciones en un archivo de texto con extensión '.output', inversión de la matriz.
#Se crea un diccionario con los alumnos y su posición en el autobus.

    text_solution = transfromToSolution(alumnos_bus, solucion)

    x = 0                
    while x < 10:
        
        other_sol = random.choice(soluciones)
        other_solution = transfromToSolution(alumnos_bus, other_sol)
                    
        print(str(other_solution))
        
        x += 1

#------------------------------------------------------------

#Se crea el archivo de salida dada los solución escogida.   
            
output_file = ""
for i in range(len(data)):
    if data[i] == ".":
        break
    output_file += data[i]
output_file += ".output"
path = pathlib.Path(__file__).parent.resolve()
output_file = os.path.join(path, output_file)
file_output = open(output_file, "w")
file_output.write("Número de soluciones: " + str(n_soluciones) + "\n")
file_output.write(str(text_solution))
file_output.close()

#------------------------------------------------------------        
        