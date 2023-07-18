package pt.ulisboa.tecnico.classes.admin;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class Admin {

  public static void main(String[] args) {
    final String host = "localhost";
    final int port = 5000;

    final ManagedChannel channel = ManagedChannelBuilder.forAddress(host, port).usePlaintext().build();

    AdminFrontend af = new AdminFrontend(channel);
    af.ScanInput();
  }
}
