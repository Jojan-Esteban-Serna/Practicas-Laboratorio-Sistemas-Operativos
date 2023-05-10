Elaporado por:   Santiago Agredo Vallejo <sagredovunicauca.edu.co>
                 Jojan Esteban Serna Serna <jeserna@unicauca.edu.co>
              
Se deberá desarrollar dos programas en C que implementen una solución a los problemas del productor-consumidor y los filósofos. presentadas por Tanenbaum [1]. La solución se deberá implementar mediante hilos.

- Problema del productor - consumidor: El programa recibirá por línea de comandos el tamaño del búfer.
- Problema de los filósofos: El programa recibirá por línea de comandos la cantidad de filósofos, mayor a 2.


Nota: El libro online que fue mecionado en clase (https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf)
menciona lo siguiente para la solucion de el problema de los filósofos

                        "4.4.6 Starving Tanenbaums
                        Unfortunately, this solution is not starvation-free. Gingras demonstrated that 
                        there are repeating patterns that allow a thread to wait forever while other
                        threads come and go [4].
                        Imagine that we are trying to starve Philosopher 0. Initially, 2 and 4 are
                        at the table and 1 and 3 are hungry. Imagine that 2 gets up and 1 sit downs;
                        then 4 gets up and 3 sits down. Now we are in the mirror image of the starting
                        position.
                        If 3 gets up and 4 sits down, and then 1 gets up and 2 sits down, we are
                        back where we started. We could repeat the cycle indefinitely and Philosopher
                        0 would starve.
                        So, Tanenbaum’s solution doesn’t satisfy all the requirements."

Se evidencio que esto si ocurrio.