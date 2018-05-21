# Proyecto2SO - Simulación de Paginación y Segmentación

![alt tag](http://forthebadge.com/images/badges/built-with-love.svg) ![alt tag](http://forthebadge.com/images/badges/built-by-developers.svg) 

Este segundo proyecto tiene como finalidad implementar un problema de sincronización de procesos en una simulación de asignación de memoria a 
procesos mediante los esquemas de paginación y segmentación. Para esto se debe implementar cada uno de los programas que se listan abajo como 
programas separados. Los programas deben ser implementados en C. Existen diferentes tipos de semáforos. Se debe tomar en cuenta que dado que 
en este problema los procesos a utilizar son programas separados, se debe buscar el tipo de semáforo que me permite la comunicación entre 
programas. Este problema incluye un archivo compartido entre todos los actores para llevar una bitácora de eventos. Se debe utilizar la memoria 
compartida para simular la memoria a asignar a los procesos. Si necesitan otro tipo de memoria compartida para la información del problema 
pueden usarla. Para implementar este problema se deben crear 4 programas que se detallan a continuación.

Programa Inicializador: Este programa se encarga de crear el ambiente. Pide los recursos y los inicializa de manera que los actores encuentren 
todo listo en el momento de empezar a funcionar. Este programa pide la cantidad de páginas o espacios de memoria que va a haber y solicita la 
memoria compartida al sistema operativo. Este proceso debe morir después de realizar la inicialización. 

Programa Productor de Procesos: Este es un programa que genera procesos que llegan al “Ready” (Threads). Lo primero que debe hacer es solicitar
el tipo de algoritmo con el que se desea correr la simulación (Paginación o Segmentación). Para cada uno de los hilos se define de manera Random
los siguientes valores:

Paginación
Cantidad de páginas (1-10)
Tiempo (20s-60s).

Segmentación
Cantidad de Segmentos (1-5)
Cantidad de espacios de memoria por segmento (1-3)
Tiempo (20s-60s).

Dependiendo del esquema que se esté usando todos los procesos se crean dentro de ese mismo esquema.La Distribución con la que se generaran los 
procesos estará determinada por un tiempo aleatorio (30s-60s). Cada proceso debe buscar dentro de la memoria las páginas o espacio para sus 
segmentos. Si no hay espacio suficiente el proceso muere. Solo un proceso a la vez puede estar corriendo el algoritmo de búsqueda de espacio a 
la vez, para que no haya choques que 2 procesos seleccionen el mismo hueco/página. Una vez que haya encontrado lugar transcurrirá un sleep con 
la cantidad de tiempo definida para él y después de forma exclusiva (Región Critica) devuelve los espacios de memoria que tenía asignados.

Programa Espía: Para efectos de control este programa debe responder a las siguientes solicitudes del usuario

Estado de la memoria en determinado momento. Debe mostrar cómo están acomodados los procesos en la memoria.
Estado de los Procesos. Al pedir el estado debe decirme:
El PID de los proceso que estén en la memoria en ese momento (sleep)
El PID del único proceso que esté buscando espacio en la memoria. (punto 2 del proceso)
El PID de los procesos que estén bloqueados (esperando por la región critica) (punto 1 o 6 del
proceso).
El PID de los procesos que han muerto por no haber espacio suficiente.
El PID de los procesos que ya terminaron su ejecución.

Bitácora: Todos los procesos deben registrar sus acciones. Esto incluye que diga por cada PID, la acción, que tipo es (asignación, desasignación), 
hora y espacio asignado para cada uno de sus páginas o segmentos. Para el caso que un proceso no haya entrado a la memoria debe registrar este hecho 
en la bitácora.

Programa Finalizador: Se encarga de matar todos los procesos que estén en escena. Devolver los recursos que había solicitado. Y cerrar el archivo de 
la Bitácora.

En general la lógica de cada proceso sería la siguiente:
1. Pedir semáforo de memoria
2. Buscar su ubicación
3. Escribir en Bitácora
4. Devolver semáforo de memoria
5. Sleep
6. Pedir Semáforo de memoria
7. Liberar memoria
8. Escribir en Bitácora
9. Devolver semáforo de memoria

Hecho por: 
### Jason Barrantes
### Manrique Durán
### Randy Morales