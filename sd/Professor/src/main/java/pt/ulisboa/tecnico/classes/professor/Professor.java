package pt.ulisboa.tecnico.classes.professor;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import java.util.Scanner;

public class Professor {

  public static void main(String[] args) {

    Scanner scanner = new Scanner(System.in);

    final String host = "localhost";
    final int port = 5000;

    final ManagedChannel channel = ManagedChannelBuilder.forAddress(host, port).usePlaintext().build();

    ProfessorFrontend pf = new ProfessorFrontend(channel);
    pf.ScanInpup();
    
  }
}
