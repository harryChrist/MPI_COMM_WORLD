void play_non_blocking_scenario() {
  MPI_Request request;
  MPI_Status status;
  int request_finished = 0;

  // Inicialização do buffer
  // ...

  MPI_Barrier(MPI_COMM_WORLD);
  // Início do cronômetro
  double time = -MPI_Wtime();

  ////////// Não deve modificar nada ANTES deste ponto //////////

  if (rank == 0) {
    sleep(3);

    // 1- Inicializar o envio não bloqueador para o processo 1
    MPI_Isend(buffer, buffer_count, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);

    double time_left = 6000.0;
    while (time_left > 0.0) {
      usleep(1000); // Trabalhamos por 1ms

      // 2- Testar se a solicitação foi concluída (somente se ainda não estiver concluída)
      MPI_Test(&request, &request_finished, &status);

      // 1ms restante para trabalhar
      time_left -= 1000.0;
    }

    // 3- Se a solicitação ainda não estiver completa, aguarde aqui.
    MPI_Wait(&request, &status);

    // Modificar o buffer para a segunda etapa
    // ...

    // 4- Preparar outra solicitação para o processo 1 com uma tag diferente
    MPI_Isend(buffer, buffer_count, MPI_INT, 1, 1, MPI_COMM_WORLD, &request);

    time_left = 3000.0;
    while (time_left > 0.0) {
      usleep(1000); // Trabalhamos por 1ms

      // 5- Testar se a solicitação foi concluída (somente se ainda não estiver concluída)
      MPI_Test(&request, &request_finished, &status);

      // 1ms restante para trabalhar
      time_left -= 1000.0;
    }
    // 6- Aguarde até que esteja completo
    MPI_Wait(&request, &status);
  }
  else {
    // Trabalhar por 5 segundos
    sleep(5);

    // 7- Inicializar a recepção não bloqueadora do processo 0
    MPI_Irecv(buffer, buffer_count, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);

    // 8- Aguarde aqui até que a solicitação seja concluída
    MPI_Wait(&request, &status);

    print_buffer();

    // Trabalhar por 3 segundos
    sleep(3);

    // 9- Inicializar outra recepção não bloqueadora
    MPI_Irecv(buffer, buffer_count, MPI_INT, 0, 1, MPI_COMM_WORLD, &request);

    // 10- Aguarde até que seja concluída
    MPI_Wait(&request, &status);

    print_buffer();
  }
  ////////// Não deve modificar nada DEPOIS deste ponto //////////

  // Parar o cronômetro
  time += MPI_Wtime();

  // Esta linha nos dá o tempo máximo decorrido em cada processo.
  // Veremos sobre a redução mais tarde!
  double final_time;
  MPI_Reduce(&time, &final_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0)
    std::cout << "Tempo total para o cenário sem bloqueio: " << final_time << "s" << std::endl;
}
