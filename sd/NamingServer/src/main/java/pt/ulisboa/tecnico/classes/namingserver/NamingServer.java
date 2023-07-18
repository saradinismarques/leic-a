package pt.ulisboa.tecnico.classes.namingserver;

import java.io.IOException;
import java.util.Scanner;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.BindableService;

public class NamingServer {

  private static int port = 5000;

  public static void main(String[] args) {
    System.out.println(NamingServer.class.getSimpleName());
    System.out.printf("Received %d Argument(s)%n", args.length);
    for (int i = 0; i < args.length; i++) {
      System.out.printf("args[%d] = %s%n", i, args[i]);
    }

    NamingServices namingServices = new NamingServices();

    final BindableService ClassServerImpl = (BindableService) new NamingServerServiceImpl(namingServices);

    Server namingServer = ServerBuilder.forPort(port).addService(ClassServerImpl).build();
    try {
      namingServer.start();
    } catch (IOException e) {
      e.printStackTrace();
    }

    // Server threads are running in the background.
    System.out.println("Naming server started");

    // Do not exit the main thread. Wait until server is terminated.
    try {
      namingServer.awaitTermination();
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }
}
