/**
 * @file
 * @brief Planificación de procesos
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @author Santiago Agredo Vallejo <sagredov@unicauca.edu.co>
 * @author Jojan Esteban Serna Serna <jeserna@unicauca.edu.co>
 * @author Joan Sebastian Tuquerrez Gomez <jtuquerrez@unicauca.edu.co>
 */

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sched.h"

/**
 * @brief Adiciona el tiempo de espera a los procesos listos
 * @param processes Lista de procesos
 * @param current_process Proceso que tuvo la CPU
 * @param current_time Tiempo actual
 * @param current_slice Cantidad de tiempo que el proceso tuvo la CPU
 */
void add_waiting_time(list *processes, process *current_process, int current_time, int current_slice);

/**
 * @brief Crea una nueva slice de tiempo
 * @param type Tipo de slice de tiempo (CPU, WAIT)
 * @param from Tiempo de inicio
 * @param to Tiempo de finalizacion
 * @return Nueva slice.
 */
slice *create_slice(slice_type type, int from, int to);

/**
 * @brief Imprime las asignaciones de CPU y tiempo de espera de un proceso.
 * @param p Proceso a imprimir
 */
void print_slices(process *p);

/* Rutina para la planificacion.*/
void schedule(list *processes, priority_queue *queues, int nqueues)
{
  int i;
  list *sequence;           // Secuencia de ejecucion
  sequence_item *si;        // Item de secuencia de CPU
  int total_waiting;        // Tiempo total de espera
  node_iterator it;         // Iterador de lista
  process *current_process; // Apuntador a un proceso

  // Preparar para una nueva simulacion
  // Inicializar las colas de prioridad con la informacion de la lista
  // de procesos leidos
  prepare(processes, queues, nqueues);

  sequence = create_list();

  /** \todo Implementar la planificacion! */

  // Obtener el tamaño de la lista de procesos
  int n = processes->count;
  // Obtener el tiempo de llegada minimo de todos los procesos
  int current_time = get_next_arrival(queues, nqueues);
  // Procesar las llegadas en el tiempo minimo
  int num_ready = process_arrival(current_time, queues, nqueues);
  // Recordar el indice la cola actual
  int current_queue_index;
  // Buscar la cola que tenga al menos un proceso en estado de listo
  priority_queue *current_queue;
  for (size_t i = 0; i < nqueues; i++)
  {
    if (!empty(queues[i].ready))
    {
      current_queue = &queues[i];
      current_queue_index = i;
      break;
    }
  }
  // proceso_actual = NULO
  current_process = NULL;

  bool there_was_expropiation = false;// Booleano que indica si hubo expropiacion
  int remaining_quantum_time = 0;
  // mientras no haya procesos por simular:
  while (n > 0)
  {
    // Obtener el primer proceso listo en la cola seleccionada.
    current_process = (process *)front(current_queue->ready);
    // Quitar el proceso de la cola de listos.
    current_queue->ready = pop_front(current_queue->ready);
    // Pasar el proceso a ejecución.
    current_process->state = RUNNING;
    // Suponer que al proceso se le puede asignar todo el quantum.
    int current_queue_quantum = current_queue->quantum;
    if(there_was_expropiation){
      current_queue_quantum = remaining_quantum_time;
    }
    int assigned_time = current_queue_quantum;
    // Si el tiempo restante del proceso es menor al tiempo que se puede asignar, tomar solo el tiempo restante.

    if (current_process->remaining_time < current_queue_quantum)
    {
      assigned_time = current_process->remaining_time;
    }

    // Validar SRT:
    // Verificar si llega un proceso a la cola SRT en este quantum y tiene menor tiempo restante (en el momento en el que llega?)
    // Solo dar el tiempo de CPU desde el tiempo actual hasta el tiempo de llegada del nuevo proceso
    there_was_expropiation = false;
    if (current_queue->strategy == SRT && !empty(current_queue->arrival))
    {
      // Obtener el primer proceso de la lista de llegadas
      process *arrived_srt_process = front(current_queue->arrival);
      int arrived_arrival_time = arrived_srt_process->arrival_time;
      // Verificar si llega en el tiempo asignado de ejecucion
      if (arrived_arrival_time > current_time && arrived_arrival_time < current_time + assigned_time)
      {
        // Obtener el tiempo que llevaria ejecutandose hasta que llega el proceso
        int executed_time_at_arrival = arrived_arrival_time - current_time;
        // Obtener el tiempo que le queda cuando llega el nuevo proceso
        int remaining_time_at_arrival = current_process->remaining_time - executed_time_at_arrival;
        // Verificar si cuando llega el siguiente proceso este tiene menor tiempo restante de ejecucion (expropiar si es el caso)
        if (arrived_srt_process->remaining_time < remaining_time_at_arrival)
        {
          assigned_time = executed_time_at_arrival;
          there_was_expropiation = true;
          // POST: Hay un proceso que va a expropiar al proceso actual cuando este termine su tiempo asignado no se avanzara de cola
        }
      }
    }
    //  Si se dio CPU al proceso:
    if (assigned_time > 0)
    {
      // Actualizar el tiempo de uso de CPU del proceso
      current_process->cpu_time += assigned_time;
      // Disminuir el tiempo restante en el tiempo de CPU dado
      current_process->remaining_time -= assigned_time;
      // Aumentar el tiempo de espera de los demas procesos
      // Usar add_waiting_time()
      add_waiting_time(processes, current_process, current_time, assigned_time);
      // Adicionar un nuevo elemento a la secuencia de ejecucion
      /** Usar create_slice() y \todo adicionar a la lista de secuencias de ejecucion*/
      push_back(current_process->slices, create_slice(CPU, current_time, current_time + assigned_time));
      si = (sequence_item *)malloc(sizeof(sequence_item));
      si->name = current_process->name;
      si->time = assigned_time;
      push_back(sequence, si);
    }

    // Fin si

    // Verificar si el proceso ha finalizado
    // Si el proceso finalizo:

    // Nota: El unico proceso que se ve afectado por no procesar las llegadas antes es Round Robin ya que los procesos que pasan
    // A listo se insertan al final sin agregar los que llegan, por lo cual se decidio validar eso despues de este if siguiente y de procesar 
    // Las llegadas, FIFO y SJF no se ven afectadas ya que son no expropiativas y sus procesos sin terminar de ejecutar se pasan
    // Al frente de la lista de listos, para SRT tampoco ya que se valido su lista de arrivals y se verificaron las expropiaciones
    // Que pudiesen ocurrir y se pasa a su lista de listos de acuerdo al tiempo restante del proceso que se pasa a listo
    
    if (current_process->remaining_time == 0)
    {
      // Levar a la lista finished de su cola de prioridad
      current_process->state = FINISHED;
      current_process->finished_time = current_time + assigned_time;
      current_queue->finished = push_back(current_queue->finished, current_process);
      // Restar uno al total de procesos que falta por simular
      n--;
      // SRT: Si el proceso finaliza justo cuando llega el otro, pasar a la siguiente cola.
      // En caso contrario, no se debe cambiar de cola de prioridad!
      // Nota: esto ya lo verifica Validar SRT, esta condicion: (arrived_arrival_time < current_time + assigned_time) hace que
      // No se le expropie la CPU y there_was_expropiation seria falso
    }
    // Si el proceso no finalizo:
    else
    {
      // Pasar a estado de listo
      current_process->state = READY;
      // Enviar a la cola de listos de su prioridad, de acuerdo con el algoritmo de esa cola.
      if (current_queue->strategy == SJF)
      {
        // Para SJF se inserta el proceso en el frente porque no se puede expropiar
        push_front(current_queue->ready, current_process);
      }
      else if (current_queue->strategy == SJF || current_queue->strategy == SRT)
      {
        // Para SRT, el proceso se inserta de acuerdo con el tiempo faltante
        insert_ordered(current_queue->ready, current_process, compare_srt);
      }
      else if (current_queue->strategy == FIFO)
      {
        // Para FIFO se inserta al frente ya que no se puede expropiar
        push_front(current_queue->ready, current_process);
      }
      /*else
      {
        // Para round robin, procesar los procesos que llegan en ese quantum, ya que si se manda al final el
        // Proceso que se estaba ejecutando, la lista no conservara el orden de llegada
        process_arrival(current_time + assigned_time, queues, nqueues);
        push_back(current_queue->ready, current_process);
      }*/
    }

    // Fin si

    // Avanzar el tiempo a la cantidad de CPU asignada.
    current_time += assigned_time;
    // Procesar las llegadas de nuevos procesos al tiempo actual.
    int num_ready = process_arrival(current_time, queues, nqueues);
    
    // Para round robin los procesos deben ir hacia al final de la cola de listos despues de procesar las llegadas (no antes),
    // ya que si no se procesan los procesos que llegaron durante ese tiempo de ejecucion el proceso que pasa a listo quedara
    // en frente de los procesos que llegaron mientras se ejecutaba y no al final, esto llevaria a que  en el siguiete ciclo
    // se le de la cpu antes
    if(current_process->remaining_time> 0 && current_queue->strategy == RR){
      push_back(current_queue->ready,current_process);
    }
    // Terminar si ya no existen procesos por planificar.
    if (n == 0)
    {
      break;
    }
    //POST: Quedan procesos, por lo que para el proximo ciclo infinito se garantiza que se va a romper 

    // Avanzar a la siguiente cola de prioridad (Usando round robin)
    // SRT: Si el proceso que se expropio de la CPU no uso todo el quantum
    // se debe asignar el tiempo restante del quantum al proceso que llego
    // no se cambia de cola de prioridad!
    // En cualquier otro caso, se pasa a la siguiente cola de prioridad.
    if (!there_was_expropiation)
    {
      int last_index = current_queue_index;
      while (1)
      {
        current_queue_index = ((current_queue_index + 1) % nqueues);
        if (!empty(queues[current_queue_index].ready))
        {
          // Cola con llegadas de listos
          current_queue = &queues[current_queue_index];
          break;
        }
        // Si no existen procesos en estado de listos en ninguna cola (Si ya se dio una vuelta y las listas de listo estaban vacias)
        // avanzar hasta la siguiente llegada (en cualquier cola)
        if (last_index == current_queue_index)
        {
          current_time = get_next_arrival(queues, nqueues);
          int num_ready = process_arrival(current_time, queues, nqueues);
        }
      }
    }else{
      //Si hubo expropiacion se debe asignar el tiempo restante del quantum al proceso que llegara
      remaining_quantum_time = current_queue_quantum - assigned_time;
      //POST: Hay un proceso que llegara al terminar de ejecutarse el expropiado
    }
    // Si no existen procesos en estado de listos en ninguna cola, avanzar hasta la siguiente llegada (en cualquier cola)
    if (get_ready_count(queues, nqueues) == 0)
    {
      current_time = get_next_arrival(queues, nqueues);
      int num_ready = process_arrival(current_time, queues, nqueues);
    }
  }

  // Imprimir el resultado de la simulacion

  for (i = 0; i < nqueues; i++)
  {
    print_queue(&queues[i]);
  }
  printf("%5s%20s%5s%5s%5s\n", "#", "Process", "Arr.", "Fin.", "Wait");
  i = 1;
  total_waiting = 0;
  for (it = head(processes); it != 0; it = next(it))
  {
    current_process = (process *)it->data;
    printf("%5d%20s%5d%5d%5d\n", i++, current_process->name, current_process->arrival_time, current_process->finished_time, current_process->waiting_time);
    total_waiting = total_waiting + current_process->waiting_time;
  }

  printf("\nTotal waiting time: %d Average waiting time: %.3f\n", total_waiting, (float)((float)total_waiting / (int)processes->count));

  for (it = head(sequence); it != 0; it = next(it))
  {
    si = (sequence_item *)it->data;
    printf("%s (%d) ", si->name, si->time);
  }
  printf("\n");
}

priority_queue *create_queues(int n)
{
  priority_queue *ret;
  int i;

  ret = (priority_queue *)malloc(sizeof(priority_queue) * n);

  for (i = 0; i < n; i++)
  {
    ret[i].strategy = RR; // Por defecto RR
    ret[i].quantum = 0;
    ret[i].arrival = create_list();
    ret[i].ready = create_list();
    ret[i].finished = create_list();
  }

  return ret;
}

void print_queue(priority_queue *queue)
{
  int i;
  node_iterator ptr;

  printf("%s q=",
         (queue->strategy == RR) ? "RR" : ((queue->strategy == FIFO) ? "FIFO" : ((queue->strategy == SJF) ? "SJF" : ((queue->strategy == SRT) ? "SRT" : "UNKNOWN"))));
  printf("%d ", queue->quantum);

  printf("ready (%d): { ", queue->ready->count);

  for (ptr = head(queue->ready); ptr != 0; ptr = next(ptr))
  {
    print_process((process *)ptr->data);
  }

  printf("} \n");

  printf("arrival (%d): { ", queue->arrival->count);

  for (ptr = head(queue->arrival); ptr != 0; ptr = next(ptr))
  {
    print_process((process *)ptr->data);
  }

  printf("} \n");

  printf("finished (%d): { ", queue->finished->count);

  for (ptr = head(queue->finished); ptr != 0; ptr = next(ptr))
  {
    print_process((process *)ptr->data);
  }
  printf("}\n");
}

int compare_arrival(void *const a, void *const b)
{
  process *p1;
  process *p2;

  p1 = (process *)a;
  p2 = (process *)b;

  // printf("Comparing %s to %s : %d %d\n", p1->name, p2->name, p1->arrival_time, p2->arrival_time);

  return p2->arrival_time - p1->arrival_time;
}

int compare_sjf(void *const a, void *const b)
{
  process *p1;
  process *p2;
  int result;

  p1 = (process *)a;
  p2 = (process *)b;

  // Si el otro proceso ya recibio la CPU el otro tiene prioridad
  if (p2->cpu_time > 0)
  {
    result = -1;
  }
  else
  {
    result = p2->remaining_time - p1->remaining_time;
  }

  return result;
}

int compare_srt(void *const a, void *const b)
{
  process *p1;
  process *p2;

  int result;

  p1 = (process *)a;
  p2 = (process *)b;

  // Comparar el tiempo restante
  result = p2->remaining_time - p1->remaining_time;

  return result;
}

process *create_process(char *name, int arrival_time, int execution_time)
{

  process *p;

  // Reservar memoria para el proceso
  p = (process *)malloc(sizeof(process));

  memset(p, 0, sizeof(process));
  strcpy(p->name, name);
  p->arrival_time = arrival_time;
  p->execution_time = execution_time;
  p->priority = -1;
  p->waiting_time = -1;
  p->remaining_time = p->execution_time;
  p->finished_time = -1;
  p->cpu_time = 0;
  p->state = LOADED;
  p->slices = create_list();

  return p;
}

void restart_process(process *p)
{

  p->waiting_time = -1;
  p->finished_time = -1;
  p->remaining_time = p->execution_time;
  p->state = LOADED;
  clear_list(p->slices, 1);
}

void print_slices(process *p)
{
  node_iterator it;
  slice *s;

  for (it = head(p->slices); it != 0; it = next(it))
  {
    s = it->data;
    printf("%s %d -> %d ", (s->type == CPU ? "CPU" : "WAIT"), s->from, s->to);
  }
}

void print_process(process *p)
{
  if (p == 0)
  {
    return;
  }
  printf("(%s arrival:%d execution:%d finished:%d waiting:%d ",
         p->name, p->arrival_time, p->execution_time, p->finished_time, p->waiting_time);
  // UNDEFINED, LOADED, READY, RUNNING, FINISHED
  printf("%s )\n", (p->state == READY) ? "ready" : (p->state == LOADED) ? "loaded"
                                               : (p->state == FINISHED) ? "finished"
                                                                        : "unknown");
}

void prepare(list *processes, priority_queue *queues, int nqueues)
{
  int i;
  process *p;
  node_iterator it;

  /* Limpiar las colas de prioridad */

  for (i = 0; i < nqueues; i++)
  {
    // printf("Clearing queue %d\n", i);
    if (queues[i].ready != 0)
    {
      clear_list(queues[i].ready, 0);
      queues[i].ready = create_list();
    }
    if (queues[i].arrival != 0)
    {
      clear_list(queues[i].arrival, 0);
      queues[i].arrival = create_list();
    }
    if (queues[i].finished != 0)
    {
      clear_list(queues[i].finished, 0);
      queues[i].finished = create_list();
    }
  }

  /* Inicializar la informacion de los procesos en la lista de procesos */
  for (it = head(processes); it != 0; it = next(it))
  {
    p = (process *)it->data;
    restart_process(p);
    insert_ordered(queues[p->priority].arrival, p, compare_arrival);
  }

  printf("Prepared queues:\n");
  for (i = 0; i < nqueues; i++)
  {
    print_queue(&queues[i]);
  }
}

int process_arrival(int now, priority_queue *queues, int nqueues)
{
  int i;
  process *p;
  process *aux;
  int queue_processed;
  int total;

  // Procesar llegadas.
  total = 0;
  for (i = 0; i < nqueues; i++)
  {

    // printf("Queue %d\n", i);
    if (empty(queues[i].arrival))
    {
      // Pasar a la siguiente cola.
      continue;
    }

    queue_processed = 0;
    do
    {
      // Procesar las llegadas de nuevos procesos
      p = front(queues[i].arrival);

      if (p == 0)
      {
        queue_processed = 1;
        continue;
      }

      // Ignorar el proceso si no es momento de llevarlo a la cola de listos
      if (p->arrival_time > now)
      {
        queue_processed = 1;
        continue;
      }

      printf("[%d] Process %s arrived at %d.\n", now, p->name, p->arrival_time);
      p->state = READY;
      p->waiting_time = now - p->arrival_time;

      // Dibujar la linea del tiempo de espera
      if (p->waiting_time > 0)
      {
        push_back(p->slices, create_slice(WAIT, p->arrival_time, now));
      }

      total++;

      if (queues[i].strategy == SJF)
      {
        // Para SJF y SRT, el proceso se inserta de acuerdo con el tiempo faltante
        insert_ordered(queues[i].ready, p, compare_sjf);
      }
      else if (queues[i].strategy == SJF || queues[i].strategy == SRT)
      {
        // Para SJF y SRT, el proceso se inserta de acuerdo con el tiempo faltante
        insert_ordered(queues[i].ready, p, compare_srt);
      }
      else
      {
        // Para los demas algoritmos, el nuevo proceso se inserta al final de la cola de listos
        push_back(queues[i].ready, p);
      }

      // Quitar el proceso de la cola de llegadas
      pop_front(queues[i].arrival);

    } while (!queue_processed);
  }
  // Retorna el numero de procesos que se pasaron a las colas de listos
  return total;
}

int get_next_arrival(priority_queue *queues, int nqueues)
{
  int ret;
  process *p;
  int i;
  int arrival_time;
  int min_time;

  ret = INT_MAX;
  arrival_time = ret;

  for (i = 0; i < nqueues; i++)
  {
    // Revisar el primer proceso en la cola de listos
    p = front(queues[i].arrival);
    if (p != 0)
    {
      arrival_time = p->arrival_time;
      ret = min(ret, arrival_time);
    }
  }

  // printf("Next arrival : %d\n", ret);

  if (ret == INT_MAX)
  {
    ret = -1;
  }

  return ret;
}

int get_ready_count(priority_queue *queues, int nqueues)
{
  int ret;
  int i;

  ret = 0;

  for (i = 0; i < nqueues; i++)
  {
    ret = ret + queues[i].ready->count;
  }
  return ret;
}

int max_scheduling_time(list *processes)
{
  int process_total;
  node_iterator it;
  process *p;
  int max;

  max = 0;

  // Calcular el tiempo como la suma de los tiempos de ejecucion de los procesos.
  for (it = head(processes); it != 0; it = next(it))
  {
    p = (process *)it->data;
    // Sumar el tiempo total de CPU y lock
    if (p->finished_time > max)
    {
      max = p->finished_time;
    }
  }

  return max;
}

void add_waiting_time(list *processes, process *current_process, int current_time, int current_slice)
{
  node_iterator it;
  process *p;
  for (it = head(processes); it != 0; it = next(it))
  {
    p = (process *)it->data;
    if (p != current_process && p->state == READY)
    {
      p->waiting_time += current_slice;
      push_back(p->slices, create_slice(WAIT, current_time, current_time + current_slice));
    }
  }
}

slice *create_slice(slice_type type, int from, int to)
{
  slice *s;

  s = (slice *)malloc(sizeof(slice));
  s->type = type;
  s->from = from;
  s->to = to;
}